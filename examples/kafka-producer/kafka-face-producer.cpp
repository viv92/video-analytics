/*
    Copyright 2016 AITOE
*/

#include <cv.h>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include "../../tools/videoSources/libvlcStreamer.h"
#include "../../tools/videoSources/webcamStreamer.h"
#include "../../tools/videoSources/fileStreamer.h"
#include "../../tools/videoSources/ffmpegStreamer.h"
#include "../../tools/videoSources/aiVideoStreamer.h"
#include "../../tools/videoSources/ffmpegStreamer.h"
#include "../../tools/logging.h"
#include "../../src/aiSaac.h"

#include "rdkafkacpp.h"
#include "../../src/utils/json.hpp"

int drag;
cv::Point point1, point2; /* vertical points of the bounding box */
cv::Rect markedRect; /* bounding box */
int select_flag = 0;
cv::Mat img;

static bool run = true;
static bool exit_eof = false;

static void sigterm (int sig) {
  run = false;
}

class ExampleEventCb : public RdKafka::EventCb {
 public:
  void event_cb (RdKafka::Event &event) {
    switch (event.type()) {
      case RdKafka::Event::EVENT_ERROR:
        std::cerr << "ERROR (" << RdKafka::err2str(event.err()) << "): " <<
            event.str() << std::endl;
        if (event.err() == RdKafka::ERR__ALL_BROKERS_DOWN)
            run = false;
        break;

      case RdKafka::Event::EVENT_STATS:
        std::cerr << "\"STATS\": " << event.str() << std::endl;
        break;

      case RdKafka::Event::EVENT_LOG:
        fprintf(stderr, "LOG-%i-%s: %s\n",
                event.severity(), event.fac().c_str(), event.str().c_str());
        break;

      default:
        std::cerr << "EVENT " << event.type() <<
            " (" << RdKafka::err2str(event.err()) << "): " <<
            event.str() << std::endl;
        break;
    }
  }
};

class ExampleDeliveryReportCb : public RdKafka::DeliveryReportCb {
 public:
  void dr_cb (RdKafka::Message &message) {
    std::cout << "Message delivery for (" << message.len() << " bytes): " <<
        message.errstr() << std::endl;
    if (message.key())
      std::cout << "Key: " << *(message.key()) << ";" << std::endl;
  }
};

void mouseHandler(int event, int x, int y, int flags, void* param) {
    if (event == CV_EVENT_LBUTTONDOWN && !drag) {
        point1 = cv::Point(x, y);
        drag = 1;
    }
    if (event == CV_EVENT_MOUSEMOVE && drag) {
        cv::Mat img1 = img.clone();
        point2 = cv::Point(x, y);
        cv::rectangle(img1, point1, point2, CV_RGB(255, 0, 0), 3, 8, 0);
        cv::imshow("Mark Area", img1);
        //debugMsg("Mark Area\n");
    }
    if (event == CV_EVENT_LBUTTONUP && drag) {
        point2 = cv::Point(x, y);
        markedRect = cv::Rect(point1.x, point1.y, x-point1.x, y-point1.y);
        if (markedRect.width > 0 && markedRect.height > 0)
          drag = 0;
    }
    if (event == CV_EVENT_LBUTTONUP) {
       /* ROI selected */
        select_flag = 1;
        drag = 0;
        std::cout << "MARKED RECTANGLE" << std::endl << std::flush;
        debugMsg("Marked Rectangle");
        // std::cout << "MARKED RECTANGLE2" << std::endl << std::flush;
    }
}

void getSelection(cv::Mat &rawFrame) {
    drag = 0;
    select_flag = 0;
    img = rawFrame.clone();
    cv::namedWindow("Mark Area", CV_WINDOW_AUTOSIZE);
    cv::setMouseCallback("Mark Area", mouseHandler, NULL);
    cv::imshow("Mark Area", img);
    cv::waitKey(100);
    while (!select_flag) {
        cv::waitKey(0);
    }  // todo blocks on main thread
    // cv::destroyWindow("Mark Area");
}

aiSaac::FaceAnalytics *faceAnalytics = NULL;

void analyticsThread(aiVideoStreamer *videoStreamer, aiSaac::AiSaacSettings *aiSaacSettings) {
    signal(SIGINT, sigterm);
    signal(SIGTERM, sigterm);
    // setting up Kafka
    std::string brokers = "localhost";
    std::string topic_str = "test";
    std::string errstr;
    int32_t partition = RdKafka::Topic::PARTITION_UA;

    RdKafka::Conf *conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
    RdKafka::Conf *tconf = RdKafka::Conf::create(RdKafka::Conf::CONF_TOPIC);
    char *val = "\0";
    std::cout << "hello1" << std::endl;
    std::cout << "hello2" << std::endl;
    val++;
    std::cout << "hello3" << std::endl;

    conf->set("metadata.broker.list", brokers, errstr);

    ExampleEventCb ex_event_cb;
    conf->set("event_cb", &ex_event_cb, errstr);

    ExampleDeliveryReportCb ex_dr_cb;

    /* Set delivery report callback */
    conf->set("dr_cb", &ex_dr_cb, errstr);

    RdKafka::Producer *producer = RdKafka::Producer::create(conf, errstr);
    if (!producer) {
      std::cerr << "Failed to create producer: " << errstr << std::endl;
      exit(1);
    }

    std::cout << "% Created producer " << producer->name() << std::endl;

    /*
     * Create topic handle.
     */
    RdKafka::Topic *topic = RdKafka::Topic::create(producer, topic_str,
						   tconf, errstr);
    if (!topic) {
      std::cerr << "Failed to create topic: " << errstr << std::endl;
      exit(1);
    }



    faceAnalytics = new aiSaac::FaceAnalytics(aiSaacSettings);
    cv::Mat currentFrame;
    int frameNumber = 0;
    while (true) {
        currentFrame = videoStreamer->getFrame();
        faceAnalytics->track(currentFrame, frameNumber);
        for (int i = 0; i < faceAnalytics->blobContainer.size(); i++) {
            std::cout << "blob: " << faceAnalytics->blobContainer[i].ID << std::endl;
            // uchar *tempUChar = faceAnalytics->blobContainer[i].faceImages[faceAnalytics->blobContainer[i].faceImages.size() - 1].data;
            // char *faceImage = reinterpret_cast<char*>(tempUChar);
            std::vector<uchar> buff;
            cv::imencode(".jpg", faceAnalytics->blobContainer[i].faceImages[faceAnalytics->blobContainer[i].faceImages.size() - 1], buff);
            // for (int i = 0; i < buff.size(); i++) {
            //     std::cout << buff[i];
            // }
            // std::cout << std::endl;

            // std::string sName(reinterpret_cast<char*> (&buff[0]));

            // std::cout << "sName: " << sName << std::endl;

            nlohmann::json newFace;
            newFace["source"] = "WebCam";
            newFace["blobId"] = faceAnalytics->blobContainer[i].ID;
            newFace["value"] = buff;
            std::string KVM = newFace.dump();
            char *cstr = new char[KVM.length() + 1];
            strcpy(cstr, KVM.c_str());
            // do stuff
            // char *KVMCSTR = reinterpret_cast<char*>(KVM.c_str());
            std::cout << "final shit: " << KVM << std::endl;
            // const_cast<char *> message =
            RdKafka::ErrorCode resp =
            producer->produce(topic, partition,
    			       RdKafka::Producer::RK_MSG_COPY /* Copy payload */,
                 cstr, strlen(cstr),
                 NULL, NULL);
            if (resp != RdKafka::ERR_NO_ERROR)
         	      std::cerr << "% Produce failed: " << RdKafka::err2str(resp) << std::endl;
            else
         	     //  std::cerr << "% Produced message (" << line.size() << " bytes)" << std::endl;
            producer->poll(0);

            delete [] cstr;
        }
        frameNumber++;
    }
}

int main(int argc, char *argv[]) {
    debugMsg("Camera Initialized\n");
    if (argc > 1) {
        rtspURL = argv[1];
        debugMsg("Developer Name " + developerName);
        debugMsg("RTSP URL " + rtspURL);
    }

    // Use VLC IP camera feed
    // LibvlcStreamer *cam = new LibvlcStreamer(rtspURL);
    // std::thread cam1(&LibvlcStreamer::startStream, cam);

    // Use webcam Streamer
    WebcamStreamer *cam = new WebcamStreamer();
    std::thread cam1(&WebcamStreamer::startStream, cam);

    // Use file Streamer
    // FileStreamer *cam = new FileStreamer(filePath);
    // if (!cam->isStreaming()) {
    //    std::cout << "Unable to open file for reading" << std::endl;
    //    debugMsg("Unable to open file for reading");
    //    return 0;
    // }
    // std::cout << "File Frame rate" << cam->getFileFPS() << std::endl;

    // Use FFMPEG Streamer
    //  FfmpegStreamer *cam = new FfmpegStreamer(rtspURL);
    //  std::thread cam1(&FfmpegStreamer::startStream, cam);

    while (!cam->isStreaming()) {}

    std::cout << "camera running now " << cam->getFrame().size() << std::endl;

    aiSaac::AiSaacSettings *aiSaacSettings = new aiSaac::AiSaacSettings(storagePath + "/aiSaacSettingsconfig1.json");

    std::thread analyticsThreadInstance1(analyticsThread, cam, aiSaacSettings);
    // analyticsThread(cam, aiSaacSettings);

    cv::namedWindow("Display window", CV_WINDOW_AUTOSIZE);
    cv::Mat annotatedFrame;
    while (true) {
        annotatedFrame = cam->getFrame();
        if (faceAnalytics) {
            faceAnalytics->annotate(annotatedFrame);
        }
        if (annotatedFrame.data != NULL) {
            cv::imshow("Display window", annotatedFrame);
            cv::waitKey(10);
        }
    }
}

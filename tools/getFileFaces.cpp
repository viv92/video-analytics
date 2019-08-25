/*
    Copyright 2016 AITOE
*/

#include <cv.h>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <thread>
#include <mutex>
#include "videoSources/fileStreamer.h"
#include "videoSources/aiVideoStreamer.h"
#include "developerConfig.h"
#include "logging.h"
#include "../src/BgSubAnalytics/bgSubAnalytics.h"
#include "../src/FaceAnalytics/faceAnalytics.h"
#include "../src/aiSaacSettings.h"

int drag;
cv::Point point1, point2; /* vertical points of the bounding box */
cv::Rect markedRect; /* bounding box */
int select_flag = 0;
cv::Mat img;
// void alertEvent(struct aisl::event eventData) {
//     std::cout << "EVENT CALLBACK" << std::endl;
//     //debugMsg("Event Callback\n");
//     std::cout << eventData.eventType << std::endl;
//     std::cout << eventData.time << std::endl;
//     std::cout << eventData.imageLocation << std::endl;
//     std::cout << eventData.videoLocation << std::endl;
//     std::cout << eventData.description << std::endl;
// }
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

void analyticsThread(FileStreamer *videoStreamer,
    aiSaac::AiSaacSettings aiSaacSettings) {
    aiSaac::BgSubAnalytics bgSubAnalytics = aiSaac::BgSubAnalytics(aiSaacSettings);
    aiSaac::FaceAnalytics faceAnalytics = aiSaac::FaceAnalytics(aiSaacSettings);
    cv::Mat currentFrame;
    int frameNumber = 0;
    cv::namedWindow("Display window", CV_WINDOW_AUTOSIZE);
    while (true) {

        if(!videoStreamer->isStreaming()) {
            std::cout << "Seems to be done" <<std::endl;
            break;
        }
        currentFrame = videoStreamer->getFrame();
        std::cout << "On frame "
            << frameNumber
            << " of " << videoStreamer->getNumberOfFrames()
            << std::endl << std::flush;
        if(currentFrame.data == NULL) {
            std::cout << "Seems to be done" <<std::endl;
            for (std::vector<aiSaac::FaceBlob>::iterator it =
            faceAnalytics.blobContainer.begin();
            it != faceAnalytics.blobContainer.end();) {
                    // face storage mode true means detection algorithm will store faces.
                    // codeReview(Anurag): Currently overwrites existing data in faceStorage folder
                if (aiSaacSettings.getFaceStorageMode()) {
                    std::string personDirectory = aiSaacSettings.getFaceRecognitionTrainingDataPath()
                        + "/Person" + std::to_string(it->ID);
                    std::system(("mkdir " + personDirectory).c_str());
                    for (int i = 0; i < it->faceImages.size(); i++) {
                        std::string personImagePath =
                            personDirectory + "/" + std::to_string(i) + ".png";
                        std::cout << "storing image: " << personImagePath << std::endl;
                        cv::imwrite(personImagePath, it->faceImages[i]);
                    }
                }
                it++;
            }
            break;
        }
        std::vector<cv::Rect> localizedRectangles;

        clock_t time;
        time = clock();

        // bgSubAnalytics.track(currentFrame, frameNumber);
        //
        // bgSubAnalytics.localize(currentFrame, frameNumber, localizedRectangles);

        std::cout << "BG SUB Prediction Time: " << sec(clock() - time) << std::endl;
        time = clock();

        faceAnalytics.track(currentFrame, frameNumber);
        // for (int i = 0; i < localizedRectangles.size(); i++) {
        //      faceAnalytics.track(currentFrame, frameNumber, localizedRectangles[i]);
        // }
        std::cout << "FACE Prediction Time: " << sec(clock() - time) << std::endl;
        frameNumber++;
        cv::Mat annotateFrame;
        // for (int i = 8; i>0;i--) {
        //     annotateFrame = videoStreamer->getFrame();
        // }
        if (annotateFrame.data != NULL) {
            std::cout << "In annotate" << std::endl << std::flush;
            cv::resize(annotateFrame, annotateFrame,
                cv::Size((int)(annotateFrame.cols * 0.25), (int)(annotateFrame.rows * 0.25)));
            bgSubAnalytics.annotate(annotateFrame);
            faceAnalytics.annotate(annotateFrame);
            cv::imshow("Display window", annotateFrame);
            cv::waitKey(2);
        }
    }
}

int main(int argc, char *argv[]) {
    setDeveloperVariables(developerName);
    std::cout << storagePath << std::endl;
    debugMsg("Camera Initialized\n");
    std::string filePath;
    if (argc > 1) {
        rtspURL = argv[1];
        filePath = argv[1];
        if (argc >2) {
            storagePath = argv [2];
        }
        debugMsg("Developer Name " + developerName);
        debugMsg("RTSP URL "+rtspURL);
    }
    initDebug(isDebug, storagePath);

    // std::cout << "Hello World" << std::endl;

    // Use VLC IP camera feed
    // LibvlcStreamer *cam = new LibvlcStreamer(rtspURL);
    // std::thread cam1(&LibvlcStreamer::startStream, cam);

    // Use webcam Streamer
    // WebcamStreamer *cam = new WebcamStreamer();
    // std::thread cam1(&WebcamStreamer::startStream,cam);

    // Use file Streamer
     FileStreamer *cam = new FileStreamer(filePath);
     if (!cam->isStreaming()) {
        std::cout << "Unable to open file for reading" << std::endl;
        debugMsg("Unable to open file for reading");
        return 0;
     }
     std::cout << "File Frame rate" << cam->getFileFPS() << std::endl;

    // Use FFMPEG Streamer
    // FfmpegStreamer *cam = new FfmpegStreamer(rtspURL);
    // std::thread cam1(&FfmpegStreamer::startStream, cam);

    while (!cam->isStreaming()) {}

    std::cout << "camera running now " << cam->getFrame().size() << std::endl;

    aiSaac::AiSaacSettings aiSaacSettings =
        aiSaac::AiSaacSettings(storagePath, "config1");
    aiSaacSettings.setFaceStorageMode(true);
    analyticsThread(cam, aiSaacSettings);
}

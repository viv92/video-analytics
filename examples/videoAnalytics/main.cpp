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

aiSaac::BgSubAnalytics *bgSubAnalytics = NULL;
aiSaac::FaceAnalytics *faceAnalytics = NULL;
aiSaac::HeadAnalytics *headAnalytics = NULL;
aiSaac::HumanAnalytics *humanAnalytics = NULL;
aiSaac::ObjectAnalytics *objectAnalytics = NULL;
aiSaac::SceneAnalytics *sceneAnalytics = NULL;
aiSaac::TextAnalytics *textAnalytics = NULL;
aiSaac::VehicleAnalytics *vehicleAnalytics = NULL;
aiSaac::NSFWAnalytics *nsfwAnalytics = NULL;
// aiSaac::Summarization *summarization;

// std::vector<cv::Rect> localizedTextRectangles;
// std::vector<cv::Rect> localizedVehicleRectangles;

void analyticsThread(aiVideoStreamer *videoStreamer, aiSaac::AiSaacSettings &aiSaacSettings) {
    // bgSubAnalytics = new aiSaac::BgSubAnalytics(aiSaacSettings);
    // textAnalytics = new aiSaac::TextAnalytics(aiSaacSettings);
    // objectAnalytics = new aiSaac::ObjectAnalytics(aiSaacSettings);
    // sceneAnalytics = new aiSaac::SceneAnalytics(aiSaacSettings);
    // nsfwAnalytics = new aiSaac::NSFWAnalytics(aiSaacSettings);
    faceAnalytics = new aiSaac::FaceAnalytics(aiSaacSettings);
    // vehicleAnalytics = new aiSaac::VehicleAnalytics(aiSaacSettings);
    cv::Mat currentFrame;
    int frameNumber = 0;
    while (true) {
        currentFrame = videoStreamer->getFrame();
        // bgSubAnalytics->track(currentFrame, frameNumber);
        // vehicleAnalytics->localize(currentFrame, frameNumber, localizedVehicleRectangles);

        // textAnalytics->localize(currentFrame, frameNumber, localizedTextRectangles);
        // for (int i = 0; i < localizedTextRectangles.size(); i++) {
        //     // std::cout << "got localizations" << std::endl;
        //     std::string text =
        //         textAnalytics->recognize(currentFrame(localizedTextRectangles[i]));
        // }
        // textAnalytics->track(currentFrame, frameNumber);
        // objectAnalytics->track(currentFrame, frameNumber);
        // sceneAnalytics->track(currentFrame, frameNumber);
        // nsfwAnalytics->detect(currentFrame);
        // textAnalytics->recognize();
        faceAnalytics->track(currentFrame, frameNumber);
        // faceAnalytics->recognize();
        for (int i = 0; i < faceAnalytics->blobContainer.size(); i++) {
            std::cout << "blob: " << faceAnalytics->blobContainer[i].ID << std::endl;
            std::cout << faceAnalytics->blobContainer[i].faceImages[faceAnalytics->blobContainer[i].faceImages.size() - 1].data << std::endl;
        }
        frameNumber++;
    }
}

int main(int argc, char *argv[]) {
    debugMsg("Camera Initialized\n");
    if (argc > 1) {
        rtspURL = argv[1];
        debugMsg("Developer Name " + developerName);
        debugMsg("RTSP URL "+rtspURL);
    }
    // std::cout << "Hello World" << std::endl;

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

    aiSaac::AiSaacSettings aiSaacSettings = aiSaac::AiSaacSettings(storagePath + "/aiSaacSettingsconfig1.json");
    std::thread analyticsThreadInstance1(analyticsThread, cam, aiSaacSettings);
    // analyticsThread(cam, aiSaacSettings);

    cv::namedWindow("Display window", CV_WINDOW_AUTOSIZE);
    cv::Mat annotatedFrame;
    while (true) {
        annotatedFrame = cam->getFrame();
        // bgSubAnalytics->annotate(annotatedFrame);
        if (bgSubAnalytics) {
            bgSubAnalytics->annotate(annotatedFrame);
        }
        if (objectAnalytics) {
            objectAnalytics->annotate(annotatedFrame);
        }
        if (sceneAnalytics) {
            sceneAnalytics->annotate(annotatedFrame);
        }
        if (textAnalytics) {
            // if (localizedTextRectangles.size()) {
            //     for (int i = 0; i < localizedTextRectangles.size(); i++) {
            //         cv::rectangle(annotatedFrame,
            //             localizedTextRectangles[i],
            //             cv::Scalar(0, 128, 0), 2);
            //     }
            // }
            textAnalytics->annotate(annotatedFrame);
        }
        if (vehicleAnalytics) {
            // if (localizedVehicleRectangles.size()) {
            //     for (int i = 0; i < localizedVehicleRectangles.size(); i++) {
            //         cv::rectangle(annotatedFrame,
            //             localizedVehicleRectangles[i],
            //             cv::Scalar(0, 128, 0), 2);
            //     }
            // }
        }
        if (faceAnalytics) {
            faceAnalytics->annotate(annotatedFrame);
        }
        if (annotatedFrame.data != NULL) {
            cv::imshow("Display window", annotatedFrame);
            cv::waitKey(10);
        }
    }
}

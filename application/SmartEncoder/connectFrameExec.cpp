//
// Created by aitoe on 28/09/2017
//

#include <iostream>
#include <cv.h>
#include <opencv2/highgui.hpp>
#include "../../tools/videoSources/libvlcStreamer.h"
#include "../../tools/videoSources/aiVideoStreamer.h"
#include "../../tools/logAlert/logAlert.h"

int main(int argc, char const *argv[]) {
    if (argc < 3) {
      std::cout << "Usage: ./connectFrameExec [cameraRTSP] [cameraNum]" << std::endl;
      return -1;
    }

    logAlert logger = logAlert();

    std::string cameraRTSP = argv[1];
    std::string cameraNum = argv[2];
    std::string cameraOutputString = cameraNum + ".jpg";
    cv::Mat outputFrame = cv::Mat::zeros(180, 320, CV_8UC3);

    logger.sendLogAlert("Initialised variables", "Trivial", "connectFrameExec");

    LibvlcStreamer *rtspStreamer = new LibvlcStreamer(cameraRTSP, 20);
    std::thread vlcStreamer(&LibvlcStreamer::startStream, rtspStreamer);

    while (!rtspStreamer->isStreaming()) {
        // Wait till stream is started.
    }

    logger.sendLogAlert("Discarding few frames", "Trivial", "connectFrameExec");
    for (int i = 0; i < 10; i++) {
        outputFrame = rtspStreamer->getFrame();
    }

    outputFrame = rtspStreamer->getFrame();
    cv::resize(outputFrame, outputFrame, cv::Size(320, 180));
    cv::imwrite(cameraOutputString, outputFrame);
    logger.sendLogAlert("connectFrame written: " + cameraOutputString, "Important", "connectFrameExec");

    rtspStreamer->stopStream();
    vlcStreamer.join();
    delete rtspStreamer;
    return 0;
}

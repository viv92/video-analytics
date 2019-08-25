//
// Created by aitoe on 12/09/2017
//

#include <iostream>
#include <cstdlib>
#include <cv.h>
#include <opencv2/highgui.hpp>
#include <thread>
#include "../../tools/videoSources/libvlcStreamer.h"
#include "../../tools/videoSources/aiVideoStreamer.h"
#include "../../tools/logging.h"
#include "../../src/aiSaac.h"
#include "../../tools/daemon.h"
#include "../../tools/videoGrabber/videoGrabber.h"
#include "../../tools/streamingServer/streamingServer.h"
#include "../../tools/streamingServer/commonStream.h"

#ifdef USE_MONGO
#include "../../tools/mongoLink/mongoLink.h"
#else
#include "../../tools/mongoLink/serialLink.h"
#endif //USE_MONGO

const int max_length = 5120;
std::string delimiter = "*@*";

std::string outputFolderPath;
std::string aiSaacSettingsPath;
int port;

void initiateDaemonController(std::string cameraIP, std::string cameraStreamPort, socket_ptr sock) {
    std::cout << "Starting DaemonController" << std::endl;

    aiSaac::AiSaacSettings* aiSaacSettings = new aiSaac::AiSaacSettings(aiSaacSettingsPath);
    aiSaac::BgSubAnalytics* bgSubAnalytics = new aiSaac::BgSubAnalytics(*aiSaacSettings);

    std::string cameraRTSP = "rtsp://admin:aitoe2016@" + cameraIP + ":554";

    LibvlcStreamer *rtspStreamer = new LibvlcStreamer(cameraRTSP);
    std::thread vlcStreamer(&LibvlcStreamer::startStream, rtspStreamer);

    while (!rtspStreamer->isStreaming()) {
        // Wait till stream is started.
    }

    cv::Mat currentFrame, motionDetectFrame;
    cv::Size S = rtspStreamer->getFrame().size();
    std::cout << "Width of frame: " << S.width << std::endl;
    int frameNumber = 0;
    int FPS = 20;
    currentFrame = rtspStreamer->getFrame();
    int procFPS = aiSaacSettings->getProcFPS();

    if ( procFPS < 1 ) {
        procFPS = 1;
    } else if ( procFPS > FPS ) {
        procFPS = FPS;
    }

    outputFolderPath = outputFolderPath + "/" +  cameraIP;
    std::string folderCreationCommand = "mkdir -p " + outputFolderPath;
    system(folderCreationCommand.c_str());

    videoGrabber vidGrab = videoGrabber(FPS, S, outputFolderPath);
    commonStream* comStream = new commonStream();
    streamingServer streamServer = streamingServer(comStream, atoi(cameraStreamPort.c_str()));
    streamServer.startServer();

    while (true) {
        currentFrame = rtspStreamer->getFrame();
        comStream->takeFrame(currentFrame);
        if( currentFrame.data == NULL || currentFrame.empty() || !rtspStreamer->isStreaming()) {
            break;
        }
        frameNumber++;
        if (frameNumber % (FPS / procFPS) == 0) {
            cv::resize(currentFrame, motionDetectFrame, cv::Size(480, 360));
            if (bgSubAnalytics->detect(motionDetectFrame, frameNumber)) {
                std::cout << "Motion detected in frame: " << frameNumber << std::endl;
                vidGrab.acceptFrame(currentFrame, frameNumber);
            }
        }
    }
    streamServer.stopServer();
    delete comStream;
    delete rtspStreamer;
    delete bgSubAnalytics;
    delete aiSaacSettings;
}

void session(socket_ptr sock) {
    char data[max_length];
    memset(data, '\0', max_length);

    boost::system::error_code error;
    size_t length = sock->read_some(boost::asio::buffer(data), error);
    std::string message(data);

    if (!message.empty()) {
        std::cout << "Message: " << message << std::endl;

        int cameraStreamPortPos = message.find(delimiter);
        int cameraSettingsPos = message.find(delimiter, cameraStreamPortPos + 1);

        std::string cameraIP = message.substr(0, cameraStreamPortPos);
        std::string cameraSettingsPath = message.substr(cameraSettingsPos + delimiter.length());
        std::string cameraStreamPort = message.substr(cameraStreamPortPos + delimiter.length(), message.length() - cameraIP.length() - delimiter.length() - cameraSettingsPath.length() - delimiter.length());

        std::cout << "Camera IP: " << cameraIP << std::endl;
        std::cout << "Camera Port: " << cameraStreamPort << std::endl;
        std::cout << "Camera Settings Path: " << cameraSettingsPath << std::endl;
        initiateDaemonController(cameraIP, cameraStreamPort, sock);
    }
}

int main(int argc, char const *argv[]) {
    if ( argc < 3 ) {
        std::cout << "Usage: ./daemonControllerEncoder [outputFolderPath] [aiSaacSettingsPath] [port]" << std::endl;
        return -1;
    }

    outputFolderPath = argv[1];
    aiSaacSettingsPath = argv[2];
    port = std::atoi(argv[3]);

    boost::asio::io_service io_service;
    aiSaac::Daemon::server(session, io_service, port);

    return 0;
}

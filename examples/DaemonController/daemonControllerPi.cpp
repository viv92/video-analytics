//
// Created by aitoe on 22/08/2017
//

#include <iostream>
#include <cstdlib>
#include <cv.h>
#include <opencv2/highgui.hpp>
//#include <raspicam/raspicam_cv.h>
#include <boost/thread.hpp>
#include "../../src/aiSaac.h"
#include "../../tools/logging.h"
#include "../../tools/daemon.h"
#include "../../tools/videoGrabber/videoGrabber.h"

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

void acceptVideoGrabberFrame (videoGrabber &vidGrab, cv::Mat frame) {
    std::thread t(&videoGrabber::acceptFrame, vidGrab, frame);
    std::cout << "Frame accepted" << std::endl;
}

int initiateDaemonController () {
    int FPS = 20;
    int chunkFrameLimit = 10000;
    int frameCount = 0;
    cv::Size S = cv::Size(640,480);

    cv::Mat currentFrame;

    aiSaac::AiSaacSettings *aiSaacSettings = new aiSaac::AiSaacSettings(aiSaacSettingsPath);
    aiSaac::BgSubAnalytics *bgSubAnalytics = new aiSaac::BgSubAnalytics(*aiSaacSettings);
    // aiSaac::HumanAnalytics *humanAnalytics = new aiSaac::HumanAnalytics(aiSaacSettings);
    // aiSaac::HeadAnalytics *headAnalytics = new aiSaac::HeadAnalytics(aiSaacSettings);

    // mongoLink *mongoObject = new mongoLink(FPS);

    videoGrabber vidGrab = videoGrabber(chunkFrameLimit, FPS, S, "storageDir/outputFiles");

    //raspicam::RaspiCam_Cv *cam = new raspicam::RaspiCam_Cv();
    cv::VideoCapture *cam = new cv::VideoCapture(0);
    cam->set(CV_CAP_PROP_FRAME_WIDTH, 640);
    cam->set(CV_CAP_PROP_FRAME_HEIGHT, 480);
    cam->set(CV_CAP_PROP_FPS, FPS);

    std::cout << "Starting Camera" << std::endl;
    if (!cam->isOpened()) {
        std::cout << "Error opening camera." << std::endl;
        return -1;
    }

    // std::cout << "Stabilizing camera." << std::endl;
    // sleep(3);
    // std::cout << "Camera stabilized." << std::endl;
    std::thread t;
    while (frameCount < 700) {
        // cam->read(currentFrame);
        cam->grab();
        cam->retrieve(currentFrame);
        frameCount++;
        if (bgSubAnalytics->detect(currentFrame, frameCount)) {
            std::cout << "Motion detected in frame: " << frameCount << std::endl;
            acceptVideoGrabberFrame(vidGrab, currentFrame);
            // humanAnalytics->track(currentFrame, frameCount);
            // headAnalytics->track(currentFrame, frameCount);
        }
    }

    cam->release();
    t.join();
    delete cam;
}

void session (socket_ptr sock) {
    char data[max_length];
    memset(data, '\0', max_length);

    boost::system::error_code error;
    size_t length = sock->read_some(boost::asio::buffer(data), error);
    std::string message(data);

    if (!message.empty()) {
        std::cout << "Message: " << message << std::endl;
        initiateDaemonController();
    }
}

int main(int argc, char const *argv[]) {
    if ( argc < 3 ) {
        std::cout << "Usage: ./daemonControllerPi [outputFolderPath] [aiSaacSettingsPath] [port]" << std::endl;
        return -1;
    }

    outputFolderPath = argv[1];
    aiSaacSettingsPath = argv[2];
    port = std::atoi(argv[3]);

    boost::asio::io_service io_service;
    aiSaac::Daemon::server(session, io_service, port);

    return 0;
}

//
// Created by aitoe on 16/10/2017
//

#include <iostream>
#include <cstdlib>
#include <cv.h>
#include <opencv2/highgui.hpp>
#include <thread>
#include <mutex>
#include "../../tools/videoSources/libvlcStreamer.h"
#include "../../tools/videoSources/aiVideoStreamer.h"
#include "../../tools/logging.h"
#include "../../tools/daemon.h"
#include "../../tools/streamingServer/streamingServer.h"
#include "../../tools/streamingServer/commonStream.h"

const int maxCamera = 8;

const int max_length = 1024;
std::string delimiter = "*@*";

bool updateStream = true;
bool isStreaming = false;

int cameraActive[maxCamera];
int streamActive[maxCamera];
std::string cameraRTSP[maxCamera];

std::mutex streamMutex;

void startStreamingServer () {

    LibvlcStreamer* rtspStreamer[maxCamera];
    std::thread vlcStreamer[maxCamera];
    cv::Mat currentFrame[maxCamera];
    cv::Mat tiledFrame = cv::Mat::zeros(1080, 1920, CV_8UC3);
    cv::Mat clearFrame = cv::Mat::zeros(360, 640, CV_8UC3);

    commonStream* comStream = new commonStream();
    streamingServer streamServer = streamingServer(comStream, 5010);
    streamServer.startServer();
    std::cout << "Streaming Server started" << std::endl;

    while (!isStreaming) {
        // Wait for a camera to be connected.
    }

    while (true) {
        while (isStreaming && !updateStream) {
            streamMutex.lock();
            for (int i = 0; i < maxCamera; i++) {
                if (cameraActive[i] == 1) {
                    currentFrame[i] = rtspStreamer[i]->getFrame();
                    cv::resize(currentFrame[i], currentFrame[i], cv::Size(640, 360));
                    switch (i) {
                      case 0: currentFrame[0].copyTo(tiledFrame(cv::Rect(cv::Point(0, 0), currentFrame[0].size())));
                              break;

                      case 1: currentFrame[1].copyTo(tiledFrame(cv::Rect(cv::Point(640, 0), currentFrame[1].size())));
                              break;

                      case 2: currentFrame[2].copyTo(tiledFrame(cv::Rect(cv::Point(1280, 0), currentFrame[2].size())));
                              break;

                      case 3: currentFrame[3].copyTo(tiledFrame(cv::Rect(cv::Point(0, 360), currentFrame[3].size())));
                              break;

                      case 4: currentFrame[4].copyTo(tiledFrame(cv::Rect(cv::Point(640, 360), currentFrame[4].size())));
                              break;

                      case 5: currentFrame[5].copyTo(tiledFrame(cv::Rect(cv::Point(1280, 360), currentFrame[5].size())));
                              break;

                      case 6: currentFrame[6].copyTo(tiledFrame(cv::Rect(cv::Point(0, 720), currentFrame[6].size())));
                              break;

                      case 7: currentFrame[7].copyTo(tiledFrame(cv::Rect(cv::Point(640, 720), currentFrame[7].size())));
                              break;
                    }
                }
            }
            comStream->takeFrame(tiledFrame);
            streamMutex.unlock();
        }

        streamMutex.lock();
        if (updateStream) {
            for (int i = 0; i < maxCamera; i++) {
                if ((cameraActive[i] == 1) && (streamActive[i] == 0)) {
                    std::cout << "Activating stream in gridID: " << (i + 1) << std::endl;
                    rtspStreamer[i] = new LibvlcStreamer(cameraRTSP[i], 20);
                    vlcStreamer[i] = std::thread(&LibvlcStreamer::startStream, rtspStreamer[i]);
                    streamActive[i] = 1;
                }
            }
            std::cout << "Waiting for updated streams to start" << std::endl;
            for (int i = 0; i < maxCamera; i++) {
                if (streamActive[i] == 1) {
                    while (!rtspStreamer[i]->isStreaming()) {
                        // Wait till stream is started.
                    }
                }
            }
            std::cout << "Updated streams started" << std::endl;
            updateStream = false;
        }
        streamMutex.unlock();
    }
    streamServer.stopServer();
    delete comStream;
    // delete rtspStreamer
}

void session(socket_ptr sock) {
    char data[max_length];
    memset(data, '\0', max_length);

    boost::system::error_code error;
    size_t length = sock->read_some(boost::asio::buffer(data), error);
    std::string message(data);

    if (!message.empty()) {
        std::cout << "Message: " << message << std::endl;

        int gridIDPos = message.find(delimiter);
        int cameraRTSPPos = message.find(delimiter, gridIDPos + 1);

        int gridID = std::atoi((message.substr(0, gridIDPos)).c_str());
        std::string cameraURL = message.substr(gridIDPos + delimiter.length());

        std::cout << "Grid ID: " << gridID << std::endl;
        std::cout << "Camera RTSP: " << cameraURL << std::endl;

        streamMutex.lock();
        updateStream = true;
        std::cout << "Updating camera active and RTSP" << std::endl;
        cameraActive[gridID - 1] = 1;
        cameraRTSP[gridID - 1] = cameraURL;
        std::cout << "Updated camera active and RTSP" << std::endl;
        if (!isStreaming) {
            isStreaming = true;
        }
        streamMutex.unlock();
    }
    if (error) {
        throw boost::system::system_error(error); // Some other error.
    }
    std::cout << "Session stuff ended" << std::endl;
}

int main(int argc, char const *argv[]) {

    if ( argc < 2 ) {
        std::cout << "Usage: ./gridStreamer [port]" << std::endl;
        return -1;
    }

    // Initialise variables

    for (int i = 0; i < maxCamera; i++) {
        cameraActive[i] = 0;
        cameraRTSP[i] = "N/A";
    }

    std::thread startStreamingServerThread = std::thread(startStreamingServer);

    int port = std::atoi(argv[1]);

    boost::asio::io_service io_service;
    aiSaac::Daemon::server(session, io_service, port);

    return 0;
}

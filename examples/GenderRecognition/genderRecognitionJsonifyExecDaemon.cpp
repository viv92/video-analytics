/*
    Copyright 2016 AITOE
*/

#include <cv.h>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <dirent.h>
#include <fstream>
#include "../../tools/videoSources/fileStreamer.h"
#include "../../tools/videoSources/aiVideoStreamer.h"
#include "../../tools/logging.h"
#include "../../src/aiSaac.h"
#include "../../tools/daemon.h"
#include "../../src/utils/caffeClassifier.h"

#ifdef USE_MONGO
#include "../../tools/mongoLink/mongoLink.h"
#else
#include "../../tools/mongoLink/serialLink.h"
#endif //USE_MONGO

const int max_length = 1024;
std::string delimiter = "*@*";

aiSaac::CaffeClassifier *caffeClassifier;

int performGenderRecognition(std::string file, std::string aiSaacSettingsPath, std::string resultTableId, socket_ptr sock) {
    aiSaac::AiSaacSettings aiSaacSettings = aiSaac::AiSaacSettings(aiSaacSettingsPath);
    aiSaac::FaceAnalytics faceAnalytics = aiSaac::FaceAnalytics(aiSaacSettings);
    aiSaac::GenderRecognition genderRecognition = aiSaac::GenderRecognition(aiSaacSettings, *caffeClassifier);

    try{
        if ( file.find("mp4") < file.length() ) {
            FileStreamer *fileStreamer = new FileStreamer(file);
            if (!fileStreamer->isStreaming()) {
                debugMsg("Unable to open file for reading");
                return -1;
            }

            cv::Mat currentFrame;
            int frameNumber = 0;
            int totalProcessedFrameNumber = fileStreamer->getNumberOfFrames();
            int FPS = fileStreamer->getFileFPS();
            int procFPS = aiSaacSettings.getProcFPS();
            int height, width;
            mongoLink *mongoObject = new mongoLink(FPS);

            if ( procFPS < 1 ) {
                procFPS = 1;
            } else if ( procFPS > FPS ) {
                procFPS = FPS;
            }

            std::string log_msg;
            char data[max_length];
            memset(data, '\0', max_length);
            while (true) {
                currentFrame = fileStreamer->getFrame();
                if( currentFrame.data == NULL || currentFrame.empty() || !fileStreamer->isStreaming()) {
                    break;
                }
                height = currentFrame.rows;
                width = currentFrame.cols;
                if (frameNumber % (FPS / procFPS) == 0) {
                    faceAnalytics.track(currentFrame, frameNumber);
                    for (int i = 0; i < faceAnalytics.blobContainer.size(); i++) {
                        if (faceAnalytics.blobContainer[i].lastFrameNumber == frameNumber) {
                            faceAnalytics.blobContainer[i].gender =
                                genderRecognition.detect(
                                    faceAnalytics.blobContainer[i].faceImages.back());
                        }
                    }
                    mongoObject->genderAnalyticsMONGO(faceAnalytics.blobContainer, frameNumber, resultTableId);
                }

                if (frameNumber == floor(totalProcessedFrameNumber*0.25)) {
                    log_msg = "AISAAC_LOG: Your video is 25% processed.";
                    strcpy(data, log_msg.c_str());
                    boost::asio::write(*sock, boost::asio::buffer(data, max_length));
                    memset(data, '\0', max_length);
                    std::cout << log_msg << std::endl;
                } else if (frameNumber == floor(totalProcessedFrameNumber*0.50)) {
                    log_msg = "AISAAC_LOG: Your video is 50% processed.";
                    strcpy(data, log_msg.c_str());
                    boost::asio::write(*sock, boost::asio::buffer(data, max_length));
                    memset(data, '\0', max_length);
                    std::cout << log_msg << std::endl;
                } else if (frameNumber == floor(totalProcessedFrameNumber*0.75)) {
                    log_msg = "AISAAC_LOG: Your video is 75% processed.";
                    strcpy(data, log_msg.c_str());
                    boost::asio::write(*sock, boost::asio::buffer(data, max_length));
                    memset(data, '\0', max_length);
                    std::cout << log_msg << std::endl;
                }
                frameNumber++;
            }
            std::string videoLength = std::to_string(FPS * totalProcessedFrameNumber);
            std::cout << "Length of video: " << videoLength << std::endl;
            mongoObject->pushvideoLengthAndFrameSize(videoLength, height, width, resultTableId);

            log_msg = "AISAAC_LOG: Your video is 100% processed.";
            strcpy(data, log_msg.c_str());
            boost::asio::write(*sock, boost::asio::buffer(data, max_length));
            memset(data, '\0', max_length);
            std::cout << log_msg << std::endl;
            delete mongoObject;
            delete fileStreamer;
        } else {
            cv::Mat image = cv::imread(file, CV_LOAD_IMAGE_COLOR);

            if( !image.data ) {
                debugMsg("Unable to open file for reading");
                return -1;
            }

            mongoLink *mongoObject = new mongoLink(1);

            faceAnalytics.track(image, 1);

            std::string log_msg;
            char data[max_length];
            memset(data, '\0', max_length);
            if ( !faceAnalytics.blobContainer.size() ) {
                log_msg = "AISAAC_LOG: No faces were detected in the input image.";
                strcpy(data, log_msg.c_str());
                boost::asio::write(*sock, boost::asio::buffer(data, max_length));
                memset(data, '\0', max_length);
                std::cout << log_msg << std::endl;
            } else {
                for (int i = 0; i < faceAnalytics.blobContainer.size(); i++) {
                    faceAnalytics.blobContainer[i].gender = genderRecognition.detect(image);
                }
                mongoObject->genderAnalyticsMONGO(faceAnalytics.blobContainer, 1, resultTableId);
                log_msg = "AISAAC_LOG: Your image has been processed.";
                strcpy(data, log_msg.c_str());
                boost::asio::write(*sock, boost::asio::buffer(data, max_length));
                memset(data, '\0', max_length);
                std::cout << log_msg << std::endl;
            }
            delete mongoObject;
        }
        return 0;
    } catch (...) {
        return -1;
    }
}

void session(socket_ptr sock) {
    try {
        char data[max_length];
        memset(data, '\0', max_length);

        boost::system::error_code error;
        size_t length = sock->read_some(boost::asio::buffer(data), error);
        std::string message(data);

        if( !message.empty() ) {
            std::cout << "Message: " << message << std::endl;
            int aiSaacSettingsPos = message.find(delimiter);
            int resultTableIdPos = message.find(delimiter, aiSaacSettingsPos + 1);

            std::string filename = message.substr(0, aiSaacSettingsPos);
            std::string aiSaacSettingsPath = message.substr(aiSaacSettingsPos + delimiter.length(), resultTableIdPos - aiSaacSettingsPos - delimiter.length());
            std::string resultTableId = message.substr(resultTableIdPos + delimiter.length());

            std::cout << "Performing gender recognition...\n";
            std::cout << "Filename: " << filename << std::endl;
            std::cout << "aiSaacSettingsPath: " << aiSaacSettingsPath << std::endl;
            std::cout << "resultTableId: " << resultTableId << std::endl;
            performGenderRecognition(filename, aiSaacSettingsPath, resultTableId, sock);
        }

        if (error) {
            throw boost::system::system_error(error); // Some other error.
        }

        std::string log_message = "Gender recognition finished";
        memset(data, '\0', max_length);
        std::cout << log_message << std::endl;
        strcpy(data, log_message.c_str());
        boost::asio::write(*sock, boost::asio::buffer(data, max_length));
    } catch (std::exception& e) {
        std::cerr << "Exception in thread: " << e.what() << "\n";
    }
}

int main( int argc, char *argv[]) {
    if ( argc < 3 ) {
        std::cout << "Usage: ./genderRecognitionJsonifyExecDaemon [aiSaacSettingsPath] [port]";
        return -1;
    }
    std::string aiSaacSettingsPath = argv[1];
    int port = std::atoi(argv[2]);

    boost::asio::io_service io_service;
    // aiSaac::Daemon::daemonize(io_service);

    aiSaac::AiSaacSettings globalAiSaacSettings = aiSaac::AiSaacSettings(aiSaacSettingsPath);
    std::cout << "Creating caffe classifier" << std::endl;
    caffeClassifier = new aiSaac::CaffeClassifier(
        globalAiSaacSettings.getGenderPrototxtPath(),
        globalAiSaacSettings.getGenderCaffemodelPath(),
        globalAiSaacSettings.getGenderMeanFilePath(),
        globalAiSaacSettings.getGenderLabelFilePath());

    aiSaac::Daemon::server(session, io_service, port);
}

/*
    Copyright 2016 AITOE
*/

#include <cv.h>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <dirent.h>
#include <signal.h>
#include <fstream>
#include <string.h>
#include "datk.h"
#include "../../src/aiSaac.h"
#include "../../tools/daemon.h"
#include "../../tools/videoSources/fileStreamer.h"
#include "../../src/utils/caffeClassifier.h"

#ifdef USE_MONGO
#include "../../tools/mongoLink/mongoLink.h"
#else
#include "../../tools/mongoLink/serialLink.h"
#endif //USE_MONGO

const int max_length = 1024;
std::string delimiter = "*@*";

std::map<std::string, aiSaac::CaffeClassifier*> potentialCaffeClassifiers;
std::map<std::string, aiSaac::CustomAnalytics*> customAnalyticsMap;
int port;

int performCustomAnalyticsDetection(std::string file, std::string aiSaacSettingsPath, aiSaac::CustomAnalytics *ca1, std::string resultTableId, std::string resultTableFeature, socket_ptr sock) {
    aiSaac::AiSaacSettings aiSaacSettings = aiSaac::AiSaacSettings(aiSaacSettingsPath);
    try {
        if ( file.find("mp4") < file.length() ) {
            FileStreamer *fileStreamer = new FileStreamer(file);
            if (!fileStreamer->isStreaming()) {
               std::cout << "Unable to open file for reading" << std::endl;
               debugMsg("Unable to open file for reading");
               return -1;
            }
            cv::Mat currentFrame;
            int frameNumber = 0;
            int totalProcessedFrameNumber = fileStreamer->getNumberOfFrames();
            int FPS = fileStreamer->getFileFPS();
            int procFPS = aiSaacSettings.getProcFPS();
            mongoLink *mongoObject = new mongoLink(FPS);

            if ( procFPS < 1 ) {
                procFPS = 1;
            } else if ( procFPS > FPS ) {
                procFPS = FPS;
            }

            std::vector<std::string> labels;
            std::string log_msg;
            char data[max_length];
            memset(data, '\0', max_length);
            while (true) {
                currentFrame = fileStreamer->getFrame();
                if( currentFrame.data == NULL || currentFrame.empty() || !fileStreamer->isStreaming()) {
                    break;
                }
                if (frameNumber % (FPS / procFPS) == 0) {
                    labels.push_back(ca1->detect(currentFrame));
                    // mongoObject->faceAnalyticsMONGO(faceAnalytics.blobContainer, frameNumber, resultTableId, resultTableFeature);
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
                log_msg = "AISAAC_LOG: Your video is 100% processed.";
                strcpy(data, log_msg.c_str());
                boost::asio::write(*sock, boost::asio::buffer(data, max_length));
                memset(data, '\0', max_length);
                std::cout << log_msg << std::endl;
            }
        } else {
            cv::Mat image = cv::imread(file, CV_LOAD_IMAGE_COLOR);

            if( !image.data ) {
                debugMsg("Unable to open file for reading");
                return -1;
            }

            mongoLink *mongoObject = new mongoLink(1);
            std::string log_msg;
            char data[max_length];
            memset(data, '\0', max_length);

            std::string label = ca1->detect(image);

            log_msg = "AISAAC_RESULT: " + label;
            strcpy(data, log_msg.c_str());
            boost::asio::write(*sock, boost::asio::buffer(data, max_length));
            memset(data, '\0', max_length);
            std::cout << log_msg << std::endl;

            // mongoObject->faceAnalyticsMONGO(blobContainerSingle, 1, resultTableId, resultTableFeature);

            log_msg = "AISAAC_LOG: Your image has been processed.";
            strcpy(data, log_msg.c_str());
            boost::asio::write(*sock, boost::asio::buffer(data, max_length));
            memset(data, '\0', max_length);
            std::cout << log_msg << std::endl;
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
            int caSettingsPos = message.find(delimiter, aiSaacSettingsPos + 1);
            int resultTableIdPos = message.find(delimiter, caSettingsPos + 1);
            int resultTableFeaturePos = message.find(delimiter, resultTableIdPos + 1);

            std::string filename = message.substr(0, aiSaacSettingsPos);
            std::string aiSaacSettingsPath = message.substr(aiSaacSettingsPos + delimiter.length(), caSettingsPos - aiSaacSettingsPos - delimiter.length());
            std::string caSettingsPath = message.substr(caSettingsPos + delimiter.length(), resultTableIdPos - caSettingsPos - delimiter.length());
            std::string resultTableId = message.substr(resultTableIdPos + delimiter.length(), resultTableFeaturePos - resultTableIdPos - delimiter.length());
            std::string resultTableFeature = message.substr(resultTableFeaturePos + delimiter.length());

            std::cout << "Performing face recognition...\n";
            std::cout << "Filename: " << filename << std::endl;
            std::cout << "aiSaacSettingsPath: " << aiSaacSettingsPath << std::endl;
            std::cout << "caSettingsPath: " << caSettingsPath << std::endl;
            std::cout << "resultTableId: " << resultTableId << std::endl;
            std::cout << "resultTableFeature: " << resultTableFeature << std::endl;

            aiSaac::CASettings *caSettings = new aiSaac::CASettings(caSettingsPath);
            caSettings->setIsTrained(true);
            aiSaac::CustomAnalytics *ca1;
            if (customAnalyticsMap.count(resultTableFeature) == 1) {
                ca1 = customAnalyticsMap[resultTableFeature];
            }
            else {
                if (potentialCaffeClassifiers.count(caSettings->getCaffeNetworkFilePath()) == 0) {
                    aiSaac::CaffeClassifier *classifier = new aiSaac::CaffeClassifier(
                        caSettings->getCaffeNetworkFilePath(),
                        caSettings->getCaffeTrainedFilePath());
                    potentialCaffeClassifiers[caSettings->getCaffeNetworkFilePath()] =
                        classifier;
                }
                ca1 =
                    new aiSaac::CustomAnalytics(
                        caSettings,
                        potentialCaffeClassifiers[caSettings->getCaffeNetworkFilePath()]
                    );
                ca1->loadDatkModel();
                customAnalyticsMap[resultTableFeature] = ca1;
            }
            performCustomAnalyticsDetection(filename, aiSaacSettingsPath, ca1, resultTableId, resultTableFeature, sock);
        }

        if (error) {
            throw boost::system::system_error(error); // Some other error.
        }

        std::string log_message = "Custom analytics detection finished";
        memset(data, '\0', max_length);
        std::cout << log_message << std::endl;
        strcpy(data, log_message.c_str());
        boost::asio::write(*sock, boost::asio::buffer(data, max_length));
    } catch (std::exception& e) {
        std::cerr << "Exception in thread: " << e.what() << "\n";
    }
}

int main(int argc, char *argv[]) {
    // codeReview(Anurag): argument parser needed
    if (argc < 3) {
        std::cout << "Usage: ./customAnalyticsJsonifyExecDaemon [aiSaacSettingsPath] [port]" << std::endl;
        return -1;
    }

    std::string aiSaacSettingsPath = argv[1];
    port = std::atoi(argv[2]);

    boost::asio::io_service io_service;
    // aiSaac::Daemon::daemonize(io_service);

    std::cout << "Creating new server" << std::endl;
    aiSaac::Daemon::server(session, io_service, port);

    return 0;
}

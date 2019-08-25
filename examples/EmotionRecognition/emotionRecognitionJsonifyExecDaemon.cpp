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
#include "../../tools/mongoLink/mongoLink.h"
#include "../../src/utils/caffeClassifier.h"
const int max_length = 1024;
std::string delimiter = "*@*";
int port;

int performEmotionRecognition(std::string file, std::string aiSaacSettingsPath, std::string resultTableId, std::string isCroppedParam, socket_ptr sock) {
    cv::namedWindow("Display window", CV_WINDOW_AUTOSIZE);
    aiSaac::AiSaacSettings aiSaacSettings = aiSaac::AiSaacSettings(aiSaacSettingsPath);
    aiSaac::FaceAnalytics faceAnalytics = aiSaac::FaceAnalytics(aiSaacSettings);
    aiSaac::EmotionRecognition emotionRecognition = aiSaac::EmotionRecognition(aiSaacSettings);
    try {
        if ( file.find("mp4") < file.length() ) {
            FileStreamer *fileStreamer = new FileStreamer(file);
            if (!fileStreamer->isStreaming()) {
               std::cout << "Unable to open file for reading" << std::endl;
               debugMsg("Unable to open file for reading");
               return -1;
            }
            cv::Mat currentFrame, annotatedFrame;
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
                            //double confidence;
                            //std::string emotionLabel = emotionRecognition.detect(faceAnalytics.blobContainer[i].faceImages.back(),
                            //    confidence);
                            std::string emotionLabel = emotionRecognition.detect(faceAnalytics.blobContainer[i].faceImages.back());
                            //std::cout << faceAnalytics.blobContainer[i].ID << ":" << "confidence: " << confidence << std::endl;
                            //if (confidence >= confidenceThreshold) {
                                faceAnalytics.blobContainer[i].emotion = emotionLabel;
                            //} else {
                                //faceAnalytics.blobContainer[i].emotion = "unassigned";
                            //}
                        }
                    }
                    mongoObject->emotionAnalyticsMONGO(faceAnalytics.blobContainer, frameNumber, resultTableId);
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

                    std::cout << "Annotating frame" << std::endl;
                    annotatedFrame = currentFrame;
                    faceAnalytics.annotate(annotatedFrame);
                    if (annotatedFrame.data != NULL) {
                        cv::imshow("Annotated emotion", annotatedFrame);
                        cv::waitKey(10);
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
        } else {
            bool isCropped;
            cv::Mat image = cv::imread(file, CV_LOAD_IMAGE_COLOR);

            if( !image.data ) {
                debugMsg("Unable to open file for reading");
                return -1;
            }

            if (isCroppedParam == "true")
                isCropped = true;
            else
                isCropped = false;

            mongoLink *mongoObject = new mongoLink(1);
            std::string log_msg;
            char data[max_length];
            memset(data, '\0', max_length);

            if (!isCropped) {
                log_msg = "AISAAC_LOG: Detecting faces";
                strcpy(data, log_msg.c_str());
                boost::asio::write(*sock, boost::asio::buffer(data, max_length));
                memset(data, '\0', max_length);
                std::cout << log_msg << std::endl;
                faceAnalytics.track(image, 1);

                if ( !faceAnalytics.blobContainer.size() ) {
                    log_msg = "AISAAC_LOG: No faces were detected in the input.";
                    strcpy(data, log_msg.c_str());
                    boost::asio::write(*sock, boost::asio::buffer(data, max_length));
                    memset(data, '\0', max_length);
                    std::cout << log_msg << std::endl;
                } else {
                    for (int i = 0; i < faceAnalytics.blobContainer.size(); i++) {
                        faceAnalytics.blobContainer[i].emotion = emotionRecognition.detect(image);
                    }
                    mongoObject->emotionAnalyticsMONGO(faceAnalytics.blobContainer, 1, resultTableId);

                    log_msg = "AISAAC_LOG: Your image has been processed.";
                    strcpy(data, log_msg.c_str());
                    boost::asio::write(*sock, boost::asio::buffer(data, max_length));
                    memset(data, '\0', max_length);
                    std::cout << log_msg << std::endl;
                }
            } else {
                log_msg = "AISAAC_LOG: Detecting faces";
                strcpy(data, log_msg.c_str());
                boost::asio::write(*sock, boost::asio::buffer(data, max_length));
                memset(data, '\0', max_length);
                std::cout << log_msg << std::endl;

                std::vector<aiSaac::FaceBlob> blobContainerSingle;
                aiSaac::FaceBlob imageFace;
                imageFace.ID = 1;
                imageFace.firstFrameNumber = 1;
                imageFace.lastFrameNumber = 1;
                imageFace.frameCount = 1;
                //Code Review (Vivswan): assumption of single face being detected in image
                imageFace.emotion = emotionRecognition.detect(image);
                cv::Rect imageRect(0, 0, image.cols, image.rows);
                imageFace.firstRectangle = imageRect;
                imageFace.lastRectangle = imageRect;
                blobContainerSingle.push_back(imageFace);

                log_msg = "AISAAC_RESULT: " + imageFace.emotion;
                strcpy(data, log_msg.c_str());
                boost::asio::write(*sock, boost::asio::buffer(data, max_length));
                memset(data, '\0', max_length);
                std::cout << log_msg << std::endl;

                mongoObject->emotionAnalyticsMONGO(blobContainerSingle, 1, resultTableId);

                log_msg = "AISAAC_LOG: Your image has been processed.";
                strcpy(data, log_msg.c_str());
                boost::asio::write(*sock, boost::asio::buffer(data, max_length));
                memset(data, '\0', max_length);
                std::cout << log_msg << std::endl;
            }
        }
        return 0;
    } catch (...) {
        std::cerr << "Error\n";
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
            int isCroppedPos = message.find(delimiter, resultTableIdPos + 1);

            std::string filename = message.substr(0, aiSaacSettingsPos);
            std::string aiSaacSettingsPath = message.substr(aiSaacSettingsPos + delimiter.length(), resultTableIdPos - aiSaacSettingsPos - delimiter.length());
            std::string resultTableId = message.substr(resultTableIdPos + delimiter.length(), isCroppedPos - resultTableIdPos - delimiter.length());

            std::string isCroppedParam;
            if(isCroppedPos == -1) {
                isCroppedParam = "false";
            } else {
                isCroppedParam = message.substr(isCroppedPos + delimiter.length());
            }

            std::cout << "Performing emotion recognition...\n";
            std::cout << "Filename: " << filename << std::endl;
            std::cout << "aiSaacSettingsPath: " << aiSaacSettingsPath << std::endl;
            std::cout << "resultTableId: " << resultTableId << std::endl;
            std::cout << "isCroppedParam: " << isCroppedParam << std::endl;

            std::cout << "Starting performEmotionRecognition\n";
            performEmotionRecognition(filename, aiSaacSettingsPath, resultTableId, isCroppedParam, sock);
            std::cout << "performFaceRecognition completed\n";

            std::cout << "Checking for error\n";
            if (error) {
                throw boost::system::system_error(error); // Some other error.
            }
            std::cout << "No error\n";
            std::string log_message = "Emotion recognition finished";
            memset(data, '\0', max_length);
            std::cout << log_message << std::endl;
            strcpy(data, log_message.c_str());
            boost::asio::write(*sock, boost::asio::buffer(data, max_length));
        }
    } catch (std::exception& e) {
        std::cerr << "Exception in thread: " << e.what() << "\n";
    }
}

int main(int argc, char *argv[]) {
    // codeReview(Anurag): argument parser needed
    if (argc < 3) {
        std::cout << "Usage: ./emotionRecognitionJsonifyExecDaemon [aiSaacSettingsPath] [port]" << std::endl;
        return -1;
    }

    std::string aiSaacSettingsPath = argv[1];
    port = std::atoi(argv[2]);

    boost::asio::io_service io_service;
    // aiSaac::Daemon::daemonize(io_service);


    aiSaac::AiSaacSettings globalAiSaacSettings = aiSaac::AiSaacSettings(aiSaacSettingsPath);
    std::cout << "Creating new server" << std::endl;
    aiSaac::Daemon::server(session, io_service, port);

    return 0;
}

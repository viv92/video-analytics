#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <dirent.h>
#include <vector>
#include <fstream>
#include "../../src/FaceAnalytics/faceAnalytics.h"
#include "../../src/aiSaacSettings.h"
#include "../../tools/videoSources/fileStreamer.h"

int main(int argc, char *argv[]) {
    if(argc < 3) {
        std::cout << "Usage: ./faceAnalyticsExample [filePath] [annotatedFilePath] [aiSaacSettingsPath]" << std::endl;
        return -1;
    }

    std::string file = argv[1];
    std::string annotatedVideoFilePath = argv[2];
    aiSaac::AiSaacSettings aiSaacSettings = aiSaac::AiSaacSettings(argv[3]);
    aiSaac::FaceAnalytics faceAnalytics = aiSaac::FaceAnalytics(aiSaacSettings);

    if ( file.find("mp4") < file.length() ) {
        FileStreamer *fileStreamer = new FileStreamer(file);
        if (!fileStreamer->isStreaming()) {
           std::cerr << "Unable to open file for reading" << std::endl;
           return -1;
        }

        cv::Mat currentFrame, annotatedFrame;
        cv::VideoWriter annotatedVideo;
        int frameNumber = 0;
        int totalProcessedFrameNumber = fileStreamer->getNumberOfFrames();
        int FPS = fileStreamer->getFileFPS();
        int procFPS = aiSaacSettings.getProcFPS();
        int height, width;

        if ( procFPS < 1 ) {
            procFPS = 1;
        } else if ( procFPS > FPS ) {
            procFPS = FPS;
        }

        cv::Size S = fileStreamer->getFrameSize();
        annotatedVideo.open(annotatedVideoFilePath, CV_FOURCC('M','J','P','G'), FPS, S, true);

        std::string videoLength = std::to_string(FPS * totalProcessedFrameNumber);
        std::cout << "Length of video: " << videoLength << std::endl;

        while (true) {
            currentFrame = fileStreamer->getFrame();
            if( currentFrame.data == NULL || currentFrame.empty() || !fileStreamer->isStreaming()) {
                break;
            }
            height = currentFrame.rows;
            width = currentFrame.cols;
            if (frameNumber % (FPS / procFPS) == 0) {
                faceAnalytics.track(currentFrame, frameNumber);
                annotatedFrame = currentFrame;
                faceAnalytics.annotate(annotatedFrame);
                if (annotatedFrame.data != NULL) {
                    cv::imshow("Annotated Scene", annotatedFrame);
                    cv::waitKey(10);
                    annotatedVideo.write(annotatedFrame);
                }
            }

            if (frameNumber == floor(totalProcessedFrameNumber*0.25)) {
                std::cout << "Your video is 25% processed." << std::endl;
            } else if (frameNumber == floor(totalProcessedFrameNumber*0.50)) {
                std::cout << "Your video is 50% processed." << std::endl;
            } else if (frameNumber == floor(totalProcessedFrameNumber*0.75)) {
                std::cout << "Your video is 75% processed." << std::endl;
            }

            frameNumber++;
        }

        std::cout << "Your video is 100% processed." << std::endl;
        annotatedVideo.release();
        std::cout << "Annotated video released" << std::endl;
    } else {
        cv::Mat image = cv::imread(file, CV_LOAD_IMAGE_COLOR);
        if( !image.data ) {
            debugMsg("Unable to open file for reading");
            return -1;
        } else {
            cv::imshow("Image", image);
            cv::waitKey(0);
        }

        // aiSaacSettings.setFaceDetectAnalyticsAlgo(faceDetectAlgos[0]);
        // std::cout << "Face analytics algo: " << aiSaacSettings.getFaceDetectAnalyticsAlgo() << std::endl;
        std::cout << "Performing face analytics track" << std::endl;
        faceAnalytics.track(image, 1);
        std::cout << "Track complete" << std::endl;
        std::cout << "Number of faces detected: " << faceAnalytics.blobContainer.size() << std::endl;

        // aiSaacSettings.setFaceDetectAnalyticsAlgo(faceDetectAlgos[1]);
        // std::cout << "Changing algorithm and performing face analytics track again" << std::endl;
        // std::cout << "Face analytics algo: " << aiSaacSettings.getFaceDetectAnalyticsAlgo() << std::endl;
        // faceAnalytics.track(image, 1);
        // std::cout << "Track complete" << std::endl;
        // std::cout << "Number of faces detected now: " << faceAnalytics.blobContainer.size() << std::endl;

        if ( !faceAnalytics.blobContainer.size() ) {
            std::cout << "No faces were detected in the input." << std::endl;
            std::cout << "Trying different algorithm" << std::endl;
        } else {
            std::cout << "Number of faces detected: " << faceAnalytics.blobContainer[0].faceImages.size() << std::endl;
            }
            // std::cout << "Detection complete" << std::endl;
            // faceAnalytics.annotate(image);
            // cv::imshow("Annotated image", image);
            // cv::waitKey(0);

            std::cout << "Your image has been processed." << std::endl;
        }
    return 0;
}

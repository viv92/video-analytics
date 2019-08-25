/*
    Copyright 2017 AITOE
*/

#include <cv.h>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <mutex>
#include <boost/thread.hpp>
#include <dirent.h>
#include <fstream>
#include <string>
#include <cstdio>
#include <unistd.h>
#include "../../src/aiSaac.h"
#include "../../tools/logging.h"

void summarizeThread(aiSaac::AiSaacSettings *aiSaacSettings, std::string sourceVidOutputPath, std::string summVidOutputPath, int chunkNumber) {
    aiSaac::VideoSummarization* videoSummarization = new aiSaac::VideoSummarization(aiSaacSettings, sourceVidOutputPath);
    std::cout << "Calling summarizeStream" << std::endl << std::flush;
    videoSummarization->summarizeStream();
    std::cout << "Called summarizeStream" << std::endl << std::flush;
    videoSummarization->writeSummaryFile(summVidOutputPath + "/summary_" + std::to_string(chunkNumber) + ".avi");
    std::cout << "Summarized chunk " << chunkNumber <<" written to file." << std::endl;
}

int main(int argc, char const *argv[]) {
    if ( argc < 4 ) {
        std::cout << "Usage: ./summarizationPiStream [outputFolderPath] [aiSaacSettingsPath] [summarizationThreshold]" << std::endl;
        return -1;
    }

    std::string outputFolderPath = argv[1];
    std::string aiSaacSettingsPath = argv[2];
    std::string summarizationThreshold = argv[3];

    // cv::VideoCapture cam(0);
    cv::Mat currentFrame;
    // cv::VideoWriter sourceVid;
    std::string sourceVidOutputPath;
    std::string summVidOutputPath;
    int frameCount;
    int FPS = 20;
    int chunkFrameLimit = 1000;
    cv::Size S = cv::Size(640,480);

    boost::thread summThread;

    aiSaac::AiSaacSettings *aiSaacSettings = new aiSaac::AiSaacSettings(aiSaacSettingsPath);
    aiSaacSettings->setSummarizationThreshold(std::atof(summarizationThreshold.c_str()));

    summVidOutputPath = summarizationThreshold;
    std::replace(summVidOutputPath.begin(), summVidOutputPath.end(), '.', '_');
    summVidOutputPath = outputFolderPath + summVidOutputPath;
    std::string command = "mkdir -p \"" + summVidOutputPath +  "\"";
    std::cout << command << std::endl;
    system(command.c_str());

    int temp = 0; // replace with timestamp.
    int c = 0;

	  cv::namedWindow("Display window", CV_WINDOW_AUTOSIZE);

    while (c < 2) {
        frameCount = 0;
        sourceVidOutputPath = outputFolderPath + "source_" + std::to_string(temp) + ".avi";

        cv::VideoCapture *cam = new cv::VideoCapture(0 + CV_CAP_V4L2);
        cam->set(CV_CAP_PROP_FRAME_WIDTH, 640);
        cam->set(CV_CAP_PROP_FRAME_HEIGHT, 480);
	      cam->set(CV_CAP_PROP_FPS, FPS);
        std::cout << "Starting Camera" << std::endl;
        if (!cam->isOpened()) {
            std::cout << "Error opening camera." << std::endl;
            return -1;
        }

        std::cout << "Stabilizing camera." << std::endl;
        sleep(3);
        std::cout << "Camera stabilized." << std::endl;

        cv::VideoWriter *sourceVid = new cv::VideoWriter(sourceVidOutputPath, CV_FOURCC('M','J','P','G'), FPS, S, true);
		    std::cout << "Chunk: " << temp << std::endl;
        while (frameCount < chunkFrameLimit) {
            cam->read(currentFrame);
			      // cv::imshow("Display window", currentFrame);
            // cv::waitKey(10);
			      std::cout << "Chunk: " << temp << " Frame: " << frameCount << std::endl;
            sourceVid->write(currentFrame);
            frameCount++;
        }
        sourceVid->release();
        std::cout << "Chunk released.";
        delete sourceVid;

        cam->release();
        delete cam;

        summThread = boost::thread(summarizeThread, aiSaacSettings, sourceVidOutputPath, summVidOutputPath, temp);

        temp++;
        c++;
    }

    summThread.join();

    return 0;
}

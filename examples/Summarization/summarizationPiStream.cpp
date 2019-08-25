/*
    Copyright 2017 AITOE
*/

#include <cv.h>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <mutex>
#include <dirent.h>
#include <fstream>
#include <string>
// #include <raspicam/raspicam_cv.h>  	//Raspicam library released in 2013
#include <cstdio>
#include <unistd.h>
#include "../../src/aiSaac.h"
#include "../../src/utils/json.hpp"
#include "../../tools/logging.h"

int main(int argc, char const *argv[]) {
    if ( argc < 4 ) {
        std::cout << "Usage: ./summarizationPiStream [outputFolderPath] [aiSaacSettingsPath] [summarizationThreshold]" << std::endl;
        return -1;
    }

    std::string outputFolderPath = argv[1];
    std::string aiSaacSettingsPath = argv[2];
    std::string summarizationThreshold = argv[3];

    // raspicam::RaspiCam_Cv cam;
    cv::VideoCapture cam(0);
    cv::Mat currentFrame;
    cv::VideoWriter sourceVid;
    std::string sourceVidOutputPath;
    std::string summVidOutputPath;
    int frameCount;
    double FPS = 25;
    int chunkFrameLimit = 5000;
    cv::Size S = cv::Size(640,480);

    std::cout << "Starting Camera" << std::endl;
    if (!cam.isOpened()) {
        std::cout << "Error opening camera." << std::endl;
        return -1;
    }

    std::cout << "Stabilizing camera." << std::endl;
    sleep(3);
    std::cout << "Camera stabilized." << std::endl;

    aiSaac::AiSaacSettings *aiSaacSettings = new aiSaac::AiSaacSettings(aiSaacSettingsPath);
    aiSaacSettings->setSummarizationThreshold(std::atof(summarizationThreshold.c_str()));

    summVidOutputPath = summarizationThreshold;
    std::replace(summVidOutputPath.begin(), summVidOutputPath.end(), '.', '_');
    summVidOutputPath = outputFolderPath + summVidOutputPath;
    std::string command = "mkdir -p \"" + summVidOutputPath +  "\"";
    std::cout << command << std::endl;
    system(command.c_str());

    int temp = 0; // replace with timestamp.
	  cv::namedWindow("Display window", CV_WINDOW_AUTOSIZE);
    int c = 0;

    cam.set(CV_CAP_PROP_FRAME_WIDTH, 640);
    cam.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
	cam.set(CV_CAP_PROP_FPS, 25);

    while (c < 2) {
        frameCount = 0;
        sourceVidOutputPath = outputFolderPath + "source_" + std::to_string(temp) + ".avi";
        sourceVid.open(sourceVidOutputPath, CV_FOURCC('M','J','P','G'), 25, S, true);
		std::cout << "Chunk: " << temp << std::endl;
        while (frameCount < chunkFrameLimit) {
            cam >> currentFrame;
			// cv::imshow("Display window", currentFrame);
            // cv::waitKey(10);
			std::cout << "Chunk: " << temp << " Frame: " << frameCount << std::endl;
            sourceVid.write(currentFrame);
            frameCount++;
        }
        sourceVid.release();
        std::cout << "Chunk released.";

        aiSaac::VideoSummarization* videoSummarization = new aiSaac::VideoSummarization(aiSaacSettings, sourceVidOutputPath);
        std::cout << "Calling summarizeStream" << std::endl << std::flush;
        videoSummarization->summarizeStream();
        std::cout << "Called summarizeStream" << std::endl << std::flush;
        videoSummarization->writeSummaryFile(summVidOutputPath + "/summary_" + std::to_string(temp) + ".avi");
        std::cout << "Summarized chunk written to file." << std::endl;
        temp++;
		c++;
    }
    cam.release();
    std::cout << "Camera released." << std::endl;

    return 0;
}

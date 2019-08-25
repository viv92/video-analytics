/*
*	Abstract base class for implementing (not necessarily submodular) set functions. This implements all oracle functions of general set functions.
Author: Rishabh Iyer
Melodi Lab, University of Washington, Seattle
*
*/
#include "aiVideoShotAnalytics.h"
#include <iostream>
#include "Utils/aiTime.h"
#include "Utils/aiColorHist.h"
namespace aisaac {

    aiVideoShotAnalytics::aiVideoShotAnalytics(std::string videopath): aiVideoAnalytics(videopath){
        thresh = 0.8;
        samplingRate = 1;
        compare_method = 0;
        h_bins = 10;
        s_bins = 10;
        startFrameNumbers = std::vector<int>();
        endFrameNumbers = std::vector<int>();
    }

    aiVideoShotAnalytics::aiVideoShotAnalytics(std::string videopath, int fps): aiVideoAnalytics(videopath){
        thresh = 0.8;
        samplingRate = fps;
        compare_method = 0;
        h_bins = 10;
        s_bins = 10;
        startFrameNumbers = std::vector<int>();
        endFrameNumbers = std::vector<int>();
    }

    aiVideoShotAnalytics::~aiVideoShotAnalytics(){
        capture.release();
    }

    void aiVideoShotAnalytics::analyze(){
        getShots();
    }

    void aiVideoShotAnalytics::getShots()
    {
        Mat currFrame;
        Mat prevFrame;
        MatND histcurr;
        MatND histprev;
        int FRAME_RATE = (int) capture.get(CV_CAP_PROP_FPS);
        if( !capture.isOpened() )
            throw "Error when reading steam";
        int frame_count = 0;
        // read the video and compute the histogram at 1 FPS
        int frac = FRAME_RATE/samplingRate;
        startTimes.push_back(getTime(0));
        startFrameNumbers.push_back(0);
        while (true)
        {
            Mat frame;
            bool cap = capture.read(frame);
            // FullVideo.push_back(frame.clone()); // Read the video at 10 FPS
            if (frame_count % frac == 0){
                if(cap == false ||
                    frame.empty() ||
                    frame.data == NULL ||
                    frame.rows == 0 ||
                    frame.cols == 0){
                    std::cout << "Frame Empty" << std::endl
                        << std::flush;
                    break;
                }
                if (frame_count != 0)
                    prevFrame = currFrame.clone();
                else
                    prevFrame = frame;
                currFrame = frame.clone();
                aisaac::colorHist(prevFrame, histprev);
                aisaac::colorHist(currFrame, histcurr);
                float val = compareHist(histcurr, histprev, compare_method);
                // cout << val << "\n";
                if (val < thresh)
                {
                    startTimes.push_back(getTime(frame_count/frac));
                    endTimes.push_back(getTime(frame_count/frac));
                    shotboundaries.push_back(frame_count);

                    startFrameNumbers.push_back((frame_count/frac) * FRAME_RATE);
                    endFrameNumbers.push_back((frame_count/frac) * FRAME_RATE);
                }
            }
            frame_count++;
            // imshow("Video", frame);
            // cv::waitKey(30);
        }
        endTimes.push_back(getTime(frame_count/frac));
        endFrameNumbers.push_back((frame_count/frac) * FRAME_RATE);
    }

    void aiVideoShotAnalytics::outJSON()
    {
        assert(startTimes.size() == endTimes.size());
        std::cout << "{";
        for (int i = 0; i < startTimes.size(); i++)
        {
            std::cout << "[StartTime: " << startTimes[i].hours << ":"
            << startTimes[i].minutes << ":" << startTimes[i].seconds
            << ", EndTime: " << endTimes[i].hours << ":"
            << endTimes[i].minutes << ":" << endTimes[i].seconds
            <<  "]";
        }
        // int n = startTimes.size()-1;
        // std::cout << "[StartTime: " << startTimes[n].hours << ":"
        // << startTimes[n].minutes << ":" << startTimes[n].seconds
        // << ", EndTime: " << endTimes[n].hours << ":"
        // << endTimes[n].minutes << ":" << endTimes[n].seconds
        // <<  "]";
        std::cout << "}";
    }
}

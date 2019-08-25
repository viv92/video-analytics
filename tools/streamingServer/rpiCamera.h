//
// Created by aitoe on 12/09/2017
//

#ifndef RPICAMERA_H
#define RPICAMERA_H

#include <cv.h>
#include <opencv2/highgui.hpp>
#include <unistd.h>
#include <iostream>

class rpiCamera {

public:

    rpiCamera();
    void getFrame();
    std::vector<uchar>* getEncodedFrame();

private:
    cv::VideoCapture* camera;
    cv::Mat frame;
};

#endif // RPICAMERA_H

//
// Created by aitoe on 12/09/2017
//

#include "rpiCamera.h"

rpiCamera::rpiCamera() {
    std::cout << "Initialising Camera" << std::endl;
    this->camera = new cv::VideoCapture(0);
    this->camera->set(CV_CAP_PROP_FRAME_WIDTH, 640);
    this->camera->set(CV_CAP_PROP_FRAME_HEIGHT, 480);
    this->camera->set(CV_CAP_PROP_FPS, 20);
    std::cout << "Camera Initialised" << std::endl;
    std::cout << "Stabilising Camera" << std::endl;
    sleep(2);
    std::cout << "Camera Stabilised" << std::endl;
}

void rpiCamera::getFrame() {
    camera->read(frame);
    frame = frame.clone();
}

std::vector<uchar>* rpiCamera::getEncodedFrame() {
    std::vector<uchar> tempFrame;
    getFrame();
    cv::Mat tempDispFrame = frame;
    std::vector<int> params = std::vector<int>();
    params.push_back(CV_IMWRITE_JPEG_QUALITY);
    params.push_back(30);
    cv::imencode(".jpg", tempDispFrame, tempFrame, params);
    std::vector<uchar> *encodedImg = new std::vector<uchar>(tempFrame.begin(), tempFrame.end());
    return encodedImg;
}

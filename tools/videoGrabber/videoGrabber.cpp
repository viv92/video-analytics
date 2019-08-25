//
// Created by aitoe on 22/08/2017
//

#include "videoGrabber.h"

videoGrabber::videoGrabber(encoderCameraSettings* camSettings, cv::Size frameSize, std::string outputFolderPath, std::string prefix) {
    this->cameraRTSP = camSettings->getCameraRTSP();
    this->FPS = camSettings->getVideoGrabberFPS();
    this->frame = cv::Mat::zeros(this->frameSize.width, this->frameSize.height, CV_8UC3);
    this->frameSize = frameSize;
    this->outputFolderPath = outputFolderPath;
    this->prefix = prefix;
    this->writerReady = false;
    this->videoChunkFrameCount = -1;
    this->startTime = NULL;
    this->endTime = NULL;
    this->logger = logAlert();
    logger.sendLogAlert("Initialised videoGrabber", "Important", "videoGrabber::videoGrabber");
}

videoGrabber::~videoGrabber() {
    delete videoChunk;
    logger.sendLogAlert("Called videoGrabber destructor", "Important", "videoGrabber::~videoGrabber");
}

void videoGrabber::initialize() {
    this->startTime = time(0);
    videoChunk = new cv::VideoWriter(outputFolderPath + "/" + prefix + "-" + formatTime(this->startTime) + ".avi", CV_FOURCC('M','J','P','G'), this->FPS, this->frameSize, true);
    this->writerReady = true;
    this->videoChunkFrameCount = 0;
    this->endTime = time(0);
    logger.sendLogAlert("Initialised videoWriter", "Important", "videoGrabber::initialize");
}

void videoGrabber::releaseVideo() {
    if (this->activeVideoChunk()) {
        this->videoChunk->release();
        convertVideo();
        std::cout << "VIDEO_GRABBER_DATA_START<<<"<< this->outputFolderPath << "/"<< this->prefix << "-" << formatTime(this->startTime) << ".mp4" << "*@1@*" << formatTime(this->startTime) << "*@2@*" << formatTime(this->endTime) << ">>>VIDEO_GRABBER_DATA_END" << std::endl;
        delete this->videoChunk;
        this->writerReady = false;
        logger.sendLogAlert("Released Video", "Important", "videoGrabber::releaseVideo");
    } else {
        logger.sendLogAlert("No Video to Release!", "Important", "videoGrabber::releaseVideo");
    }
}

std::string videoGrabber::acceptFrame(cv::Mat& frame) {
    if (this->activeVideoChunk()) {
        this->frame = frame;
        saveFrame();
    } else {
        initialize();
        acceptFrame(frame);
    }
    return std::string(this->cameraRTSP + "-" + prefix + "-" + formatTime(this->startTime) + ".mp4");
}

void videoGrabber::saveFrame() {
    this->videoChunk->write(this->frame);
    this->videoChunkFrameCount++;
    this->endTime = time(0);
}

std::string videoGrabber::formatTime(std::time_t& time) {
    std::strftime(this->outTime, MAXLEN, "%m%d%Y-%H%M%S", localtime(&time));
    return std::string(this->outTime);
}

void videoGrabber::convertVideo() {
    int frameRate = (this->videoChunkFrameCount / difftime(this->endTime, this->startTime));
    std::string ffmpeg = "ffmpeg -i " + this->outputFolderPath + "/" + prefix + "-" + formatTime(this->startTime) + ".avi -c:v libx264 -pix_fmt yuv420p -movflags faststart " + this->outputFolderPath + "/" + prefix + "-" + formatTime(this->startTime) + ".mp4";
    system(ffmpeg.c_str());
    std::string fileDel = this->outputFolderPath + "/" + prefix + "-" + formatTime(this->startTime) + ".avi";
    std::remove(fileDel.c_str());
    logger.sendLogAlert("Converted videoChunk to mp4", "Trivial", "videoGrabber::convertVideo");
}

bool videoGrabber::activeVideoChunk() {
    if (this->writerReady) {
        return true;
    } else {
        return false;
    }
}

int videoGrabber::videoChunkFrameNumber() {
    return (this->videoChunkFrameCount);
}

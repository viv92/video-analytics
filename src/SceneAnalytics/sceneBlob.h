/*
    Copyright 2016 AITOE
*/

#ifndef SCENE_BLOB_H
#define SCENE_BLOB_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <vector>
#include <string>

namespace aiSaac {
class SceneBlob {
 public:
    int ID;
    std::string label;
    int firstFrameNumber;
    int lastFrameNumber;
    int frameCount;
    std::vector<cv::Mat> frames;
};
}  // namespace aiSaac

#endif  // SCENE_BLOB_H

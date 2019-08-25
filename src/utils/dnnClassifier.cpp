/*
    Copyright 2016 AITOE
*/

#include "dnnClassifier.h"

aiSaac::DNNClassifier::DNNClassifier() {
    this->empty = true;
}

aiSaac::DNNClassifier::DNNClassifier(const std::string& network_file,
    const std::string& trained_file,
    const std::string& mean_file,
    const std::string& label_file,
    float threshold) {
    this->dnnThreshold = threshold;
    cv::String modelPrototxt = network_file;
    cv::String modelCaffemodel = trained_file;
    //Initialize network
    this->dnnNet = cv::dnn::readNetFromCaffe(modelPrototxt, modelCaffemodel);
    if (this->dnnNet.empty())
    {
        std::cerr << "Can't load network by using the following files: " << std::endl;
        std::cerr << "prototxt:   " << modelPrototxt << std::endl;
        std::cerr << "caffemodel: " << modelCaffemodel << std::endl;
    }

    std::ifstream file(label_file);
    if(file.is_open()) {
        std::string label;
        while (!file.eof()) {
            file >> label;
            this->labels.push_back(label);
        }
    }
}

void aiSaac::DNNClassifier::Classify(const cv::Mat &rawFrame,
    std::vector<std::pair<std::string, cv::Rect>> &results) {

    cv::Mat detectionMat = this->Predict(rawFrame);
    for(int i = 0; i < detectionMat.rows; i++) {
        float confidence = detectionMat.at<float>(i, 2);
        if(confidence > this->dnnThreshold) {
          std::pair<std::string, cv::Rect> result;
          size_t objectClass = (size_t)(detectionMat.at<float>(i, 1));
          int xLeftBottom = static_cast<int>(detectionMat.at<float>(i, 3) * rawFrame.cols);
          int yLeftBottom = static_cast<int>(detectionMat.at<float>(i, 4) * rawFrame.rows);
          int xRightTop = static_cast<int>(detectionMat.at<float>(i, 5) * rawFrame.cols);
          int yRightTop = static_cast<int>(detectionMat.at<float>(i, 6) * rawFrame.rows);
          cv::Rect bb((int)xLeftBottom, (int)yLeftBottom,
                      (int)(xRightTop - xLeftBottom),
                      (int)(yRightTop - yLeftBottom));
          result.first = cv::String(this->labels[objectClass]);
          result.second = bb;
          results.push_back(result);
        }
      }
      std::cout << "results.size()" << results.size() << std::endl;
}

cv::Mat aiSaac::DNNClassifier::Predict(const cv::Mat &rawFrame) {
  //Preprocess frame for MobileNet
  cv::Mat inputBlob = this->Preprocess(rawFrame);

  //Set input blob
  this->dnnNet.setInput(inputBlob, "data"); //set the network input

  //Make forward pass
  cv::Mat detection = this->dnnNet.forward("detection_out"); //compute output

  //std::vector<double> layersTimings;
  //double freq = cv::getTickFrequency() / 1000;
  //double time = this->dnnNet.getPerfProfile(layersTimings) / freq;

  cv::Mat detectionMat(detection.size[2], detection.size[3], CV_32F, detection.ptr<float>());
  return detectionMat;
}

cv::Mat aiSaac::DNNClassifier::Preprocess(const cv::Mat &rawFrame) {
  const size_t inWidth = 300;
  const size_t inHeight = 300;
  const float WHRatio = inWidth / (float)inHeight;
  const float inScaleFactor = 0.007843f;
  const float meanVal = 127.5;

  if (rawFrame.channels() == 4)
      cvtColor(rawFrame, rawFrame, cv::COLOR_BGRA2BGR);

  //Prepare blob
  cv::Mat inputBlob = cv::dnn::blobFromImage(rawFrame, inScaleFactor,
                                cv::Size(inWidth, inHeight), meanVal, false); //Convert Mat to batch of images
  return inputBlob;
}

bool aiSaac::DNNClassifier::isEmpty(){
    return this->empty;
}

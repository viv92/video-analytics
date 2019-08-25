/*
    Copyright 2016 AITOE
*/

#include "darknetClassifier.h"

aiSaac::DarknetClassifier::DarknetClassifier() {
    this->empty = true;
}

aiSaac::DarknetClassifier::DarknetClassifier(std::string configFilePath,
    std::string weightFilePath,
    std::string labelFilePath,
    float darknetThreshold) {
    std::cout << "here i am" << std::endl;
#ifndef GPUDARKNET
    std::cout << "NO GPU" << std::endl;
    gpu_index = -1;
#else
    std::cout << "GPU IS PRESENT YAY" << std::endl;
    if(gpu_index >= 0){
        std::cout << "setting cuda device" << std::endl;
        cuda_set_device(gpu_index);
        // std::cout << "gpu device id: " << cuda_get_device() << std::endl;
    }
#endif
    this->empty = false;
    char *configFilePathCstr = new char[configFilePath.length() + 1];
    strcpy(configFilePathCstr, configFilePath.c_str());
    std::cout << "configFilePath: " << configFilePathCstr << std::endl;
    this->net = parse_network_cfg(configFilePathCstr);

    std::cout << "this->net.n: " << this->net.n << std::endl;
    char *weightFilePathCstr = new char[weightFilePath.length() + 1];
    strcpy(weightFilePathCstr, weightFilePath.c_str());
    load_weights(&this->net, weightFilePathCstr);
    set_batch_network(&this->net, 1);
    srand(2222222);

    // std::cout << "this->net size: " << (sizeof(this->net)/sizeof(*this->net)) << std::endl;

    this->detectionLayer = this->net.layers[this->net.n - 1];
    this->darknetThreshold = darknetThreshold;
    std::cout << "this->net.layers[this->net.n - 1].side: " << this->net.layers[this->net.n - 1].side << std::endl;
    std::cout << "this->net.layers[this->net.n - 1].n: " << this->net.layers[this->net.n - 1].n << std::endl;

    // std::cout << "this->detectionLayer.side: " << this->detectionLayer.side << std::endl;

    this->boxes = (box*) calloc(this->detectionLayer.w
        * this->detectionLayer.h
        * this->detectionLayer.n,
        sizeof(box));
    this->probs = (float**) calloc(this->detectionLayer.w
        * this->detectionLayer.h
        * this->detectionLayer.n,
        sizeof(float *));
    for (int i = 0;
        i < this->detectionLayer.w * this->detectionLayer.h * this->detectionLayer.n;
        i++) {
        this->probs[i] = (float*) calloc(this->detectionLayer.classes, sizeof(float));
    }
    std::ifstream file(labelFilePath);
    if(file.is_open()) {
        std::string label;
        while (!file.eof()) {
            file >> label;
            this->labels.push_back(label);
        }
    }

    delete [] configFilePathCstr;
    delete [] weightFilePathCstr;
}

aiSaac::DarknetClassifier::~DarknetClassifier() {
  delete this->boxes;
}

bool aiSaac::DarknetClassifier::isEmpty() {
    return this->empty;
}

float aiSaac::DarknetClassifier::Classify(const cv::Mat &rawFrame,
    std::vector<std::pair<std::string, cv::Rect>> &results) {
    IplImage *iplFrame = new IplImage(rawFrame);
    image rawIm = ipl_to_image(iplFrame);
    rgbgr_image(rawIm);
    image sizedIm = resize_image(rawIm, this->net.w, this->net.h);
    float *X = sizedIm.data;
    clock_t time;
    time = clock();
    network_predict(this->net, X);
    std::cout << "Prediction Time: " << sec(clock() - time) << std::endl;
    if(this->detectionLayer.type == DETECTION){
        get_detection_boxes(this->detectionLayer,
            1,
            1,
            this->darknetThreshold,
            this->probs,
            this->boxes,
            0);
    } else if (this->detectionLayer.type == REGION){
// #ifndef GPUDARKNET
//           std::cout << "NO GPU" << std::endl;
//           get_region_boxes(this->detectionLayer,
//               1,
//               1,
//               this->darknetThreshold,
//               this->probs,
//               this->boxes,
//               0);
// #else
          std::cout << "GPU IS PRESENT YAY" << std::endl;
          get_region_boxes(this->detectionLayer,
              1,
              1,
              this->darknetThreshold,
              this->probs,
              this->boxes,
              0,
              0,
              0.5);
// #endif
    } else {
        std::cout << "Last layer must produce detections" << std::endl;
    }

    std::cout << "this->boxes->length" << (sizeof(this->boxes)/sizeof(*this->boxes)) << std::endl;
    do_nms_sort(boxes,
        probs,
        this->detectionLayer.w * this->detectionLayer.h * this->detectionLayer.n,
        this->detectionLayer.classes,
        0.4);  // 0.4 is nms
    std::cout << "this->detectionLayer.side" << this->detectionLayer.h << std::endl;
    std::cout << "this->detectionLayer.n" << this->detectionLayer.n << std::endl;
    for (int i = 0;
        i < this->detectionLayer.w * this->detectionLayer.h * this->detectionLayer.n;
        i++) {
        int selectedClass = max_index(this->probs[i], this->labels.size());
        if (this->probs[i][selectedClass] > this->darknetThreshold) {
            std::pair<std::string, cv::Rect> result;
            result.first = this->labels[selectedClass];
            int left  = (boxes[i].x - boxes[i].w / 2.) * rawFrame.cols;
            int right = (boxes[i].x + boxes[i].w / 2.) * rawFrame.cols;
            int top   = (boxes[i].y - boxes[i].h / 2.) * rawFrame.rows;
            int bot   = (boxes[i].y + boxes[i].h / 2.) * rawFrame.rows;

            result.second = cv::Rect(cv::Point(left, top), cv::Point(right, bot));
            results.push_back(result);
        }
    }
    std::cout << "results.size()" << results.size() << std::endl;
    free_image(rawIm);
    free_image(sizedIm);
    delete iplFrame;
    return 0;
}

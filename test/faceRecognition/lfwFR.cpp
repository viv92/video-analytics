#include <cv.h>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <thread>
#include <vector>
#include "../../src/aiSaac.h"
#include "../../src/FaceAnalytics/faceRecognition.h"

int main() {
    aiSaac::AiSaacSettings *aiSaacSettings = new aiSaac::AiSaacSettings(
        "/Users/anurag/Documents/projects/git-stuff/temp-stash/aiSaac/build/storageDir",
        "config1");
    aiSaac::FaceAnalytics faceAnalytics(aiSaacSettings);
    aiSaac::FaceRecognition faceRecognition(aiSaacSettings);
}

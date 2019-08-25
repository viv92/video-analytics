#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <dirent.h>
#include <vector>
#include <signal.h>
#include <fstream>
#include "../../src/CustomAnalytics/customAnalytics.h"
#include "datk.h"
#include "../../src/aiSaacSettings.h"
#include "../../tools/videoSources/fileStreamer.h"

volatile sig_atomic_t flag = 1;

void handler(int sig){ // can be called asynchronously
    flag = 0; // set flag
}

int main(int argc, char *argv[]) {
    // codeReview(Anurag): argument parser needed
    if (argc != 2) {
        std::cout << "usage: ./customAnalyticsTrainingTesting "
            << "[caSettingsFilePath]"
            << std::endl;
        return -1;
    }

    signal(SIGINT, handler);

    std::string caSettingsPath = argv[1];
    aiSaac::CASettings *caSettings = new aiSaac::CASettings(caSettingsPath);
    aiSaac::CustomAnalytics *ca1 = new aiSaac::CustomAnalytics(caSettings);
    if(caSettings->getIsTrained()) {
      std::cout <<"--------------Model already trained - loading model" << std::endl;;
      ca1->loadDatkModel();
    } else {
      std::cout <<"Training model" << std::endl;
      ca1->train();
    }
    std::cout << "Testing model " << std::endl;
    ca1->test();
}

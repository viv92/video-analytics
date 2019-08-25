#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <dirent.h>
#include <vector>
#include <signal.h>
#include <fstream>
#include <ctime>
// #include "../../src/aiSaac.h"
#include "../../src/CustomAnalytics/caSettings.h"
#include "../../src/CustomAnalytics/customAnalytics.h"
#include "../../src/aiSaacSettings.h"
#include "../../src/FaceAnalytics/faceAnalytics.h"
#include "../../src/utils/datk/src/datk.h"
#include "../../src/aiSaacSettings.h"
#include "../../tools/videoSources/fileStreamer.h"

volatile sig_atomic_t flag = 1;

void handler(int sig){ // can be called asynchronously
    flag = 0; // set flag
}

int main(int argc, char *argv[]) {
    // codeReview(Anurag): argument parser needed
    if (argc < 5 || argc > 13) {
        std::cout << "usage: ./faceRecognitionExample2 [videoFilePath] [outputFilePath] "
            << "[aiSaacSettingsPath] [caSettingsFilePath] [deploy_network.prototxt] "
            << "[weights.caffemodel] [trainingFeatureJson] [testingFeatureJson] [trainingDataDir] "
            << "[testingDataDir] [labelFilePath] [datkModelPath]" << std::endl;
        return -1;
    }

    signal(SIGINT, handler);

    std::time_t start;
    std::time_t end;

    std::string videoFilePath;
    std::string outputFilePath;
    std::string aiSaacSettingsPath;
    std::string caSettingsPath;
    std::string caffeNetworkPath;
    std::string caffeWeightsPath;
    std::string trainingFeatureJsonPath;
    std::string testingFeatureJsonPath;
    std::string trainingDataDir;
    std::string testingDataDir;
    std::string labelFilePath;
    std::string datkModelPath;

    std::string caSettingsFolderPath;
    std::string caSettingsFileName;
    std::string aiSaacSettingsFolderPath;
    std::string aiSaacSettingsFileName;

    aiSaac::CASettings *caSettings = NULL;
    // aiSaac::AiSaacSettings aiSaacSettings;

    //if(argc >= 5) {
        videoFilePath = argv[1];
        outputFilePath = argv[2];
        aiSaacSettingsPath = argv[3];
        caSettingsPath = argv[4];

        for (int i = caSettingsPath.length() - 1; i >= 0; i--) {
            if (caSettingsPath[i] == '/') {
                caSettingsFolderPath = caSettingsPath.substr(0, i + 1);
                caSettingsFileName = caSettingsPath.substr(i + 1, caSettingsPath.length() - i);
                break;
            }
        }

        caSettings = new aiSaac::CASettings(caSettingsPath);

        if (argc <= 9) {
            std::cout << "hello2" << std::endl;
            caSettings->setIsTrained(true);
            caSettings->setIsTested(true);
        } else {
            caSettings->setIsTrained(false);
            caSettings->setIsTested(false);
        }

        aiSaac::AiSaacSettings aiSaacSettings = aiSaac::AiSaacSettings(aiSaacSettingsPath);

        if (argc >= 9) {
            std::cout << "hello1" << std::endl;
            caffeNetworkPath = argv[5];
            caffeWeightsPath = argv[6];
            trainingFeatureJsonPath = argv[7];
            testingFeatureJsonPath = argv[8];


                std::cout << "hello3" << std::endl;
            caSettings->setMode(0); // set transfer learning
            caSettings->setCaffeNetworkFilePath(caffeNetworkPath);
            caSettings->setCaffeTrainedFilePath(caffeWeightsPath);
            caSettings->setTrainingJsonDataPath(trainingFeatureJsonPath);
            caSettings->setTestingJsonDataPath(testingFeatureJsonPath);

            std::cout << "hello4" << std::endl;

            if (argc == 13) {
                std::cout << "hello5" << std::endl;
                trainingDataDir = argv[9];
                testingDataDir = argv[10];
                labelFilePath = argv[11];
                datkModelPath = argv[12];

                caSettings->setTrainingDataPath(trainingDataDir);
                caSettings->setTestingDataPath(testingDataDir);
                caSettings->setLabelFilePath(labelFilePath);
                caSettings->setDATKSaveModelPath(datkModelPath);
            }
        }
        std::cout << "face recognition on: " << videoFilePath << std::endl;
    // }

    std::cout << "hello6" << std::endl;
	caSettings->setIsTrained(false);
    aiSaac::CustomAnalytics *ca1 = new aiSaac::CustomAnalytics(caSettings);
    // codeReview(Anurag): no need to train if DATK Model is ready
    ca1->train();

    std::cout << "hello7" << std::endl;
    FileStreamer *fileStreamer = new FileStreamer(argv[1]);
    if (!fileStreamer->isStreaming()) {
       std::cout << "Unable to open file for reading" << std::endl;
       debugMsg("Unable to open file for reading");
       return 0;
    }


    std::cout << "hello8" << std::endl;
    std::cout << "File Frame rate" << fileStreamer->getFileFPS() << std::endl;

    while (!fileStreamer->isStreaming()) {}

    std::cout << "hello9" << std::endl;
    std::cout << "camera running now " << fileStreamer->getFrame().size() << std::endl;

    cv::VideoWriter outputFile;
    outputFile.open(outputFilePath,
        // CV_FOURCC('D','I','V','X'),
        CV_FOURCC('M','J','P','G'),//fileStreamer->getFourCC(),
        fileStreamer->getFileFPS(),
        fileStreamer->getFrameSize(),
        true);

    std::cout << "hello10" << std::endl;
    aiSaac::FaceAnalytics *faceAnalytics = new aiSaac::FaceAnalytics(aiSaacSettings);

    std::cout << "hello11" << std::endl;
    cv::Mat currentFrame;
    cv::Mat annotatedFrame;
    int frameNumber = 0;
    cv::namedWindow("Display window", CV_WINDOW_AUTOSIZE);
    int fontFace = cv::FONT_HERSHEY_SIMPLEX;
    double fontScale = 1;
    int thickness = 2;
    int FPS = fileStreamer->getFileFPS();
    int procFPS = FPS;

    std::vector<std::string> labels;
    std::vector<cv::Rect> localizedFaces;

    while (flag) {
        if (!fileStreamer->isStreaming()) {
            std::cout << "not streaming anymore" << std::endl;
            break;
        }
        currentFrame = fileStreamer->getFrame();
        if (frameNumber >= fileStreamer->cap->get(CV_CAP_PROP_FRAME_COUNT)) {
            std::cout << "finished full video" << std::endl;
            break;
        }
        annotatedFrame = currentFrame;
        if (frameNumber % FPS / procFPS == 0) {
            labels.clear();
            localizedFaces.clear();
            start = time(NULL);
            faceAnalytics->localize(currentFrame, localizedFaces);
            end = time(NULL);
            std::cout << "\nTime Taken for localize: " << difftime(end, start) << std::endl;

            for (int i = 0; i < localizedFaces.size(); i++) {
                start = time(NULL);
                std::string label = ca1->detect(currentFrame(localizedFaces[i]));
                end = time(NULL);
                std::cout << "\nTime Taken for detect: " << difftime(end, start) << std::endl;
                labels.push_back(label);
            }
        }

        if (annotatedFrame.data != NULL) {
            std::cout << "Annotating frame " << frameNumber << std::endl << std::flush;
            for (int i = 0; i < localizedFaces.size(); i++) {
                cv::rectangle(annotatedFrame, localizedFaces[i], cv::Scalar(255, 255, 255), 2);
                std::string label = labels[i];
                int baseline = 0;
                cv::Size textSize = cv::getTextSize(label,
                    fontFace,
                    fontScale,
                    thickness,
                    &baseline);
                baseline += thickness;
                cv::Point textOrg(localizedFaces[i].x
                    + (localizedFaces[i].width / 2)
                    - (textSize.width / 2),
                    localizedFaces[i].y - 5);
                if (textOrg.x < 0) {
                    textOrg.x = 0;
                }
                if (textOrg.y < 0) {
                    textOrg.y = 0;
                }
                cv::putText(annotatedFrame,
                    label,
                    textOrg,
                    fontFace,
                    fontScale,
                    cv::Scalar(127, 255, 0),
                    thickness);
            }

            cv::imshow("Annotated Object", annotatedFrame);
            cv::waitKey(10);
            // if (argc > 2) {
                outputFile.write(annotatedFrame);
            // }
        }
        frameNumber++;
    }
    outputFile.release();
}

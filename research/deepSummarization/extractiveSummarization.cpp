#include <iostream>
#include <fstream>
#include <cv.h>
#include <opencv2/highgui.hpp>
#include <time.h>
// #include "aiSaac.h"
#include "../../tools/videoSources/fileStreamer.h"
#include "../../tools/logging.h"
#include "../../src/utils/json.hpp"
#include "../../src/utils/caffeClassifier.h"
#include "datk.h"

int h_bins = 10;
int s_bins = 10;
std::ofstream timings;

void loadData(std::string filePath, nlohmann::json &jsonData) {
    std::ifstream dataFile;
    dataFile.open(filePath);
    dataFile >> jsonData;
    dataFile.close();
}

void constructLabelsVector(std::vector<std::string> &sceneLabels,
    std::vector<std::string> &objectLabels,
    std::vector<datk::Set> &coverSet) {
    std::map<std::string, int> sceneStringToInt;
    std::map<std::string, int> objectStringToInt;

    int sceneLabelNumber = 0;
    int objectLabelNumber = 0;
    for (int i = 0; i < sceneLabels.size(); i++) {
        if (sceneStringToInt.find(sceneLabels[i]) == sceneStringToInt.end()) {
            sceneStringToInt[sceneLabels[i]] = sceneLabelNumber++;
        }
        if (objectStringToInt.find(objectLabels[i]) == objectStringToInt.end()) {
            objectStringToInt[objectLabels[i]] = objectLabelNumber++;
        }

        datk::Set setOfLabels;
        setOfLabels.insert(sceneStringToInt[sceneLabels[i]]);
        setOfLabels.insert(objectStringToInt[objectLabels[i]]);
        coverSet.push_back(setOfLabels);
    }
}

void constructProbVector() {

}

void getSubsetFeatureBasedFunctions(std::vector<std::vector<double>> &featureVectors,
        datk::Set &indices,
        double budget,
        int mode) {
    //for each feature vector i (0 to n), compute its sparse features
    // datk::Set indices;
    std::vector<datk::SparseFeature> sparseFeatures = std::vector<datk::SparseFeature>();
    std::vector<double> featureWeights = std::vector<double>();
    for (int i = 0; i < featureVectors[0].size(); i++) {
        featureWeights.push_back(1);
    }
    for (int i = 0; i < featureVectors.size(); i++) {
        datk::SparseFeature s;
        s.featureIndex = std::vector<int>();
        s.featureVec = std::vector<double>();
        s.index = i;
        for (int j = 0; j < featureVectors[i].size(); j++) {
            if (featureVectors[i][j] != 0) {
                s.featureIndex.push_back(j);
                s.featureVec.push_back(featureVectors[i][j]);
            }
        }
        s.numFeatures = featureVectors[i].size();
        s.numUniqueFeatures = s.featureVec.size();
        sparseFeatures.push_back(s);
    }
    //instantiate Feature Based Functions using sparseFeatures
    datk::FeatureBasedFunctions featBasedFunc(
        sparseFeatures.size(),
        mode,
        sparseFeatures,
        featureWeights);
    //call lazygreedymax with this Feature Based Functions fuction, B, null output set
    clock_t time;
    time = clock();
    datk::lazyGreedyMax(featBasedFunc, budget, indices);
    // std::cout << "~~~~~~~~~~~ FeatureBased " << mode << " Prediction Time: " << (float) (clock() - time)/CLOCKS_PER_SEC << std::endl;
    timings << "Feature Based " << mode << ": " << (float) (clock() - time) / CLOCKS_PER_SEC << '\n';
}

void colorHist(std::vector<cv::Mat>& frames, int startframe, int endframe, cv::Mat& hist) {
    int histSize[] = { h_bins, s_bins };
    // hue varies from 0 to 179, saturation from 0 to 255
    float h_ranges[] = { 0, 180 };
    float s_ranges[] = { 0, 256 };
    const float* ranges[] = { h_ranges, s_ranges };
    int channels[] = { 0, 1 };
    cv::Mat histcurr;
    /// Calculate the histograms for the HSV images
    cv::calcHist(&frames[startframe],
        1,
        channels,
        cv::Mat(),
        hist,
        2,
        histSize,
        ranges,
        true,
        false);
    cv::normalize(hist,
        hist,
        0,
        1,
        cv::NORM_MINMAX,
        -1,
        cv::Mat());
    for (int j = startframe + 1; j <= endframe; j++) {
        calcHist( &frames[j], 1, channels, cv::Mat(), histcurr, 2, histSize, ranges, true, false );
        normalize(histcurr, histcurr, 0, 1, cv::NORM_MINMAX, -1, cv::Mat() );
        hist += histcurr;
    }
    normalize(hist, hist, 0, 1, cv::NORM_MINMAX, -1, cv::Mat());
}

void computeHistKernel(std::string filename,
    int SNP_SIZE,
    std::vector<std::vector<float>> &kernel,
    int procFPS) {
    cv::VideoCapture capture;
    capture.open(filename);
    int framerate = capture.get(CV_CAP_PROP_FPS);
    cv::Mat frame;
    std::vector<cv::Mat> CurrVideo = std::vector<cv::Mat> ();
    if (!capture.isOpened()) {
      return;
    }
    int frame_count = 0;
    cout << "Starting to read video file and get features\n" << flush;
    // read the video
    std::vector<cv::Mat> snippetHist;
    while (frame_count < capture.get(CV_CAP_PROP_FRAME_COUNT)) {
        cv::MatND hist;
        if (frame_count % framerate / procFPS == 0) {
            capture.set(CV_CAP_PROP_POS_FRAMES, frame_count);
            capture >> frame;
            if (frame.empty() || frame.data==NULL) {
                break;
            }
            CurrVideo.push_back(frame.clone());
        }
        if (frame_count % (SNP_SIZE * framerate) == 0) {
            colorHist(CurrVideo, 0, CurrVideo.size() - 1, hist);
            snippetHist.push_back(hist);
            CurrVideo.clear();
        }
        frame_count++;
    }
    capture.release();
    cout << "Done reading video file\n" << flush;
    int snippetHistSize = snippetHist.size();
    std::cout << "snippetHistSize: " << snippetHist.size() << std::endl;

    // computing kernel based on constructed snippets

    // std::vector<std::vector<float>> kernel;
    float max = 0;
    for (int i = 0; i < snippetHist.size(); i++) {
        std::vector<float> currvector;
        for (int j = 0; j < snippetHist.size(); j++) {
            float val = cv::compareHist(snippetHist[i], snippetHist[j], 0);
            if (max < val)
                max = val;
            currvector.push_back(val);
        }
        kernel.push_back(currvector);
    }
    for (int i = 0; i < snippetHist.size(); i++) {
        for (int j = 0; j < snippetHist.size(); j++) {
            // if ((compare_method == 0) || (compare_method == 2) )
            kernel[i][j] = kernel[i][j]/max;
            // else if ((compare_method == 1) || (compare_method == 3) )
                // kernel[i][j] = (max - kernel[i][j])/max;
            // if (compare_method == 0)
            kernel[i][j] = (kernel[i][j] + 1) / 2;
            // kernel[i][j] = pow(kernel[i][j], gamma);
        }
    }
    std::cout << "computed kernel" << std::endl;
}

void normalizeVector(std::vector<double> &featureVector) {
    float max = 0;
    for (int i = 0; i < featureVector.size(); i++) {
        max = (featureVector[i] > max) ? featureVector[i] : max;
    }
    for (int i = 0; i < featureVector.size(); i++) {
        featureVector[i] = featureVector[i] / max;
    }
}

void computeKernel(std::vector<std::vector<double>> &featureVectors,
    std::vector<std::vector<float>> &kernel,
    std::vector<std::vector<float>> &histKernel) {
    double val = 0;
    std::cout << "computing main kernel: " << featureVectors.size() << "=" << histKernel.size() << std::endl;
    double min = std::numeric_limits<double>::max();
    double max = std::numeric_limits<double>::min();
    for (int i = 0; i < featureVectors.size(); i++) {
        std::vector<float> currentVector;
        for (int j = 0; j < featureVectors.size(); j++) {
            val = datk::innerProduct(featureVectors[i], featureVectors[j]);
            // if (histKernel.size()) {
            //   val += histKernel[i][j];
            // }
            max = (val > max) ? val : max;
            min = (val < min) ? val : min;
            currentVector.push_back(val);
        }
        kernel.push_back(currentVector);
    }
    // Normalize kernel matrix:
    double range = max - min;
    for (int i = 0; i < kernel.size(); i++) {
        for (int j = 0; j < kernel.size(); j++) {
            kernel[i][j] = (kernel[i][j] - min) / range;
            if (histKernel.size()) {
              val += histKernel[i][j];
            }
        }
    }

}

// feature mode: 0 - colorhist, 1 - scene + object, 2 - object + scene + colorhist
int main (int argc, char *argv[]) {
    if (argc < 3) {
        std::cout << "Usage: ./deepSummarization <video_to_summarize> <summary_destination_path> <mode_of_operation> <%> <snippet_size> <processing_fps> <path_to_json_folder> <feature_mode>" << std::endl;
        return 0;
    }
    // FileStreamer *fileStreamer = new FileStreamer(argv[1]);
    std::string extractiveSummaryDir = argv[2];
    std::string command = "mkdir -p " + extractiveSummaryDir;
    system(command.c_str());
    std::string summaryDestinationPath = extractiveSummaryDir + "/summary.avi";
    std::string keyFramesPath = extractiveSummaryDir + "/keyFrames";
    command = "mkdir -p " + keyFramesPath;
    system(command.c_str());
    std::string timingsPath = extractiveSummaryDir + "/timings.txt";
    timings.open(timingsPath);

    // int budgetPerformance = std::atoi(argv[3]);
    int modeOfOperation = std::atoi(argv[3]);
    std::cout << "Frame Rate: " << std::atoi(argv[6]) << std::endl;
    int snippetSize = std::atoi(argv[5]) * std::atoi(argv[6]);
    std::cout << "snippetSize: " << snippetSize << std::endl;


    // std::cout << "Waiting for file to stream ...";
    // while (!fileStreamer->isStreaming()) {
    //     std::cout << ".";
    // }
    // std::cout << "Ready and streaming!" << std::endl;

    nlohmann::json sceneJson, ffObjectJson;
    std::string jsonFolderPath = argv[7];
    int featureMode = std::atoi(argv[8]);

    std::cout << "Loading scene data" << std::endl;
    loadData(jsonFolderPath + "/SceneSchema.json", sceneJson);
    std::cout << "Loaded scene data" << std::endl;

    //Loading full frame object data
    std::cout << "Loading FFObject data" << std::endl;
    loadData(jsonFolderPath + "/FFObjectSchema.json", ffObjectJson);
    std::cout << "Loaded FFObject data" << std::endl;

    // std::cout << "here i am 1" << std::endl;
    cv::Mat currentFrame;
    std::vector<std::vector<double>> finalFeatureVectors;
    // std::vector<cv::Mat> snippetBuffer;
    std::vector<int> frameNumbers;
    // int frameNumber = 0;
    int snippetNumber = 0;
    bool videoEnded = false;


    std::cout << "Going through extracted features" << std::endl;

    std::vector<double> snippetFeatureVector;

    if ( sceneJson["frameNumber"].size() != ffObjectJson["frameNumber"].size() ) {
        std::cout << "Unequal number of frames in sceneJson and ffObjectJson";
        return -1;
    }

    for ( int i=0; i < sceneJson["frameNumber"].size() - (sceneJson["frameNumber"].size() % snippetSize) ; i++) {
        std::vector<double> sceneFeatureVector = sceneJson["features"][i].get<std::vector<double>>();
        std::vector<double> ffObjectFeatureVector = ffObjectJson["features"][i].get<std::vector<double>>();
        std::vector<double> massiveFeatureVector = sceneFeatureVector;
        massiveFeatureVector.insert(massiveFeatureVector.end(), ffObjectFeatureVector.begin(), ffObjectFeatureVector.end());
        normalizeVector(massiveFeatureVector);
        if (snippetFeatureVector.empty()) {
            snippetFeatureVector = massiveFeatureVector;
        } else {
            for (int k = 0; k < snippetFeatureVector.size(); k++) {
                // std::cout << snippetFeatureVector[k] << " + " << massiveFeatureVector[k] << " = ";
                snippetFeatureVector[k] += massiveFeatureVector[k];
                // std::cout << snippetFeatureVector[j] << std::endl;
            }
        }
        if ( (i+1) % snippetSize == 0 ) {
            std::cout << "*********Averaging feature vector*********" << std::endl;
            for (int k = 0; k < snippetFeatureVector.size(); k++) {
                // std::cout << snippetFeatureVector[i] << " / " << snippetBuffer.size() << " = ";
                snippetFeatureVector[k] = snippetFeatureVector[k] / (double) snippetSize;
                // std::cout << snippetFeatureVector[i] << std::endl;
            }
            finalFeatureVectors.push_back(snippetFeatureVector);
            snippetFeatureVector.clear();
            snippetNumber++;
        }
    }
    snippetFeatureVector.clear();
    int j = sceneJson["frameNumber"].size() - (sceneJson["frameNumber"].size() % snippetSize);
    if (sceneJson["frameNumber"].size() % snippetSize) {
        while( j < sceneJson["frameNumber"].size() ) {
            std::vector<double> sceneFeatureVector = sceneJson["features"][j].get<std::vector<double>>();
            std::vector<double> ffObjectFeatureVector = ffObjectJson["features"][j].get<std::vector<double>>();
            std::vector<double> massiveFeatureVector = sceneFeatureVector;
            massiveFeatureVector.insert(massiveFeatureVector.end(), ffObjectFeatureVector.begin(), ffObjectFeatureVector.end());
            normalizeVector(massiveFeatureVector);
            if (snippetFeatureVector.empty()) {
                snippetFeatureVector = massiveFeatureVector;
            } else {
                for (int k = 0; k < snippetFeatureVector.size(); k++) {
                    // std::cout << snippetFeatureVector[k] << " + " << massiveFeatureVector[k] << " = ";
                    snippetFeatureVector[k] += massiveFeatureVector[k];
                    // std::cout << snippetFeatureVector[j] << std::endl;
                }
            }
            j++;
        }
        std::cout << "*********Averaging feature vector (remainder)*********" << std::endl;
        for (int k = 0; k < snippetFeatureVector.size(); k++) {
            // std::cout << snippetFeatureVector[i] << " / " << snippetBuffer.size() << " = ";
            snippetFeatureVector[k] = snippetFeatureVector[k] / (double) snippetSize;
            // std::cout << snippetFeatureVector[i] << std::endl;
        }
        finalFeatureVectors.push_back(snippetFeatureVector);
        snippetFeatureVector.clear();
        snippetNumber++;
    }

    std::cout << finalFeatureVectors.size() << std::endl;
    double budget = ceil((std::atoi(argv[4]) * finalFeatureVectors.size()) / 100);
    std::cout << "Budget: " << budget << std::endl;

    datk::Set indices;
    clock_t time;

    if (modeOfOperation == 5) { // use set covers
        std::vector<datk::Set> coverSet = std::vector<datk::Set>();
        std::vector<std::string> sceneLabels = sceneJson["labels"];
        std::vector<std::string> objectLabels = ffObjectJson["labels"];
        constructLabelsVector(sceneLabels, objectLabels, coverSet);
        timings << "Set Cover time: ";
        datk::SetCover setCover(coverSet.size(), coverSet);
        time = clock();
        datk::lazyGreedyMax(setCover, budget, indices);
        timings << (float) (clock() - time) / CLOCKS_PER_SEC << '\n';
    } else if (modeOfOperation == 6) { // use probabilistic set cover
        std::cout << "initializing probabilistic set cover" << std::endl;
        std::cout << "finalFeatureVectors.size() = " << finalFeatureVectors.size() << std::endl;
        std::cout << "finalFeatureVectors[0].size() = " << finalFeatureVectors[0].size() << std::endl;
        datk::ProbabilisticSetCover probabilisticSetCover(finalFeatureVectors.size(), finalFeatureVectors[0].size(), finalFeatureVectors);
        std::cout << "done initializing set cover" << std::endl;
        timings << "Probabilistic Set Cover Time: ";
        time = clock();
        std::cout << "starting lazy max" << std::endl;
        datk::lazyGreedyMax(probabilisticSetCover, budget, indices);
        std::cout << "done with lazy max" << std::endl;
        timings << (float) (clock() - time) / CLOCKS_PER_SEC << '\n';
    } else {
        std::vector<std::vector<float>> kernel;
        std::vector<std::vector<float>> histKernel;
        if (featureMode == 0 || featureMode == 2) {
          timings << "Computing hist kernel time: ";
          time = clock();
          computeHistKernel(argv[1], snippetSize, histKernel, std::atoi(argv[6]));
          timings << (float) (clock() - time) / CLOCKS_PER_SEC << '\n';
        }

        if (featureMode == 1 || featureMode == 2) {
          timings << "Computing main kernel time:";
          time = clock();
          computeKernel(finalFeatureVectors, kernel, histKernel);
          timings << (float) (clock() - time) / CLOCKS_PER_SEC << '\n';
        } else {
          kernel = histKernel;
        }

        timings << "Ground Set: " << kernel.size() << '\n';
        timings << "Budget Set: " << budget << '\n';


        if (modeOfOperation == 0) {
            timings << "Facility Location time: ";
            time = clock();
            datk::FacilityLocation facilityLocation(kernel.size(), kernel);
            datk::lazyGreedyMax(facilityLocation, budget, indices);
            timings << (float) (clock() - time) / CLOCKS_PER_SEC << '\n';
        } else if (modeOfOperation == 1) {
            timings << "Disparity Min time: ";
            time = clock();
            datk::DisparityMin dispMin(kernel.size(), kernel);
            //call lazygreedymax with this facility location fuction, B, null output set
            datk::naiveGreedyMax(dispMin, budget, indices, 0, false, true);
            timings << (float) (clock() - time) / CLOCKS_PER_SEC << '\n';
        } else {
            std::cout << "Running feature based function" << std::endl;
            // modeOfOperation - 1 since getSubsetFeatureBasedFunctions needs 1, 2, 3 modes.
            getSubsetFeatureBasedFunctions(finalFeatureVectors, indices, budget, modeOfOperation - 1);
        }
    }
    std::cout << "Indices: " << indices.size() << std::endl;
    std::set<int> keyPointIndices = set<int>();
    for (datk::Set::iterator it = indices.begin(); it!=indices.end(); it++) {
        keyPointIndices.insert(*it);
    }



    // now store the video
    FileStreamer *fileStreamer = new FileStreamer(argv[1]);
    cv::VideoWriter summarizedVideo;
    cv::Size S = fileStreamer->getFileFrameSize();
    cv::Size finalS;
    if(S.width>= S.height) {
        finalS.height = S.width;
        finalS.width = S.width;
    }
    else {
        finalS.height = S.height;
        finalS.width = S.height;
    }
    int FPS = fileStreamer->getFileFPS();
    summarizedVideo.open(summaryDestinationPath, CV_FOURCC('M', 'J', 'P', 'G') , FPS, finalS, true);
    cv::Scalar color = cv::Scalar(0,0,0);
    int padV = 0, padH = 0;
    if (S.height < finalS.height) {
        int padSize = finalS.height - S.height;
        padV = padSize/2;
    }
    if (S.width < finalS.width) {
        //TODO pad width
        int padSize = finalS.width - S.width;
        padH = padSize/2;
    }
    std::cout << "Waiting for file to stream ...";
    while (!fileStreamer->isStreaming()) {
        std::cout << ".";
    }
    std::cout << "Ready and streaming!" << std::endl;
    cv::Mat summaryVideoFrames;
    bool stop = false;
    // for (set<int>::iterator it = keyPointIndices.begin(); it!=keyPointIndices.end(); it++) {
    //     std::cout << "keyPoint: " << *it << std::endl;
    //     std::cout << "keyPoint * fileStreamer->getFileFPS(): " << *it * fileStreamer->getFileFPS() << std::endl;
    // }
    std::cout << fileStreamer->cap->get(CV_CAP_PROP_FRAME_COUNT) << std::endl;
    for (set<int>::iterator it = keyPointIndices.begin(); it!=keyPointIndices.end(); it++) {
        std::cout << "keyPoint: " << *it << std::endl;
        std::cout << "frameNumbers[keyPoint] * fileStreamer->getFileFPS: "
            << *it * FPS << std::endl;
        int seekFrame = *it * std::atoi(argv[5]) * FPS;
        std::cout << "Seek frame: " << seekFrame << std::endl;
        fileStreamer->cap->set(CV_CAP_PROP_POS_FRAMES, seekFrame);
        int count = 0;
        int midPoint = std::atoi(argv[5]) * FPS / 2;
        while (count <= std::atoi(argv[5]) * FPS) {
            if (!fileStreamer->cap->read(summaryVideoFrames)) {
                stop = true;
                break;
            } else {
                if (count == midPoint) {
                    cv::imwrite(keyFramesPath + "/" + std::to_string(seekFrame + count) + ".jpg", summaryVideoFrames);
                }
                cv::copyMakeBorder(
                    summaryVideoFrames,
                    summaryVideoFrames,
                    padV,
                    padV,
                    padH,
                    padH,
                    cv::BORDER_CONSTANT,
                    color);
                std::cout << "Writing frame of size r,c "
                    << summaryVideoFrames.rows
                    << " "
                    << summaryVideoFrames.cols
                    << std::endl
                    << std::flush;
                summarizedVideo.write(summaryVideoFrames);
            }
            count++;
        }
        if (stop) {
            break;
        }
    }
    summarizedVideo.release();

    return 0;
}

/*
    Copyright 2016 AITOE
*/

#include "videoSummarization.h"

aiSaac::VideoSummarization::VideoSummarization(AiSaacSettings *aiSaacSettings,
    std::string srcFilePath) {
    this->aiSaacSettings = aiSaacSettings;
    this->srcFilePath = srcFilePath;
    std::cout << "Source File Path " << this->srcFilePath << std::endl;
}

aiSaac::VideoSummarization::~VideoSummarization() {
    std::cout << "VideoSummarization destructor called." << std::endl;
}

void aiSaac::VideoSummarization::summarizeStream() {
    keyPoints.clear();
    std::vector<cv::Mat> snippetHist = std::vector<cv::Mat>();
    int n; // number of frames Code review(Vivswan):or snippets?
    int SNP_SIZE = this->aiSaacSettings->getSummarizationSNPSize();
    int fps;
    cv::Size S; //frame size
    int status = extractFeatures(this->srcFilePath, fps, SNP_SIZE, snippetHist, n, S);
    cv::VideoCapture capture(this->srcFilePath);
    if (status > 0) {
        if (this->aiSaacSettings->getSummarizationStreamAlgo() == "MMR") {
            summarizeStreamMMR(snippetHist,
                n,
                this->aiSaacSettings->getSummarizationThreshold(),
                keyPoints);
        }
        else if (this->aiSaacSettings->getSummarizationStreamAlgo() == "FL") {
            summarizeStreamFL(snippetHist,
                n,
                this->aiSaacSettings->getSummarizationThreshold(),
                keyPoints);
        } else {
            std::cout << "Unknown Stream Summarization Algorithms\n";
        }
    }
    capture.release();
}

void aiSaac::VideoSummarization::summarizeGreedy(int budget) {
    keyPoints.clear();
    std::vector<cv::Mat> snippetHist = std::vector<cv::Mat>();
    int n; // number of frames Code review(Vivswan):or snippets?
    int SNP_SIZE = this->aiSaacSettings->getSummarizationSNPSize();
    int fps;
    cv::Size S;
    int status = extractFeatures(this->srcFilePath,
        fps,
        SNP_SIZE,
        snippetHist,
        n,
        S);
    cv::VideoCapture capture(this->srcFilePath);
    if (status > 0) {
        if (this->aiSaacSettings->getSummarizationGreedyAlgo() == "DM") {
            summarizeGreedyDM(snippetHist, n, budget, keyPoints);
        } else if (this->aiSaacSettings->getSummarizationGreedyAlgo() == "FL") {
            summarizeGreedyFL(snippetHist, n, budget, keyPoints);
        } else if (this->aiSaacSettings->getSummarizationGreedyAlgo() == "GC") {
            summarizeGreedyGC(snippetHist, n, budget, keyPoints);
        } else {
            std::cout << "Unknown Stream Summarization Algorithms\n";
        }
    }
    capture.release();
}

void aiSaac::VideoSummarization::summarizeGreedyOrdering() {
    keyPoints.clear();
    std::vector<cv::Mat> snippetHist = std::vector<cv::Mat>();
    int n; // number of frames Code review(Vivswan):or snippets?
    int SNP_SIZE = this->aiSaacSettings->getSummarizationSNPSize();
    int fps;
    cv::Size S;
    int status = extractFeatures(this->srcFilePath, fps, SNP_SIZE, snippetHist, n, S);
    cv::VideoCapture capture(this->srcFilePath);
    std::cout << "debug 1" << std::endl;
    if (status > 0) {
        if (this->aiSaacSettings->getSummarizationGreedyAlgo() == "DM") {
            std::cout << "Greedy Algo Ordering call SO DM" << std::endl;
            summarizeOrderingDM(snippetHist, n, orderedKeyPoints);
            std::cout << "Greedy Algo Ordering exit SO DM" << std::endl;
        } else if (this->aiSaacSettings->getSummarizationGreedyAlgo() == "FL") {
            std::cout << "Greedy Algo Ordering call SO FL" << std::endl;
            summarizeOrderingFL(snippetHist, n, orderedKeyPoints);
            std::cout << "Greedy Algo Ordering exit SO FL" << std::endl;
        } else if (this->aiSaacSettings->getSummarizationGreedyAlgo() == "GC") {
            std::cout << "Greedy Algo Ordering call SO GC" << std::endl;
            summarizeOrderingGC(snippetHist, n, orderedKeyPoints);
            std::cout << "Greedy Algo Ordering exit SO GC" << std::endl;
        } else {
            std::cout << "Unknown Stream Summarization Algorithms\n";
        }
    }
    std::cout << "Greedy Algo Ordering DONE" << std::endl;
    capture.release();
}

void aiSaac::VideoSummarization::writeSummaryFile(std::string destFilePath) {
    std::cout << "Destination Path is " << destFilePath << std::endl;
    cv::VideoCapture capture(this->srcFilePath);
    cv::VideoWriter SummVideo;
    cv::Size S = cv::Size(capture.get(CV_CAP_PROP_FRAME_WIDTH),
        capture.get(CV_CAP_PROP_FRAME_HEIGHT));
    int fps = capture.get(CV_CAP_PROP_FPS);
    int SNP_SIZE = this->aiSaacSettings->getSummarizationSNPSize();
    std::cout << "making the summary video" << std::endl;

    SummVideo.open(destFilePath.c_str(),
        // capture.get(CV_CAP_PROP_FOURCC),
        CV_FOURCC('M', 'J', 'P', 'G'),
        capture.get(CV_CAP_PROP_FPS),
        S,
        true);
    std::cout << "created file" << std::endl;
    cv::Mat frame;
    bool stop = false;;
    for (set<int>::iterator it = keyPoints.begin(); it!=keyPoints.end(); it++) {
        int frameId = (*it)*fps*SNP_SIZE;
        capture.set(CV_CAP_PROP_POS_FRAMES,frameId);
        int count = 0;
        while (count <= fps * SNP_SIZE) {
            if (!capture.read(frame)) {
                stop = true;
                break;
            } else {
                SummVideo.write(frame);
            }
            count++;
        }
        if (stop == true) {
            break;
        }
    }
    std::cout << "releasing" << std::endl;
    SummVideo.release();
}

std::set<int> aiSaac::VideoSummarization::getKeyPoints() {
    return keyPoints;
}

std::vector<int> aiSaac::VideoSummarization::getKeyPointsOrdered() {
    return orderedKeyPoints;
}

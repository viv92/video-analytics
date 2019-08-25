#ifndef SUMMARIZER_H
#define SUMMARIZER_H

//#include "set.h"
//#include "mmr.h"
//#include "streamgreedy.h"
#include "datk.h"
#include "representation/Set.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv/cv.hpp>
#include <opencv2/video/background_segm.hpp>
#include <iostream>
#include <set>
// #include <opencv2/contrib/contrib.hpp>

/* Video Summarization Code
 *
 */
// Compute the average color histogram for frames between startframe and endframe

namespace aiSaac{
  void colorHist(std::vector<cv::Mat>& frames, int startframe, int endframe, cv::MatND& hist);
  std::vector<std::vector<float> > computeKernel(std::vector<cv::MatND>& hist, int n, int compare_method = 0, double gamma = 1);
  int extractFeatures(string filename, int& framerate, int SNP_SIZE, std::vector<cv::Mat>& snipetHist, int& n, cv::Size& S);
  void summarizeStreamMMR(std::vector<cv::Mat>& snipetHist, int n, double epsilon, std::set<int>& summarySet);
  void summarizeStreamFL(std::vector<cv::Mat>& snipetHist, int n, double epsilon, std::set<int>& summarySet);
  void summarizeGreedyDM(std::vector<cv::Mat>& snipetHist, int n, double budget, std::set<int>& summarySet);
  void summarizeGreedyFL(std::vector<cv::Mat>& snipetHist, int n, double budget, std::set<int>& summarySet);
  void summarizeGreedyGC(std::vector<cv::Mat>& snipetHist, int n, double budget, std::set<int>& summarySet);
  void summarizeOrderingDM(std::vector<cv::Mat>& snipetHist, int n, std::vector<int>& ordering);
  void summarizeOrderingFL(std::vector<cv::Mat>& snipetHist, int n, std::vector<int>& ordering);
  void summarizeOrderingGC(std::vector<cv::Mat>& snipetHist, int n, std::vector<int>& ordering);
}
#endif // SUMMARIZER_H

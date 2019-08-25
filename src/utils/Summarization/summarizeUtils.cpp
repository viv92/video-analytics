/*
    Copyright 2016 AITOE
*/

#include "summarizeUtils.h"

namespace aiSaac{

  int h_bins = 10;
  int s_bins = 10;
  int similarity_type = 0;

  void colorHist(std::vector<cv::Mat>& frames, int startframe, int endframe, cv::MatND& hist)
  {
      int histSize[] = { h_bins, s_bins };
      // hue varies from 0 to 179, saturation from 0 to 255
      float h_ranges[] = { 0, 180 };
      float s_ranges[] = { 0, 256 };
      const float* ranges[] = { h_ranges, s_ranges };
      int channels[] = { 0, 1 };
      cv::MatND histcurr;
      /// Calculate the histograms for the HSV images
      cv::calcHist( &frames[startframe], 1, channels, cv::Mat(), hist, 2, histSize, ranges, true, false );
      cv::normalize(hist, hist, 0, 1, cv::NORM_MINMAX, -1, cv::Mat());
      for (int j = startframe + 1; j <= endframe; j++) {
          calcHist( &frames[j], 1, channels, cv::Mat(), histcurr, 2, histSize, ranges, true, false );
          normalize(histcurr, histcurr, 0, 1, cv::NORM_MINMAX, -1, cv::Mat() );
          hist += histcurr;
      }
      normalize(hist, hist, 0, 1, cv::NORM_MINMAX, -1, cv::Mat() );
  }

  std::vector<std::vector<float> > computeKernel(std::vector<cv::MatND>& hist, int n, int compare_method, double gamma)
  {
      // compare_method is the comparision method for histogram similarity
      std::vector<std::vector<float> > kernel;
      float max = 0;
      for (int i = 0; i < n; i++)
      {
          std::vector<float> currvector;
          for (int j = 0; j < n; j++)
          {
              float val = compareHist(hist[i], hist[j], compare_method);
              if (max < val)
                  max = val; //stored for normalisation done later
              currvector.push_back(val);
          }
          kernel.push_back(currvector); //stores all cross product scores
      }
      for (int i = 0; i < n; i++)
      {
          for (int j = 0; j < n; j++)
          {
              if ((compare_method == 0) || (compare_method == 2) )
                  kernel[i][j] = kernel[i][j]/max;
              else if ((compare_method == 1) || (compare_method == 3) )
                  kernel[i][j] = (max - kernel[i][j])/max;
              if (compare_method == 0)
                  kernel[i][j] = (kernel[i][j] + 1)/2;
              kernel[i][j] = pow(kernel[i][j], gamma);
          }
      }
      return kernel;
  }

  int extractFeatures(string filename,
      int& framerate,
      int SNP_SIZE,
      std::vector<cv::Mat>& snippetHist,
      int& n,
      cv::Size& S) {
      cv::VideoCapture capture;
      capture.open(filename);
      framerate = capture.get(CV_CAP_PROP_FPS);
      S = cv::Size(capture.get(CV_CAP_PROP_FRAME_WIDTH), capture.get(CV_CAP_PROP_FRAME_HEIGHT));
      cv::Mat frame;
      std::vector<cv::Mat> CurrVideo = std::vector<cv::Mat> ();
      if (!capture.isOpened()) {
        return 0;
      }
      int frame_count = 0;
      cout << "Starting to read video file and get features\n" << flush;
      // read the video
      int samplingRate = 1;
      int totalFrames = capture.get(CV_CAP_PROP_FRAME_COUNT);
      while (frame_count < totalFrames) {
          cv::MatND hist;
          if (frame_count % framerate/samplingRate == 0) {
              capture.set(CV_CAP_PROP_POS_FRAMES, frame_count);
              capture >> frame;
              if (frame.empty() || frame.data==NULL) {
                  break;
              }
              CurrVideo.push_back(frame.clone());
          }
          if (frame_count % (SNP_SIZE*framerate) == 0) {
              colorHist(CurrVideo, 0, CurrVideo.size() - 1, hist);
              snippetHist.push_back(hist);
              CurrVideo.clear();
          }
          frame_count++;
          int percentProcessed = (frame_count*100)/totalFrames;
          std::string analysisProgressOutput = "ANALYSIS_SUMMARIZATION_PROGRESS_START<<<" + std::to_string(percentProcessed) + ">>>ANALYSIS_SUMMARIZATION_PROGRESS_END";
          std::cout << analysisProgressOutput << std::endl;
      }
      capture.release();
      cout << "Done reading video file\n" << flush;
      n = snippetHist.size();
      return 1;
  }

  void summarizeStreamMMR(std::vector<cv::Mat>& snippetHist, int n, double epsilon, std::set<int>& summarySet){
      datk::Set optSet;
      std::vector<std::vector<float> > kernel = computeKernel(snippetHist, n, similarity_type);
      datk::MMR m(n, kernel);
      optSet.insert(0);
      std::vector<int> order(n, 1);
      for (int i = 0; i < n; i++)
          order[i] = i;
      datk::streamGreedy(m, epsilon, optSet, order);
      optSet.insert(n);
      summarySet = set<int>();
      for (datk::Set::iterator it = optSet.begin(); it!=optSet.end(); it++)
      {
          summarySet.insert(*it);
      }
      cout << "Done with Stream Summarization\n" << flush;
  }

  void summarizeStreamFL(std::vector<cv::Mat>& snippetHist, int n, double epsilon, std::set<int>& summarySet) {
      datk::Set optSet;
      std::vector<std::vector<float> > kernel = computeKernel(snippetHist, n, similarity_type);
      datk::FacilityLocation fL(n, kernel);
      optSet.insert(0);
      std::vector<int> order(n, 1);
      for (int i = 0; i < n; i++)
          order[i] = i;
      datk::streamGreedy(fL, epsilon, optSet, order);
      optSet.insert(n);
      summarySet = set<int>();
      for (datk::Set::iterator it = optSet.begin(); it!=optSet.end(); it++)
      {
          summarySet.insert(*it);
      }
      cout << "Done with Stream Summarization\n" << flush;
  }

  void summarizeGreedyDM(std::vector<cv::Mat>& snippetHist, int n, double budget, std::set<int>& summarySet){
      datk::Set optSet;
      std::vector<std::vector<float> > kernel = computeKernel(snippetHist, n, similarity_type);
      datk::DisparityMin dM(n, kernel);
      int inititem = rand()%n;
      optSet.insert(inititem);
      //Code Review: (Khoshrav) Why budget-1?
      datk::naiveGreedyMax(dM, budget-1, optSet, 0, false, true);
      summarySet = std::set<int>();
      for (datk::Set::iterator it = optSet.begin(); it!=optSet.end(); it++)
      {
          summarySet.insert(*it);
      }
      // cout << "Done with summarization\n" << flush;
  }

  void summarizeOrderingDM(std::vector<cv::Mat>& snippetHist, int n, std::vector<int>& ordering){
      datk::Set optSet;
      std::vector<std::vector<float> > kernel = computeKernel(snippetHist, n, similarity_type);
      datk::DisparityMin dM(n, kernel);
      int inititem = rand()%n;
      optSet.insert(inititem);
      std::cout << "Greedy Algo Ordering Enter datk" << std::endl;
      datk::naiveGreedyMaxOrdering(dM, optSet, ordering, 0);//PROBLEMATIC DOES NOT EXIT
      std::cout << "Greedy Algo Ordering Exit datk" << std::endl;
  }

  void summarizeGreedyFL(std::vector<cv::Mat>& snippetHist, int n, double budget, std::set<int>& summarySet){
      datk::Set optSet;
      std::vector<std::vector<float> > kernel = computeKernel(snippetHist, n, similarity_type);
      datk::FacilityLocation fL(n, kernel);
      //Code Review: (Khoshrav) Why budget-1?
      datk::lazyGreedyMax(fL, budget-1, optSet, 0);
      summarySet = std::set<int>();
      for (datk::Set::iterator it = optSet.begin(); it!=optSet.end(); it++)
      {
          summarySet.insert(*it);
      }
  }

  void summarizeOrderingFL(std::vector<cv::Mat>& snippetHist, int n, std::vector<int>& ordering){
      datk::Set optSet;
      std::vector<std::vector<float> > kernel = computeKernel(snippetHist, n, similarity_type);
      datk::FacilityLocation fL(n, kernel);
      datk::lazyGreedyMaxOrdering(fL, optSet, ordering, 0);
  }

  void summarizeGreedyGC(std::vector<cv::Mat>& snippetHist, int n, double budget, std::set<int>& summarySet){
      datk::Set optSet;
      std::vector<std::vector<float> > kernel = computeKernel(snippetHist, n, similarity_type);
      datk::GraphCutFunctions gC(n, kernel, 1);
      //Code Review: (Khoshrav) Why budget-1?
      datk::lazyGreedyMax(gC, budget-1, optSet, 0);
      summarySet = std::set<int>();
      for (datk::Set::iterator it = optSet.begin(); it!=optSet.end(); it++)
      {
          summarySet.insert(*it);
      }
  }

  void summarizeOrderingGC(std::vector<cv::Mat>& snippetHist, int n, std::vector<int>& ordering){
      datk::Set optSet;
      std::vector<std::vector<float> > kernel = computeKernel(snippetHist, n, similarity_type);
      datk::GraphCutFunctions gC(n, kernel, 1);
      datk::lazyGreedyMaxOrdering(gC, optSet, ordering, 0);
  }

}

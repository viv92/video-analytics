#ifndef AISAAC_COLORHIST
#define AISAAC_COLORHIST
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
using namespace cv;

namespace aisaac{
	static int h_bins = 10;
	static int s_bins = 10;
	// Color Histogram of a set of frames determined by the startframe and endframe indices respectively
	inline void colorHist(vector<Mat>& frames, int startframe, int endframe, MatND& hist)
	{
	    int histSize[] = { h_bins, s_bins };
	    // hue varies from 0 to 179, saturation from 0 to 255
	    float h_ranges[] = { 0, 180 };
	    float s_ranges[] = { 0, 256 };
	    const float* ranges[] = { h_ranges, s_ranges };
	    int channels[] = { 0, 1 };
			MatND histcurr;
	    /// Calculate the histograms for the HSV images
	    calcHist( &frames[startframe], 1, channels, Mat(), hist, 2, histSize, ranges, true, false );
	    normalize(hist, hist, 0, 1, NORM_MINMAX, -1, Mat() );
			for (int j = startframe + 1; j <= endframe; j++){
			    calcHist( &frames[j], 1, channels, Mat(), histcurr, 2, histSize, ranges, true, false );
			    normalize(histcurr, histcurr, 0, 1, NORM_MINMAX, -1, Mat() );
				hist += histcurr;
			}
	    normalize(hist, hist, 0, 1, NORM_MINMAX, -1, Mat() );
	}

	inline void colorHist(Mat image, MatND& hist){
    int histSize[] = { h_bins, s_bins };
    // hue varies from 0 to 179, saturation from 0 to 255
    float h_ranges[] = { 0, 180 };
    float s_ranges[] = { 0, 256 };
    const float* ranges[] = { h_ranges, s_ranges };
    int channels[] = { 0, 1 };
    /// Calculate the histograms for the HSV images
    calcHist( &image, 1, channels, Mat(), hist, 2, histSize, ranges, true, false );
    normalize(hist, hist, 0, 1, NORM_MINMAX, -1, Mat() );
	}


}

#endif

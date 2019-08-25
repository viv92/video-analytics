/*
    Copyright 2016 AITOE
*/

#include "colorClassifier.h"

enum {
    cBLACK=0,
    cWHITE,
    cGREY,
    cRED,
    cORANGE,
    cYELLOW,
    cGREEN,
    cAQUA,
    cBLUE,
    cPURPLE,
    cPINK,
    NUM_COLOR_TYPES
};

const char* sCTypes[NUM_COLOR_TYPES] = {
    "Black",
    "White",
    "Grey",
    "Red",
    "Orange",
    "Yellow",
    "Green",
    "Aqua",
    "Blue",
    "Purple",
    "Pink"
};

aiSaac::ColorClassifier::ColorClassifier () {}

int aiSaac::ColorClassifier::getPixelColorType(int H, int S, int V) {
    int color;
    if (V < 75) {
        color = cBLACK;
    } else if (V > 190 && S < 27) {
        color = cWHITE;
    } else if (S < 53 && V < 185) {
        color = cGREY;
    } else {	// Is a color
        if (H < 14) {
            color = cRED;
        } else if (H < 25) {
            color = cORANGE;
        } else if (H < 34) {
            color = cYELLOW;
        } else if (H < 73) {
            color = cGREEN;
        } else if (H < 102) {
            color = cAQUA;
        } else if (H < 127) {
            color = cBLUE;
        } else if (H < 149) {
            color = cPURPLE;
        } else if (H < 175) {
            color = cPINK;
        } else {
            color = cRED;
        }
    }
    return color;
}

std::string aiSaac::ColorClassifier::patchColor(const cv::Mat &rawFrame, cv::Rect &roiRect) {
    cv::Mat roiImage = rawFrame(roiRect);
    cv::Mat roiImageHSV;
    cvtColor(roiImage, roiImageHSV, CV_BGR2HSV);

    std::cout << "Determining color type of the roi" << std::endl;
    int h = roiImageHSV.rows; // Pixel height
    int w = roiImageHSV.cols; // Pixel width
    int rowSize = roiImageHSV.step; // Size of row in bytes, including extra padding
    uchar *imOfs = roiImageHSV.data; // Pointer to the start of the image HSV pixels.
    //std::cout << "h:" << h << " w:" << w << std::endl << std::flush;

    int tallyColors[NUM_COLOR_TYPES];
    for (int i = 0; i < NUM_COLOR_TYPES; i++) {
        tallyColors[i] = 0;
    }

    // Scan the roi image to find the tally of pixel colors
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            // Get the HSV pixel components
            uchar H = *(imOfs + y * rowSize + x * 3 + 0); // Hue
            uchar S = *(imOfs + y * rowSize + x * 3 + 1); // Saturation
            uchar V = *(imOfs + y * rowSize + x * 3 + 2); // Value (Brightness)
            //std::cout << "H:" << H << " S:" << S << " V:" << V << std::endl << std::flush;

            // Determine what type of color the HSV pixel is.
            int ctype = getPixelColorType(H, S, V);
            //std::cout << "ctype index:" << ctype << std::endl << std::flush;
            // Keep count of these colors.
            tallyColors[ctype]++;
            //std::cout << "ctype updated" << std::endl << std::flush;
        }
    }

    // Print a report about color types, and find the max tally
    // std::cout << "Number of pixels found using each color type (out of " << (w*h) << ":\n";
    int tallyMaxIndex = 0;
    int tallyMaxCount = -1;
    int pixels = w * h;
    //std::cout << "pixels:" << pixels << std::endl << std::flush;
    for (int i = 0; i < NUM_COLOR_TYPES; i++) {
      int v = tallyColors[i];
      std::cout << sCTypes[i] << " " << (v * 100/pixels) << "%, ";
      if (v > tallyMaxCount) {
        tallyMaxCount = tallyColors[i];
        tallyMaxIndex = i;
      }
    }
    std::cout << std::endl;
    int percentage = tallyMaxCount * 100 / pixels;
    std::cout << "Color of roi: " << sCTypes[tallyMaxIndex] << " (" << percentage << "% confidence)." << std::endl << std::endl;

    return sCTypes[tallyMaxIndex];

    // Display the color type over the roi in the image.
    // cv::putText(rawimage, sCTypes[tallyMaxIndex], cv::Point(roiRect.x, roiRect.y + roiRect.height + 12), FONT_HERSHEY_PLAIN, 2, Scalar(0,0,255,255));
    // Free resources.
}

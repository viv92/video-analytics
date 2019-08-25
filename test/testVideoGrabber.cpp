#include "../tools/videoGrabber/videoGrabber.h"
#include <thread>
// #include <raspicam/raspicam_cv.h>

int main(int argc, char const *argv[]) {
    std::cout << "In Main" << std::endl;

    cv::VideoCapture cap = cv::VideoCapture(0);
    // raspicam::RaspiCam_Cv cap = raspicam::RaspiCam_Cv();
    int c = 0;
    int FPS = 20;
    int chunkFrameLimit = 1000;
    cv::Size S = cv::Size(640,480);
    cv::Mat currentFrame;

    std::cout << "Starting Camera" << std::endl;
    if (!cap.isOpened()) {
        std::cout << "Error opening camera." << std::endl;
        return -1;
    }

    cap.set(CV_CAP_PROP_FRAME_WIDTH, 640);
    cap.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
    cap.set(CV_CAP_PROP_FPS, FPS);

    videoGrabber vidGrab = videoGrabber(chunkFrameLimit, FPS, S, "storageDir/outputFiles");

    while (c < 200) {
        cap.grab();
        cap.retrieve(currentFrame);
        std::thread test(&videoGrabber::acceptFrame, vidGrab, currentFrame);
        c++;
    }
    cap.release();
    return 0;
}

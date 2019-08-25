#include <string>
#include "aiVideoShotAnalytics.h"

int main(int argc, char *argv[]) {
    if (argc < 4) {
        std::cout << "usage: ./shotSplitter [sourceFilePath]  "
            << "[destinationFolder] [minimum/shot/seconds] [fps(optional)]" << std::endl;
        return -1;
    }
    int samplingRate = 1;
    std::string sourceFilePath = argv[1];
    std::string destinationFolder = argv[2];
    float minShotLength = std::atof(argv[3]);
    if (argc > 4) {
        samplingRate = std::atoi(argv[4]);
    }

    std::string command = "mkdir -p \""
        + destinationFolder
        + "/LessThan"
        + std::to_string(minShotLength)
        +  "\"";
    system(command.c_str());

    command = "mkdir -p \""
        + destinationFolder
        + "/GreaterThan"
        + std::to_string(minShotLength)
        +  "\"";
    system(command.c_str());


    aisaac::aiVideoShotAnalytics shotAnalytics =
        aisaac::aiVideoShotAnalytics(sourceFilePath, samplingRate);
    shotAnalytics.analyze();
    shotAnalytics.outJSON();
    cv::VideoCapture videoStreamer = cv::VideoCapture(sourceFilePath);
    for (int i = 0; i < shotAnalytics.startFrameNumbers.size(); i++) {
        int startFrame = shotAnalytics.startFrameNumbers[i];
        int endFrame = shotAnalytics.endFrameNumbers[i];
        std::string outputFilePath;
        if ((endFrame - startFrame) <
            (minShotLength*videoStreamer.get(CV_CAP_PROP_FPS))) {
            outputFilePath = destinationFolder + "/"
                + "LessThan"
                + std::to_string(minShotLength)
                + "/"
                + std::to_string(startFrame)
                + "-"
                + std::to_string(endFrame)
                + ".avi";
        }
        else {
            outputFilePath = destinationFolder + "/"
                + "GreaterThan"
                + std::to_string(minShotLength)
                + "/"
                + std::to_string(startFrame)
                + "-"
                + std::to_string(endFrame)
                + ".avi";
        }
        std::cout << "File name " << outputFilePath << std::endl
            << std::flush;
        cv::VideoWriter outputFile;
        outputFile.open(outputFilePath.c_str(),
            CV_FOURCC('M', 'J', 'P', 'G'),
            videoStreamer.get(CV_CAP_PROP_FPS),
            cv::Size(
                videoStreamer.get(CV_CAP_PROP_FRAME_WIDTH),
                videoStreamer.get(CV_CAP_PROP_FRAME_HEIGHT)),
            true);
        videoStreamer.set(CV_CAP_PROP_POS_FRAMES, startFrame);
        int frameNumber = startFrame;
        while (frameNumber <= endFrame -1 ) {
            cv::Mat frame;
            videoStreamer >> frame;
            outputFile << frame;
            frameNumber++;
        }
        outputFile.release();
    }
    videoStreamer.release();
}

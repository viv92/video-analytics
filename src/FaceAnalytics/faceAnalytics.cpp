/*
    Copyright 2016 AITOE
*/

/*
    Todo:
    1. Make another recognize function which takes in a matrix
    2. Pass settings which enables/disables age/gender/faceRec/emotion.
    3. Add all algorithm parameters in runAlgo() to aiSaacSettings
*/

#include "faceAnalytics.h"

aiSaac::FaceAnalytics::FaceAnalytics(aiSaac::AiSaacSettings &aiSaacSettings) : aiSaacSettings(aiSaacSettings) {
    this->initialize();
}

aiSaac::FaceAnalytics::~FaceAnalytics() {
    if (this->aiSaacSettings.getFaceStorageMode()) {
        for (std::vector<aiSaac::FaceBlob>::iterator it =
        this->blobContainer.begin();
        it != this->blobContainer.end();) {
                // face storage mode true means detection algorithm will store faces.
                // codeReview(Anurag): Currently overwrites existing data in faceStorage folder
                std::string personDirectory = this->aiSaacSettings.getFaceRecognitionTrainingDataPath()
                    + "/Person" + std::to_string(it->ID);
                std::system(("mkdir " + personDirectory).c_str());
                for (int i = 0; i < it->faceImages.size(); i++) {
                    std::string personImagePath =
                        personDirectory + "/" + std::to_string(i) + ".png";
                    std::cout << "storing image: " << personImagePath << std::endl;
                    cv::imwrite(personImagePath, it->faceImages[i]);
                }

            it++;
        }
    }
}

void aiSaac::FaceAnalytics::initialize() {
    this->analyzedFrameNumber = 0;
    this->ID = 0;
    // this->localizedRectangles.clear();
    // this->faceRecognition = new FaceRecognition(this->aiSaacSettings);
    // this->genderRecognition = new GenderRecognition(this->aiSaacSettings);
    // this->ageRecognition = new AgeRecognition(this->aiSaacSettings);
    // this->emotionRecognition = new EmotionRecognition(this->aiSaacSettings);
    // codeReview(Anurag): kept here for testing
    // this->faceRecognition->addStoredFacesToDB();
    this->blobContainer.clear();
    this->loadAlgorithmType();
    std::cout << "loaded and initialized faceAnalytics" << std::endl;
}

int aiSaac::FaceAnalytics::detect(const cv::Mat &rawFrame,
    int frameNumber,
    cv::Rect areaOfInterest) {
    std::vector<cv::Rect> localizedRectangles;

    if (areaOfInterest.area() != 0) {
        cv::Mat subRawFrame = rawFrame(areaOfInterest);
        this->runAlgo(subRawFrame, frameNumber, localizedRectangles);
    } else {
        this->runAlgo(rawFrame, frameNumber, localizedRectangles);
    }

    return localizedRectangles.size();
}

void aiSaac::FaceAnalytics::localize(const cv::Mat &rawFrame,
    std::vector<cv::Rect> &localizedRectangles,
    int frameNumber,
    cv::Rect areaOfInterest) {
    if (areaOfInterest.area() != 0) {
        cv::Mat subRawFrame = rawFrame(areaOfInterest);
        this->runAlgo(subRawFrame, frameNumber, localizedRectangles);
        for (int i = 0; i < localizedRectangles.size(); i++) {
            localizedRectangles[i].x += areaOfInterest.x;
            localizedRectangles[i].y += areaOfInterest.y;
        }
    } else {
        this->runAlgo(rawFrame, frameNumber, localizedRectangles);
    }
}

void aiSaac::FaceAnalytics::track(const cv::Mat &rawFrame,
    int frameNumber,
    cv::Rect areaOfInterest) {

    this->preProcessBlobContainer(frameNumber);
    std::vector<cv::Rect> localizedRectangles;
    this->localize(rawFrame, localizedRectangles, frameNumber, areaOfInterest);

    std::vector<int> rectTaken(localizedRectangles.size(), 0);

    for (int i = 0; i < this->blobContainer.size(); i++) {
        cv::Point latestPosition = this->blobContainer[i].pastPositions.back();
        for (int j = 0; j < localizedRectangles.size(); j++) {
            if (rectTaken[j] == 1) {
                continue;
            }
            cv::Point potentialPosition(localizedRectangles[j].x + localizedRectangles[j].width / 2,
                localizedRectangles[j].y + localizedRectangles[j].height / 2);

            if ((abs(potentialPosition.x - latestPosition.x) < (rawFrame.size().width / 4))
                && (abs(potentialPosition.y - latestPosition.y) < (rawFrame.size().height / 4))) {
                rectTaken[j] = 1;
                this->blobContainer[i].lastFrameNumber = frameNumber;
                this->blobContainer[i].frameCount++;
                this->blobContainer[i].lastRectangle = localizedRectangles[j];
                //this->blobContainer[i].label = "person";

                if (this->blobContainer[i].pastPositions.size() == 50) {
                    this->blobContainer[i].pastPositions.erase(
                        this->blobContainer[i].pastPositions.begin());
                }
                cv::Point position;
                this->blobContainer[i].pastPositions.push_back(potentialPosition);
                // std::cout << "Appended existing blob" << std::endl;
                break;
            }
        }
    }

    for (int i = 0; i < localizedRectangles.size(); i++) {
        if (!rectTaken[i]) {
            aiSaac::FaceBlob newFaceBlob;
            this->ID++;
            newFaceBlob.ID = ID;
            newFaceBlob.frameCount = 1;
            newFaceBlob.firstFrameNumber = frameNumber;
            newFaceBlob.lastFrameNumber = frameNumber;
            newFaceBlob.firstRectangle = localizedRectangles[i];
            newFaceBlob.lastRectangle = localizedRectangles[i];
            cv::Point position;
            position.x = newFaceBlob.lastRectangle.x + newFaceBlob.lastRectangle.width / 2;
            position.y = newFaceBlob.lastRectangle.y + newFaceBlob.lastRectangle.height /2;
            int topLeftX = localizedRectangles[i].x - (localizedRectangles[i].width / 4);
            (topLeftX >= 0) ? topLeftX = topLeftX : topLeftX = 0;
            int topLeftY = localizedRectangles[i].y - (localizedRectangles[i].height / 4);
            (topLeftY >= 0) ? topLeftY = topLeftY : topLeftY = 0;
            cv::Point topLeft(topLeftX, topLeftY);

            int bottomRightX = localizedRectangles[i].x + localizedRectangles[i].width +
                (localizedRectangles[i].width / 4);
            (bottomRightX <= rawFrame.cols) ? bottomRightX = bottomRightX :
                bottomRightX = rawFrame.cols;
            int bottomRightY = localizedRectangles[i].y + localizedRectangles[i].height +
                (localizedRectangles[i].height / 4);
            (bottomRightY <= rawFrame.rows) ? bottomRightY = bottomRightY :
                bottomRightY = rawFrame.rows;
            cv::Point bottomRight(bottomRightX, bottomRightY);
            cv::Rect largerLocalization(topLeft, bottomRight);
            cv::Mat faceImage = rawFrame(largerLocalization);
            newFaceBlob.faceImages.push_back(faceImage);
            newFaceBlob.pastPositions.push_back(position);
            newFaceBlob.label = "person";

            this->blobContainer.push_back(newFaceBlob);
            std::cout << "Added a new blob.. " << this->blobContainer.size() << std::endl;
        }
    }
}

/*
    codeReview(Anurag): add logic not check if model objects are empty. if empty, load.
*/

void aiSaac::FaceAnalytics::annotate(cv::Mat &rawFrame) {
    this->annotate(rawFrame, this->analyzedFrameNumber);
}

void aiSaac::FaceAnalytics::annotate(cv::Mat &rawFrame, int frameNumber) {
    for (int i = 0; i < this->blobContainer.size(); i++) {
        if (this->blobContainer[i].lastFrameNumber == frameNumber){// && this->blobContainer[i].gender != "unassigned") {
            //std::string label = "person";
            std::string label = this->blobContainer[i].label;
            cv::rectangle(rawFrame,
                this->blobContainer[i].lastRectangle,
                cv::Scalar(255, 192, 203), 2);

                int baseline = 0;
                int fontScale = ceil(rawFrame.cols * 0.0003);
                int thickness = ceil(rawFrame.cols * 0.0025);

                cv::Size textSize = cv::getTextSize(label,
                     cv::FONT_HERSHEY_SIMPLEX,
                    fontScale,
                    thickness,
                    &baseline);
                baseline += thickness;
                cv::Point textOrg(this->blobContainer[i].lastRectangle.x
                    + (this->blobContainer[i].lastRectangle.width / 2)
                    - (textSize.width / 2),
                    this->blobContainer[i].lastRectangle.y - 5);
                if (textOrg.x < 0) {
                    textOrg.x = 0;
                }
                if (textOrg.y < 0) {
                    textOrg.y = 0;
                }
                cv::putText(rawFrame,
                    label,
                    textOrg,
                    cv::FONT_HERSHEY_SIMPLEX,
                    fontScale,
                    cv::Scalar(127, 255, 0),
                    thickness);
            // cv::putText(rawFrame,
            //     this->blobContainer[i].label,
            //     this->blobContainer[i].pastPositions.back(),
            //     cv::FONT_HERSHEY_PLAIN,
            //     1.0,
            //     CV_RGB(0,255,0),
            //     2.0);

            // for (int j = 1; j < blobContainer[i].pastPositions.size(); j++) {
            //     cv::Point pointOne((this->blobContainer[i].pastPositions[j - 1].x),
            //         (this->blobContainer[i].pastPositions[j - 1].y));
            //     cv::Point pointTwo((this->blobContainer[i].pastPositions[j].x),
            //         (blobContainer[i].pastPositions[j].y));
            //     cv::line(rawFrame, pointOne, pointTwo, cv::Scalar(0, 0, 255), 1, 8);
            // }
        }
    }
}

void aiSaac::FaceAnalytics::loadAlgorithmType() {
    std::string faceDetectAlgo = this->aiSaacSettings.getFaceDetectAnalyticsAlgo();
    std::cout << "Face Analytics Algo: " << faceDetectAlgo << std::endl;

    if (faceDetectAlgo == "NPD") {
        this->npd.load(this->aiSaacSettings.getNPDFaceDetectFilePath().c_str());
    } else if (faceDetectAlgo == "HAAR") {
        if (!this->faceCascadeClassifier.load(this->aiSaacSettings.getHaarCascadeFilePath()
            + "haarcascade_frontalface_alt.xml")) {
            std::cout << "Sorry cascade not found at: "
                << this->aiSaacSettings.getHaarCascadeFilePath()
                    + "haarcascade_frontalface_alt.xml"
                << std::endl;
        }
    } else if (faceDetectAlgo == "DLIB") {
        this->dlibDetector = dlib::get_frontal_face_detector();
    }
}

void aiSaac::FaceAnalytics::runAlgo(const cv::Mat &rawFrame,
    int frameNumber,
    std::vector<cv::Rect> &localizedRectangles) {
    std::string faceDetectAlgo = this->aiSaacSettings.getFaceDetectAnalyticsAlgo();
    std::vector<double> minSize = this->aiSaacSettings.getMinimumFaceSize();
    std::vector<double> maxSize = this->aiSaacSettings.getMaximumFaceSize();
    localizedRectangles.clear();

    if (faceDetectAlgo == "NPD") {
        cv::Mat grayRawFrame;
        cv::cvtColor(rawFrame, grayRawFrame, cv::COLOR_BGR2GRAY);

        int numberOfFaces = this->npd.detect(grayRawFrame.data,
            grayRawFrame.cols,
            grayRawFrame.rows);

        std::vector<int> &xCoordinates = this->npd.getXs();
        std::vector<int> &yCoordinates = this->npd.getYs();
        std::vector<int> &sizes = this->npd.getSs();
        std::vector<float> &scores = this->npd.getScores();
        // std::cout << numberOfFaces << "\n" << std::endl;
        for (int i = 0; i < numberOfFaces; i++) {
            if (scores[i] > 20) {  // to be put in aiSaacSettings
                // cv::Rect FaceRect = cv::Rect(Xs[i], Ys[i], Ss[i], Ss[i]);
                localizedRectangles.push_back(cv::Rect(xCoordinates[i],
                    yCoordinates[i],
                    sizes[i],
                    sizes[i]));
            }
        }

        //area thresholds
        int minCols = grayRawFrame.cols * minSize[0];
        int minRows = grayRawFrame.rows * minSize[1];
        int maxCols = grayRawFrame.cols * maxSize[0];
        int maxRows = grayRawFrame.rows * maxSize[1];
        int minAreaThreshold = minCols * minRows;
        int maxAreaThreshold = maxCols * maxRows;

        // creating largerLocalization
        // codeReview(Anurag): to put largerLocalizationFactor in aiSaacSettings
        for (int i = 0; i < localizedRectangles.size(); i++) {
            int topLeftX = localizedRectangles[i].x
                - (localizedRectangles[i].width / 4);
            (topLeftX >= 0) ? topLeftX = topLeftX : topLeftX = 0;
            int topLeftY = localizedRectangles[i].y
                - (localizedRectangles[i].height / 2);
            (topLeftY >= 0) ? topLeftY = topLeftY : topLeftY = 0;
            cv::Point topLeft(topLeftX, topLeftY);
            int bottomRightX = localizedRectangles[i].x
                + localizedRectangles[i].width
                + (localizedRectangles[i].width / 4);
            (bottomRightX <= rawFrame.cols) ? bottomRightX = bottomRightX :
                bottomRightX = rawFrame.cols;
            int bottomRightY = localizedRectangles[i].y
                + localizedRectangles[i].height +
                (localizedRectangles[i].height / 4);
            (bottomRightY <= rawFrame.rows) ? bottomRightY = bottomRightY :
                bottomRightY = rawFrame.rows;
            cv::Point bottomRight(bottomRightX, bottomRightY);
            cv::Rect largerLocalization(topLeft, bottomRight);
            if (largerLocalization.area() >= minAreaThreshold
                && largerLocalization.area() <= maxAreaThreshold) {
                localizedRectangles[i] = largerLocalization;
            } else {
              localizedRectangles.erase(localizedRectangles.begin() + i);
              i--;
            }
        }
    } else if (faceDetectAlgo == "HAAR") {
        this->faceCascadeClassifier.detectMultiScale(rawFrame,
            localizedRectangles,
            1.1,  // algo param
            4,  // algo param
            CV_HAAR_SCALE_IMAGE,
            cv::Size(minSize[0] * rawFrame.cols, minSize[1] * rawFrame.rows),
            cv::Size(maxSize[0] * rawFrame.cols, maxSize[1] * rawFrame.rows));

            // creating largerLocalization
            // codeReview(Anurag): to put largerLocalizationFactor in aiSaacSettings
            for (int i = 0; i < localizedRectangles.size(); i++) {
                int topLeftX = localizedRectangles[i].x
                    - (localizedRectangles[i].width / 4);
                (topLeftX >= 0) ? topLeftX = topLeftX : topLeftX = 0;
                int topLeftY = localizedRectangles[i].y
                    - (localizedRectangles[i].height / 2);
                (topLeftY >= 0) ? topLeftY = topLeftY : topLeftY = 0;
                cv::Point topLeft(topLeftX, topLeftY);
                int bottomRightX = localizedRectangles[i].x
                    + localizedRectangles[i].width
                    + (localizedRectangles[i].width / 4);
                (bottomRightX <= rawFrame.cols) ? bottomRightX = bottomRightX :
                    bottomRightX = rawFrame.cols;
                int bottomRightY = localizedRectangles[i].y
                    + localizedRectangles[i].height +
                    (localizedRectangles[i].height / 4);
                (bottomRightY <= rawFrame.rows) ? bottomRightY = bottomRightY :
                    bottomRightY = rawFrame.rows;
                cv::Point bottomRight(bottomRightX, bottomRightY);
                cv::Rect largerLocalization(topLeft, bottomRight);

                localizedRectangles[i] = largerLocalization;
            }
    } else if (faceDetectAlgo == "DLIB") {
        dlib::array2d<dlib::rgb_pixel> dlibImage;
        dlib::assign_image(dlibImage, dlib::cv_image<dlib::bgr_pixel>(rawFrame));
        // dlib::cv_image<dlib::bgr_pixel> dlibImage(rawFrame);
        dlib::pyramid_up(dlibImage);
        std::vector<dlib::rectangle> dlibFaces = this->dlibDetector(dlibImage);

        dlib::pyramid_down<2> pyrDown;


        for (int i = 0; i < dlibFaces.size(); i++) {
            dlib::rectangle scaledDownRect = pyrDown.rect_down(dlibFaces[i]);
            localizedRectangles.push_back(cv::Rect(
                cv::Point(scaledDownRect.left(), scaledDownRect.top()),
                cv::Point(scaledDownRect.right(), scaledDownRect.bottom())));
        }

        // creating largerLocalization
        // codeReview(Anurag): to put largerLocalizationFactor in aiSaacSettings
        for (int i = 0; i < localizedRectangles.size(); i++) {
            int topLeftX = localizedRectangles[i].x
                - (localizedRectangles[i].width / 4);
            (topLeftX >= 0) ? topLeftX = topLeftX : topLeftX = 0;
            int topLeftY = localizedRectangles[i].y
                - (localizedRectangles[i].height / 2);
            (topLeftY >= 0) ? topLeftY = topLeftY : topLeftY = 0;
            cv::Point topLeft(topLeftX, topLeftY);
            int bottomRightX = localizedRectangles[i].x
                + localizedRectangles[i].width
                + (localizedRectangles[i].width / 4);
            (bottomRightX <= rawFrame.cols) ? bottomRightX = bottomRightX :
                bottomRightX = rawFrame.cols;
            int bottomRightY = localizedRectangles[i].y
                + localizedRectangles[i].height +
                (localizedRectangles[i].height / 4);
            (bottomRightY <= rawFrame.rows) ? bottomRightY = bottomRightY :
                bottomRightY = rawFrame.rows;
            cv::Point bottomRight(bottomRightX, bottomRightY);
            cv::Rect largerLocalization(topLeft, bottomRight);

            localizedRectangles[i] = largerLocalization;
        }
    }
    this->analyzedFrameNumber = frameNumber;
}

void aiSaac::FaceAnalytics::preProcessBlobContainer(int frameNumber) {
    if (this->blobContainer.size() > 400) {  // add to aiSaac settings
        this->blobContainer.erase(this->blobContainer.begin(), this->blobContainer.begin() + 200);
    }
    //
    for (std::vector<aiSaac::FaceBlob>::iterator it = this->blobContainer.begin();
        it != this->blobContainer.end();) {
        if ((frameNumber - it->lastFrameNumber) > this->aiSaacSettings.getFaceBlobDeleteTime()) {
            // face storage mode true means detection algorithm will store faces.
            // codeReview(Anurag): Currently overwrites existing data in faceStorage folder
            if (this->aiSaacSettings.getFaceStorageMode()) {
                std::string personDirectory = this->aiSaacSettings.getFaceRecognitionTrainingDataPath()
                    + "/Person" + std::to_string(it->ID);
                std::system(("mkdir " + personDirectory).c_str());
                for (int i = 0; i < it->faceImages.size(); i++) {
                    std::string personImagePath =
                        personDirectory + "/" + std::to_string(i) + ".png";
                    std::cout << "storing image: " << personImagePath << std::endl;
                    cv::imwrite(personImagePath, it->faceImages[i]);
                }
            }
            it = this->blobContainer.erase(it);
        } else {
            ++it;
        }
    }
}

float aiSaac::FaceAnalytics::mean(std::vector<float> vector) {
    float mean = 0.0;
    for(int i = 0; i < vector.size(); i++){
        mean += vector[i];
    }
    return mean/vector.size();
}

float aiSaac::FaceAnalytics::pearsonSimilarity(std::vector<float> features1, std::vector<float> features2) {
    int n = features1.size();

    float sumFeatures1 = 0.0, sumFeatures2 = 0.0;
    for(int i = 0; i < features1.size(); i++){
        sumFeatures1 += features1[i];
        sumFeatures2 += features2[i];
    }

    float squareSumFeatures1 = 0.0, squareSumFeatures2 = 0.0;
    for(int i = 0; i < features1.size(); i++){
        squareSumFeatures1 += pow(features1[i], 2);
        squareSumFeatures2 += pow(features2[i], 2);
    }

    float productFeatures = 0.0;
    for(int i = 0; i < features1.size(); i++){
        productFeatures += features1[i] * features2[i];
    }

    float numerator = productFeatures - ((sumFeatures1 * sumFeatures2) / n);
    float denominator = sqrt((squareSumFeatures1 - (pow(sumFeatures1, 2)/n)) * (squareSumFeatures2 - (pow(sumFeatures2, 2)/n)));

    if (denominator == 0.0) return 0.0;
    else return numerator/denominator;
}

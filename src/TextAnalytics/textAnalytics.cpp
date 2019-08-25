/*
    Copyright 2016 AITOE
*/

#include "textAnalytics.h"

aiSaac::TextAnalytics::TextAnalytics(AiSaacSettings &aiSaacSettings): aiSaacSettings(aiSaacSettings) {
    // this->aiSaacSettings = aiSaacSettings;
    this->initialize();
}

aiSaac::TextAnalytics::~TextAnalytics() {}

void aiSaac::TextAnalytics::initialize() {
    this->analyzedFrameNumber = 0;
    this->ID = 0;
    this->blobContainer.clear();
    this->loadAlgorithmType();
    std::cout << "loaded and initialized textAnalytics" << std::endl;
}

int aiSaac::TextAnalytics::detect(const cv::Mat &rawFrame,
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

void aiSaac::TextAnalytics::localize(const cv::Mat &rawFrame,
    std::vector<cv::Rect> &localizedRectangles,
    int frameNumber,
    cv::Rect areaOfInterest) {
        std::cout << "localize called" << std::endl;
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
    std::cout << "done localizing" << std::endl;
}

void aiSaac::TextAnalytics::track(const cv::Mat &rawFrame,
    int frameNumber,
    cv::Rect areaOfInterest) {
    this->preProcessBlobContainer(frameNumber);
    std::vector<cv::Rect> localizedRectangles;
    this->localize(rawFrame, localizedRectangles, frameNumber, areaOfInterest);
    std::cout << "hello1" << std::endl;
    std::vector<int> rectTaken(localizedRectangles.size(), 0);
    for (int i = 0; i < this->blobContainer.size(); i++) {
        std::cout << "hello2" << std::endl;
        cv::Point latestPosition = this->blobContainer[i].pastPositions.back();
        std::cout << "hello3" << std::endl;
        for (int j = 0; j < localizedRectangles.size(); j++) {
            if (rectTaken[j] == 1) {
                continue;
            }
            std::cout << "hello4" << std::endl;
            cv::Point potentialPosition(localizedRectangles[j].x
                + localizedRectangles[j].width/2,
                localizedRectangles[j].y + localizedRectangles[j].height/2);
            std::cout << "hello5" << std::endl;
            if ((abs(potentialPosition.x - latestPosition.x) < (rawFrame.size().width / 3))
                && (abs(potentialPosition.y - latestPosition.y) < (rawFrame.size().height / 3))) {
                rectTaken[j] = 1;
                std::cout << "hello6" << std::endl;
                this->blobContainer[i].lastFrameNumber = frameNumber;
                this->blobContainer[i].frameCount++;
                this->blobContainer[i].lastRectangle = localizedRectangles[j];

                // int topLeftX = localizedRectangles[j].x - (localizedRectangles[j].width / 4);
                // (topLeftX >= 0) ? topLeftX = topLeftX : topLeftX = 0;
                // int topLeftY = localizedRectangles[j].y - (localizedRectangles[j].height / 2);
                // (topLeftY >= 0) ? topLeftY = topLeftY : topLeftY = 0;
                // cv::Point topLeft(topLeftX, topLeftY);
                //
                // int bottomRightX = localizedRectangles[j].x + localizedRectangles[j].width +
                //     (localizedRectangles[j].width / 4);
                // (bottomRightX <= rawFrame.cols) ? bottomRightX = bottomRightX :
                //     bottomRightX = rawFrame.cols;
                // int bottomRightY = localizedRectangles[j].y + localizedRectangles[j].height +
                //     (localizedRectangles[j].height / 4);
                // (bottomRightY <= rawFrame.rows) ? bottomRightY = bottomRightY :
                //     bottomRightY = rawFrame.rows;
                // cv::Point bottomRight(bottomRightX, bottomRightY);
                // cv::Rect largerLocalization(topLeft, bottomRight);
                this->blobContainer[i].textImages.push_back(rawFrame(localizedRectangles[j]));
                std::cout << "hello7" << std::endl;
                if (this->blobContainer[i].pastPositions.size() == 50) {
                    this->blobContainer[i].pastPositions.erase(
                        this->blobContainer[i].pastPositions.begin());
                }
                cv::Point position;
                this->blobContainer[i].pastPositions.push_back(potentialPosition);
                // time(&this->blobContainer[i].endTime);
                std::cout << "Appended existing blob" << std::endl;
                break;
            }
        }
    }
    std::cout << "hello8" << std::endl;
    // Create new TextBlob
    for (int i = 0; i < localizedRectangles.size(); i++) {
        if (!rectTaken[i]) {
            std::cout << "hello9" << std::endl;
            aiSaac::TextBlob newTextBlob;
            this->ID++;
            newTextBlob.ID = ID;
            newTextBlob.frameCount = 1;
            newTextBlob.firstFrameNumber = frameNumber;
            newTextBlob.lastFrameNumber = frameNumber;
            newTextBlob.firstRectangle = localizedRectangles[i];
            newTextBlob.lastRectangle = localizedRectangles[i];
            cv::Point position;
            position.x = newTextBlob.lastRectangle.x + newTextBlob.lastRectangle.width / 2;
            position.y = newTextBlob.lastRectangle.y + newTextBlob.lastRectangle.height /2;
            std::cout << "hello10" << std::endl;
            newTextBlob.pastPositions.push_back(position);

            // int topLeftX = localizedRectangles[i].x - (localizedRectangles[i].width / 4);
            // (topLeftX >= 0) ? topLeftX = topLeftX : topLeftX = 0;
            // int topLeftY = localizedRectangles[i].y - (localizedRectangles[i].height / 2);
            // (topLeftY >= 0) ? topLeftY = topLeftY : topLeftY = 0;
            // cv::Point topLeft(topLeftX, topLeftY);
            //
            // int bottomRightX = localizedRectangles[i].x + localizedRectangles[i].width +
            //     (localizedRectangles[i].width / 4);
            // (bottomRightX <= rawFrame.cols) ? bottomRightX = bottomRightX :
            //     bottomRightX = rawFrame.cols;
            // int bottomRightY = localizedRectangles[i].y + localizedRectangles[i].height +
            //     (localizedRectangles[i].height / 4);
            // (bottomRightY <= rawFrame.rows) ? bottomRightY = bottomRightY :
            //     bottomRightY = rawFrame.rows;
            // cv::Point bottomRight(bottomRightX, bottomRightY);
            // cv::Rect largerLocalization(topLeft, bottomRight);
            std::cout << "hello11" << std::endl;
            newTextBlob.textImages.push_back(rawFrame(localizedRectangles[i]));
            std::cout << "hello12" << std::endl;
            newTextBlob.text = "?";
            this->blobContainer.push_back(newTextBlob);
            std::cout << "Added a new blob.. " << this->blobContainer.size() << std::endl;
        }
    }
}

void aiSaac::TextAnalytics::recognize() {
    for (int i = 0; i < this->blobContainer.size(); i++) {
        if (this->blobContainer[i].lastFrameNumber == this->analyzedFrameNumber) {
            this->blobContainer[i].text =
                this->recognize(this->blobContainer[i].textImages.back());
        }
    }
}

std::string aiSaac::TextAnalytics::recognize(const cv::Mat &rawFrame) {
    tess->SetImage((uchar*)rawFrame.data,
        rawFrame.size().width,
        rawFrame.size().height,
        rawFrame.channels(),
        rawFrame.step1());
    tess->Recognize(0);
    // const char* out = tess->GetUTF8Text();
    std::cout << "recognizing" << std::endl;
    tesseract::ResultIterator* ri = tess->GetIterator();
    tesseract::PageIteratorLevel level = tesseract::RIL_WORD;
    std::string predictedText = "";
    if (ri != 0) {
        do {
            const char* word = ri->GetUTF8Text(level);
            float conf = ri->Confidence(level);
            std::cout << "confidence = " << conf << std::endl;
            std::cout << "hello1" << std::endl << std::flush;
            if (conf < 50) {
                predictedText += "--";
            } else {
                std::string wordString(word);
                predictedText += wordString;
                predictedText += " ";
            }
            std::cout << "hello heloo" << std::endl;
        } while (ri->Next(level));
    }
    std::cout << "recognized" << std::endl;
    return predictedText;
}

void aiSaac::TextAnalytics::annotate(cv::Mat &rawFrame) {
    this->annotate(rawFrame, this->analyzedFrameNumber);
}

void aiSaac::TextAnalytics::annotate(cv::Mat &rawFrame, int frameNumber) {
    std::cout << "annotating blobs" << std::endl;
    for (int i = 0; i < this->blobContainer.size(); i++) {
        if (this->blobContainer[i].lastFrameNumber == frameNumber) {
            cv::rectangle(rawFrame,
                this->blobContainer[i].lastRectangle,
                cv::Scalar(255, 255, 255), 2);

            // bool fits = false;
            // std::vector<std::string> subTexts;
            // subTexts.push_back(this->blobContainer[i].text);
            // int fontFace = cv::FONT_HERSHEY_PLAIN;
            // int thickness = 1;
            // while (!fits) {
            //
            // }
            // std::string text = this->blobContainer[i].text;
            // double fontScale = 2;
            // int baseline = 0;
            // cv::Size textSize = cv::getTextSize(text, fontFace, fontScale, thickness, &baseline);
            // baseline += thickness;

            cv::putText(rawFrame,
                this->blobContainer[i].text,
                cv::Point(this->blobContainer[i].lastRectangle.x + 5,
                    this->blobContainer[i].lastRectangle.y + 30),
                cv::FONT_HERSHEY_PLAIN,
                2,
                CV_RGB(255, 255, 255),
                1);
            // for (int j = 1; j < this->blobContainer[i].pastPositions.size(); j++) {
            //     cv::Point pointOne((this->blobContainer[i].pastPositions[j - 1].x),
            //         (this->blobContainer[i].pastPositions[j - 1].y));
            //     cv::Point pointTwo((this->blobContainer[i].pastPositions[j].x),
            //         (blobContainer[i].pastPositions[j].y));
            //     cv::line(rawFrame, pointOne, pointTwo, cv::Scalar(0, 0, 255), 1, 8);
            // }
        }
    }
}

void aiSaac::TextAnalytics::annotate(cv::Mat &rawFrame, std::vector<cv::Rect> localizedRectangles) {
    for (int i = 0; i < localizedRectangles.size(); i++) {
        cv::rectangle(rawFrame, localizedRectangles[i], cv::Scalar(255, 255, 255), 2);
    }
}

void aiSaac::TextAnalytics::annotate(cv::Mat &rawFrame,
    std::vector<std::pair<std::string, cv::Rect>> localizedLabeledRectangles) {
    for (int i = 0; i < localizedLabeledRectangles.size(); i++) {
        // cv::rectangle(rawFrame, localizedLabeledRectangles[i].second, cv::Scalar(255, 255, 255), 2);

        cv::putText(rawFrame,
            localizedLabeledRectangles[i].first,
            cv::Point(localizedLabeledRectangles[i].second.x + 5,
                localizedLabeledRectangles[i].second.y + 30),
            cv::FONT_HERSHEY_PLAIN,
            3,
            CV_RGB(255, 255, 255),
            2);

        // cv::putText(rawFrame,
        //     localizedLabeledRectangles[i].first,
        //     cv::Point(localizedLabeledRectangles[i].second.x + 5,
        //         localizedLabeledRectangles[i].second.y + 15),
        //     cv::FONT_HERSHEY_PLAIN,
        //     1.0,
        //     CV_RGB(124, 252, 0),
        //     2.0);
    }
}

void aiSaac::TextAnalytics::loadAlgorithmType() {
    std::string textAlgo = this->aiSaacSettings.getTextAnalyticsAlgo();
    std::cout << "Text Analytics Algo: " << textAlgo << std::endl;
    if (textAlgo == "SWT+TES") {
        tess = new tesseract::TessBaseAPI();
        if (tess->Init(NULL, "eng", tesseract::OEM_DEFAULT)) {
            std::cout << "Sorry, english language not found for tesseract." << std::endl;
            return;
        }
    }
}

void aiSaac::TextAnalytics::runAlgo(const cv::Mat &rawFrame,
    int frameNumber,
    std::vector<cv::Rect> &localizedRectangles) {
    std::string textAlgo = this->aiSaacSettings.getTextAnalyticsAlgo();

    localizedRectangles.clear();
    if (textAlgo == "SWT+TES") {
      	ccv_dense_matrix_t* image = 0;
        ccv_read(rawFrame.data,
            &image,
            CCV_IO_BGR_RAW | CCV_IO_ANY_RAW | CCV_IO_GRAY,
            rawFrame.rows,
            rawFrame.cols,
            rawFrame.step[0]);
        ccv_enable_default_cache();
        // std::cout << "read image : " << std::flush;
		ccv_array_t* words = ccv_swt_detect_words(image, ccv_swt_default_params);
        // std::cout << "got words" << std::endl << std::flush;
		if (words) {
			int i;
            // add code to combine close by rectangles
			for (i = 0; i < words->rnum; i++) {
				ccv_rect_t* rect = (ccv_rect_t*) ccv_array_get(words, i);

                cv::Rect localizedRectangle(rect->x,
                    rect->y,
                    rect->width,
                    rect->height);

                int topLeftX = localizedRectangle.x
                    - this->aiSaacSettings.getTextRectDistanceThresh();
                (topLeftX >= 0) ? topLeftX = topLeftX : topLeftX = 0;
                int topLeftY = localizedRectangle.y;
                cv::Point topLeft(topLeftX, topLeftY);

                int bottomRightX = localizedRectangle.x
                    + localizedRectangle.width
                    + this->aiSaacSettings.getTextRectDistanceThresh();
                (bottomRightX <= rawFrame.cols) ? bottomRightX = bottomRightX :
                    bottomRightX = rawFrame.cols;
                int bottomRightY = localizedRectangle.y + localizedRectangle.height;
                cv::Point bottomRight(bottomRightX, bottomRightY);
                std::cout << "whadap whadap" << std::endl;
                cv::Rect largerLocalization(topLeft, bottomRight);
                bool taken = false;
                for (int j = 0; j < localizedRectangles.size(); j++) {
                    if ((localizedRectangles[j] & largerLocalization).area() > 0) {
                        std::cout << "merging" << std::endl;
                        localizedRectangles[j] = localizedRectangles[j] | largerLocalization;
                        // largerLocalization = localizedRectangles[j];
                        std::cout << "mergingcomplete" << std::endl;
                        taken = true;
                    }
                }
                if (!taken) {
                    std::cout << "no merge found pushing back" << std::endl;
                    localizedRectangles.push_back(largerLocalization);
                }
			}
            for (int i = 0; i < localizedRectangles.size(); i++) {
                std::cout << "increaseing localizedRectangles heights" << std::endl;
                localizedRectangles[i].y -= this->aiSaacSettings.getTextRectDistanceThresh();
                localizedRectangles[i].height += this->aiSaacSettings.getTextRectDistanceThresh();
                (localizedRectangles[i].x < 0) ? localizedRectangles[i].x = 0
                    : localizedRectangles[i].x = localizedRectangles[i].x;
                (localizedRectangles[i].y < 0) ? localizedRectangles[i].y = 0
                    : localizedRectangles[i].y = localizedRectangles[i].y;

                ((localizedRectangles[i].width + localizedRectangles[i].x) >= rawFrame.cols) ?
                    localizedRectangles[i].width = rawFrame.cols - localizedRectangles[i].x
                    : localizedRectangles[i].width = localizedRectangles[i].width;
                ((localizedRectangles[i].height + localizedRectangles[i].y) >= rawFrame.rows) ?
                    localizedRectangles[i].height = rawFrame.rows - localizedRectangles[i].y
                    : localizedRectangles[i].height = localizedRectangles[i].height;
            }
            std::cout << "done increasing" << std::endl;
			ccv_array_free(words);
            std::cout << "freed words" << std::endl;
		}
        std::cout << "done iterating" << std::endl;
		ccv_matrix_free(image);
        std::cout << "freed image" << std::endl;
    }
    std::cout << "done with everything" << std::endl;
    this->analyzedFrameNumber = frameNumber;
}

void aiSaac::TextAnalytics::preProcessBlobContainer(int frameNumber) {
    if (this->blobContainer.size() > 400) {  //add to aiSaac settings
        this->blobContainer.erase(this->blobContainer.begin(), this->blobContainer.begin() + 200);
    }

    // Remove old objects with few frames
    for (std::vector<aiSaac::TextBlob>::iterator it = this->blobContainer.begin();
        it != this->blobContainer.end();) {
        if (frameNumber - it->lastFrameNumber > this->aiSaacSettings.getTextBlobDeleteTime()) {
            it = this->blobContainer.erase(it);
        } else {
            ++it;
        }
    }
}

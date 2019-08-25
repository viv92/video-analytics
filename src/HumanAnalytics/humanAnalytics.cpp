/*
    Copyright 2016 AITOE
*/

/*
    TODO:
    1. Add all Algo Parameters in runAlgo() to aiSaacSettings
*/

#include "humanAnalytics.h"

static unsigned int get_current_time(void) {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

aiSaac::HumanAnalytics::HumanAnalytics(aiSaac::AiSaacSettings *aiSaacSettings) {
    this->aiSaacSettings = aiSaacSettings;
    this->initialize();
}

aiSaac::HumanAnalytics::~HumanAnalytics() {}

void aiSaac::HumanAnalytics::initialize() {
    this->analyzedFrameNumber = 0;
    this->blobContainer.clear();
    this->colorClassifier = new ColorClassifier();
    this->loadAlgorithmType();
}

void aiSaac::HumanAnalytics::runAlgo(const cv::Mat &rawFrame,
    int frameNumber,
    std::vector<cv::Rect> &localizedRectangles) {
    std::string humanDetectAlgo = this->aiSaacSettings->getHumanDetectAnalyticsAlgo();
    if (humanDetectAlgo == "HOG") {
        localizedRectangles.clear();
        std::vector<cv::Rect> found, found_filtered;
      	this->hog.detectMultiScale(rawFrame,
            found,
            0,
            cv::Size(8, 8),
            cv::Size(32, 32),
            1.05,
            2);
      	for (int i = 0; i < found.size(); i++) {
            cv::Rect r = found[i];
            int j;
      		for (j = 0; j < found.size(); j++) {
      			if (j != i && (r & found[j]) == r) {
      				break;
                }
            }
      		if (j == found.size()) {
      			found_filtered.push_back(r);
            }
        }
      	for (int i = 0; i < found_filtered.size(); i++) {
      		cv::Rect r = found_filtered[i];
      		r.x += cvRound(r.width * 0.1);
      		r.width = cvRound(r.width * 0.8);
      		r.y += cvRound(r.height * 0.06);
      		r.height = cvRound(r.height * 0.9);
            if (r.x < 0) {
                r.x = 0;
            }
            if (r.y < 0) {
                r.y = 0;
            }
            if ((r.x + r.width) > rawFrame.cols) {
                r.width = rawFrame.cols - r.x;
            }
            if ((r.y + r.height) > rawFrame.rows) {
                r.height = rawFrame.rows - r.y;
            }
      		localizedRectangles.push_back(r);
        }
    } else if (humanDetectAlgo == "HAAR_UPPERBODY") {
        cv::Mat frame_gray;
        cvtColor(rawFrame, frame_gray, CV_BGR2GRAY );
        equalizeHist(frame_gray, frame_gray );
        this->upperbody_cascade.detectMultiScale(frame_gray,
            localizedRectangles,
            1.1,  // algo param
            3,  // algo param
            CV_HAAR_SCALE_IMAGE);
    } else if (humanDetectAlgo == "HAAR_LOWERBODY") {
        cv::Mat frame_gray;
        cv::cvtColor(rawFrame, frame_gray, CV_BGR2GRAY );
        cv::equalizeHist(frame_gray, frame_gray );
        this->lowerbody_cascade.detectMultiScale(frame_gray,
            localizedRectangles,
            1.1,  // algo param
            3,  // algo param
            CV_HAAR_SCALE_IMAGE);
    } else if (humanDetectAlgo == "HAAR_FULLBODY") {
        cv::Mat frame_gray;
        cv::cvtColor(rawFrame, frame_gray, CV_BGR2GRAY );
        cv::equalizeHist(frame_gray, frame_gray );
        this->fullbody_cascade.detectMultiScale(frame_gray,
            localizedRectangles,
            1.1, // algo param
            3, // algo param
            CV_HAAR_SCALE_IMAGE);
    } else if (humanDetectAlgo == "DPM") {
      	ccv_enable_default_cache();
      	ccv_dense_matrix_t* image = 0;
        ccv_read(rawFrame.data,
            &image,
            CCV_IO_BGR_RAW,
            rawFrame.rows,
            rawFrame.cols,
            rawFrame.step[0]);
        //ccv_read(argv[1], &image, CCV_IO_ANY_FILE);
  		unsigned int elapsed_time = get_current_time();
  		ccv_array_t* seq = ccv_dpm_detect_objects(image,
            &dpmmodel,
            1,
            ccv_dpm_params);
  		elapsed_time = get_current_time() - elapsed_time;
  		if (seq) {
  			for (int i = 0; i < seq->rnum; i++) {
  				ccv_root_comp_t* comp = (ccv_root_comp_t*)ccv_array_get(seq, i);
  				printf("%d %d %d %d %f %d\n",
                    comp->rect.x,
                    comp->rect.y,
                    comp->rect.width,
                    comp->rect.height,
                    comp->classification.confidence,
                    comp->pnum);
                localizedRectangles.push_back(cv::Rect(comp->rect.x, comp->rect.y, comp->rect.width, comp->rect.height));
  				for (int j = 0; j < comp->pnum; j++) {
  					printf("| %d %d %d %d %f\n",
                        comp->part[j].rect.x,
                        comp->part[j].rect.y,
                        comp->part[j].rect.width,
                        comp->part[j].rect.height,
                        comp->part[j].classification.confidence);
                }
  			}
  			printf("total : %d in time %dms\n", seq->rnum, elapsed_time);
  			ccv_array_free(seq);
        } else {
  			printf("elapsed time %dms\n", elapsed_time);
  		}
    } else if (humanDetectAlgo == "ICF") {
        cv::Mat frame = rawFrame.clone();
        ccv_dense_matrix_t* image = 0;
        ccv_read(frame.data, &image, CCV_IO_BGR_RAW, frame.rows, frame.cols, frame.step[0]);
    	ccv_enable_default_cache();
    	unsigned int elapsed_time = get_current_time();
        ccv_array_t* seq = ccv_icf_detect_objects(image, &icfcascade, 1, ccv_icf_params);
  		elapsed_time = get_current_time() - elapsed_time;
        for (int i = 0; i < seq->rnum; i++) {
  			ccv_comp_t* comp = (ccv_comp_t*)ccv_array_get(seq, i);
  			printf("%d %d %d %d %f\n",
                comp->rect.x,
                comp->rect.y,
                comp->rect.width,
                comp->rect.height,
                comp->classification.confidence);
        localizedRectangles.push_back(cv::Rect(comp->rect.x,
            comp->rect.y,
            comp->rect.width,
            comp->rect.height));
        }
        printf("total : %d in time %dms\n", seq->rnum, elapsed_time);
        ccv_array_free(seq);
        ccv_matrix_free(image);
    }
    this->analyzedFrameNumber = frameNumber;
    return;
}

int aiSaac::HumanAnalytics::detect(const cv::Mat &rawFrame, int frameNumber,
    cv::Rect areaOfInterest) {
    std::vector<cv::Rect> localizedRectangles;
    if (areaOfInterest.area() != 0) {
      cv::Mat subRawFrame = rawFrame(areaOfInterest);
      this->detect(subRawFrame, frameNumber);
      for (int i = 0; i < localizedRectangles.size(); i++) {
          localizedRectangles[i].x += areaOfInterest.x;
          localizedRectangles[i].y += areaOfInterest.y;
        }
      }
      else
      {
        this->runAlgo(rawFrame, frameNumber, localizedRectangles);
      }
    return localizedRectangles.size();
}

void aiSaac::HumanAnalytics::localize(const cv::Mat &rawFrame,
    int frameNumber,
    std::vector<cv::Rect> &localizedRectangles,
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

void aiSaac::HumanAnalytics::track(const cv::Mat &rawFrame,
    int frameNumber,
    cv::Rect areaOfInterest) {
    // codeReview(Anurag): should we do this when we loop through the blobContainer in this code
    //      block?
    this->preProcessBlobContainer(frameNumber);
    std::vector<cv::Rect> localizedRectangles;
    this->localize(rawFrame, frameNumber, localizedRectangles, areaOfInterest);

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
                cv::Rect shirtRectangle(
                    cv::Point(potentialPosition.x - localizedRectangles[j].width / 6,
                        potentialPosition.y - localizedRectangles[j].height / 3),
                    cv::Point(potentialPosition.x + localizedRectangles[j].width / 6,
                        potentialPosition.y - localizedRectangles[j].height / 6));
                this->blobContainer[i].shirtColor =
                    this->colorClassifier->patchColor(rawFrame, shirtRectangle);
                std::cout << "final shirt color: "
                    << this->blobContainer[i].shirtColor
                    << std::endl;

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

    // Create new HumanBlob
    for (int i = 0; i < localizedRectangles.size(); i++) {
        if (!rectTaken[i]) {
            aiSaac::HumanBlob newHumanBlob;
            this->ID++;
            newHumanBlob.ID = ID;
            newHumanBlob.frameCount = 1;
            newHumanBlob.firstFrameNumber = frameNumber;
            newHumanBlob.lastFrameNumber = frameNumber;
            newHumanBlob.firstRectangle = localizedRectangles[i];
            newHumanBlob.lastRectangle = localizedRectangles[i];
            cv::Point position;
            position.x = newHumanBlob.lastRectangle.x + newHumanBlob.lastRectangle.width / 2;
            position.y = newHumanBlob.lastRectangle.y + newHumanBlob.lastRectangle.height /2;
            newHumanBlob.pastPositions.push_back(position);

             cv::Rect shirtRectangle(
                    cv::Point(position.x - localizedRectangles[i].width / 6,
                        position.y - localizedRectangles[i].height / 3),
                    cv::Point(position.x + localizedRectangles[i].width / 6,
                        position.y - localizedRectangles[i].height / 6));

                newHumanBlob.shirtColor =
                    this->colorClassifier->patchColor(rawFrame, shirtRectangle);

                std::cout << "final shirt color: "
                    << newHumanBlob.shirtColor
                    << std::endl;

            this->blobContainer.push_back(newHumanBlob);
            std::cout << "Added a new blob.. " << this->blobContainer.size() << std::endl;
        }
    }
}

void aiSaac::HumanAnalytics::annotate(cv::Mat &rawFrame, cv::Rect &localizedRectangle) {
    this->annotate(rawFrame, localizedRectangle, this->analyzedFrameNumber);
}

void aiSaac::HumanAnalytics::annotate(cv::Mat &rawFrame, cv::Rect &localizedRectangle, int frameNumber) {
            cv::rectangle(rawFrame,
                localizedRectangle,
                cv::Scalar(255, 192, 203), 2);
}

void aiSaac::HumanAnalytics::annotate(cv::Mat &rawFrame) {
    this->annotate(rawFrame, this->analyzedFrameNumber);
}

void aiSaac::HumanAnalytics::annotate(cv::Mat &rawFrame, int frameNumber) {
    for (int i = 0; i < this->blobContainer.size(); i++) {
        if (this->blobContainer[i].lastFrameNumber == frameNumber) {
            cv::rectangle(rawFrame,
                this->blobContainer[i].lastRectangle,
                cv::Scalar(255, 192, 203), 2);

            cv::Point potentialPosition(this->blobContainer[i].lastRectangle.x + this->blobContainer[i].lastRectangle.width / 2,
                this->blobContainer[i].lastRectangle.y + this->blobContainer[i].lastRectangle.height / 2);

            cv::Rect shirtRectangle(
                    cv::Point(potentialPosition.x - this->blobContainer[i].lastRectangle.width / 6,
                        potentialPosition.y - this->blobContainer[i].lastRectangle.height / 3),
                    cv::Point(potentialPosition.x + this->blobContainer[i].lastRectangle.width / 6,
                        potentialPosition.y - this->blobContainer[i].lastRectangle.height / 6));
            cv::rectangle(rawFrame,
                shirtRectangle,
                cv::Scalar(0, 255, 0), 2);

             int baseline = 0;
                int fontScale = ceil(rawFrame.cols * 0.0003);
                int thickness = ceil(rawFrame.cols * 0.0025);

                cv::Size textSize = cv::getTextSize(this->blobContainer[i].shirtColor,
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
                    textOrg.y = 0 + textSize.height;
                }
                cv::putText(rawFrame,
                    this->blobContainer[i].shirtColor,
                    textOrg,
                    cv::FONT_HERSHEY_SIMPLEX,
                    fontScale,
                    cv::Scalar(127, 255, 0),
                    thickness);

        }
    }
}

void aiSaac::HumanAnalytics::preProcessBlobContainer(int frameNumber) {
    if (this->blobContainer.size() > 400) {  // add to aiSaac settings
        this->blobContainer.erase(this->blobContainer.begin(), this->blobContainer.begin() + 200);
    }

    // Remove old objects with few frames
    for (std::vector<aiSaac::HumanBlob>::iterator it = this->blobContainer.begin();
        it != this->blobContainer.end();) {
        if (frameNumber - it->lastFrameNumber > this->aiSaacSettings->getHumanBlobDeleteTime()
            && it->frames.size() < this->aiSaacSettings->getMinimumHumanBlobFrames()) {
            it = this->blobContainer.erase(it);
        } else {
            ++it;
        }
    }
}

void aiSaac::HumanAnalytics::loadAlgorithmType() {
    std::string humanDetectAlgo = this->aiSaacSettings->getHumanDetectAnalyticsAlgo();
    std::cout << "Human Analytics Algo: " << humanDetectAlgo << std::endl;

    if (humanDetectAlgo == "HOG") {
        this->hog.setSVMDetector(cv::HOGDescriptor::getDefaultPeopleDetector());
    } else if (humanDetectAlgo == "HAAR_UPPERBODY") {
        if (!this->upperbody_cascade.load(this->aiSaacSettings->getHaarCascadeFilePath()
            + "haarcascade_upperbody.xml")) {
            printf("--(!)Error loading\n");
        }
    } else if (humanDetectAlgo == "HAAR_LOWERBODY"){
        if (!this->lowerbody_cascade.load(this->aiSaacSettings->getHaarCascadeFilePath()
            + "haarcascade_lowerbody.xml")) {
            printf("--(!)Error loading\n");
        }
    } else if (humanDetectAlgo == "HAAR_FULLBODY") {
        if (!this->fullbody_cascade.load(this->aiSaacSettings->getHaarCascadeFilePath()
            + "haarcascade_fullbody.xml")) {
            printf("--(!)Error loading\n");
        }
    } else if (humanDetectAlgo == "DPM") {
        this->dpmmodel = ccv_dpm_read_mixture_model(
            this->aiSaacSettings->getDPMPedestrianModelFilePath().c_str());
        ccv_dpm_params = {
            .interval = 8,
            .min_neighbors = 1,
            .flags = 0,
            .threshold = 0.4, // 0.8
        };
    } else if (humanDetectAlgo == "ICF") {
        this->icfcascade = ccv_icf_read_classifier_cascade(
                this->aiSaacSettings->getICFPedestrianModelFilePath().c_str());
        ccv_icf_params = {
        	.min_neighbors = 1,
            .flags = 0,
        	.step_through = 1,
            .interval = 8,
            .threshold = 0
        };
    }
}

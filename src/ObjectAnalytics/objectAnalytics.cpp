/*
    Copyright 2016 AITOE
*/

// Code Review: (Rishabh) Color Analytics must only be performed here if it has been set in aisaac settings.
#include "objectAnalytics.h"

// aiSaac::ObjectAnalytics::ObjectAnalytics(aiSaac::AiSaacSettings &aiSaacSettings) : aiSaacSettings(aiSaacSettings) {
//     std::cout << "now initializing object analytics" << std::endl;
//     this->initialize();
// }
//

aiSaac::ObjectAnalytics::ObjectAnalytics(aiSaac::AiSaacSettings &aiSaacSettings) :
    aiSaacSettings(aiSaacSettings) {
    this->initialize();
    std::cout << "initializing algorithm type" << std::endl;
    this->loadAlgorithmType();
    std::cout << "loaded and initialized objectAnalytics" << std::endl;
}

#ifdef CAFFE_DARKNET
    aiSaac::ObjectAnalytics::ObjectAnalytics(aiSaac::AiSaacSettings &aiSaacSettings, aiSaac::CaffeClassifier &caffeClassifier, aiSaac::DarknetClassifier &darknetClassifier) : aiSaacSettings(aiSaacSettings) {
         std::cout << "now initializing object analytics with caffe and darknet classifiers" << std::endl;
         this->initialize(caffeClassifier, darknetClassifier);
#endif

#ifdef CAFFE
    aiSaac::ObjectAnalytics::ObjectAnalytics(aiSaac::AiSaacSettings &aiSaacSettings,
        aiSaac::CaffeClassifier &caffeClassifier):
        aiSaacSettings(aiSaacSettings) {
        std::cout << "initializing object analytics with pre-existing caffe classifier" << std::endl;
        this->initialize();
        this->caffeClassifier = &caffeClassifier;
    }
#endif

#ifdef DARKNET
    aiSaac::ObjectAnalytics::ObjectAnalytics(aiSaac::AiSaacSettings &aiSaacSettings,
        aiSaac::DarknetClassifier &darknetClassifier):
        aiSaacSettings(aiSaacSettings) {
        std::cout << "initializing object analytics with pre-existing darknet classifier" << std::endl;
        this->initialize();
        this->darknetClassifier = &darknetClassifier;
    }
#endif

#ifdef DNN_CLASSIFIER
    aiSaac::ObjectAnalytics::ObjectAnalytics(aiSaac::AiSaacSettings &aiSaacSettings,
        aiSaac::DNNClassifier &dnnClassifier):
        aiSaacSettings(aiSaacSettings) {
        std::cout << "initializing object analytics with pre-existing dnn classifier" << std::endl;
        this->initialize();
        this->dnnClassifier = &dnnClassifier;
    }
#endif

#ifdef CAFFE_DARKNET
    aiSaac::ObjectAnalytics::ObjectAnalytics(aiSaac::AiSaacSettings &aiSaacSettings,
        aiSaac::CaffeClassifier &caffeClassifier,
        aiSaac::DarknetClassifier &darknetClassifier) :
        aiSaacSettings(aiSaacSettings) {
        std::cout << "now initializing object analytics" << std::endl;
        std::cout << "~~~~Using deprecated functionality~~~~" << std::endl;
        this->initialize();
        this->caffeClassifier = &caffeClassifier;
        this->darknetClassifier = &darknetClassifier;
    }
#endif

aiSaac::ObjectAnalytics::~ObjectAnalytics() {
    this->blobContainer.clear();
    if (loadedAlgos) {
        //if (this->caffeClassifier) {
        //  delete this->caffeClassifier;
        //}
        //if (this->darknetClassifier) {
        //  delete this->darknetClassifier;
        //}
        // codeReview(Rishabh): The pointer objects here are borrowed pointers, i.e. defined via references of objects defined in the main loop. They are not initialized with a new. WE SHOULD NOT BE DELETING THEM`

        //if (this->dnnClassifier) {
        //  delete this->dnnClassifier;
        //}
    }
}

void aiSaac::ObjectAnalytics::initialize() {
    this->analyzedFrameNumber = 0;
    this->ID = 0;
    this->blobContainer.clear();
}

std::vector<std::string> aiSaac::ObjectAnalytics::detect(const cv::Mat &rawFrame,
    int frameNumber,
    cv::Rect areaOfInterest) {
    std::vector<aiSaac::ObjectBlob> detectedObjects;

    if (areaOfInterest.area() != 0) {
        cv::Mat subRawFrame = rawFrame(areaOfInterest);
        this->runAlgo(subRawFrame, frameNumber, detectedObjects);
    } else {
        this->runAlgo(rawFrame, frameNumber, detectedObjects);
    }
    std::vector<std::string> predictionLabels;
    for (int i = 0; i < detectedObjects.size(); i++) {
        std::cout << "DetectedObjectLabel: " << detectedObjects[i].label << std::endl;
        predictionLabels.push_back(detectedObjects[i].label);
    }
    return predictionLabels;
}

void aiSaac::ObjectAnalytics::localize(const cv::Mat &rawFrame,
    std::vector<std::pair<std::string, cv::Rect>> &localizedLabeledRectangles,
    int frameNumber,
    cv::Rect areaOfInterest) {
    std::vector<aiSaac::ObjectBlob> detectedObjects;
    if (areaOfInterest.area() != 0) {
        cv::Mat subRawFrame = rawFrame(areaOfInterest);
        this->runAlgo(subRawFrame, frameNumber, detectedObjects);
    } else {
        this->runAlgo(rawFrame, frameNumber, detectedObjects);
    }

    for (int i = 0; i < detectedObjects.size(); i++) {
        std::pair<std::string, cv::Rect> localizedLabeledRectangle;
        localizedLabeledRectangle.first = detectedObjects[i].label;
        localizedLabeledRectangle.second = detectedObjects[i].lastRectangle;
        localizedLabeledRectangles.push_back(localizedLabeledRectangle);
    }
}

void aiSaac::ObjectAnalytics::track(const cv::Mat &rawFrame,
    int frameNumber,
    cv::Rect areaOfInterest) {
    this->preProcessBlobContainer(frameNumber);

    std::vector<aiSaac::ObjectBlob> detectedObjects;
    if (areaOfInterest.area() != 0) {
        cv::Mat subRawFrame = rawFrame(areaOfInterest);
        this->runAlgo(subRawFrame, frameNumber, detectedObjects);
    } else {
        this->runAlgo(rawFrame, frameNumber, detectedObjects);
    }

    std::vector<int> rectTaken(detectedObjects.size(), 0);
    for (int i = 0; i < this->blobContainer.size(); i++) {
        cv::Point latestPosition = this->blobContainer[i].pastPositions.back();
        for (int j = 0; j < detectedObjects.size(); j++) {
            if (rectTaken[j] == 1) {
                continue;
            }

            cv::Point potentialPosition(
                detectedObjects[j].lastRectangle.x + detectedObjects[j].lastRectangle.width/2,
                detectedObjects[j].lastRectangle.y + detectedObjects[j].lastRectangle.height/2);

            if ((this->blobContainer[i].label == detectedObjects[j].label)
                && (abs(potentialPosition.x - latestPosition.x) < (rawFrame.size().width / 3))
                && (abs(potentialPosition.y - latestPosition.y) < (rawFrame.size().height / 3))) {
                rectTaken[j] = 1;
                this->blobContainer[i].lastFrameNumber = frameNumber;
                this->blobContainer[i].frameCount++;
                if ((this->blobContainer[i].frameCount == this->aiSaacSettings.getMinimumObjectBlobFrames()) && (this->blobContainer[i].label == "person" || this->blobContainer[i].label == "cat" || this->blobContainer[i].label == "dog" || this->blobContainer[i].label == "car" || this->blobContainer[i].label == "motorbike" || this->blobContainer[i].label == "bus" || this->blobContainer[i].label == "bicycle")) {
                  this->blobContainer[i].isValid = true;
                }
                this->blobContainer[i].lastRectangle = detectedObjects[j].lastRectangle;

                if (this->blobContainer[i].pastPositions.size() == 50) {
                    this->blobContainer[i].pastPositions.erase(
                        this->blobContainer[i].pastPositions.begin());
                }
                cv::Point position;
                this->blobContainer[i].pastPositions.push_back(potentialPosition);
                // time(&this->blobContainer[i].endTime);

                //sublocalisation for object color
                cv::Rect colorRoi(
                    cv::Point(potentialPosition.x - detectedObjects[j].lastRectangle.width / 10,
                        potentialPosition.y - detectedObjects[j].lastRectangle.height / 10),
                    cv::Point(potentialPosition.x + detectedObjects[j].lastRectangle.width / 10,
                        potentialPosition.y + detectedObjects[j].lastRectangle.height / 10));

                std::cout << "colorRoi.width: " << colorRoi.width << " colorRoi.height:" << colorRoi.height << std::endl << std::flush;
                if (colorRoi.width > 0 && colorRoi.height > 0) {
                std::string color =
                    this->colorClassifier->patchColor(
                        rawFrame,
                        colorRoi
                    );
                    // this->blobContainer[i].metaData.push_back(this->blobContainer[i].label);
                    // this->blobContainer[i].metaData.push_back(color);
                    this->blobContainer[i].metaData[0] = this->blobContainer[i].label;
                    this->blobContainer[i].metaData[1] = color;
                }
                //        std::cout << "Color from Blob: " << this->blobContainer[i].metaData << std::endl;

                //this->blobContainer[i].metaData = "[" + std::to_string(this->blobContainer[i].ID) + "] " + this->blobContainer[i].label;

                std::cout << "Appended existing blob" << std::endl;
                break;
            }
        }
    }

    // Create new ObjectBlob
    for (int i = 0; i < detectedObjects.size(); i++) {
        if (!rectTaken[i]) {
            aiSaac::ObjectBlob newObjectBlob;
            this->ID++;
            newObjectBlob.ID = ID;
            newObjectBlob.frameCount = 1;
            newObjectBlob.isValid = false;
            newObjectBlob.label = detectedObjects[i].label;
            newObjectBlob.firstFrameNumber = frameNumber;
            newObjectBlob.lastFrameNumber = frameNumber;
            newObjectBlob.firstRectangle = detectedObjects[i].lastRectangle;
            newObjectBlob.lastRectangle = detectedObjects[i].lastRectangle;
            cv::Point position;
            position.x = newObjectBlob.lastRectangle.x + newObjectBlob.lastRectangle.width / 2;
            position.y = newObjectBlob.lastRectangle.y + newObjectBlob.lastRectangle.height /2;
            newObjectBlob.pastPositions.push_back(position);

            //sublocalisation for object color
            cv::Rect colorRoi(
                cv::Point(position.x - detectedObjects[i].lastRectangle.width / 10,
                    position.y - detectedObjects[i].lastRectangle.height / 10),
                cv::Point(position.x + detectedObjects[i].lastRectangle.width / 10,
                    position.y + detectedObjects[i].lastRectangle.height / 10));

            std::cout << "colorRoi.width: " << colorRoi.width << " colorRoi.height:" << colorRoi.height << std::endl << std::flush;
            if (colorRoi.width > 0 && colorRoi.height > 0) {
              std::string color =
                  this->colorClassifier->patchColor(
                      rawFrame,
                      colorRoi
                  );
                  newObjectBlob.metaData.push_back(newObjectBlob.label);
                  newObjectBlob.metaData.push_back(color);
            }
            //newObjectBlob.metaData = "[" + std::to_string(newObjectBlob.ID) + "] " + newObjectBlob.label;

            this->blobContainer.push_back(newObjectBlob);
            // std::cout << "Added a new blob.. " << this->blobContainer.size() << std::endl;
        }
    }
}

//track function overloaded for motionFrameNumber
void aiSaac::ObjectAnalytics::track(const cv::Mat &rawFrame,
    int frameNumber, int motionFrameNumber,
    cv::Rect areaOfInterest) {
    this->preProcessBlobContainerMotion(motionFrameNumber);

    std::vector<aiSaac::ObjectBlob> detectedObjects;
    if (areaOfInterest.area() != 0) {
        cv::Mat subRawFrame = rawFrame(areaOfInterest);
        this->runAlgo(subRawFrame, frameNumber, detectedObjects);
    } else {
        this->runAlgo(rawFrame, frameNumber, detectedObjects);
    }

    std::vector<int> rectTaken(detectedObjects.size(), 0);
    for (int i = 0; i < this->blobContainer.size(); i++) {
        cv::Point latestPosition = this->blobContainer[i].pastPositions.back();
        for (int j = 0; j < detectedObjects.size(); j++) {
            if (rectTaken[j] == 1) {
                continue;
            }

            cv::Point potentialPosition(
                detectedObjects[j].lastRectangle.x + detectedObjects[j].lastRectangle.width/2,
                detectedObjects[j].lastRectangle.y + detectedObjects[j].lastRectangle.height/2);

            if ((this->blobContainer[i].label == detectedObjects[j].label)
                && (abs(potentialPosition.x - latestPosition.x) < (rawFrame.size().width / 3))
                && (abs(potentialPosition.y - latestPosition.y) < (rawFrame.size().height / 3))) {
                rectTaken[j] = 1;
                this->blobContainer[i].lastFrameNumber = frameNumber;
                this->blobContainer[i].motionFrameNumber = motionFrameNumber;
                this->blobContainer[i].frameCount++;
                if ((this->blobContainer[i].frameCount == this->aiSaacSettings.getMinimumObjectBlobFrames()) && (this->blobContainer[i].label == "person" || this->blobContainer[i].label == "cat" || this->blobContainer[i].label == "dog" || this->blobContainer[i].label == "car" || this->blobContainer[i].label == "motorbike" || this->blobContainer[i].label == "bus" || this->blobContainer[i].label == "bicycle")) {
                  this->blobContainer[i].isValid = true;
                }
                this->blobContainer[i].lastRectangle = detectedObjects[j].lastRectangle;

                if (this->blobContainer[i].pastPositions.size() == 50) {
                    this->blobContainer[i].pastPositions.erase(
                        this->blobContainer[i].pastPositions.begin());
                }
                cv::Point position;
                this->blobContainer[i].pastPositions.push_back(potentialPosition);
                // time(&this->blobContainer[i].endTime);

                //sublocalisation for object color
                cv::Rect colorRoi(
                    cv::Point(potentialPosition.x - detectedObjects[j].lastRectangle.width / 10,
                        potentialPosition.y - detectedObjects[j].lastRectangle.height / 10),
                    cv::Point(potentialPosition.x + detectedObjects[j].lastRectangle.width / 10,
                        potentialPosition.y + detectedObjects[j].lastRectangle.height / 10));

                std::cout << "colorRoi.width: " << colorRoi.width << " colorRoi.height:" << colorRoi.height << std::endl << std::flush;
                if (colorRoi.width > 0 && colorRoi.height > 0) {
                std::string color =
                    this->colorClassifier->patchColor(
                        rawFrame,
                        colorRoi
                    );
                    // this->blobContainer[i].metaData.push_back(this->blobContainer[i].label);
                    // this->blobContainer[i].metaData.push_back(color);
                    this->blobContainer[i].metaData[0] = this->blobContainer[i].label;
                    this->blobContainer[i].metaData[1] = color;
                }
                //        std::cout << "Color from Blob: " << this->blobContainer[i].metaData << std::endl;

                //this->blobContainer[i].metaData = "[" + std::to_string(this->blobContainer[i].ID) + "] " + this->blobContainer[i].label;

                std::cout << "Appended existing blob" << std::endl;
                break;
            }
        }
    }

    // Create new ObjectBlob
    for (int i = 0; i < detectedObjects.size(); i++) {
        if (!rectTaken[i]) {
            aiSaac::ObjectBlob newObjectBlob;
            this->ID++;
            newObjectBlob.ID = ID;
            newObjectBlob.frameCount = 1;
            newObjectBlob.isValid = false;
            newObjectBlob.label = detectedObjects[i].label;
            newObjectBlob.firstFrameNumber = frameNumber;
            newObjectBlob.lastFrameNumber = frameNumber;
            newObjectBlob.motionFrameNumber = motionFrameNumber;
            newObjectBlob.firstRectangle = detectedObjects[i].lastRectangle;
            newObjectBlob.lastRectangle = detectedObjects[i].lastRectangle;
            cv::Point position;
            position.x = newObjectBlob.lastRectangle.x + newObjectBlob.lastRectangle.width / 2;
            position.y = newObjectBlob.lastRectangle.y + newObjectBlob.lastRectangle.height /2;
            newObjectBlob.pastPositions.push_back(position);

            //sublocalisation for object color
            cv::Rect colorRoi(
                cv::Point(position.x - detectedObjects[i].lastRectangle.width / 10,
                    position.y - detectedObjects[i].lastRectangle.height / 10),
                cv::Point(position.x + detectedObjects[i].lastRectangle.width / 10,
                    position.y + detectedObjects[i].lastRectangle.height / 10));

            std::cout << "colorRoi.width: " << colorRoi.width << " colorRoi.height:" << colorRoi.height << std::endl << std::flush;
            if (colorRoi.width > 0 && colorRoi.height > 0) {
              std::string color =
                  this->colorClassifier->patchColor(
                      rawFrame,
                      colorRoi
                  );
                  newObjectBlob.metaData.push_back(newObjectBlob.label);
                  newObjectBlob.metaData.push_back(color);
            }
            //newObjectBlob.metaData = "[" + std::to_string(newObjectBlob.ID) + "] " + newObjectBlob.label;

            this->blobContainer.push_back(newObjectBlob);
            // std::cout << "Added a new blob.. " << this->blobContainer.size() << std::endl;
        }
    }
}

// Computes IOU between two bounding boxes
double GetIOU(Rect_<float> bb_test, Rect_<float> bb_gt) {
  std::cout << "in IOU" << std::endl << std::flush;
	float in = (bb_test & bb_gt).area();
	float un = bb_test.area() + bb_gt.area() - in;

	if (un < DBL_EPSILON)
		return 0;

	return (double)(in / un);
}

//global variables for SORT Tracking
typedef struct TrackingBox
{
	int frame;
	int id;
  string metaData;
	Rect_<float> box;
}TrackingBox;

static int frame_count = 0;
static int max_age = 100;
static int min_hits = 3;
static double iouThreshold = 0.3;
vector<KalmanTracker> trackers;

void aiSaac::ObjectAnalytics::trackSort(const cv::Mat &rawFrame, int frameNumber, cv::Rect areaOfInterest) {
  // variables used
  vector<Rect_<float>> predictedBoxes;
  vector<vector<double>> iouMatrix;
  vector<int> assignment;
  set<int> unmatchedDetections;
  set<int> unmatchedTrajectories;
  set<int> allItems;
  set<int> matchedItems;
  vector<cv::Point> matchedPairs;
  unsigned int trkNum = 0;
  unsigned int detNum = 0;

  //preProcess BlobContainer
  this->preProcessBlobContainer(frameNumber);
  std::vector<aiSaac::ObjectBlob> detectedObjects;
  std::cout << "done preprocessing" << std::endl << std::flush;
  //runalgo and get detections for the frame
  if (areaOfInterest.area() != 0) {
      cv::Mat subRawFrame = rawFrame(areaOfInterest);
      this->runAlgo(subRawFrame, frameNumber, detectedObjects);
  } else {
      this->runAlgo(rawFrame, frameNumber, detectedObjects);
  }
  std::cout << "done runalgo" << std::endl << std::flush;
  //initialise trackers if empty
  if (trackers.size() == 0) {
    std::cout << "initialise tracker" << std::endl << std::flush;
    // initialize kalman trackers using detections
    for (unsigned int i = 0; i < detectedObjects.size(); i++) {
      KalmanTracker trk = KalmanTracker(detectedObjects[i].lastRectangle, detectedObjects[i].label);
      trackers.push_back(trk);
    }
    return;
  }

  //get predicted locations from existing trackers.
  std::cout << "get predicted locations from existing trackers" << std::endl << std::flush;
  predictedBoxes.clear();
  for (auto it = trackers.begin(); it != trackers.end();) {
    Rect_<float> pBox = (*it).predict();
    std::cout << "pBox.x=" << pBox.x << "pBox.y=" << pBox.y << std::endl << std::flush;
    if (pBox.x >= 0 && pBox.y >= 0) {
      predictedBoxes.push_back(pBox);
      it++;
    } else {
      it = trackers.erase(it);

      //reinitialise trackers if size becomes zero after erase
      if (trackers.size() == 0) {
        for (unsigned int i = 0; i < detectedObjects.size(); i++) {
          KalmanTracker trk = KalmanTracker(detectedObjects[i].lastRectangle, detectedObjects[i].label);
          trackers.push_back(trk);
        }
        return;
      }
    }
  }

  //create IOU Matrix
  std::cout << "create IOU Matrix" << std::endl << std::flush;
  trkNum = predictedBoxes.size();
  detNum = detectedObjects.size();
  std::cout << "Tracknum:" << trkNum << "Detnum:" << detNum << std::endl << std::flush;
  iouMatrix.clear();
  iouMatrix.resize(trkNum, vector<double>(detNum, 0));
  for (unsigned int i = 0; i < trkNum; i++) {
    for (unsigned int j = 0; j < detNum; j++) {
      // use 1-iou because the hungarian algorithm computes a minimum-cost assignment.
      iouMatrix[i][j] = 1 - GetIOU(predictedBoxes[i], detectedObjects[j].lastRectangle);
    }
  }

  // solve the assignment problem using hungarian algorithm.
	// the resulting assignment is [track(prediction) : detection], with len=preNum
  std::cout << "solve the assignment problem" << std::endl << std::flush;
	HungarianAlgorithm HungAlgo;
	assignment.clear();
  std::cout << "IOUmatrix rows:" << iouMatrix.size() << " cols:" << iouMatrix[0].size() << std::endl << std::flush;
	HungAlgo.Solve(iouMatrix, assignment);

	// find matches, unmatched_detections and unmatched_predictions
  std::cout << "find matches, unmatched_detections and unmatched_predictions" << std::endl << std::flush;
	unmatchedTrajectories.clear();
	unmatchedDetections.clear();
	allItems.clear();
	matchedItems.clear();

	if (detNum > trkNum) { //	there are unmatched detections
    std::cout << "there are unmatched detections" << std::endl << std::flush;
		for (unsigned int n = 0; n < detNum; n++)
			allItems.insert(n);

		for (unsigned int i = 0; i < trkNum; ++i)
			matchedItems.insert(assignment[i]);

		set_difference(allItems.begin(), allItems.end(),
			matchedItems.begin(), matchedItems.end(),
			insert_iterator<set<int>>(unmatchedDetections, unmatchedDetections.begin()));
	}
	else if (detNum < trkNum) { // there are unmatched trajectory/predictions
    std::cout << "there are unmatched trajectory/predictions" << std::endl << std::flush;
		for (unsigned int i = 0; i < trkNum; ++i) {
			if (assignment[i] == -1) // unassigned label will be set as -1 in the assignment algorithm
				unmatchedTrajectories.insert(i);
    }
	}
	else
		;

	// filter out matched with low IOU and wrong metaData
  std::cout << "filter out matched with low IOU" << std::endl << std::flush;
	matchedPairs.clear();
	for (unsigned int i = 0; i < trkNum; ++i) {
		if (assignment[i] == -1) // pass over invalid values
			continue;
		if ((1 - iouMatrix[i][assignment[i]] < iouThreshold) || (trackers[i].metaData != detectedObjects[assignment[i]].label)) {
			unmatchedTrajectories.insert(i);
			unmatchedDetections.insert(assignment[i]);
		}
		else
			matchedPairs.push_back(cv::Point(i, assignment[i]));
	}

	// update matched trackers with assigned detections.
	// each prediction is corresponding to a tracker
  std::cout << "update matched trackers with assigned detections" << std::endl << std::flush;
	int detIdx, trkIdx;
	for (unsigned int i = 0; i < matchedPairs.size(); i++) {
		trkIdx = matchedPairs[i].x;
		detIdx = matchedPairs[i].y;
		trackers[trkIdx].update(detectedObjects[detIdx].lastRectangle);
	}

  // create and initialise new trackers for unmatched detections
	for (auto umd : unmatchedDetections) {
		KalmanTracker tracker = KalmanTracker(detectedObjects[umd].lastRectangle, detectedObjects[umd].label);
		trackers.push_back(tracker);
	}

  //remove dead tracks
  for (auto it = trackers.begin(); it != trackers.end();) {
    if (it != trackers.end() && (*it).m_time_since_update > max_age)
      it = trackers.erase(it);
    it++;
  }

}

void aiSaac::ObjectAnalytics::annotateSort(cv::Mat &rawFrame, int frameNumber) {

  vector<TrackingBox> frameTrackingResult;
  // get trackers' output
  frameTrackingResult.clear();
  for (auto it = trackers.begin(); it != trackers.end();) {
    if (((*it).m_time_since_update < 1) &&
      ((*it).m_hit_streak >= min_hits || frame_count <= min_hits)) {
      TrackingBox res;
      res.box = (*it).get_state();
      res.id = (*it).m_id+1;
      res.metaData = (*it).metaData;
      if (false == (*it).counted) {
        (*it).counted = true;
      }
      //res.frame = frame_count;
      frameTrackingResult.push_back(res);
      it++;
    }
    else
      it++;
  }

  //loop over track results
  for (auto it = frameTrackingResult.begin(); it != frameTrackingResult.end(); it++) {
    if ("person" == (*it).metaData || "car" == (*it).metaData) {

      //draw box
      cv::Scalar colour = cv::Scalar(0, 255, 0);
      cv::rectangle(rawFrame,
          (*it).box,
          colour,
          2);

      //draw text
      int baseline = 0;
      int fontScale = ceil(rawFrame.cols * 0.001);
      //int fontScale = 1;
      int thickness = ceil(rawFrame.cols * 0.002);

      cv::Size textSize = cv::getTextSize(std::to_string((*it).id),
          cv::FONT_HERSHEY_PLAIN,
          fontScale,
          thickness,
          &baseline);
      baseline += thickness;
      cv::Point textOrg((*it).box.x
          + ((*it).box.width / 2)
          - (textSize.width / 2),
          (*it).box.y - 5);
      if (textOrg.x < 0) {
          textOrg.x = 0;
      }
      if (textOrg.y < 0) {
          textOrg.y = 0;
      }
      cv::putText(rawFrame,
          "[" + std::to_string((*it).id) + "] " + (*it).metaData,
          textOrg,
          cv::FONT_HERSHEY_PLAIN,
          fontScale,
          colour,
          thickness);
      }
  }
}

void aiSaac::ObjectAnalytics::annotateSort(cv::Mat &rawFrame) {
    this->annotateSort(rawFrame, this->analyzedFrameNumber);
}

void aiSaac::ObjectAnalytics::annotate(cv::Mat &rawFrame) {
    this->annotate(rawFrame, this->analyzedFrameNumber);
}

void aiSaac::ObjectAnalytics::annotate(cv::Mat &rawFrame, int frameNumber) {
    int count = 0;
    for (int i = 0; i < this->blobContainer.size(); i++) {
        //if ((this->blobContainer[i].lastFrameNumber == frameNumber) && (this->blobContainer[i].label == "person" || this->blobContainer[i].label == "car")) {
        if (this->blobContainer[i].lastFrameNumber == frameNumber) {
            if (this->aiSaacSettings.getObjectAnalyticsAlgo() == "CAFFE") {
                cv::putText(rawFrame,
                    this->blobContainer[i].label,
                    cv::Point(20, 20 * (count + 1)),
                    cv::FONT_HERSHEY_PLAIN,
                    1.0,
                    CV_RGB(124, 252, 0),
                    2.0);
                count++;
            } else if (this->aiSaacSettings.getObjectAnalyticsAlgo() == "DARKNET") {
                cv::rectangle(rawFrame,
                    this->blobContainer[i].lastRectangle,
                    cv::Scalar(255, 255, 255),
                    2);

                //sublocalised frame for object color
                //cv::Point potentialPosition(this->blobContainer[i].lastRectangle.x + this->blobContainer[i].lastRectangle.width / 2,
                //    this->blobContainer[i].lastRectangle.y + this->blobContainer[i].lastRectangle.height / 2);

                //cv::Rect colorRoi(
                //    cv::Point(potentialPosition.x - this->blobContainer[i].lastRectangle.width / 10,
                //        potentialPosition.y - this->blobContainer[i].lastRectangle.height / 10),
                //    cv::Point(potentialPosition.x + this->blobContainer[i].lastRectangle.width / 10,
                //        potentialPosition.y + this->blobContainer[i].lastRectangle.height / 10));

                //cv::rectangle(rawFrame,
                //    colorRoi,
                //   cv::Scalar(127, 255, 0), 2);

                int baseline = 0;
                int fontScale = ceil(rawFrame.cols * 0.001);
                int thickness = ceil(rawFrame.cols * 0.002);

                cv::Size textSize = cv::getTextSize("[" + std::to_string(this->blobContainer[i].ID) + "] " + this->blobContainer[i].label,
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
                    "[" + std::to_string(this->blobContainer[i].ID) + "] " + this->blobContainer[i].label,
                    textOrg,
                    cv::FONT_HERSHEY_SIMPLEX,
                    fontScale,
                    cv::Scalar(127, 255, 0),
                    thickness);
            } 
            else if (this->aiSaacSettings.getObjectAnalyticsAlgo() == "DNN") {

                cv::Scalar colour;
                if(this->blobContainer[i].label == "person") {
                  colour = cv::Scalar(0, 255, 0);
                } else {
                  colour = cv::Scalar(0, 0, 255);
                }

                cv::rectangle(rawFrame,
                    this->blobContainer[i].lastRectangle,
                    colour,
                    2);

                //sublocalised frame for object color
                //cv::Point potentialPosition(this->blobContainer[i].lastRectangle.x + this->blobContainer[i].lastRectangle.width / 2,
                    //this->blobContainer[i].lastRectangle.y + this->blobContainer[i].lastRectangle.height / 2);

                /*cv::Rect colorRoi(
                    cv::Point(potentialPosition.x - this->blobContainer[i].lastRectangle.width / 10,
                        potentialPosition.y - this->blobContainer[i].lastRectangle.height / 10),
                    cv::Point(potentialPosition.x + this->blobContainer[i].lastRectangle.width / 10,
                        potentialPosition.y + this->blobContainer[i].lastRectangle.height / 10));

                cv::rectangle(rawFrame,
                    colorRoi,
                    cv::Scalar(127, 255, 0), 2);*/

                int baseline = 0;
                //int fontScale = ceil(rawFrame.cols * 0.00005);
                int fontScale = 1;
                int thickness = ceil(rawFrame.cols * 0.002);

                cv::Size textSize = cv::getTextSize("[" + std::to_string(this->blobContainer[i].ID) + "] " + this->blobContainer[i].label,
                    cv::FONT_HERSHEY_PLAIN,
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
                    "[" + std::to_string(this->blobContainer[i].ID) + "] " + this->blobContainer[i].label,
                    textOrg,
                    cv::FONT_HERSHEY_PLAIN,
                    fontScale,
                    colour,
                    thickness);
            }
        }
    }
}

void aiSaac::ObjectAnalytics::annotate(cv::Mat &rawFrame,
    const std::vector<std::string> &labels) {
    /*if (this->aiSaacSettings.getObjectAnalyticsAlgo() == "CAFFE") {
        for (int i = 0; i < labels.size(); i++) {
            cv::putText(rawFrame,
                labels[i],
                cv::Point(20, 20 * (i + 1)),
                cv::FONT_HERSHEY_PLAIN,
                1.0,
                CV_RGB(124, 252, 0),
                2.0);
        }
    } else if (this->aiSaacSettings.getObjectAnalyticsAlgo() == "DARKNET") {
        std::cerr << "This function does not support DARKNET."
            << " Use void annotate(cv::Mat &rawFrame,"
            << " std::vector<std::pair<std::string, cv::Rect>>"
            << " &localizedLabeledRectangles);" <<
            std::endl << std::flush;
    } else*/ if (this->aiSaacSettings.getObjectAnalyticsAlgo() == "DNN") {
        std::cerr << "This function does not support DNN."
            << " Use void annotate(cv::Mat &rawFrame,"
            << " std::vector<std::pair<std::string, cv::Rect>>"
            << " &localizedLabeledRectangles);" <<
            std::endl << std::flush;
    }
}

void aiSaac::ObjectAnalytics::annotate(cv::Mat &rawFrame,
    const std::vector<std::pair<std::string, cv::Rect>> &localizedLabeledRectangles) {
    /*if (this->aiSaacSettings.getObjectAnalyticsAlgo() == "CAFFE") {
        std::cerr << "This function does not support CAFFE."
            << " Use void annotate(cv::Mat &rawFrame,"
            << " std::vector<std::string>& labels"
            << std::endl << std::flush;
    } else if (this->aiSaacSettings.getObjectAnalyticsAlgo() == "DARKNET") {
        for (int i = 0; i < localizedLabeledRectangles.size(); i++) {
            cv::rectangle(rawFrame,
                localizedLabeledRectangles[i].second,
                cv::Scalar(255, 20, 147),
                2);
            cv::putText(rawFrame,
                localizedLabeledRectangles[i].first,
                cv::Point(localizedLabeledRectangles[i].second.x + 5,
                    localizedLabeledRectangles[i].second.y + 15),
                cv::FONT_HERSHEY_PLAIN,
                1.0,
                CV_RGB(124, 252, 0),
                2.0);
        }
    } else*/ if (this->aiSaacSettings.getObjectAnalyticsAlgo() == "DNN") {
        for (int i = 0; i < localizedLabeledRectangles.size(); i++) {
            cv::rectangle(rawFrame,
                localizedLabeledRectangles[i].second,
                cv::Scalar(255, 20, 147),
                2);
            cv::putText(rawFrame,
                localizedLabeledRectangles[i].first,
                cv::Point(localizedLabeledRectangles[i].second.x + 5,
                    localizedLabeledRectangles[i].second.y + 15),
                cv::FONT_HERSHEY_PLAIN,
                1.0,
                CV_RGB(124, 252, 0),
                2.0);
        }
    }
}

void aiSaac::ObjectAnalytics::runAlgo(const cv::Mat &rawFrame,
    int frameNumber,
    std::vector<aiSaac::ObjectBlob> &detectedObjects) {
    #ifdef CAFFE
        if (this->aiSaacSettings.getObjectAnalyticsAlgo() == "CAFFE") {
            std::vector<std::pair<std::string, float>> predictions
                = this->caffeClassifier->Classify(rawFrame);
            for (int i = 0; i < predictions.size(); i++) {
                if (predictions[i].second >=
                    this->aiSaacSettings.getObjectAnalyticsConfidenceThreshold()) {
                    aiSaac::ObjectBlob newObject;
                    newObject.label = predictions[i].first;
                    newObject.lastRectangle = cv::Rect(0, 0, rawFrame.cols, rawFrame.rows);
                    newObject.pastPositions.push_back(cv::Point(
                        rawFrame.cols / 2,
                        rawFrame.rows / 2));
                    detectedObjects.push_back(newObject);
                }
            }
        }
    #endif
    #ifdef DARKNET
        if (this->aiSaacSettings.getObjectAnalyticsAlgo() == "DARKNET") {
            std::vector<std::pair<std::string, cv::Rect>> results;
            this->darknetClassifier->Classify(rawFrame, results);
            for (int i = 0; i < results.size(); i++) {
                aiSaac::ObjectBlob detectedObject;
                detectedObject.label = results[i].first;
                detectedObject.lastRectangle = results[i].second;

                detectedObject.lastRectangle.x = (detectedObject.lastRectangle.x < 0) ? 0 :
                    detectedObject.lastRectangle.x;
                detectedObject.lastRectangle.y = (detectedObject.lastRectangle.y < 0) ? 0 :
                    detectedObject.lastRectangle.y;
                detectedObject.lastRectangle.width = ((detectedObject.lastRectangle.width + detectedObject.lastRectangle.x) > rawFrame.cols) ? rawFrame.cols - detectedObject.lastRectangle.x : detectedObject.lastRectangle.width;
                detectedObject.lastRectangle.height = ((detectedObject.lastRectangle.height + detectedObject.lastRectangle.y) > rawFrame.rows) ? rawFrame.rows - detectedObject.lastRectangle.y : detectedObject.lastRectangle.height;

                detectedObject.pastPositions.push_back(cv::Point(
                    detectedObject.lastRectangle.x + detectedObject.lastRectangle.width / 2,
                    detectedObject.lastRectangle.y + detectedObject.lastRectangle.height / 2));
                detectedObjects.push_back(detectedObject);
            }
        }
    #endif
    #ifdef DNN_CLASSIFIER
        if (this->aiSaacSettings.getObjectAnalyticsAlgo() == "DNN") {

          std::vector<std::pair<std::string, cv::Rect>> results;
          this->dnnClassifier->Classify(rawFrame, results);
          for (int i = 0; i < results.size(); i++) {
              aiSaac::ObjectBlob detectedObject;
              detectedObject.label = results[i].first;
              detectedObject.lastRectangle = results[i].second;
              detectedObject.lastRectangle.x = (detectedObject.lastRectangle.x < 0) ? 0 :
                  detectedObject.lastRectangle.x;
              detectedObject.lastRectangle.y = (detectedObject.lastRectangle.y < 0) ? 0 :
                  detectedObject.lastRectangle.y;
              detectedObject.lastRectangle.width = ((detectedObject.lastRectangle.width + detectedObject.lastRectangle.x) > rawFrame.cols) ? rawFrame.cols - detectedObject.lastRectangle.x : detectedObject.lastRectangle.width;
              detectedObject.lastRectangle.height = ((detectedObject.lastRectangle.height + detectedObject.lastRectangle.y) > rawFrame.rows) ? rawFrame.rows - detectedObject.lastRectangle.y : detectedObject.lastRectangle.height;
              detectedObject.pastPositions.push_back(cv::Point(
                  detectedObject.lastRectangle.x + detectedObject.lastRectangle.width / 2,
                  detectedObject.lastRectangle.y + detectedObject.lastRectangle.height / 2));
              detectedObjects.push_back(detectedObject);
          }
        }
    #endif
    this->analyzedFrameNumber = frameNumber;
}

void aiSaac::ObjectAnalytics::preProcessBlobContainer(int frameNumber) {
    if (this->blobContainer.size() > 400) {  //add to aiSaac settings
        this->blobContainer.erase(this->blobContainer.begin(), this->blobContainer.begin() + 200);
    }

    // Remove old objects with few frames
    for (std::vector<aiSaac::ObjectBlob>::iterator it = this->blobContainer.begin();
        it != this->blobContainer.end();) {
        if (frameNumber - it->lastFrameNumber > this->aiSaacSettings.getObjectBlobDeleteTime()
            && it->frames.size() < this->aiSaacSettings.getMinimumObjectBlobFrames()) {
            it = this->blobContainer.erase(it);
        } else {
            it++;
        }
    }
}

void aiSaac::ObjectAnalytics::preProcessBlobContainerMotion(int frameNumber) {
    if (this->blobContainer.size() > 400) {  //add to aiSaac settings
        this->blobContainer.erase(this->blobContainer.begin(), this->blobContainer.begin() + 200);
    }

    // Remove old objects with few frames
    for (std::vector<aiSaac::ObjectBlob>::iterator it = this->blobContainer.begin();
        it != this->blobContainer.end();) {
        if (frameNumber - it->motionFrameNumber > this->aiSaacSettings.getObjectBlobDeleteTime()
            && it->frames.size() < this->aiSaacSettings.getMinimumObjectBlobFrames()) {
            it = this->blobContainer.erase(it);
        } else {
            it++;
        }
    }
}

void aiSaac::ObjectAnalytics::loadAlgorithmType() {
    this->loadedAlgos = true;
    #ifdef CAFFE
        if (this->aiSaacSettings.getObjectAnalyticsAlgo() == "CAFFE") {
            std::cout << "selected caffe. creating caffe classifier" << std::endl;
            this->caffeClassifier = new CaffeClassifier(
                this->aiSaacSettings.getObjectCaffePrototxtPath(),
                this->aiSaacSettings.getObjectCaffeCaffemodelPath(),
                this->aiSaacSettings.getObjectCaffeMeanFilePath(),
                this->aiSaacSettings.getObjectCaffeLabelFilePath());
        }
    #endif
    #ifdef DARKNET
        if (this->aiSaacSettings.getObjectAnalyticsAlgo() == "DARKNET") {
            std::cout << "selected darknet. creating darknet classifier" << std::endl;
            this->darknetClassifier = new DarknetClassifier(
                this->aiSaacSettings.getObjectDarknetConfigFilePath(),
                this->aiSaacSettings.getObjectDarknetWeightFilePath(),
                this->aiSaacSettings.getObjectDarknetLabelFilePath(),
                this->aiSaacSettings.getObjectAnalyticsConfidenceThreshold());
        }
    #endif
    #ifdef DNN_CLASSIFIER
        if (this->aiSaacSettings.getObjectAnalyticsAlgo() == "DNN") {
            std::cout << "selected opencv3 dnn framework. Creating dnn" << std::endl;
            this->dnnClassifier = new DNNClassifier(
                this->aiSaacSettings.getObjectDNNPrototxtPath(),
                this->aiSaacSettings.getObjectDNNCaffemodelPath(),
                this->aiSaacSettings.getObjectDNNMeanFilePath(),
                this->aiSaacSettings.getObjectDNNLabelFilePath(),
                this->aiSaacSettings.getObjectAnalyticsConfidenceThreshold());
        }
    #endif

}

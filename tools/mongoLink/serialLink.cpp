//
// Created by aitoe on 10/7/17.
//

#include "serialLink.h"

const int max_length = 5120;

mongoLink::mongoLink(int procFPS) {
    this->procFPS = procFPS;
}

//motionMONGO for SmartEncoder
void mongoLink::motionMONGO(std::vector<aiSaac::BlobObject> blobContainer, int frameNumber, std::string resultTableId, int chunkStartFrameNumber) {

    std::string motionOutputBlob;
    nlohmann::json motionAnalyticsJson;
		nlohmann::json motionAnalyticsBlobs;

		for (int i = 0; i < blobContainer.size(); i++) {
        if (!blobContainer[i].isValid) {
          continue;
        }
        nlohmann::json blobMotion;
        try {
            blobMotion = motionAnalyticsBlobs[std::to_string(blobContainer[i].ID)];
        } catch (std::out_of_range) {
            std::cout << "Doesn't exist" << std::endl;
        }
        blobMotion["type"] = "motion";
        blobMotion["parentID"] = resultTableId;
        blobMotion["startTime"] = (blobContainer[i].firstFrameNumber >= chunkStartFrameNumber)?((((double)blobContainer[i].firstFrameNumber) - ((double)chunkStartFrameNumber))/((double)this->procFPS)):((double)blobContainer[i].firstFrameNumber/(double)this->procFPS);
        blobMotion["endTime"] = (blobContainer[i].lastFrameNumber >= chunkStartFrameNumber)?((((double)blobContainer[i].lastFrameNumber) - ((double)chunkStartFrameNumber))/((double)this->procFPS)):((double)blobContainer[i].lastFrameNumber/(double)this->procFPS);
        nlohmann::json obj;

        if (blobContainer[i].lastFrameNumber == frameNumber) {
            obj["x"] = blobContainer[i].lastRectangle.x;
            obj["y"] = blobContainer[i].lastRectangle.y;
            obj["height"] = blobContainer[i].lastRectangle.height;
            obj["width"] = blobContainer[i].lastRectangle.width;
            obj["rectTime"] =
                    ((double)frameNumber)/((double)this->procFPS);
            blobMotion["rectangles"].push_back(obj);
        }
        motionAnalyticsBlobs[std::to_string(blobContainer[i].ID)] = blobMotion;
    }
    motionAnalyticsJson["motionAnalytics"] = motionAnalyticsBlobs;
    motionOutputBlob = motionAnalyticsJson.dump();
    motionOutputBlob = "MOTION_OUTPUT_BLOB_START<<<" + motionOutputBlob + ">>>MOTION_OUTPUT_BLOB_END";
    std::cout << motionOutputBlob << std::endl;

    //std::cout << std::setw(4) << motionAnalyticsJson << std::endl;

    // std::ofstream motionJsonFile;
    // motionJsonFile.open("/Users/pratik/AITOE/aisaac/build/storageDir/outputFiles/motionJSONPratik.json");
    // motionJsonFile << motionAnalyticsJson.dump();
    // motionJsonFile.close();
}

//motionMONGO for SSP
void mongoLink::motionMONGO(std::vector<aiSaac::BlobObject> blobContainer, int frameNumber, std::string resultTableId) {

    std::string motionOutputBlob;
    nlohmann::json motionAnalyticsJson;
		nlohmann::json motionAnalyticsBlobs;

		for (int i = 0; i < blobContainer.size(); i++) {
        if (!blobContainer[i].isValid) {
          continue;
        }
        nlohmann::json blobMotion;
        try {
            blobMotion = motionAnalyticsBlobs[std::to_string(blobContainer[i].ID)];
        } catch (std::out_of_range) {
            std::cout << "Doesn't exist" << std::endl;
        }
        blobMotion["type"] = "motion";
        blobMotion["parentId"] = resultTableId;
        blobMotion["startTime"] = ((double)blobContainer[i].firstFrameNumber/(double)this->procFPS);
        blobMotion["endTime"] = ((double)blobContainer[i].lastFrameNumber/(double)this->procFPS);
        nlohmann::json obj;

        if (blobContainer[i].lastFrameNumber == frameNumber) {
            obj["x"] = blobContainer[i].lastRectangle.x;
            obj["y"] = blobContainer[i].lastRectangle.y;
            obj["height"] = blobContainer[i].lastRectangle.height;
            obj["width"] = blobContainer[i].lastRectangle.width;
            obj["rectTime"] =
                    ((double)frameNumber)/((double)this->procFPS);
            blobMotion["rectangles"].push_back(obj);
        }
        motionAnalyticsBlobs[std::to_string(blobContainer[i].ID)] = blobMotion;
    }
    motionAnalyticsJson["motionAnalytics"] = motionAnalyticsBlobs;
    motionOutputBlob = motionAnalyticsJson.dump();
    motionOutputBlob = "MOTION_OUTPUT_BLOB_START<<<" + motionOutputBlob + ">>>MOTION_OUTPUT_BLOB_END";
    std::cout << motionOutputBlob << std::endl;

    //std::cout << std::setw(4) << motionAnalyticsJson << std::endl;

    // std::ofstream motionJsonFile;
    // motionJsonFile.open("/Users/pratik/AITOE/aisaac/build/storageDir/outputFiles/motionJSONPratik.json");
    // motionJsonFile << motionAnalyticsJson.dump();
    // motionJsonFile.close();
}

//objectMONGO for SmartEncoder
void mongoLink::objectMONGO(std::vector<aiSaac::ObjectBlob> blobContainer, int frameNumber, std::string resultTableId, int chunkStartFrameNumber) {

    std::string objectOutputBlob;
    nlohmann::json objectAnalyticsJson;
		nlohmann::json objectAnalyticsBlobs;

		for (int i = 0; i < blobContainer.size(); i++) {
        if (!blobContainer[i].isValid) {
          continue;
        }
        nlohmann::json blobObject;
        try {
            blobObject = objectAnalyticsBlobs[std::to_string(blobContainer[i].ID)];
        } catch (std::out_of_range) {
            std::cout << "Doesn't exist" << std::endl;
        }
        blobObject["type"] = "object";
        blobObject["label"] = blobContainer[i].label;
        blobObject["parentID"] = resultTableId;
        blobObject["startTime"] = (blobContainer[i].firstFrameNumber >= chunkStartFrameNumber)?((((double)blobContainer[i].firstFrameNumber) - ((double)chunkStartFrameNumber))/((double)this->procFPS)):((double)blobContainer[i].firstFrameNumber/(double)this->procFPS);
        blobObject["endTime"] = (blobContainer[i].lastFrameNumber >= chunkStartFrameNumber)?((((double)blobContainer[i].lastFrameNumber) - ((double)chunkStartFrameNumber))/((double)this->procFPS)):((double)blobContainer[i].lastFrameNumber/(double)this->procFPS);
        blobObject["metaData"] = blobContainer[i].metaData;
        nlohmann::json obj;

        if (blobContainer[i].lastFrameNumber == frameNumber) {
            obj["x"] = blobContainer[i].lastRectangle.x;
            obj["y"] = blobContainer[i].lastRectangle.y;
            obj["height"] = blobContainer[i].lastRectangle.height;
            obj["width"] = blobContainer[i].lastRectangle.width;
            obj["rectTime"] =
                    ((double)frameNumber)/((double)this->procFPS);
            blobObject["rectangles"].push_back(obj);
        }
        objectAnalyticsBlobs[std::to_string(blobContainer[i].ID)] = blobObject;
    }
    objectAnalyticsJson["objectAnalytics"] = objectAnalyticsBlobs;
    objectOutputBlob = objectAnalyticsJson.dump();
    objectOutputBlob = "OBJECT_OUTPUT_BLOB_START<<<" + objectOutputBlob + ">>>OBJECT_OUTPUT_BLOB_END";
    std::cout << objectOutputBlob << std::endl;

    //std::cout << std::setw(4) << motionAnalyticsJson << std::endl;

    // std::ofstream motionJsonFile;
    // motionJsonFile.open("/Users/pratik/AITOE/aisaac/build/storageDir/outputFiles/motionJSONPratik.json");
    // motionJsonFile << motionAnalyticsJson.dump();
    // motionJsonFile.close();
}

//objectMONGO for SSP
void mongoLink::objectMONGO(std::vector<aiSaac::ObjectBlob> blobContainer, int frameNumber, std::string resultTableId) {

    std::string objectOutputBlob;
    nlohmann::json objectAnalyticsJson;
		nlohmann::json objectAnalyticsBlobs;

		for (int i = 0; i < blobContainer.size(); i++) {

      if (!blobContainer[i].isValid) {
        continue;
      }
        nlohmann::json blobObject;
        try {
            blobObject = objectAnalyticsBlobs[std::to_string(blobContainer[i].ID)];
        } catch (std::out_of_range) {
            std::cout << "Doesn't exist" << std::endl;
        }
        blobObject["type"] = "object";
        blobObject["label"] = blobContainer[i].label;
        blobObject["parentId"] = resultTableId;
        blobObject["startTime"] = ((double)blobContainer[i].firstFrameNumber/(double)this->procFPS);
        blobObject["endTime"] = ((double)blobContainer[i].lastFrameNumber/(double)this->procFPS);
        std::string metaDataString = "";
        for (int j = 0; j < blobContainer[i].metaData.size(); j++) {
           metaDataString += blobContainer[i].metaData[j] + ",";
        }
        metaDataString.pop_back();
        blobObject["metaData"] = metaDataString;
        nlohmann::json obj;

        if (blobContainer[i].lastFrameNumber == frameNumber) {
            obj["x"] = blobContainer[i].lastRectangle.x;
            obj["y"] = blobContainer[i].lastRectangle.y;
            obj["height"] = blobContainer[i].lastRectangle.height;
            obj["width"] = blobContainer[i].lastRectangle.width;
            obj["rectTime"] =
                    ((double)frameNumber)/((double)this->procFPS);
            blobObject["rectangles"].push_back(obj);
        }
        objectAnalyticsBlobs[std::to_string(blobContainer[i].ID)] = blobObject;
    }
    objectAnalyticsJson["objectAnalytics"] = objectAnalyticsBlobs;
    objectOutputBlob = objectAnalyticsJson.dump();
    objectOutputBlob = "OBJECT_OUTPUT_BLOB_START<<<" + objectOutputBlob + ">>>OBJECT_OUTPUT_BLOB_END";
    std::cout << objectOutputBlob << std::endl;

    //std::cout << std::setw(4) << motionAnalyticsJson << std::endl;

    // std::ofstream motionJsonFile;
    // motionJsonFile.open("/Users/pratik/AITOE/aisaac/build/storageDir/outputFiles/motionJSONPratik.json");
    // motionJsonFile << motionAnalyticsJson.dump();
    // motionJsonFile.close();
}

//future requirement maybe
void mongoLink::motionPlusObjectMONGO(std::vector<aiSaac::ObjectBlob> blobContainer, int frameNumber, std::string resultTableId) {

    std::string motionPlusObjectOutputBlob;
    nlohmann::json motionPlusObjectAnalyticsJson;
		nlohmann::json motionPlusObjectAnalyticsBlobs;

		for (int i = 0; i < blobContainer.size(); i++) {
        nlohmann::json blobMotionPlusObject;
        try {
            blobMotionPlusObject = motionPlusObjectAnalyticsBlobs[std::to_string(blobContainer[i].ID)];
        } catch (std::out_of_range) {
            std::cout << "Doesn't exist" << std::endl;
        }
        blobMotionPlusObject["type"] = "motionPlusObject";
        blobMotionPlusObject["parentID"] = resultTableId;
        blobMotionPlusObject["startTime"] = ((double)blobContainer[i].firstFrameNumber)/((double)this->procFPS);
        blobMotionPlusObject["endTime"] = ((double)blobContainer[i].lastFrameNumber)/((double)this->procFPS);
        nlohmann::json obj;

        if (blobContainer[i].lastFrameNumber == frameNumber) {
            obj["x"] = blobContainer[i].lastRectangle.x;
            obj["y"] = blobContainer[i].lastRectangle.y;
            obj["height"] = blobContainer[i].lastRectangle.height;
            obj["width"] = blobContainer[i].lastRectangle.width;
            obj["rectTime"] =
                    ((double)frameNumber)/((double)this->procFPS);
            blobMotionPlusObject["rectangles"].push_back(obj);
        }
        motionPlusObjectAnalyticsBlobs[std::to_string(blobContainer[i].ID)] = blobMotionPlusObject;
    }
    motionPlusObjectAnalyticsJson["motionPlusObjectAnalytics"] = motionPlusObjectAnalyticsBlobs;
    motionPlusObjectOutputBlob = motionPlusObjectAnalyticsJson.dump();
    motionPlusObjectOutputBlob = "MOTION_PLUS_OBJECT_OUTPUT_BLOB_START<<<" + motionPlusObjectOutputBlob + ">>>MOTION_PLUS_OBJECT_OUTPUT_BLOB_END";
    std::cout << motionPlusObjectOutputBlob << std::endl;

    //std::cout << std::setw(4) << motionAnalyticsJson << std::endl;

    // std::ofstream motionJsonFile;
    // motionJsonFile.open("/Users/pratik/AITOE/aisaac/build/storageDir/outputFiles/motionJSONPratik.json");
    // motionJsonFile << motionAnalyticsJson.dump();
    // motionJsonFile.close();
}

void mongoLink::motionEventAlert(AlertBlob alertBlob) {
    nlohmann::json motionAlertBlob;
    std::string motionAlert;

    motionAlertBlob["alertID"] = alertBlob.alertID;
    motionAlertBlob["ruleID"] = 0;
    motionAlertBlob["blobID"] = 0;
    motionAlertBlob["alertType"] = alertBlob.alertType;
    motionAlertBlob["startTime"] = alertBlob.startTime;
    motionAlertBlob["endTime"] = alertBlob.endTime;
    motionAlertBlob["alertActive"] = alertBlob.alertActive;
    motionAlertBlob["alertFramePath"] = alertBlob.alertFramePath;

    motionAlert = motionAlertBlob.dump();
    motionAlert = "MOTION_EVENT_ALERT_BLOB_START<<<" + motionAlert + ">>>MOTION_EVENT_ALERT_BLOB_END";

    std::cout << motionAlert << std::endl;

}

void mongoLink::ruleAlertMONGO(AlertBlob alertBlob) {
    nlohmann::json ruleAlertBlob;
    std::string ruleAlert;

    ruleAlertBlob["alertID"] = alertBlob.alertID;
    ruleAlertBlob["ruleID"] = alertBlob.ruleID;
    ruleAlertBlob["blobID"] = alertBlob.blobID;
    ruleAlertBlob["alertType"] = alertBlob.alertType;
    ruleAlertBlob["startTime"] = alertBlob.startTime;
    ruleAlertBlob["endTime"] = alertBlob.endTime;
    ruleAlertBlob["alertActive"] = alertBlob.alertActive;
    ruleAlertBlob["alertFramePath"] = alertBlob.alertFramePath;

    ruleAlert = ruleAlertBlob.dump();
    ruleAlert = "RULE_ALERT_BLOB_START<<<" + ruleAlert + ">>>RULE_ALERT_BLOB_END";

    std::cout << ruleAlert << std::endl;
}

void mongoLink::extractedFeatureMONGO(std::string label, std::vector<float> extractedFeatureVector) {
      nlohmann::json extractedFeatureJson;
      nlohmann::json extractedFeatureBlob;
      std::string tempFeature;
      std::string featureOutputBlob;

      extractedFeatureBlob["type"] = label;
      extractedFeatureBlob["featureVector"] = extractedFeatureVector;
      extractedFeatureJson["extractedFeatureVector"] = extractedFeatureBlob;

      featureOutputBlob = extractedFeatureJson.dump();
      featureOutputBlob = "EXTRACTED_FEATURE_VECTOR_BLOB_START<<<" + featureOutputBlob + ">>>EXTRACTED_FEATURE_VECTOR_BLOB_END";

      std::cout << featureOutputBlob << std::endl;
}

void mongoLink::faceAnalyticsMONGO(std::vector<aiSaac::FaceBlob> blobContainer, int frameNumber, std::string resultTableId, std::string resultTableFeature) {

			nlohmann::json faceAnalyticsJson;
			nlohmann::json faceAnalyticsBlobs;
			for (int i = 0; i < blobContainer.size(); i++) {
			  nlohmann::json blobFace;
			  try {
			      blobFace = faceAnalyticsBlobs[std::to_string(blobContainer[i].ID)];
			  } catch (std::out_of_range) {
			      std::cout << "Doesn't exist" << std::endl;
			  }
			  blobFace["type"] = "faceAnalytics";
			  blobFace["label"] = blobContainer[i].label;
			  blobFace["parentID"] = resultTableId;
			  blobFace["startTime"] = ((double)blobContainer[i].firstFrameNumber)/((double)this->procFPS);
			  blobFace["endTime"] = ((double)blobContainer[i].lastFrameNumber)/((double)this->procFPS);
			  nlohmann::json obj;

			  if (blobContainer[i].lastFrameNumber == frameNumber) {
			      obj["x"] = blobContainer[i].lastRectangle.x;
			      obj["y"] = blobContainer[i].lastRectangle.y;
			      obj["height"] = blobContainer[i].lastRectangle.height;
			      obj["width"] = blobContainer[i].lastRectangle.width;
			      obj["rectTime"] =
			              ((double)frameNumber)/((double)this->procFPS);
			      blobFace["rectangles"].push_back(obj);
			  }
			  faceAnalyticsBlobs[std::to_string(blobContainer[i].ID)] = blobFace;
			}
			faceAnalyticsJson["faceAnalytics"] = faceAnalyticsBlobs;

			std::cout << std::setw(4) << faceAnalyticsJson << std::endl;

			// std::ofstream faceJsonFile;
			// faceJsonFile.open("/Users/pratik/AITOE/aisaac/build/storageDir/Output/faceJSONPratik.json");
			// faceJsonFile << faceAnalyticsJson.dump(4);
			// faceJsonFile.close();
}

void mongoLink::faceAnalyticsMONGO(std::vector<aiSaac::FaceBlob> blobContainer, int frameNumber, std::string resultTableId, std::string resultTableFeature, socket_ptr sock) {

			std::string outputBlob;
	    char data[max_length];
      memset(data, '\0', max_length);

			nlohmann::json faceAnalyticsJson;
			nlohmann::json faceAnalyticsBlobs;
			for (int i = 0; i < blobContainer.size(); i++) {
			  nlohmann::json blobFace;
			  try {
			      blobFace = faceAnalyticsBlobs[std::to_string(blobContainer[i].ID)];
			  } catch (std::out_of_range) {
			      std::cout << "Doesn't exist" << std::endl;
			  }
			  blobFace["type"] = "faceAnalytics";
			  blobFace["label"] = blobContainer[i].label;
			  blobFace["parentID"] = resultTableId;
			  blobFace["startTime"] = ((double)blobContainer[i].firstFrameNumber)/((double)this->procFPS);
			  blobFace["endTime"] = ((double)blobContainer[i].lastFrameNumber)/((double)this->procFPS);
			  nlohmann::json obj;

			  if (blobContainer[i].lastFrameNumber == frameNumber) {
			      obj["x"] = blobContainer[i].lastRectangle.x;
			      obj["y"] = blobContainer[i].lastRectangle.y;
			      obj["height"] = blobContainer[i].lastRectangle.height;
			      obj["width"] = blobContainer[i].lastRectangle.width;
			      obj["rectTime"] =
			              ((double)frameNumber)/((double)this->procFPS);
			      blobFace["rectangles"].push_back(obj);
			  }
			  faceAnalyticsBlobs[std::to_string(blobContainer[i].ID)] = blobFace;
			}
			faceAnalyticsJson["faceAnalytics"] = faceAnalyticsBlobs;

      outputBlob = faceAnalyticsJson.dump();
      outputBlob = "OUTPUT_BLOB_START<<<" + outputBlob + ">>>OUTPUT_BLOB_END";

      strcpy(data, outputBlob.c_str());
      boost::asio::write(*sock, boost::asio::buffer(data, max_length));
      memset(data, '\0', max_length);
      std::cout << outputBlob << std::endl;

			// std::cout << std::setw(4) << faceAnalyticsJson << std::endl;

			// std::ofstream faceJsonFile;
			// faceJsonFile.open("/Users/pratik/AITOE/aisaac/build/storageDir/Output/faceJSONPratik.json");
			// faceJsonFile << faceAnalyticsJson.dump(4);
			// faceJsonFile.close();
}

void mongoLink::humanAnalyticsMONGO(std::vector<aiSaac::HumanBlob> blobContainer, int frameNumber, std::string resultTableId) {

		nlohmann::json humanAnalyticsJson;
		nlohmann::json humanAnalyticsBlobs;
		for (int i = 0; i < blobContainer.size(); i++) {
		  nlohmann::json blobHuman;
		  try {
		      blobHuman = humanAnalyticsBlobs[std::to_string(blobContainer[i].ID)];
		  } catch (std::out_of_range) {
		      std::cout << "Doesn't exist" << std::endl;
		  }
		  blobHuman["type"] = "humanAnalytics";
		  blobHuman["parentID"] = resultTableId;
		  blobHuman["shirtColor"] = blobContainer[i].shirtColor;
		  blobHuman["startTime"] = ((double)blobContainer[i].firstFrameNumber)/((double)this->procFPS);
		  blobHuman["endTime"] = ((double)blobContainer[i].lastFrameNumber)/((double)this->procFPS);
		  nlohmann::json obj;

		  if (blobContainer[i].lastFrameNumber == frameNumber) {
		      obj["x"] = blobContainer[i].lastRectangle.x;
		      obj["y"] = blobContainer[i].lastRectangle.y;
		      obj["height"] = blobContainer[i].lastRectangle.height;
		      obj["width"] = blobContainer[i].lastRectangle.width;
		      obj["rectTime"] =
		              ((double)frameNumber)/((double)this->procFPS);
		      blobHuman["rectangles"].push_back(obj);
		  }
		  humanAnalyticsBlobs[std::to_string(blobContainer[i].ID)] = blobHuman;
		}
		humanAnalyticsJson["humanAnalytics"] = humanAnalyticsBlobs;

		std::cout << std::setw(4) << humanAnalyticsJson << std::endl;

		// std::ofstream humanJsonFile;
		// humanJsonFile.open("/Users/pratik/AITOE/aisaac/build/storhumanDir/Output/humanJSONPratik.json");
		// humanJsonFile << humanAnalyticsJson.dump(4);
		// humanJsonFile.close();

}

void mongoLink::humanAnalyticsMONGO(std::vector<aiSaac::HumanBlob> blobContainer, int frameNumber, std::string resultTableId, socket_ptr sock) {

    std::string outputBlob;
    char data[max_length];
    memset(data, '\0', max_length);

		nlohmann::json humanAnalyticsJson;
		nlohmann::json humanAnalyticsBlobs;
		for (int i = 0; i < blobContainer.size(); i++) {
		  nlohmann::json blobHuman;
		  try {
		      blobHuman = humanAnalyticsBlobs[std::to_string(blobContainer[i].ID)];
		  } catch (std::out_of_range) {
		      std::cout << "Doesn't exist" << std::endl;
		  }
		  blobHuman["type"] = "humanAnalytics";
		  blobHuman["parentID"] = resultTableId;
		  blobHuman["shirtColor"] = blobContainer[i].shirtColor;
		  blobHuman["startTime"] = ((double)blobContainer[i].firstFrameNumber)/((double)this->procFPS);
		  blobHuman["endTime"] = ((double)blobContainer[i].lastFrameNumber)/((double)this->procFPS);
		  nlohmann::json obj;

		  if (blobContainer[i].lastFrameNumber == frameNumber) {
		      obj["x"] = blobContainer[i].lastRectangle.x;
		      obj["y"] = blobContainer[i].lastRectangle.y;
		      obj["height"] = blobContainer[i].lastRectangle.height;
		      obj["width"] = blobContainer[i].lastRectangle.width;
		      obj["rectTime"] =
		              ((double)frameNumber)/((double)this->procFPS);
		      blobHuman["rectangles"].push_back(obj);
		  }
		  humanAnalyticsBlobs[std::to_string(blobContainer[i].ID)] = blobHuman;
		}
		humanAnalyticsJson["humanAnalytics"] = humanAnalyticsBlobs;

    outputBlob = humanAnalyticsJson.dump();
    outputBlob = "OUTPUT_BLOB_START<<<" + outputBlob + ">>>OUTPUT_BLOB_END";

    strcpy(data, outputBlob.c_str());
    boost::asio::write(*sock, boost::asio::buffer(data, max_length));
    memset(data, '\0', max_length);
    std::cout << outputBlob << std::endl;

		// std::cout << std::setw(4) << humanAnalyticsJson << std::endl;

		// std::ofstream humanJsonFile;
		// humanJsonFile.open("/Users/pratik/AITOE/aisaac/build/storhumanDir/Output/humanJSONPratik.json");
		// humanJsonFile << humanAnalyticsJson.dump(4);
		// humanJsonFile.close();

}

void mongoLink::summarizationMONGO(std::vector<int> orderedKeyPoints, aiSaac::AiSaacSettings *aiSaacSettings, std::string resultTableId) {

		nlohmann::json summarizationJson;
		nlohmann::json summarizationBlobs;

    std::string outputBlob;

		for (int i = 0; i < orderedKeyPoints.size(); i++) {
        nlohmann::json blobSummarization;
        try {
            blobSummarization = summarizationBlobs[std::to_string(i)];
        } catch (std::out_of_range) {
            std::cout << "Doesn't exist" << std::endl;
        }
        blobSummarization["type"] = "summarization";
        blobSummarization["parentID"] = resultTableId;
        int startTime = orderedKeyPoints[i] * aiSaacSettings->getSummarizationSNPSize();
        int endTime = startTime + aiSaacSettings->getSummarizationSNPSize();
        blobSummarization["startTime"] = startTime;
        blobSummarization["endTime"] = endTime;
        summarizationBlobs[std::to_string(i)] = blobSummarization;
		}
		summarizationJson["summarization"] = summarizationBlobs;

    outputBlob = summarizationJson.dump();
    outputBlob = "SUMMARIZATION_BLOB_START<<<" + outputBlob + ">>>SUMMARIZATION_BLOB_END";
    std::cout << outputBlob << std::endl;

		// std::cout << std::setw(4) << summarizationJson << std::endl;

		// std::ofstream summarizationJsonFile;
		// summarizationJsonFile.open("/Users/pratik/AITOE/aisaac/build/storageDir/Output/summarizationJSONPratik.json");
		// summarizationJsonFile << summarizationJson.dump(4);
		// summarizationJsonFile.close();
}

void mongoLink::summarizationMONGO(std::vector<int> orderedKeyPoints, aiSaac::AiSaacSettings *aiSaacSettings, std::string resultTableId, socket_ptr sock) {

    std::string outputBlob;
    char data[max_length];
    memset(data, '\0', max_length);

		nlohmann::json summarizationJson;
		nlohmann::json summarizationBlobs;
		for (int i = 0; i < orderedKeyPoints.size(); i++) {
        nlohmann::json blobSummarization;
        try {
            blobSummarization = summarizationBlobs[std::to_string(i)];
        } catch (std::out_of_range) {
            std::cout << "Doesn't exist" << std::endl;
        }
        blobSummarization["type"] = "summarization";
        blobSummarization["parentID"] = resultTableId;
        int startTime = orderedKeyPoints[i] * aiSaacSettings->getSummarizationSNPSize();
        int endTime = startTime + aiSaacSettings->getSummarizationSNPSize();
        blobSummarization["startTime"] = startTime;
        blobSummarization["endTime"] = endTime;
        summarizationBlobs[std::to_string(i)] = blobSummarization;
		}
		summarizationJson["summarization"] = summarizationBlobs;

    outputBlob = summarizationJson.dump();
    outputBlob = "OUTPUT_BLOB_START<<<" + outputBlob + ">>>OUTPUT_BLOB_END";

    strcpy(data, outputBlob.c_str());
    boost::asio::write(*sock, boost::asio::buffer(data, max_length));
    memset(data, '\0', max_length);
    std::cout << outputBlob << std::endl;

		// std::cout << std::setw(4) << summarizationJson << std::endl;

		// std::ofstream summarizationJsonFile;
		// summarizationJsonFile.open("/Users/pratik/AITOE/aisaac/build/storageDir/Output/summarizationJSONPratik.json");
		// summarizationJsonFile << summarizationJson.dump(4);
		// summarizationJsonFile.close();
}


void mongoLink::pushvideoLengthAndFrameSize(std::string videoLength, int height, int width, std::string resultTableId, socket_ptr sock) {

    std::string outputBlob;
    char data[max_length];
    memset(data, '\0', max_length);

    nlohmann::json dimensionsJson;
    nlohmann::json dimensionsBlobs;
    nlohmann::json obj;

    dimensionsBlobs["parentID"] = resultTableId;
    dimensionsBlobs["videoLength"] = videoLength;

    obj["height"] = height;
    obj["width"] = width;
    dimensionsBlobs["frameSize"].push_back(obj);

    dimensionsJson["dimensions"] =  dimensionsBlobs;

    outputBlob = dimensionsJson.dump();
    outputBlob = "VIDEO_LENGTH_FRAME_SIZE_START<<<" + outputBlob + ">>>VIDEO_LENGTH_FRAME_SIZE_END";

    strcpy(data, outputBlob.c_str());
    boost::asio::write(*sock, boost::asio::buffer(data, max_length));
    memset(data, '\0', max_length);
    std::cout << outputBlob << std::endl;

    // std::cout << std::setw(4) << dimensionsJson << std::endl;

		// std::ofstream dimensionsJsonFile;
		// dimensionsJsonFile.open("/Users/pratik/AITOE/aisaac/build/storageDir/Output/dimensionsJSONPratik.json");
		// dimensionsJsonFile << dimensionsJson.dump(4);
		// dimensionsJsonFile.close();
}

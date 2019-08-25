//
// Created by aitoe on 10/2/17.
//

#ifndef AISAAC_MONGOLINK_H
#define AISAAC_MONGOLINK_H


#include "../../src/aiSaac.h"
#include <bsoncxx/v_noabi/bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/v_noabi/bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/v_noabi/bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/v_noabi/bsoncxx/json.hpp>
#include <bsoncxx/v_noabi/bsoncxx/string/view_or_value.hpp>
#include <bsoncxx/v_noabi/bsoncxx/builder/core.hpp>
#include <bsoncxx/v_noabi/bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/v_noabi/bsoncxx/stdx/string_view.hpp>
#include <mongocxx/v_noabi/mongocxx/client.hpp>
#include <mongocxx/v_noabi/mongocxx/instance.hpp>
#include <mongocxx/v_noabi/mongocxx/stdx.hpp>
#include <mongocxx/v_noabi/mongocxx/uri.hpp>
#include <typeinfo>
#include <stdlib.h>
class mongoLink {

public:
	  mongoLink(int procFPS);
    void motionMONGO(std::vector<aiSaac::BlobObject> blobContainer, int frameNumber, std::string resultTableId);
    void faceAnalyticsMONGO(std::vector<aiSaac::FaceBlob> blobContainer,int frameNumber,std::string resultTableId,std::string resultTableFeature);
    void ageAnalyticsMONGO(std::vector<aiSaac::FaceBlob> blobContainer,int frameNumber,std::string resultTableId);
    void genderAnalyticsMONGO(std::vector<aiSaac::FaceBlob> blobContainer,int frameNumber,std::string resultTableId);
    void emotionAnalyticsMONGO(std::vector<aiSaac::FaceBlob> blobContainer,int frameNumber,std::string resultTableId);
    void headAnalyticsMONGO();
    void humanAnalyticsMONGO(std::vector<aiSaac::HumanBlob> blobContainer,int frameNumber,std::string resultTableId);
    void objectAnalyticsMONGO(std::vector<aiSaac::ObjectBlob> blobContainer, int frameNumber, std::string resultTableId);
    void sceneAnalyticsMONGO(std::vector<aiSaac::SceneBlob> blobContainer, int frameNumber, std::string resultTableId);
    void textAnalyticsMONGO(std::vector<aiSaac::TextBlob> blobContainer, int frameNumber, std::string resultTableId);
    void summarizationMONGO(std::vector<int> orderedKeyPoints, aiSaac::AiSaacSettings *aiSaacSettings, std::string resultTableId);
	  void pushvideoLengthAndFrameSize(std::string videoLength, int height, int width, std::string resultTableId);

private:
	int procFPS;
};


#endif //AISAAC_MONGOLINK_H

//
// Created by aitoe on 10/7/17.
//

#ifndef AISAAC_SERIAL_LINK_H
#define AISAAC_SERIAL_LINK_H

#include "../../src/aiSaac.h"
#include "../../src/utils/json.hpp"
#include <stdlib.h>
#include <typeinfo>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include "../realTimeAlerts/alertBlob.h"

using boost::asio::ip::tcp;
typedef boost::shared_ptr<tcp::socket> socket_ptr;

class mongoLink {

public:
    mongoLink(int procFPS);
    void motionMONGO(std::vector<aiSaac::BlobObject> blobContainer, int frameNumber, std::string resultTableId, int chunkStartFrameNumber);
    void motionMONGO(std::vector<aiSaac::BlobObject> blobContainer, int frameNumber, std::string resultTableId);
    void objectMONGO(std::vector<aiSaac::ObjectBlob> blobContainer, int frameNumber, std::string resultTableId, int chunkStartFrameNumber);
    void objectMONGO(std::vector<aiSaac::ObjectBlob> blobContainer, int frameNumber, std::string resultTableId);
    void motionPlusObjectMONGO(std::vector<aiSaac::ObjectBlob> blobContainer, int frameNumber, std::string resultTableId);
    void motionEventAlert(AlertBlob alertBlob);
    void ruleAlertMONGO(AlertBlob alertBlob);
    void extractedFeatureMONGO(std::string label, std::vector<float> extractedFeatureVector);
    void faceAnalyticsMONGO(std::vector<aiSaac::FaceBlob> blobContainer, int frameNumber,std::string resultTableId, std::string resultTableFeature);
    void faceAnalyticsMONGO(std::vector<aiSaac::FaceBlob> blobContainer, int frameNumber,std::string resultTableId, std::string resultTableFeature, socket_ptr sock);
    void headAnalyticsMONGO();
    void headAnalyticsMONGO(socket_ptr sock); // Yet to be implemented.
    void humanAnalyticsMONGO(std::vector<aiSaac::HumanBlob> blobContainer, int frameNumber, std::string resultTableId);
    void humanAnalyticsMONGO(std::vector<aiSaac::HumanBlob> blobContainer, int frameNumber, std::string resultTableId, socket_ptr sock);
    void summarizationMONGO(std::vector<int> orderedKeyPoints, aiSaac::AiSaacSettings *aiSaacSettings, std::string resultTableId);
    void summarizationMONGO(std::vector<int> orderedKeyPoints, aiSaac::AiSaacSettings *aiSaacSettings, std::string resultTableId, socket_ptr sock);
    void pushvideoLengthAndFrameSize(std::string videoLength, int height, int width, std::string resultTableId, socket_ptr sock);

private:
    int procFPS;
};


#endif //AISAAC_SERIAL_LINK_H

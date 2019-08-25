//
// Created by aitoe on 07/11/2017
//

#ifndef AISAAC_REALTIMEALERTS_H
#define AISAAC_REALTIMEALERTS_H

#include <iostream>
#include <vector>
#include "alertBlob.h"
#include "activeAlertBlob.h"
#include "motionEvent.h"
#include "ruleAlert.h"
#include "../../src/encoderCameraSettings.h"
#include "../../src/BgSubAnalytics/blobObject.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#ifdef USE_MONGO
#include "../../tools/mongoLink/mongoLink.h"
#else
#include "../../tools/mongoLink/serialLink.h"
#endif //USE_MONGO

class realTimeAlerts {

public:
  realTimeAlerts(encoderCameraSettings* camSettings, int procFPS);
  ~realTimeAlerts();
  std::string generateRandomString();
  bool isIntersect(cv::Rect ruleRect, cv::Rect blobRect);
  void purgeBlobContainer();
  bool checkLoiterCondition(ActiveAlertBlob& currentActiveAlertBlob, int ruleLoiterTime);
  bool checkLoiterEndCondition(ActiveAlertBlob& currentActiveAlertBlob, int loiteringAlertSensitivity);
  void sendMotionAlertStart(cv::Mat& frame);
  void sendMotionAlertEnd();
  void initializeMotionEventAlertObject();
  void sendLoiteringRuleAlertStart(aiSaac::BlobObject& motionBlob, cv::Mat& frame, ruleAlert& rtRuleAlert);
  void sendLoiteringRuleAlertEnd(aiSaac::BlobObject& motionBlob, ruleAlert& rtRuleAlert);
  void sendObjectRuleAlertStart(aiSaac::ObjectBlob& objectBlob, cv::Mat& frame, ruleAlert& rtRuleAlert);
  void sendObjectRuleAlertEnd(aiSaac::ObjectBlob& objectBlob, ruleAlert& rtRuleAlert);

private:
  int motionAlertSensitivity;
  int inactivityAlertSensitivity;
  int loiteringAlertSensitivity;
  int activeAlertSensitivity;
  bool motionAlertFlag;
  bool inactivityAlertFlag;
  int alertID;
  float intersectionSensitivity;
  std::string alertImageStoragePath;
  time_t motionEventTime;
  motionEvent* MotionEvent;
  AlertBlob motionEventAlert;
  AlertBlob tempRuleAlert;
  ActiveAlertBlob tempActiveRuleAlert;
  std::vector <AlertBlob> activeRuleAlert;
  std::vector <ActiveAlertBlob> activeRuleAlertBlob;
  mongoLink* mongoObject;
};


#endif // AISAAC_REALTIMEALERTS_H

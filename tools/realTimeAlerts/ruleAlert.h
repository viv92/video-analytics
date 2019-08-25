//
//  Created by AITOE on 9th January, 2018
//

#ifndef AISAAC_RULEALERT_H
#define AISAAC_RULEALERT_H

#include <iostream>
#include "../../src/encoderCameraSettings.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

class ruleAlert {
public:
  ruleAlert();
  ruleAlert(encoderCameraSettings* camSettings, int ruleID);
  ~ruleAlert();
  std::string getRuleType();

  nlohmann::json rule;
  cv::Rect markedRect;
  int ruleID;
  std::string ruleType;
  std::string objectLabel;
  std::string objectColor;
  int loiterTime;
  bool ruleEnabled;
};
#endif // AISAAC_RULEALERT_H

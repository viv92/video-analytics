//
//  Created by AITOE on 9th January, 2018
//

#include "ruleAlert.h"

ruleAlert::ruleAlert(encoderCameraSettings* camSettings, int ruleID) {
    std::cout << "ruleAlerts constructor called" << std::endl;
    this->rule = camSettings->getRuleValues(ruleID);
    this->ruleID = this->rule["ruleID"];
    this->ruleType = this->rule["ruleType"];
    this->markedRect = cv::Rect(this->rule["area"]["x"], this->rule["area"]["y"], this->rule["area"]["width"], this->rule["area"]["height"]);
    this->loiterTime = this->rule["loiterTime"];
    this->ruleEnabled = this->rule["enabled"];
    if (this->ruleType == "object") {
        this->objectLabel = this->rule["objectLabel"];
        this->objectColor = this->rule["objectColor"];
    }
}

ruleAlert::ruleAlert() {
    std::cout << "ruleAlert empty constructor called" << std::endl;
}

ruleAlert::~ruleAlert() {
    std::cout << "ruleAlert destructor called" << std::endl;
}

std::string ruleAlert::getRuleType() {
    return this->ruleType;
}

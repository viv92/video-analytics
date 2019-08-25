//
// Created by aitoe on 07/11/2017
//

#include "realTimeAlerts.h"

realTimeAlerts::realTimeAlerts(encoderCameraSettings* camSettings, int procFPS) {
    this->motionAlertFlag = camSettings->getMotionAlertFlag();
    this->motionAlertSensitivity = camSettings->getMotionAlertSensitivity();
    this->loiteringAlertSensitivity = camSettings->getLoiteringAlertSensitivity();
    this->intersectionSensitivity = camSettings->getIntersectionSensitivity();
    this->alertImageStoragePath = camSettings->getAlertImageStoragePath();
    this->MotionEvent = new motionEvent(this->motionAlertSensitivity);
    this->alertID = 0;
    this->motionEventTime = NULL;
    initializeMotionEventAlertObject();
    this->mongoObject = new mongoLink(procFPS);
    std::cout << "Exiting realTimeAlerts constructor" << std::endl;
}

realTimeAlerts::~realTimeAlerts() {
    std::cout << "realTimeAlerts destructor called" << std::endl;
}

std::string realTimeAlerts::generateRandomString() {
    std::string alphanum = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    std::string randomString;
    srand(time(NULL));
    for (int i = 0; i < 10; ++i) {
        randomString.push_back(alphanum[rand() % (sizeof(alphanum) - 1)]);
    }
    return randomString;
}

bool realTimeAlerts::isIntersect(cv::Rect ruleRect, cv::Rect blobRect) {
    if ((ruleRect & blobRect).area() >= (ruleRect.area() * this->intersectionSensitivity)) {
        return true;
    }
    return false;
}

void realTimeAlerts::purgeBlobContainer() {
    if (this->activeRuleAlert.size() > 400) {
        this->activeRuleAlert.erase(this->activeRuleAlert.begin(), this->activeRuleAlert.begin() + 200);
        std::cout << "Purged activeRuleAlert container" << std::endl;
    }
    if (this->activeRuleAlertBlob.size() > 400) {
        this->activeRuleAlertBlob.erase(this->activeRuleAlertBlob.begin(), this->activeRuleAlertBlob.begin() + 200);
        std::cout << "Purged activeRuleAlertBlob container" << std::endl;
    }
    for (std::vector <ActiveAlertBlob>::iterator it = this->activeRuleAlertBlob.begin(); it != this->activeRuleAlertBlob.end(); it++) {
        if (difftime(time(NULL), it->lastUpdatedTime) > 20) {
            it = this->activeRuleAlertBlob.erase(it);
        }
    }
}

void realTimeAlerts::initializeMotionEventAlertObject() {
    this->motionEventAlert.alertID = 0;
    this->motionEventAlert.alertType = "";
    this->motionEventAlert.startTime = NULL;
    this->motionEventAlert.endTime = NULL;
    this->motionEventAlert.alertActive = false;
    this->motionEventAlert.alertFramePath = "";
}

void realTimeAlerts::sendMotionAlertStart(cv::Mat& frame) {
    if (this->motionAlertFlag) {
        this->motionEventTime = time(0);
        std::cout << "New MotionEventAlert Started" << std::endl;
        this->alertID++;
        this->motionEventAlert.alertID = this->alertID;
        this->motionEventAlert.alertType = "motion";
        this->motionEventAlert.startTime = this->motionEventTime;
        this->motionEventAlert.alertActive = true;
        std::string fileName = this->alertImageStoragePath + "/" + generateRandomString() + ".jpg";
        cv::imwrite(fileName, frame);
        this->motionEventAlert.alertFramePath = fileName;
        mongoObject->motionEventAlert(motionEventAlert);
    }
}

void realTimeAlerts::sendMotionAlertEnd() {
    if (this->motionAlertFlag) {
        this->motionEventAlert.alertActive = false;
        this->motionEventAlert.endTime = this->motionEventTime;
        mongoObject->motionEventAlert(motionEventAlert);
        std::cout << "MotionEventAlert Ended" << std::endl;
    }
}

bool realTimeAlerts::checkLoiterCondition(ActiveAlertBlob& currentActiveAlertBlob, int ruleLoiterTime) {
    if (difftime(currentActiveAlertBlob.lastLoiterTime, currentActiveAlertBlob.firstLoiterTime) >= ruleLoiterTime) {
        return true;
    }
    return false;
}

bool realTimeAlerts::checkLoiterEndCondition(ActiveAlertBlob& currentActiveAlertBlob, int loiteringAlertSensitivity) {
    if (difftime(currentActiveAlertBlob.lastUpdatedTime, currentActiveAlertBlob.lastLoiterTime) >= loiteringAlertSensitivity) {
        return true;
    }
    return false;
}

void realTimeAlerts::sendLoiteringRuleAlertStart(aiSaac::BlobObject& motionBlob, cv::Mat& frame, ruleAlert& rtRuleAlert) {
    // this->purgeBlobContainer();
    if (this->isIntersect(rtRuleAlert.markedRect, motionBlob.lastRectangle)) {
        int activeAlertflag = 0;
        if (!this->activeRuleAlert.empty()) {
            for (int i = 0; i < this->activeRuleAlert.size(); i++) {
                if ((this->activeRuleAlert.at(i).alertActive == true) && (this->activeRuleAlert.at(i).blobID == motionBlob.ID) && (this->activeRuleAlert.at(i).ruleID == rtRuleAlert.ruleID) && (this->activeRuleAlert.at(i).alertType == "loitering")) {
                    std::cout << "Rule Active for blob " << this->activeRuleAlert.at(i).blobID << " alert ID " << this->activeRuleAlert.at(i).alertID << std::endl;
                    this->activeRuleAlert.at(i).endTime = time(NULL);
                    activeAlertflag = 1;
                    break;
                }
            }
        }
        if (!activeAlertflag) {
            int activeRuleAlertBlobFlag = 0;
            if (!this->activeRuleAlertBlob.empty()) {
                for (int i = 0; i < this->activeRuleAlertBlob.size(); i++) {
                    if ((this->activeRuleAlertBlob.at(i).blobID == motionBlob.ID) && (this->activeRuleAlertBlob.at(i).ruleID == rtRuleAlert.ruleID) && (this->activeRuleAlertBlob.at(i).alertType == "loitering") && (this->checkLoiterCondition(this->activeRuleAlertBlob.at(i), rtRuleAlert.loiterTime))) {
                        this->tempRuleAlert.alertID = this->activeRuleAlertBlob.at(i).alertID;
                        this->tempRuleAlert.ruleID = rtRuleAlert.ruleID;
                        this->tempRuleAlert.blobID = motionBlob.ID;
                        this->tempRuleAlert.alertType = "loitering";
                        this->tempRuleAlert.startTime = time(NULL);
                        this->tempRuleAlert.endTime = time(NULL);
                        this->tempRuleAlert.alertActive = true;
                        std::string fileName = this->alertImageStoragePath + "/" + generateRandomString() + ".jpg";
                        cv::rectangle(frame, rtRuleAlert.markedRect, cv::Scalar(0, 255, 0));
                        cv::rectangle(frame, motionBlob.lastRectangle, cv::Scalar(0, 0, 255));
                        cv::imwrite(fileName, frame);
                        this->tempRuleAlert.alertFramePath = fileName;
                        mongoObject->ruleAlertMONGO(tempRuleAlert);
                        std::cout << "Rule Alert Sent" << std::endl;
                        activeRuleAlert.push_back(tempRuleAlert);
                        this->activeRuleAlertBlob.at(i).lastLoiterTime = time(NULL);
                        this->activeRuleAlertBlob.at(i).lastUpdatedTime = time(NULL);
                        this->activeRuleAlertBlob.at(i).alertActive = true;
                        activeRuleAlertBlobFlag = 1;
                        break;
                    } else if ((this->activeRuleAlertBlob.at(i).blobID == motionBlob.ID) && (this->activeRuleAlertBlob.at(i).ruleID == rtRuleAlert.ruleID) && (this->activeRuleAlertBlob.at(i).alertType == "loitering") && !(this->checkLoiterCondition(this->activeRuleAlertBlob.at(i), rtRuleAlert.loiterTime))) {
                        this->activeRuleAlertBlob.at(i).lastLoiterTime = time(NULL);
                        this->activeRuleAlertBlob.at(i).lastUpdatedTime = time(NULL);
                        activeRuleAlertBlobFlag = 1;
                        break;
                    }
                }
            }
            if (!activeRuleAlertBlobFlag) {
                this->alertID++;
                this->tempActiveRuleAlert.alertID = this->alertID;
                this->tempActiveRuleAlert.ruleID = rtRuleAlert.ruleID;
                this->tempActiveRuleAlert.blobID = motionBlob.ID;
                this->tempActiveRuleAlert.alertType = "loitering";
                this->tempActiveRuleAlert.firstLoiterTime = time(NULL);
                this->tempActiveRuleAlert.lastLoiterTime = time(NULL);
                this->tempActiveRuleAlert.lastUpdatedTime = time(NULL);
                this->tempActiveRuleAlert.alertActive = false;
                activeRuleAlertBlob.push_back(tempActiveRuleAlert);
                std::cout << "Pushed back potential loitering alert with alertID " << this->alertID << std::endl;
            }
        }
    } else {
        int activeAlertflag = 0;
        if (!this->activeRuleAlert.empty()) {
            for (int i = 0; i < this->activeRuleAlert.size(); i++) {
                if ((this->activeRuleAlert.at(i).alertActive == true) && (this->activeRuleAlert.at(i).blobID == motionBlob.ID) && (this->activeRuleAlert.at(i).ruleID == rtRuleAlert.ruleID) && (this->activeRuleAlert.at(i).alertType == "loitering")) {
                    std::cout << "Rule Active for blob " << this->activeRuleAlert.at(i).blobID << " alert ID " << this->activeRuleAlert.at(i).alertID << std::endl;
                    this->activeRuleAlert.at(i).endTime = time(NULL);
                    if (!this->activeRuleAlertBlob.empty()) {
                        for (int j = 0; j < this->activeRuleAlertBlob.size(); j++) {
                            if ((this->activeRuleAlertBlob.at(i).blobID == motionBlob.ID) && (this->activeRuleAlertBlob.at(i).ruleID == rtRuleAlert.ruleID) && (this->activeRuleAlert.at(i).alertID == this->activeRuleAlertBlob.at(i).alertID)) {
                                this->activeRuleAlertBlob.at(i).lastUpdatedTime = time(NULL);
                                break;
                            }
                        }
                    }
                    activeAlertflag = 1;
                    break;
                }
            }
        }
        if (!activeAlertflag) {
            if (!this->activeRuleAlertBlob.empty()) {
                for (int i = 0; i < this->activeRuleAlertBlob.size(); i++) {
                    if ((this->activeRuleAlertBlob.at(i).blobID == motionBlob.ID) && (this->activeRuleAlertBlob.at(i).ruleID == rtRuleAlert.ruleID) && (this->activeRuleAlertBlob.at(i).alertType == "loitering")) {
                        this->activeRuleAlertBlob.at(i).lastUpdatedTime = time(NULL);
                        break;
                    }
                }
            }
        }
    }
}

void realTimeAlerts::sendLoiteringRuleAlertEnd(aiSaac::BlobObject& motionBlob, ruleAlert& rtRuleAlert) {
    int deleteFlag = 0;
    int activeAlertflag = 0;
    if (!this->activeRuleAlert.empty()) {
        for (int i = 0; i < this->activeRuleAlert.size(); i++) {
            if ((this->activeRuleAlert.at(i).alertActive == true) && (this->activeRuleAlert.at(i).blobID == motionBlob.ID) && (this->activeRuleAlert.at(i).ruleID == rtRuleAlert.ruleID) && (this->activeRuleAlertBlob.at(i).alertType == "loitering")) {
                this->activeRuleAlert.at(i).endTime = time(NULL);
                if (!this->activeRuleAlertBlob.empty()) {
                    for (int j = 0; j < this->activeRuleAlertBlob.size(); j++) {
                        if ((this->activeRuleAlertBlob.at(j).blobID == motionBlob.ID) && (this->activeRuleAlertBlob.at(j).ruleID == rtRuleAlert.ruleID) && (this->activeRuleAlertBlob.at(i).alertType == "loitering") && (this->checkLoiterEndCondition(this->activeRuleAlertBlob.at(j), this->loiteringAlertSensitivity))) {
                            this->activeRuleAlertBlob.at(j).lastUpdatedTime = time(NULL);
                            this->activeRuleAlertBlob.at(j).alertActive = false;
                            this->activeRuleAlert.at(i).alertActive = false;
                            mongoObject->ruleAlertMONGO(this->activeRuleAlert.at(i));
                            this->activeRuleAlertBlob.erase(this->activeRuleAlertBlob.begin() + j);
                            deleteFlag = 1;
                            break;
                        }
                    }
                    if (deleteFlag) {
                        std::cout << "Deleted rule for blob ID " << this->activeRuleAlert.at(i).blobID << " alert ID " << this->activeRuleAlert.at(i).alertID << std::endl;
                        this->activeRuleAlert.erase(this->activeRuleAlert.begin() + i);
                        activeAlertflag = 1;
                        break;
                    }
                }
            }
        }
    }
    if (!activeAlertflag) {
        if (!this->activeRuleAlertBlob.empty()) {
            for (int k = 0; k < this->activeRuleAlertBlob.size(); k++) {
                if ((this->activeRuleAlertBlob.at(k).blobID == motionBlob.ID) && (this->activeRuleAlertBlob.at(k).ruleID == rtRuleAlert.ruleID) && (this->activeRuleAlertBlob.at(k).alertType == "loitering") && (this->checkLoiterEndCondition(this->activeRuleAlertBlob.at(k), this->loiteringAlertSensitivity))) {
                    this->activeRuleAlertBlob.at(k).lastUpdatedTime = time(NULL);
                    this->activeRuleAlertBlob.at(k).alertActive = false;
                    this->activeRuleAlertBlob.erase(this->activeRuleAlertBlob.begin() + k);
                    break;
                }
            }
        }
    }
}

void realTimeAlerts::sendObjectRuleAlertStart(aiSaac::ObjectBlob& objectBlob, cv::Mat& frame, ruleAlert& rtRuleAlert) {
    // this->purgeBlobContainer();
    if (this->isIntersect(rtRuleAlert.markedRect, objectBlob.lastRectangle) && (rtRuleAlert.objectLabel == objectBlob.metaData[0]) && (rtRuleAlert.objectColor == objectBlob.metaData[1])) {
        int activeAlertflag = 0;
        if (!this->activeRuleAlert.empty()) {
            for (int i = 0; i < this->activeRuleAlert.size(); i++) {
                if ((this->activeRuleAlert.at(i).alertActive == true) && (this->activeRuleAlert.at(i).blobID == objectBlob.ID) && (this->activeRuleAlert.at(i).ruleID == rtRuleAlert.ruleID) && (this->activeRuleAlert.at(i).alertType == "object")) {
                    std::cout << "Rule Active for blob " << this->activeRuleAlert.at(i).blobID << " alert ID " << this->activeRuleAlert.at(i).alertID << std::endl;
                    this->activeRuleAlert.at(i).endTime = time(NULL);
                    activeAlertflag = 1;
                    break;
                }
            }
        }
        if (!activeAlertflag) {
            int activeRuleAlertBlobFlag = 0;
            if (!this->activeRuleAlertBlob.empty()) {
                for (int i = 0; i < this->activeRuleAlertBlob.size(); i++) {
                    if ((this->activeRuleAlertBlob.at(i).blobID == objectBlob.ID) && (this->activeRuleAlertBlob.at(i).ruleID == rtRuleAlert.ruleID) && (this->activeRuleAlertBlob.at(i).alertType == "object") && (this->checkLoiterCondition(this->activeRuleAlertBlob.at(i), rtRuleAlert.loiterTime))) {
                        this->tempRuleAlert.alertID = this->activeRuleAlertBlob.at(i).alertID;
                        this->tempRuleAlert.ruleID = rtRuleAlert.ruleID;
                        this->tempRuleAlert.blobID = objectBlob.ID;
                        this->tempRuleAlert.alertType = "object";
                        this->tempRuleAlert.startTime = time(NULL);
                        this->tempRuleAlert.endTime = time(NULL);
                        this->tempRuleAlert.alertActive = true;
                        std::string fileName = this->alertImageStoragePath + "/" + generateRandomString() + ".jpg";
                        cv::rectangle(frame, rtRuleAlert.markedRect, cv::Scalar(0, 255, 0));
                        cv::rectangle(frame, objectBlob.lastRectangle, cv::Scalar(0, 0, 255));
                        cv::imwrite(fileName, frame);
                        this->tempRuleAlert.alertFramePath = fileName;
                        mongoObject->ruleAlertMONGO(tempRuleAlert);
                        activeRuleAlert.push_back(tempRuleAlert);
                        this->activeRuleAlertBlob.at(i).lastLoiterTime = time(NULL);
                        this->activeRuleAlertBlob.at(i).lastUpdatedTime = time(NULL);
                        this->activeRuleAlertBlob.at(i).alertActive = true;
                        activeRuleAlertBlobFlag = 1;
                        break;
                    } else if ((this->activeRuleAlertBlob.at(i).blobID == objectBlob.ID) && (this->activeRuleAlertBlob.at(i).ruleID == rtRuleAlert.ruleID) && (this->activeRuleAlertBlob.at(i).alertType == "object") && !(this->checkLoiterCondition(this->activeRuleAlertBlob.at(i), rtRuleAlert.loiterTime))) {
                        this->activeRuleAlertBlob.at(i).lastLoiterTime = time(NULL);
                        this->activeRuleAlertBlob.at(i).lastUpdatedTime = time(NULL);
                        activeRuleAlertBlobFlag = 1;
                        break;
                    }
                }
            }
            if (!activeRuleAlertBlobFlag) {
                this->alertID++;
                this->tempActiveRuleAlert.alertID = this->alertID;
                this->tempActiveRuleAlert.ruleID = rtRuleAlert.ruleID;
                this->tempActiveRuleAlert.blobID = objectBlob.ID;
                this->tempActiveRuleAlert.alertType = "object";
                this->tempActiveRuleAlert.firstLoiterTime = time(NULL);
                this->tempActiveRuleAlert.lastLoiterTime = time(NULL);
                this->tempActiveRuleAlert.lastUpdatedTime = time(NULL);
                this->tempActiveRuleAlert.alertActive = false;
                activeRuleAlertBlob.push_back(tempActiveRuleAlert);
            }
        }
    } else {
        int activeAlertflag = 0;
        if (!this->activeRuleAlert.empty()) {
            for (int i = 0; i < this->activeRuleAlert.size(); i++) {
                if ((this->activeRuleAlert.at(i).alertActive == true) && (this->activeRuleAlert.at(i).blobID == objectBlob.ID) && (this->activeRuleAlert.at(i).ruleID == rtRuleAlert.ruleID) && (this->activeRuleAlert.at(i).alertType == "object")) {
                    std::cout << "Rule Active for blob " << this->activeRuleAlert.at(i).blobID << " alert ID " << this->activeRuleAlert.at(i).alertID << std::endl;
                    this->activeRuleAlert.at(i).endTime = time(NULL);
                    if (!this->activeRuleAlertBlob.empty()) {
                        for (int j = 0; j < this->activeRuleAlertBlob.size(); j++) {
                            if ((this->activeRuleAlertBlob.at(i).blobID == objectBlob.ID) && (this->activeRuleAlertBlob.at(i).ruleID == rtRuleAlert.ruleID) && (this->activeRuleAlert.at(i).alertID == this->activeRuleAlertBlob.at(i).alertID) && (this->activeRuleAlertBlob.at(i).alertType == "object")) {
                                this->activeRuleAlertBlob.at(i).lastUpdatedTime = time(NULL);
                                break;
                            }
                        }
                    }
                    activeAlertflag = 1;
                    break;
                }
            }
        }
        if (!activeAlertflag) {
            if (!this->activeRuleAlertBlob.empty()) {
                for (int i = 0; i < this->activeRuleAlertBlob.size(); i++) {
                    if ((this->activeRuleAlertBlob.at(i).blobID == objectBlob.ID) && (this->activeRuleAlertBlob.at(i).ruleID == rtRuleAlert.ruleID) && (this->activeRuleAlertBlob.at(i).alertType == "object")) {
                        this->activeRuleAlertBlob.at(i).lastUpdatedTime = time(NULL);
                        break;
                    }
                }
            }
        }
    }
}

void realTimeAlerts::sendObjectRuleAlertEnd(aiSaac::ObjectBlob& objectBlob, ruleAlert& rtRuleAlert) {
    int deleteFlag = 0;
    int activeAlertflag = 0;
    if (!this->activeRuleAlert.empty()) {
        for (int i = 0; i < this->activeRuleAlert.size(); i++) {
            if ((this->activeRuleAlert.at(i).alertActive == true) && (this->activeRuleAlert.at(i).blobID == objectBlob.ID) && (this->activeRuleAlert.at(i).ruleID == rtRuleAlert.ruleID) && (this->activeRuleAlertBlob.at(i).alertType == "object")) {
                this->activeRuleAlert.at(i).endTime = time(NULL);
                if (!this->activeRuleAlertBlob.empty()) {
                    for (int j = 0; j < this->activeRuleAlertBlob.size(); j++) {
                        if ((this->activeRuleAlertBlob.at(j).blobID == objectBlob.ID) && (this->activeRuleAlertBlob.at(j).ruleID == rtRuleAlert.ruleID) && (this->activeRuleAlertBlob.at(i).alertType == "object") && (this->checkLoiterEndCondition(this->activeRuleAlertBlob.at(j), this->loiteringAlertSensitivity))) {
                            this->activeRuleAlertBlob.at(j).lastUpdatedTime = time(NULL);
                            this->activeRuleAlertBlob.at(j).alertActive = false;
                            this->activeRuleAlert.at(i).alertActive = false;
                            mongoObject->ruleAlertMONGO(this->activeRuleAlert.at(i));
                            this->activeRuleAlertBlob.erase(this->activeRuleAlertBlob.begin() + j);
                            deleteFlag = 1;
                            break;
                        }
                    }
                    if (deleteFlag) {
                        std::cout << "Deleted rule for blob ID " << this->activeRuleAlert.at(i).blobID << " alert ID " << this->activeRuleAlert.at(i).alertID << std::endl;
                        this->activeRuleAlert.erase(this->activeRuleAlert.begin() + i);
                        activeAlertflag = 1;
                        break;
                    }
                }
            }
        }
    }
    if (!activeAlertflag) {
        if (!this->activeRuleAlertBlob.empty()) {
            for (int k = 0; k < this->activeRuleAlertBlob.size(); k++) {
                if ((this->activeRuleAlertBlob.at(k).blobID == objectBlob.ID) && (this->activeRuleAlertBlob.at(k).ruleID == rtRuleAlert.ruleID) && (this->activeRuleAlertBlob.at(k).alertType == "object") && (this->checkLoiterEndCondition(this->activeRuleAlertBlob.at(k), this->loiteringAlertSensitivity))) {
                    this->activeRuleAlertBlob.at(k).lastUpdatedTime = time(NULL);
                    this->activeRuleAlertBlob.at(k).alertActive = false;
                    this->activeRuleAlertBlob.erase(this->activeRuleAlertBlob.begin() + k);
                    break;
                }
            }
        }
    }
}

//
// Created by aitoe on 25/11/2017
//

#include "logAlert.h"

logAlert::logAlert() {
    std::cout << "Calling logAlert constructor" << std::endl;
    // this->logID = 0;
    // this->currentBlob[ID] = NULL;
    // this->currentBlob[logTime] = NULL;
    this->currentBlob.logType = "";
    this->currentBlob.logMessage = "";
    this->currentBlob.callingFunction = "";
}

logAlert::~logAlert() {
    std::cout << "Called logAlert destructor" << std::endl;
}

void logAlert::updateLogBlob (std::string message, std::string type, std::string callingFunction) {
    // this->logID++;
    // this->currentBlob[ID] = logID;
    // this->currentBlob[logTime] = time(0);
    this->currentBlob.logType = type;
    this->currentBlob.logMessage = message;
    this->currentBlob.callingFunction = callingFunction;
}

std::string logAlert::jsonifyAlert() {
    nlohmann::json logAlertJson;
    std::string logAlertString;
    // logAlertJson[ID] = this->currentBlob[ID];
    // logAlertJson[logTime] = this->currentBlob[logTime];
    logAlertJson["logType"] = this->currentBlob.logType;
    logAlertJson["logMessage"] = this->currentBlob.logMessage;
    logAlertJson["callingFunction"] = this->currentBlob.callingFunction;
    logAlertString = logAlertJson.dump();
    return logAlertString;
}

void logAlert::sendLogAlert (std::string message, std::string type, std::string callingFunction) {
    updateLogBlob(message, type, callingFunction);
    std::string logAlert = jsonifyAlert();
    logAlert = "LOG_ALERT_BLOB_START<<<" + logAlert + ">>>LOG_ALERT_BLOB_END";
    std::cout << logAlert << std::endl;
}

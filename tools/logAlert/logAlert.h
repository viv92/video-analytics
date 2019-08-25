//
// Created by aitoe on 25/11/2017
//

#ifndef LOG_ALERT_H
#define LOG_ALERT_H

#include <iostream>
#include <string>
#include <ctime>
#include "../../src/utils/json.hpp"
#include "logBlob.h"

class logAlert {
public:
    logAlert();
    ~logAlert();
    void updateLogBlob (std::string message, std::string type, std::string callingFunction);
    std::string jsonifyAlert();
    void sendLogAlert(std::string message, std::string type, std::string callingFunction);
private:
    // int logID;
    LogBlob currentBlob;
};

#endif // LOG_ALERT_H

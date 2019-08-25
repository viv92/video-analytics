//
// Created by aitoe on 08/01/2018
//

#ifndef ALERT_BLOB_H
#define ALERT_BLOB_H

#include <vector>
#include <string>

class AlertBlob {
 public:
    int alertID;
    int ruleID;
    int blobID;
    std::string alertType;
    time_t startTime;
    time_t endTime;
    bool alertActive;
    std::string alertFramePath;
};

#endif  // ALERT_BLOB_H

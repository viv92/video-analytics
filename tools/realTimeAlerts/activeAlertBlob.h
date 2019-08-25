//
// Created by aitoe on 11/01/2018
//

#ifndef ACTIVE_ALERT_BLOB_H
#define ACTIVE_ALERT_BLOB_H

#include <iostream>

class ActiveAlertBlob {
 public:
    int alertID;
    int ruleID;
    int blobID;
    std::string alertType;
    time_t firstLoiterTime;
    time_t lastLoiterTime;
    time_t lastUpdatedTime;
    bool alertActive;
};

#endif  // ACTIVE_ALERT_BLOB_H

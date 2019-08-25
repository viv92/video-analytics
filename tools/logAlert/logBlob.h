//
// Created by aitoe on 25/11/2017
//

#ifndef LOG_BLOB_H
#define LOG_BLOB_H

#include <iostream>
#include <string>
#include <ctime>

class LogBlob {
public:
    // int ID;
    // time_t logTime;
    std::string logType;
    std::string logMessage;
    std::string callingFunction;
};

#endif // LOG_BLOB_H

/*
    Copyright 2016 AITOE
*/

#include <string>
#include <iostream>
#include "logging.h"
#include <fstream>
#include <algorithm>

bool setValue=false;
std::ofstream myFile;
void initDebug(bool fromMain, std::string storagePath) {
    if (fromMain) {
        std::string fileName = getInitTime();
        std::cout << "storage path " << storagePath << std::endl;
        std::cout << "file name " << fileName << std::endl;
        setValue = fromMain;
        myFile.open(storagePath+"/"+"debugFile.txt", std::ios_base::app);
        debugMsg("\n\n\n\n**************NEW  BUILD AT TIME "+fileName+"******\n\n\n\n");
    }
}

void debugMsg(std::string message) {
    if( setValue) {
        myFile << "[" << getInitTime() << "]  " << message << std::endl;
    }
}

std::string BoolToString(bool b) {
  return b ? "true" : "false";
}

std::string convertValid(std::string text) {
    std::replace(text.begin(), text.end(), ' ', '_');
    return text;
}

std::string getInitTime() {
    time_t rawtime;
    struct tm * timeinfo;
    char buffer[80];
    time (&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(buffer,80,"%d-%m-%Y %I:%M:%S",timeinfo);
    std::string str(buffer);
    return str;
}

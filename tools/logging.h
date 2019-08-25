/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   logger.h
 * Author: aitoe
 *
 * Created on 21 November, 2016, 4:33 PM
 */

#ifndef SRC_LOGGING_H_
#define SRC_LOGGING_H_
#include <string>

void initDebug (bool fromMain,std::string storagePath);
void debugMsg(std::string);
std::string BoolToString(bool b);
std::string convertValid(std::string text);
std::string getInitTime();

#endif  // SRC_LOGGING_H_

//
// Created by aitoe on 11/11/2017
//

#include "motionEvent.h"

motionEvent::motionEvent(int motionAlertSensitivity) {
    std::cout << "motionEvent constructor called" << std::endl;
    this->activeAlert = 0;
    this->isMotion = false;
    this->motionEndTime = NULL;
    this->motionAlertSensitivity = motionAlertSensitivity;
}

motionEvent::~motionEvent() {
    std::cout << "motionEvent destructor called" << std::endl;
}

bool motionEvent::checkMotionEventBegin(time_t motionTime) {
    if (this->isMotion) {
        this->motionEndTime = motionTime;
        return false;
    } else {
        this->isMotion = true;
        this->activeAlert = 1;
        this->motionEndTime = motionTime;
        return true;
    }
}

bool motionEvent::checkMotionEventEnd(time_t motionTime) {
    if (this->activeAlert == 1) {
        if (difftime(motionTime, this->motionEndTime) > this->motionAlertSensitivity) {
            this->activeAlert = 0;
            this->isMotion = false;
            return true;
        }
        return false;
    }
    return false;
}

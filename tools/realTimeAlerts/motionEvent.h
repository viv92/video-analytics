//
// Created by aitoe on 11/11/2017
//

#ifndef AISAAC_MOTIONEVENT_H
#define AISAAC_MOTIONEVENT_H

#include <iostream>
#include <ctime>

class motionEvent {

public:
    motionEvent(int motionAlertSensitivity);
    ~motionEvent();
    bool checkMotionEventBegin(time_t motionTime);
    bool checkMotionEventEnd(time_t motionTime);

private:
    bool isMotion;
    int activeAlert;
    int motionAlertSensitivity;
    time_t motionEndTime;
};
#endif // AISAAC_MOTIONEVENT_H

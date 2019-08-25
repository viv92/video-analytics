/*
    Copyright 2017 AITOE
*/

#ifndef ENCODER_CAMERA_SETTINGS_H
#define ENCODER_CAMERA_SETTINGS_H

#include <string>
#include <fstream>
#include "utils/json.hpp"

class encoderCameraSettings {
public:
    encoderCameraSettings(std::string encoderCameraSettingsFilePath);
    ~encoderCameraSettings();

    void loadSettings();
    void saveSettings();
    void resetDefaultValues();

    void setVideoGrabberFPS(int FPS);
    int getVideoGrabberFPS();

    void setCameraRTSP(std::string cameraRTSP);
    std::string getCameraRTSP();

    void setFaceImageStoragePath(std::string faceImageStoragePath);
    std::string getFaceImageStoragePath();

    void setAlertImageStoragePath(std::string alertImageStoragePath);
    std::string getAlertImageStoragePath();

    void setInactivityPeriod(int inactivityPeriod);
    int getInactivityPeriod();

    void setMinimumChunkSize(int minimumChunkSize);
    int getMinimumChunkSize();

    void setMaximumChunkSize(int maximumChunkSize);
    int getMaximumChunkSize();

    void setVlcFPS(int FPS);
    int getVlcFPS();

    void setInactivityAlertFlag(bool inactivityAlertFlag);
    bool getInactivityAlertFlag();

    void setActiveAlertSensitivity(int activeAlertSensitivity);
    int getActiveAlertSensitivity();

    void setMotionAlertFlag(bool motionAlertFlag);
    bool getMotionAlertFlag();

    void setLoiteringAlertSensitivity(int loiteringAlertSensitivity);
    int getLoiteringAlertSensitivity();

    void setInactivityAlertSensitivity(int inactivityAlertSensitivity);
    int getInactivityAlertSensitivity();

    void setMotionAlertSensitivity(int motionAlertSensitivity);
    int getMotionAlertSensitivity();

    void setIntrusionAlertSensitivty(int intrusionAlertSensitivty);
    int getIntrusionAlertSensitivty();

    void setIntrusionAreaSensitivty(int intrusionAreaSensitivty);
    int getIntrusionAreaSensitivty();

    void setIntersectionSensitivity(float intersectionSensitivity);
    float getIntersectionSensitivity();

    void setObjectAnalyticsEnabledFlag(bool flag);
    bool getObjectAnalyticsEnabledFlag();

    void setFacecAnalyticsEnabledFlag(bool flag);
    bool getFaceAnalyticsEnabledFlag();

    void setViewMotionAnnotateFlag(bool flag);
    bool getViewMotionAnnotateFlag();

    void setWriteMotionAnnotateFlag(bool flag);
    bool getWriteMotionAnnotateFlag();

    void setViewObjectAnnotateFlag(bool flag);
    bool getViewObjectAnnotateFlag();

    void setWriteObjectAnnotateFlag(bool flag);
    bool getWriteObjectAnnotateFlag();

    void setLoiteringAlertEnabledFlag(bool flag);
    bool getLoiteringAlertEnabledFlag();

    void setObjectAlertEnabledFlag(bool flag);
    bool getObjectAlertEnabledFlag();

    void setMotionAlertEnabledFlag(bool flag);
    bool getMotionAlertEnabledFlag();

    void setScaleFactor(double scaleFactor);
    double getScaleFactor();

    void setRuleValues(int index);
    nlohmann::json getRuleValues(int index);

    void setRuleCount(int index);
    int getRuleCount();

    nlohmann::json settings;

private:
    std::string settingsFilePath;
};

#endif  // ENCODER_CAMERA_SETTINGS_H

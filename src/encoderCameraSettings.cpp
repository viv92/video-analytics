/*
    Copyright 2017 AITOE
*/

#include "encoderCameraSettings.h"

encoderCameraSettings::encoderCameraSettings(std::string encoderCameraSettingsFilePath) {
    this->settingsFilePath = encoderCameraSettingsFilePath;
    std::ifstream f(this->settingsFilePath);
    if (f.good()) {
        std::cout << "Loading settings" << std::endl;
        this->loadSettings();
    } else {
        std::cout << "Cannot find settings file. Creating." << std::endl;
        this->resetDefaultValues();
    }
}

encoderCameraSettings::~encoderCameraSettings() {
    std::cout << "Deleting encoderCameraSettings" << std::endl;
}

void encoderCameraSettings::loadSettings() {
    std::ifstream configFile;
    configFile.open(this->settingsFilePath);
    configFile >> this->settings;
}

void encoderCameraSettings::saveSettings() {
    std::cout << "saving settings file" << this->settingsFilePath << std::endl;
    std::ofstream configFile;
    configFile.open(this->settingsFilePath);
    configFile << this->settings.dump(4);
    configFile.close();
}

void encoderCameraSettings::resetDefaultValues() {
    this->settings["encoderCameraSettings"]["videoGrabberFPS"] = 10;
    this->settings["encoderCameraSettings"]["cameraRTSP"] = "rtsp://admin:aitoe2016@192:168.1.210:554";
    this->settings["encoderCameraSettings"]["cameraStreamPort"] = 5010;
    this->settings["encoderCameraSettings"]["inactivityPeriod"] = 2;
    this->settings["encoderCameraSettings"]["minimumChunkSize"] = 10;
    this->settings["encoderCameraSettings"]["maximumChunkSize"] = 300;
    saveSettings();
}

void encoderCameraSettings::setVideoGrabberFPS(int FPS) {
    this->settings["encoderCameraSettings"]["videoGrabberFPS"] = FPS;
    saveSettings();
}

int encoderCameraSettings::getVideoGrabberFPS() {
    return this->settings["encoderCameraSettings"]["videoGrabberFPS"];
}

void encoderCameraSettings::setCameraRTSP(std::string cameraRTSP) {
    this->settings["encoderCameraSettings"]["cameraRTSP"] = cameraRTSP;
    saveSettings();
}

std::string encoderCameraSettings::getCameraRTSP() {
    return this->settings["encoderCameraSettings"]["cameraRTSP"];
}

void encoderCameraSettings::setFaceImageStoragePath(std::string faceImageStoragePath) {
    this->settings["encoderCameraSettings"]["analytics"]["faces"]["faceImageStoragePath"] = faceImageStoragePath;
    saveSettings();
}

std::string encoderCameraSettings::getFaceImageStoragePath() {
    return this->settings["encoderCameraSettings"]["analytics"]["faces"]["faceImageStoragePath"];
}

void encoderCameraSettings::setAlertImageStoragePath(std::string alertImageStoragePath) {
    this->settings["encoderCameraSettings"]["analytics"]["alertImageStoragePath"] = alertImageStoragePath;
    saveSettings();
}

std::string encoderCameraSettings::getAlertImageStoragePath() {
    return this->settings["encoderCameraSettings"]["analytics"]["alertImageStoragePath"];
}

void encoderCameraSettings::setInactivityPeriod(int inactivityPeriod) {
    this->settings["encoderCameraSettings"]["inactivityPeriod"] = inactivityPeriod;
    saveSettings();
}

int encoderCameraSettings::getInactivityPeriod() {
    return this->settings["encoderCameraSettings"]["inactivityPeriod"];
}

void encoderCameraSettings::setMinimumChunkSize(int minimumChunkSize) {
    this->settings["encoderCameraSettings"]["minimumChunkSize"] = minimumChunkSize;
    saveSettings();
}

int encoderCameraSettings::getMinimumChunkSize() {
    return this->settings["encoderCameraSettings"]["minimumChunkSize"];
}

void encoderCameraSettings::setMaximumChunkSize(int maximumChunkSize) {
    this->settings["encoderCameraSettings"]["maximumChunkSize"] = maximumChunkSize;
    saveSettings();
}

int encoderCameraSettings::getMaximumChunkSize() {
    return this->settings["encoderCameraSettings"]["maximumChunkSize"];
}

void encoderCameraSettings::setVlcFPS(int FPS) {
    this->settings["encoderCameraSettings"]["VlcFPS"] = FPS;
    saveSettings();
}

int encoderCameraSettings::getVlcFPS() {
    return this->settings["encoderCameraSettings"]["VlcFPS"];
}

void encoderCameraSettings::setMotionAlertFlag(bool motionAlertFlag) {
    this->settings["encoderCameraSettings"]["motionAlertFlag"] = motionAlertFlag;
    saveSettings();
}

bool encoderCameraSettings::getMotionAlertFlag() {
    return this->settings["encoderCameraSettings"]["motionAlertFlag"];
}

void encoderCameraSettings::setInactivityAlertFlag(bool inactivityAlertFlag) {
    this->settings["encoderCameraSettings"]["inactivityAlertFlag"] = inactivityAlertFlag;
    saveSettings();
}

bool encoderCameraSettings::getInactivityAlertFlag() {
    return this->settings["encoderCameraSettings"]["inactivityAlertFlag"];
}

void encoderCameraSettings::setMotionAlertSensitivity(int motionAlertSensitivity) {
    this->settings["encoderCameraSettings"]["analytics"]["advancedSettings"]["motionAlertSensitivity"] = motionAlertSensitivity;
    saveSettings();
}

int encoderCameraSettings::getMotionAlertSensitivity() {
    return this->settings["encoderCameraSettings"]["analytics"]["advancedSettings"]["motionAlertSensitivity"];
}

void encoderCameraSettings::setInactivityAlertSensitivity(int inactivityAlertSensitivity) {
    this->settings["encoderCameraSettings"]["analytics"]["motion"]["advancedSettings"]["inactivityAlertSensitivity"] = inactivityAlertSensitivity;
    saveSettings();
}

int encoderCameraSettings::getInactivityAlertSensitivity() {
    return this->settings["encoderCameraSettings"]["analytics"]["motion"]["advancedSettings"]["inactivityAlertSensitivity"];
}

void encoderCameraSettings::setLoiteringAlertSensitivity(int loiteringAlertSensitivity) {
    this->settings["encoderCameraSettings"]["analytics"]["advancedSettings"]["loiteringAlertSensitivity"] = loiteringAlertSensitivity;
    saveSettings();
}

int encoderCameraSettings::getLoiteringAlertSensitivity() {
    return this->settings["encoderCameraSettings"]["analytics"]["advancedSettings"]["loiteringAlertSensitivity"];
}

void encoderCameraSettings::setIntersectionSensitivity(float intersectionSensitivity) {
    this->settings["encoderCameraSettings"]["analytics"]["advancedSettings"]["intersectionSensitivity"] = intersectionSensitivity;
    saveSettings();
}

float encoderCameraSettings::getIntersectionSensitivity() {
    return this->settings["encoderCameraSettings"]["analytics"]["advancedSettings"]["intersectionSensitivity"];
}

void encoderCameraSettings::setActiveAlertSensitivity(int activeAlertSensitivity) {
    this->settings["encoderCameraSettings"]["analytics"]["motion"]["advancedSettings"]["activeAlertSensitivity"] = activeAlertSensitivity;
    saveSettings();
}

int encoderCameraSettings::getActiveAlertSensitivity() {
    return this->settings["encoderCameraSettings"]["analytics"]["motion"]["advancedSettings"]["activeAlertSensitivity"];
}

void encoderCameraSettings::setIntrusionAlertSensitivty(int intrusionAlertSensitivty) {
    this->settings["encoderCameraSettings"]["analytics"]["motion"]["advancedSettings"]["intrusionAlertSensitivty"] = intrusionAlertSensitivty;
    saveSettings();
}

int encoderCameraSettings::getIntrusionAlertSensitivty() {
    return this->settings["encoderCameraSettings"]["analytics"]["motion"]["advancedSettings"]["intrusionAlertSensitivty"];
}

void encoderCameraSettings::setIntrusionAreaSensitivty(int intrusionAreaSensitivty) {
    this->settings["encoderCameraSettings"]["analytics"]["motion"]["advancedSettings"]["intrusionAreaSensitivty"] = intrusionAreaSensitivty;
    saveSettings();
}

int encoderCameraSettings::getIntrusionAreaSensitivty() {
    return this->settings["encoderCameraSettings"]["analytics"]["motion"]["advancedSettings"]["intrusionAreaSensitivty"];
}

void encoderCameraSettings::setObjectAnalyticsEnabledFlag(bool flag) {
    this->settings["encoderCameraSettings"]["objectAnalyticsEnabledFlag"] = flag;
    saveSettings();
}

bool encoderCameraSettings::getObjectAnalyticsEnabledFlag() {
    return this->settings["encoderCameraSettings"]["objectAnalyticsEnabledFlag"];
}

void encoderCameraSettings::setFacecAnalyticsEnabledFlag(bool flag) {
    this->settings["encoderCameraSettings"]["faceAnalyticsEnabledFlag"] = flag;
    saveSettings();
}

bool encoderCameraSettings::getFaceAnalyticsEnabledFlag() {
    return this->settings["encoderCameraSettings"]["faceAnalyticsEnabledFlag"];
}

void encoderCameraSettings::setViewMotionAnnotateFlag(bool flag) {
    this->settings["encoderCameraSettings"]["viewMotionAnnotateFlag"] = flag;
    saveSettings();
}

bool encoderCameraSettings::getViewMotionAnnotateFlag() {
    return this->settings["encoderCameraSettings"]["viewMotionAnnotateFlag"];
}

void encoderCameraSettings::setWriteMotionAnnotateFlag(bool flag) {
    this->settings["encoderCameraSettings"]["writeMotionAnnotateFlag"] = flag;
    saveSettings();
}

bool encoderCameraSettings::getWriteMotionAnnotateFlag() {
    return this->settings["encoderCameraSettings"]["writeMotionAnnotateFlag"];
}

void encoderCameraSettings::setViewObjectAnnotateFlag(bool flag) {
    this->settings["encoderCameraSettings"]["viewObjectAnnotateFlag"] = flag;
    saveSettings();
}

bool encoderCameraSettings::getViewObjectAnnotateFlag() {
    return this->settings["encoderCameraSettings"]["viewObjectAnnotateFlag"];
}

void encoderCameraSettings::setWriteObjectAnnotateFlag(bool flag) {
    this->settings["encoderCameraSettings"]["writeObjectAnnotateFlag"] = flag;
    saveSettings();
}

bool encoderCameraSettings::getWriteObjectAnnotateFlag() {
    return this->settings["encoderCameraSettings"]["writeObjectAnnotateFlag"];
}

void encoderCameraSettings::setLoiteringAlertEnabledFlag(bool flag) {
    this->settings["encoderCameraSettings"]["loiteringAlertEnabled"] = flag;
    saveSettings();
}

bool encoderCameraSettings::getLoiteringAlertEnabledFlag() {
    return this->settings["encoderCameraSettings"]["loiteringAlertEnabled"];
}

void encoderCameraSettings::setObjectAlertEnabledFlag(bool flag) {
    this->settings["encoderCameraSettings"]["objectAlertEnabled"] = flag;
    saveSettings();
}

bool encoderCameraSettings::getObjectAlertEnabledFlag() {
    return this->settings["encoderCameraSettings"]["objectAlertEnabled"];
}

void encoderCameraSettings::setMotionAlertEnabledFlag(bool flag) {
    this->settings["encoderCameraSettings"]["motionAlertEnabled"] = flag;
    saveSettings();
}

bool encoderCameraSettings::getMotionAlertEnabledFlag() {
    return this->settings["encoderCameraSettings"]["motionAlertEnabled"];
}

void encoderCameraSettings::setScaleFactor(double scaleFactor) {
    this->settings["encoderCameraSettings"]["scaleFactor"] = scaleFactor;
    saveSettings();
}

double encoderCameraSettings::getScaleFactor() {
    return this->settings["encoderCameraSettings"]["scaleFactor"];
}


void encoderCameraSettings::setRuleValues(int index) {
  // Placeholder
}

nlohmann::json encoderCameraSettings::getRuleValues(int index) {
    return this->settings["encoderCameraSettings"]["analytics"]["rules"][index];
}

void encoderCameraSettings::setRuleCount(int index) {
  // Placeholder
}

int encoderCameraSettings::getRuleCount() {
    return this->settings["encoderCameraSettings"]["analytics"]["ruleCount"];
}

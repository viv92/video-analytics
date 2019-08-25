(function() {
  "use strict";
  module.exports = {
    port: 4000,
    dbUrl: 'localhost',
    dbPort: 27017,
    dbName: 'SmartEncoder',
    dbUsername: '',
    dbPassword: '',
    jwtSecret: 'Krishna@108',
    masterUrl: 'http://localhost:4000',
    newtonUrl: 'http://www.aitoelabs.com:8001',
    newtonUploadUrl: 'http://www.aitoelabs.com:7001',
    // newtonUrl: 'http://localhost:8000',
    // newtonUploadUrl: 'http://localhost:7001',
    projectRoot: '/home/aitoe/Desktop/smart_encoder_node/',
    daemonControllerExec: '/home/aitoe/Desktop/aisaac_lite/build/daemonControllerEncoderNoClient',
    storageDirectory: '/home/aitoe/Desktop/',
    aiSaacSettingsPath: '/home/aitoe/Desktop/aisaac_lite/build/storageDir/aiSaacSettingsconfig.json',
    downloadedMediaDir: '/home/aitoe/Desktop/smart_encoder_node/frontend/videos/',
    summarizationExec: '/home/aitoe/Desktop/aisaac_lite/build/summarizationJsonifyExec',
    connectCameraExec: '/home/aitoe/Desktop/aisaac_lite/build/connectFrameExec',
    connectImagePath: '/home/aitoe/Desktop/smart_encoder_node/frontend/images/camera/',
    cameraSettingsJson: '/home/aitoe/Desktop/smart_encoder_node/encoderCameraSettingsConfig.json',
    streamingExec: '/home/aitoe/Desktop/aisaac_lite/build/clientStreamingServer',
    gridStreamer: '/home/aitoe/Desktop/aisaac_lite/build/gridStreamer'
  };
}());

/*
    Copyright 2016 AITOE
*/

#include "libvlcStreamer.h"

// cv::Mat libVLCFrame;
std::mutex libMutex;

void display(void *data, void *id) {
    (void) data;
    assert(id == NULL);
}

void* lock(void *data, void**p_pixels) {
    struct ctx *ctx = (struct ctx*)data;
    // if ((*p_pixels != NULL) && (data != NULL)) {
        // WaitForSingleObject(ctx->mutex, INFINITE);
        *p_pixels = ctx->pixels;
    // }
    return NULL;
}

void unlock(void *data, void *id, void *const *p_pixels) {
    struct ctx *ctx = (struct ctx*)data;
    // if ((ctx->image != NULL) && (ctx->image->data != NULL)
    //    && (data != NULL)) {
        libMutex.lock();
        // libVLCFrame = *ctx->image;
        libMutex.unlock();
    // }
    (ctx->mutexObj->unlock());
}

LibvlcStreamer::LibvlcStreamer(std::string rtspUrl, int vlcFPS)  {
    this->cap = new cv::VideoCapture();
    this->stop = true;
    this->streaming = false;
    this->rtspUrl = rtspUrl;
    this->cap->set(CV_CAP_PROP_FPS, vlcFPS);
}

LibvlcStreamer::~LibvlcStreamer() {
    if (cap != NULL) {
        debugMsg("Releasing capture object in LibvlcStreamer");
        std::cout << "qDebug" << "Releasing capture object in LibvlcStreamer" << std::endl;
        this->cap->release();
    }
    delete this->cap;
}

cv::Size LibvlcStreamer::getFrameSize(const char* path) {
    libvlc_instance_t *vlcInstance;
    libvlc_media_player_t *mp;
    libvlc_media_t *media;

    char *vlc_argv[7];
    /*
    vlc_argv[0] = new char[3];
    strcpy_s(vlc_argv[0], 3, "-I");
    vlc_argv[1] = new char[6];
    strcpy_s(vlc_argv[1], 6, "dummy"); // Don't use any interface
    vlc_argv[2] = new char[16];
    strcpy_s(vlc_argv[2], 16, "--ignore-config"); // Don't use VLC's config
    vlc_argv[3] = new char[23];
    strcpy_s(vlc_argv[3], 23, "--plugin-path=/plugins");
    vlc_argv[4] = new char[3];
    strcpy_s(vlc_argv[4],3,"-R");
    vlc_argv[5] = new char[23];
    strcpy_s(vlc_argv[5],23,"--network-caching=1000");
    */
    vlc_argv[0] = "-I";
    vlc_argv[1] = "dummy";
    vlc_argv[2] = "--ignore-config";
    vlc_argv[3] = "--plugin-path=/plugins";
    vlc_argv[4] = "-R";
    vlc_argv[5] = "--network-caching=1000";

    int vlc_argc = 6;
    std::cout << "VLC OPENED" << std::endl;
    vlcInstance = libvlc_new(vlc_argc, vlc_argv);
    std::cout << "VLC OPENED 1" << std::endl;
    /*
    for (int i = 0; i < vlc_argc; i++) {
        delete[] vlc_argv[i];
    }
    */

    media = libvlc_media_new_location(vlcInstance, path);
    mp = libvlc_media_player_new_from_media(media);

    libvlc_media_release(media);
    libvlc_video_set_callbacks(mp, NULL, NULL, NULL, NULL);
    libvlc_video_set_format(mp, "RV24", 100, 100, 100 * 24 / 8);
    // pitch = width * BitsPerPixel / 8
    // libvlc_video_set_format(mp, "RV32", 100, 100, 100 * 4);
    libvlc_media_player_play(mp);

    // Sleep(2000);
    // wait a while so that something get rendered so that size info is
    // available
    unsigned int width = 1280, height = 720;

    for (int i = 0; i < 30 && height == 0; i++) {
        libvlc_video_get_size(mp, 0, &width, &height);
        if (width != 0 && height != 0) {
            break;
        }
    }

    if (width == 0 || height == 0) {
        width = 1280;
        height = 720;
    }

    libvlc_media_player_stop(mp);
    libvlc_release(vlcInstance);
    libvlc_media_player_release(mp);
    return cv::Size(width, height);
}

void LibvlcStreamer::startStream() {
    this->stop = false;
    std::cout << "Calling size" << std::endl;
    cv::Size sz = this->getFrameSize(this->rtspUrl.c_str());
    std::cout << sz.height << " " << sz.width << std::endl;
    debugMsg("Calling Size Height:"+std::to_string(sz.height)+"Width:"+std::to_string(sz.width));
    char *vlc_argv[7];
    /*
    vlc_argv[0] = new char[3];
    strcpy_s(vlc_argv[0], 3, "-I");
    vlc_argv[1] = new char[6];
    strcpy_s(vlc_argv[1], 6, "dummy"); // Don't use any interface
    vlc_argv[2] = new char[16];
    strcpy_s(vlc_argv[2], 16, "--ignore-config");
    // Don't use VLC's config
    vlc_argv[3] = new char[23];
    strcpy_s(vlc_argv[3], 23, "--plugin-path=/plugins");
    vlc_argv[4] = new char[3];
    strcpy_s(vlc_argv[4],3,"-R");
    vlc_argv[5] = new char[23];
    strcpy_s(vlc_argv[5],23,"--network-caching=1000");
    */
    vlc_argv[0] = "-I";
    vlc_argv[1] = "dummy";
    vlc_argv[2] = "--ignore-config";
    vlc_argv[3] = "--plugin-path=/plugins";
    vlc_argv[4] = "-R";
    vlc_argv[5] = "--network-caching=1000";

    int vlc_argc = 6;
    this->vlcInstance = libvlc_new(vlc_argc, vlc_argv);
    /*
    for (int i = 0; i < vlc_argc; i++) {
        delete[] vlc_argv[i];
    }
    */
    this->vlcMedia = libvlc_media_new_location(this->vlcInstance,
        this->rtspUrl.c_str());
    this->vlcMediaPlayer = libvlc_media_player_new_from_media(
        this->vlcMedia);
    libvlc_media_release(this->vlcMedia);
    this->context = (struct ctx*)malloc(sizeof(*this->context));
    // maybe to be done in constructor?
    this->context->mutexObj = new std::mutex();
    // CreateMutex(NULL, FALSE, NULL);
    this->context->image = new cv::Mat(sz.height, sz.width, CV_8UC3);
    this->context->pixels =
        (unsigned char *) this->context->image->data;
    libvlc_video_set_callbacks(this->vlcMediaPlayer, lock, unlock,
        display, this->context);
    libvlc_video_set_format(this->vlcMediaPlayer, "RV24", sz.width,
        sz.height, sz.width * 24 / 8);
    // pitch = width * BitsPerPixel / 8
    libvlc_media_player_play(this->vlcMediaPlayer);
    while (!this->stop) {
        // this->msleep(100);
        this->mutex.lock();
        libMutex.lock();
        cv::Mat libVLCFrame = *(this->context->image);
        this->in = libVLCFrame.clone();
        libMutex.unlock();
        this->mutex.unlock();
        if (this->in.rows > 0 && this->in.cols > 0) {
            this->streaming = true;
            // std::cout << this->in.size() << std::endl;
            // cv::imshow("MyVideo",in);
            cv::waitKey(5);
        } else {
            // std::cout << "no image" << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    libvlc_media_player_stop(this->vlcMediaPlayer);
    libvlc_release(this->vlcInstance);
    libvlc_media_player_release(this->vlcMediaPlayer);
    this->in.data = NULL;

    std::cout << "qDebug"
 << "LibvlcStreamer came out of while loop" << std::endl;
    // codeReview(Pratik): std::terminate should not be called. Use <threadName>.join() in the calling program instead.
    // std::terminate();
    debugMsg("Came out of the while loop");
}

void LibvlcStreamer::stopStream() {
    // cap->release();
    std::cout << "qDebug" << "LibvlcStreamer stop called" << std::endl;
    debugMsg("LibvlcStreamer stop called");
    this->streaming = false;
    this->stop = true;
}

cv::Mat LibvlcStreamer::getFrame() {
    aiVideoStreamer::mutex.lock();
    cv::Mat retFrame = this->in.clone();
    if (!(retFrame.empty() || retFrame.data == NULL)) {
        cv::resize(retFrame, retFrame, cv::Size((int)(retFrame.cols * 0.5), (int)(retFrame.rows * 0.5)));
    }
    this->mutex.unlock();
    // cv::waitKey(50);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    return retFrame;
}

bool LibvlcStreamer::isStreaming() {
    return this->streaming;
}

//
// Created by aitoe on 06/09/2017
//

#ifndef STREAMINGSERVER_H
#define STREAMINGSERVER_H

#include <cv.h>
#include <opencv2/highgui.hpp>

#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Net/SocketAddress.h"

#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/Net/MultipartWriter.h>
#include <string>
#include <iostream>

#include "commonStream.h"

class streamingServer {
public:
    streamingServer(commonStream* comStream, int PortNum);
    ~streamingServer();
    void initialise();
    void startServer();
    void stopServer();

private:
    Poco::Net::HTTPServer* server;
    Poco::Net::ServerSocket* socket;
    Poco::Net::HTTPServerParams* pParams;
    commonStream* comStream;
    std::string uri;
    int PortNum;
};

class VideoStreamingRequestHandler : public Poco::Net::HTTPRequestHandler {
    public:
        VideoStreamingRequestHandler(commonStream* comStream) {
            this->comStream = comStream;
            this->boundary = "VIDEOSTREAM";
            std::cout << "created handler" << std::endl;
        }

        ~VideoStreamingRequestHandler() {
            std::cout << "destructor called" << std::endl;
        }

        void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response) {
            response.set("Max-Age", "0");
            response.set("Expires", "0");
            response.set("Cache-Control", "no-cache, private");
            response.set("Pragma", "no-cache");
            response.setContentType("multipart/x-mixed-replace; boundary=--" + boundary);
            response.setChunkedTransferEncoding(false);

            std::cout << "got a request to stream" <<std::endl;

            std::ostream& out = response.send();
            int frames = 0;

            while (out.good()) {
                Poco::Net::MultipartWriter writer(out, boundary);
                std::vector<uchar>* buf = comStream->getEncodedFrame();
                if (buf->size() == 0) {
                    delete buf;
                    continue;
                }

                Poco::Net::MessageHeader header = Poco::Net::MessageHeader();
                header.set("Content-Length", std::to_string(buf->size()));
                header.set("Content-Type", "image/jpeg");
                writer.nextPart(header);
                out.write(reinterpret_cast<const char*>(buf->data()), buf->size());
                out << "\r\n\r\n";

                delete buf;
                buf = nullptr;

                ++frames;
                cv::waitKey(10);
            }
            std::cout << "ended request!!!" << std::endl;
        }

    private:
        commonStream* comStream;
        std::string boundary;
};

class VideoStreamingRequestHandlerFactory : public Poco::Net::HTTPRequestHandlerFactory {
    public:
        VideoStreamingRequestHandlerFactory(commonStream* comStream, const std::string& uri) {
            this->comStream = comStream;
            this->uri = uri;
            std::cout << "videostreamingrequesthandlerfactory constructor" <<std::endl;
        }

        Poco::Net::HTTPRequestHandler* createRequestHandler(const Poco::Net::HTTPServerRequest& request) {
            std::cout << "about to route" << std::endl;
            if (request.getURI() == uri) {
                std::cout << "created request handler" << std::endl;
                return new VideoStreamingRequestHandler(comStream);
            } else {
                std::cout << "/videostream is missing" << std::endl;
                return NULL;
            }
        }

    private:
        std::string uri;
        commonStream* comStream;
};

#endif // STREAMINGSERVER_H

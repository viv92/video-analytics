//
// Created by aitoe on 07/09/2017
//

#include "streamingServer.h"

streamingServer::streamingServer(int PortNum, int numStreams, std::string uri[]) {
    std::cout << "Initialising streamingServer" << std::endl;
    this->PortNum = PortNum;
    this->numStreams = numStreams;
    for(int i = 0; i < numStreams; i++) {
        this->uri[i] = uri[i];
    }
    initialise();
    std::cout << "Initialised streamingServer" << std::endl;
}

streamingServer::~streamingServer() {
    std::cout << "Called streamingServer destructor" << std::endl;
}

void streamingServer::initialise() {
    socket = new Poco::Net::ServerSocket(PortNum);
    pParams = new Poco::Net::HTTPServerParams;
    pParams->setMaxQueued(100);
    pParams->setMaxThreads(3);
    pParams->setKeepAlive(true);
    server = new Poco::Net::HTTPServer(new VideoStreamingRequestHandlerFactory(uri), *socket, pParams);
}

void streamingServer::startServer() {
    std::cout << "Starting Server" << std::endl;
    server->start();
    std::cout << "Server Started" << std::endl;
}

void streamingServer::stopServer() {
    std::cout << "Stopping Server" << std::endl;
    server->stop();
    std::cout << "Server Stopped" << std::endl;
}

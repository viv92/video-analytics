//
// Created by aitoe on 07/09/2017
//

#include "streamingServer.h"

streamingServer::streamingServer(commonStream* comStream, int PortNum) {
    std::cout << "Initialising streamingServer" << std::endl;
    this->comStream = comStream;
    this->PortNum = PortNum;
    initialise();
    std::cout << "Initialised streamingServer" << std::endl;
}

streamingServer::~streamingServer() {
    std::cout << "Called streamingServer destructor" << std::endl;
}

void streamingServer::initialise() {
    socket = new Poco::Net::ServerSocket(PortNum);
    this->uri = "/videostream";
    pParams = new Poco::Net::HTTPServerParams;
    pParams->setMaxQueued(100);
    pParams->setMaxThreads(3);
    pParams->setKeepAlive(true);
    server = new Poco::Net::HTTPServer(new VideoStreamingRequestHandlerFactory(comStream, uri), *socket, pParams);
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

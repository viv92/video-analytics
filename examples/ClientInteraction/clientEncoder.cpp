#include <cstdlib>
#include <cstring>
#include <iostream>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

enum { max_length = 5120 };

int main(int argc, char const *argv[]) {
    if (argc < 5) {
        std::cerr << "Usage: ./clientEncoder [host] [port] [cameraIP] [cameraPort] [cameraSettingsPath]" << std::endl;
        return -1;
    }

    boost::asio::io_service io_service;

    tcp::resolver resolver(io_service);
    tcp::resolver::query query(tcp::v4(), argv[1], argv[2]);
    tcp::resolver::iterator iterator = resolver.resolve(query);

    tcp::socket s(io_service);
    boost::asio::connect(s, iterator);
    std::string delimiter = "*@*";
    std::string message = std::string(argv[3]);
    for (int i = 4; i < argc; i++) {
        message = message + delimiter + std::string(argv[i]);
    }

    char request[max_length];
    memset(request, '\0', max_length);
    strcpy(request, message.c_str());
    size_t request_length = std::strlen(request);

    boost::asio::write(s, boost::asio::buffer(request, request_length));

    std::string terminating_message = "clientEncoder out!";

    while(1) {
        char reply[max_length];
        memset(reply, '\0', max_length);
        std::cout << std::flush;
        size_t reply_length = boost::asio::read(s, boost::asio::buffer(reply, max_length));
        std::cout.write(reply, reply_length);
        std::cout << std::flush;
        std::cout << "\n";
        std::cout << std::flush;
        if(strstr(reply, terminating_message.c_str()) != NULL) break;
    }
    return 0;
}

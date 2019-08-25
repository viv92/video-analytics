/*
    Copyright 2016 AITOE
*/

#ifndef SRC_AISAAC_DAEMON_H_
#define SRC_AISAAC_DAEMON_H_

#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include <syslog.h>
using boost::asio::ip::tcp;
typedef boost::shared_ptr<tcp::socket> socket_ptr;

namespace aiSaac{
    class Daemon{
    public:
        static void server(void (*session)(socket_ptr),boost::asio::io_service& io_service, short port);
        static int daemonize(boost::asio::io_service &io_service);
        
    private:
        const int max_length = 1024;
    };
}

#endif

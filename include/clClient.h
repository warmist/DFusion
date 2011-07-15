#ifndef CLCLIENT_H
#define CLCLIENT_H
#include <boost/asio.hpp>
#include <string>
#include <boost/system/error_code.hpp>
#include "network_msg.h"
#include <iostream>
using boost::asio::ip::tcp;
class clClient
{
        boost::asio::io_service &serv;
        std::vector <size_t> rd_int;
        std::vector <unsigned char> rd_char;
    public:
        clClient(boost::asio::io_service &serv,std::string address,std::string port);
        ~clClient();
        size_t Try_Read();
        size_t SendData(const message &data);
        tcp::socket socket;
};

#endif // CLCLIENT_H

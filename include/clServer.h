#ifndef CLSERVER_H
#define CLSERVER_H
#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/array.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include "network_msg.h"
#include "luastuff.h"
#include <map>
class clServer;
class connection:public boost::enable_shared_from_this<connection>
{
    boost::asio::io_service& service;
    boost::asio::ip::tcp::socket _socket;
    clServer *parent;
    unsigned id;
    connection(boost::asio::io_service &serv,int id,clServer *parent);
    message lastmsg;
    boost::array<size_t,2> header;
    std::vector<unsigned char> rd_char;
public:
    typedef boost::shared_ptr <connection> pointer;
    static pointer create(boost::asio::io_service &serv,int id,clServer *parent)
    {
        return pointer(new connection(serv,id,parent));
    }
    void close();
    void do_write(const message &msg);
    void do_close();
    void handle_read_header(const boost::system::error_code& error,std::size_t size);
    void handle_read_body(const boost::system::error_code& error,std::size_t size);
    void handle_write(const boost::system::error_code& error,std::size_t size);
    void start();
    boost::asio::ip::tcp::socket& socket(){return _socket;};
    ~connection();
};
class clServer
{
    public:
        clServer(boost::asio::io_service &serv,int port);
        virtual ~clServer();
        void start_accept();
        void Send(message &t,size_t id);
    protected:
    private:
        typedef std::map <size_t,boost::weak_ptr<connection> > mapCons;
        mapCons conmap;
        unsigned p;
        void handle_accept(connection::pointer new_connection,
                const boost::system::error_code& error);

        boost::asio::ip::tcp::acceptor accept;
        boost::asio::io_service &service;
};

#endif // CLSERVER_H

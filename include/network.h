#ifndef NETWORK_H
#define NETWORK_H
#include "luastuff.h"
#include <boost/asio.hpp>
#include <iostream>
#include "clServer.h"
#include "clClient.h"
enum NET_MODES
{
    MODE_OFF,MODE_CLIENT,MODE_SERVER
};
class Network_Base
{

    static Network_Base *me;
    NET_MODES mymode;
    Network_Base(){mymode=MODE_OFF;};
public:
    clServer *server;
    clClient *client;
    static Network_Base &Get()
    {
        if(!me)
            me=new Network_Base;
        return *me;
    }
    void StartClient(string address,string port);
    void StartServer(int port);
    void Send(message &msg,size_t id=0);
    void Tick();
    void Stop();
    /*template <class T>
    void Send(int trg,T const &ref)
    {

    }*/
    NET_MODES GetMode(){return mymode;};
    boost::asio::io_service io;

};
namespace lua
{
    void RegNetwork(lua::state &L);
}
#endif // NETWORK_H

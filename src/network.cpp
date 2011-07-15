#include "network.h"
Network_Base *Network_Base::me=0;
void Network_Base::StartClient(string address,string port)
{
    if(mymode==MODE_OFF)
    {
        try
        {
            client=new clClient(io,address,port);
            mymode=MODE_CLIENT;
        }
        catch(std::exception &e)
        {
            std::cout<<"Failed to start client:"<<e.what()<<std::endl;
        }
    }
    else
    {
        std::cout<<"Cant start client. Already started something"<<std::endl;
    }
}
void Network_Base::StartServer(int port)
{
    if(mymode==MODE_OFF)
    {
        try
        {
            server=new clServer(io,port);
            server->start_accept();
            mymode=MODE_SERVER;

        }
        catch(std::exception &e)
        {
            std::cout<<"Failed to start server:"<<e.what()<<std::endl;
        }
    }
    else
    {
        std::cout<<"Cant start server. Already started something"<<std::endl;
    }
}
void Network_Base::Tick()
{
    io.run_one();
    if(mymode==MODE_CLIENT)
    {
        client->Try_Read();
    }
}
void Network_Base::Send(message &msg,size_t id)
{
    if(mymode==MODE_CLIENT)
    {
        client->SendData(msg);
    }
    if(mymode==MODE_SERVER)
    {
        server->Send(msg,id);
    }
}
void Network_Base::Stop()
{
    io.reset();
    if(mymode==MODE_CLIENT)
        delete client;
    if(mymode==MODE_SERVER)
        delete server;
    mymode=MODE_OFF;
}
int startserver(lua_State *L)
{
    lua::state st(L);
    Network_Base::Get().StartServer(st.as<int>(1));
    return 0;
}
int startclient(lua_State *L)
{
    lua::state st(L);
    Network_Base::Get().StartClient(st.as<string>(1),st.as<string>(2));
    return 0;
}
int stopnet(lua_State *L)
{
    Network_Base::Get().Stop();
    return 0;
}
int send(lua_State *L)
{
    lua::state st(L);
    size_t id=st.as<size_t>(2);
    message msg;
    msg=LuaToMessage(st);

    Network_Base::Get().Send(msg,id);
    return 0;
}
int tick(lua_State *L)
{
    Network_Base::Get().Tick();
    return 0;
}
void lua::RegNetwork(lua::state &L)
{
    L.newtable();
    L.push(&startserver);
    L.setfield("server");//port
    L.push(&startclient);
    L.setfield("client");//host, port
    L.push(&stopnet);
    L.setfield("stop");

    L.push(&send);
    L.setfield("send"); //string,double,bool...
    L.newtable();
    L.setfield("OnRecieve");
    //L.push(&pushrecieve)
    //L.setfield("setOnRecieve"); //function, taking id and data
    L.push(&tick);
    L.setfield("tick");

    L.setglobal("net");
}

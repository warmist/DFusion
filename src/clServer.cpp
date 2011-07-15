#include "clServer.h"
connection::connection(boost::asio::io_service &serv,int id,clServer *parent):service(serv),id(id),parent(parent),
            _socket(serv)
{

}
void connection::close()
{
    service.post(boost::bind(&connection::do_close,shared_from_this()));
}
void connection::do_write(const message &msg)
{
    boost::array< boost::asio::const_buffer ,3> buffs;
    buffs[0]=boost::asio::buffer(&msg.msgtype,sizeof(msg.msgtype));
    buffs[1]=boost::asio::buffer(&msg.bodysize,sizeof(msg.bodysize));
    buffs[2]=boost::asio::buffer(msg.body.c_str(),msg.bodysize);
    boost::asio::async_write(_socket, buffs,
                             boost::bind(&connection::handle_write, shared_from_this(),
                                         boost::asio::placeholders::error,
                                         boost::asio::placeholders::bytes_transferred));

}
void connection::handle_read_header(const boost::system::error_code& error,std::size_t size)
{
    if(!error)
    {

        lastmsg.msgtype=header[0];
        lastmsg.bodysize=header[1];
        lastmsg.body.clear();
        rd_char.clear();
        rd_char.resize(lastmsg.bodysize);
        //std::cout<<"Handle read_header OK!"<<std::endl;
        //std::cout<<"Waiting for body of size:"<<lastmsg.bodysize<<std::endl;
        //rd_char=new char[lastmsg.bodysize];
        //readbuf=boost::asio::buffer(rd_chare);
        boost::asio::async_read(_socket,boost::asio::buffer(rd_char),
                            boost::bind(&connection::handle_read_body, shared_from_this(),
                                                  boost::asio::placeholders::error,boost::asio::placeholders::bytes_transferred));
        /*boost::asio::async_read(_socket,rbuf,boost::asio::transfer_at_least(3),
                boost::bind(&connection::handle_read_body, shared_from_this(),
                                                  boost::asio::placeholders::error,boost::asio::placeholders::bytes_transferred));*/
    }
    else
    {
        std::cout<<"Closing coz read err:";
        std::cout<<error.message()<<std::endl;
        do_close();
    }
}
void connection::handle_read_body(const boost::system::error_code& error,std::size_t size)
{
    if(!error)
    {

        lastmsg.body=std::string(rd_char.begin(),rd_char.end());
        //service.post(boost::bind(&clServer::AddEvent,parent,lastmsg,myid));
        MessageToLua(lastmsg,id);
        boost::asio::async_read(_socket,boost::asio::buffer(header),
                    boost::bind(&connection::handle_read_header, shared_from_this(),
                                                  boost::asio::placeholders::error,boost::asio::placeholders::bytes_transferred));
    }
    else
    {
        std::cout<<"Closing coz read err!:";
        std::cout<<error.message()<<std::endl;
        do_close();
    }
}
void connection::handle_write(const boost::system::error_code& error,std::size_t size)
{
    if (error)
        do_close();
}
void connection::do_close()
{
    _socket.close();
}
void connection::start()
{
    boost::asio::async_read(_socket,boost::asio::buffer(header),
                    boost::bind(&connection::handle_read_header, shared_from_this(),
                    boost::asio::placeholders::error,boost::asio::placeholders::bytes_transferred));
}
connection::~connection()
{

}





clServer::clServer(boost::asio::io_service &serv,int port):service(serv),
    accept(serv,boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
{
    //ctor
    p=0;
}

clServer::~clServer()
{
    //dtor
}
void clServer::start_accept()
{
    connection::pointer new_connection =
        connection::create(accept.io_service(),p,this);


    accept.async_accept(new_connection->socket(),
                        boost::bind(&clServer::handle_accept, this, new_connection,
                                    boost::asio::placeholders::error));
}
void clServer::handle_accept(connection::pointer new_connection,
                const boost::system::error_code& error)
{
    //std::cout<<"Trying to connect..."<<std::endl;
    if (!error)
    {
        std::cout<<"Connection made, id:"<<p<<std::endl;
        new_connection->start();
        conmap[p]=new_connection;
        p++;
        start_accept();

    }
    else
    {
        std::cout<<error.message()<<std::endl;
    }
}
void clServer::Send(message &t,size_t id)
{
    mapCons::iterator it;
    it=conmap.find(id);
    if(it!=conmap.end())
    {
        boost::weak_ptr<connection> cp=conmap[id];
       if(boost::shared_ptr<connection> r = cp.lock())
        {
            r->do_write(t);
        }
        else
        {
            conmap.erase(it);
        }
    }
}


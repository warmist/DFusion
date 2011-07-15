#include "clClient.h"
clClient::clClient(boost::asio::io_service &serv,std::string address,std::string port):serv(serv),socket(serv)
{
    rd_int.resize(2);
    tcp::resolver::iterator iterator;
    tcp::resolver::iterator end;
    //try
    //{
        tcp::resolver resolver(serv);
        tcp::resolver::query query(address,port);
        iterator =resolver.resolve(query);
        //std::cout<<iterator->host_name()<<" "<<iterator->service_name()<<std::endl;
    //}
    //catch (std::exception &e)
    //{
    //    std::cout<<e.what()<<std::endl;
    //    throw;
    //}
    boost::system::error_code error = boost::asio::error::host_not_found;
    while(error && iterator != end) {
            socket.close();
            socket.connect(*iterator++, error);
    }
    if (error)
            throw boost::system::system_error(error);

    /*try
    {
        socket.connect(*iterator);
    }
    catch (std::exception &e)
    {
        std::cout<<e.what()<<std::endl;
        throw;
    }*/
}
size_t clClient::Try_Read()
{

    boost::system::error_code error;
    size_t bytes_readable =socket.available(error); //bRead.get();

    size_t l=0;
    if (error)
    {
        std::cout<<error.message()<<std::endl;
        return 0;
    }

    if (bytes_readable!=0)
    {
        message data;
        l+=boost::asio::read(socket,boost::asio::buffer(rd_int),boost::asio::transfer_all(),error);
        if (!error)
        {
            data.msgtype=rd_int[0];
            data.bodysize=rd_int[1];
            rd_char.clear();
            rd_char.resize(data.bodysize);
            l+=boost::asio::read(socket,boost::asio::buffer(rd_char));
            data.body=std::string(rd_char.begin(),rd_char.end());
        }
        if (error)
        {
            std::cout<<error.message()<<std::endl;
        }

        MessageToLua(data,-1);

    }

    return l;
}

clClient::~clClient()
{

}
size_t clClient::SendData(const message &data)
{
    boost::array< boost::asio::const_buffer, 3> bufs;
    bufs[0]=boost::asio::buffer(&data.msgtype,sizeof(data.msgtype));
    bufs[1]=boost::asio::buffer(&data.bodysize,sizeof(data.bodysize));
    bufs[2]=boost::asio::buffer(data.body.c_str(),data.bodysize);

    return boost::asio::write(socket,bufs);
}


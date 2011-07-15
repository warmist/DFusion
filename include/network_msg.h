#ifndef NETWORK_MSG_H
#define NETWORK_MSG_H
#include <string>
#include "luastuff.h"
struct message
{
    //header
    size_t msgtype;
    size_t bodysize;
    //std::vector<unsigned char> body;
    std::string body;
    message(){bodysize=0;};
    static message Create(size_t type,std::string data);
    void updHeader();
    size_t size()const;
    void AppendData(void  const *p,size_t size);
    void ExtractData(void *p,size_t size,size_t offset=-1)const;
};

void MessageToLua(message &t,size_t id);
message LuaToMessage(lua::state &st);
#endif // NETWORK_MSG_H

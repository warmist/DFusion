#include "network_msg.h"


message message::Create(size_t type,std::string data)
{
    message t;
    t.msgtype=type;
    t.body=data;
    t.updHeader();
    //std::cout<<"Message created with:"<<t.body<<" its size:"<<t.bodysize<<std::endl;
    return t;
}
void message::updHeader()
{
    bodysize=body.size();
}
size_t message::size() const
{
    return sizeof(msgtype)+sizeof(bodysize)+bodysize;
}
void message::AppendData(void const *p,size_t size)
{

    for(size_t o=0;o<size;o++)
        {
            bodysize++;
            body+=((char*)p)[o];
        }
}
void message::ExtractData(void *p,size_t size,size_t offset)const
{
    if(offset==-1)
        offset=bodysize-size;
    for(size_t o=offset;o<size+offset;o++)
        ((char*)p)[o-offset]=body[o];
}


void MessageToLua(message &t,size_t id)
{
    lua::state st=lua::glua::Get();
    //LUA_TNIL, LUA_TNUMBER, LUA_TBOOLEAN, LUA_TSTRING, LUA_TTABLE, LUA_TFUNCTION, LUA_TUSERDATA, LUA_TTHREAD, and LUA_TLIGHTUSERDATA
    switch(t.msgtype)
    {
    case LUA_TNIL:
        st.push();
        break;
    case LUA_TNUMBER:
        double num;
        t.ExtractData(&num,sizeof(num));
        st.push(num);
        break;
    case LUA_TBOOLEAN:
        bool b;
        t.ExtractData(&b,sizeof(b));
        st.push(b);
        break;
    case LUA_TSTRING:
        st.push(t.body);
        break;
    default:
        ;
        //TODO...
    }
    int argpos=st.gettop();
    st.getglobal("net");
    st.getfield("OnRecieve");
    if(!st.is<lua::table>())
    {
        st.settop(0);
        return;
    }
    st.push();
    while(st.next())
    {
        st.push(id);
        st.pushvalue(argpos);
        st.pcall(2);
    }
    st.settop(0);
}
message LuaToMessage(lua::state &st)
{
    message ret;
    ret.bodysize=0;
    ret.msgtype=lua_type(st,1);
    switch(ret.msgtype)
    {
    case LUA_TNUMBER:
        {
        double n=st.as<double>(1);
        ret.AppendData(&n,sizeof(double));
        break;
        }
     case LUA_TBOOLEAN:
         {
        bool b=st.as<bool>(1);
        ret.AppendData(&b,sizeof(b));
        break;
         }
    case LUA_TSTRING:
        {
        std::string s=st.as<string>(1);
        ret.body=s;
        ret.bodysize=s.size();
        break;
        }
    case LUA_TNIL:
    default:
        ;
    }
    return ret;
}

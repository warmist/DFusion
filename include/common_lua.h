#ifndef COMMON_LUA_H
#define COMMON_LUA_H
#include "luastuff.h"
#include "DF_COMMON.h"
#include "bit.h"
namespace lua
{
    void RegCommon(lua::state &L);
    class Lua_Offset:public iDF_Offset
    {
        lua::state L;
        lua::object f;
        string name;
        bool lazy;
    public:
        Lua_Offset(lua::state iL,string name,int stackpos,bool lazy=false):L(iL),f(iL,stackpos),name(name),lazy(lazy)
        {
            offset=0;
            f.Get();
            iL.pop();
            DF_Offsets::Get().Register(this);
        };
        /*size_t GetOffset()
        {
            if(offset==0) //bad way of doing it...->> move to lua_getoffset.
                if(FindOff())
                    DF_Offsets::Get().Save();

            return offset;
        }*/
        bool FindOff();
        string GetName(){return name;};
        virtual ~Lua_Offset(){};
        bool isLazy(){return lazy;};
    };
}

#endif // COMMON_LUA_H

#ifndef DEBUGER_LUA_H
#define DEBUGER_LUA_H
#include "Debuger.h"
#include "luastuff.h"

namespace lua //TODO finish this...
{
    void RegDebuger(lua::state &L);
    class ll_bp;
    class lua_Breakpoint
    {
        int tid;
        ll_bp *mybp;
        state st;
    public:
        lua_Breakpoint(lua_State *L,int tid);
        ~lua_Breakpoint();
        int GetTableId(){return tid;};

        int IsOn(lua_State *L);
        int SetOn(lua_State *L);
        void Hit();

        DEF_LUNE(lua_Breakpoint);
    };
    class ll_bp:public Breakpoint
    {
        lua_Breakpoint *parent;
    public:
        ll_bp(DWORD pos,lua_Breakpoint *parent):Breakpoint(pos),parent(parent){};

        void Hit(){parent->Hit();};
    };
}

#endif // DEBUGER_LUA_H

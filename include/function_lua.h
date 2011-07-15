#ifndef FUNCTION_LUA_H
#define FUNCTION_LUA_H
#include "lune.h"
#include "Function.h"
class lua_Functional
{
    int tblid;
    FunctionCaller fc;
public:
    lua_Functional(lua_State *L,int id);
    int GetTableId(){return tblid;}
    int Run(lua_State *L);
    int isRunning(lua_State *L);
    int GetResult(lua_State *L);
    DEF_LUNE(lua_Functional);

};

namespace lua
{
    void RegFunctional(lua::state &L);
}

#endif // FUNCTION_LUA_H

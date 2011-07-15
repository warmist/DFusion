#ifndef LUASTUFF_H
#define LUASTUFF_H


extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

#include "lune.h" //TODO rewrite wrapper into lightuserdata type
#include "luaxx/luaxx.hpp"

namespace lua
{
    class glua
    {
    public:
        static state &Get();
    private:
        glua();
        static glua *ptr;
        state mystate;
    };
    void RegBasics(lua::state &L);
    string DebugDump(lua::state &L);

}
void RegFunctions(lua::state &L,luaL_reg const *arr);
void RegFunctionsLocal(lua::state &L,luaL_reg const *arr);


#endif // LUASTUFF_H

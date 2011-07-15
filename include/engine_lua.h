#ifndef ENGINE_LUA_H
#define ENGINE_LUA_H
#include "luastuff.h"
#include "PlugManager.h"
#include "OutFile.h"
#include "filesystem_lua.h"

namespace lua
{
    void RegEngine(lua::state &L);
}
#endif // ENGINE_LUA_H

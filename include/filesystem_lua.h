#ifndef FILESYSTEM_LUA_H
#define FILESYSTEM_LUA_H
#include "luastuff.h"
#include <boost/filesystem.hpp>
namespace lua
{
    void RegFilesystem(state &L);
}



#endif // FILESYSTEM_LUA_H

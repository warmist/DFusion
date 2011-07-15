//#define SOCKET

#include <network.h>
#include <iostream>
#include <list>
#include "DF_Common.h"
#include "DF_31_structs.h"
#include "PlugManager.h"
#include "OutFile.h"
#include "common_lua.h"
#include "engine_lua.h"
#include "function_lua.h"
#include "debuger_lua.h"
using namespace std;
enum runflags
{
    FLG_NONE=0,FLG_FIND=1,FLG_COMPAT=2,FLG_FILE=4,FLG_NET=8
};
std::string fname;
int parseflags(int argc, char ** argv)
{
    int ret=0;
    for (int i=1;i<argc;i++)
    {
        if(string(argv[i])==string("-f"))
            ret|=FLG_FIND;
        else if(string(argv[i])==string("-c"))
            ret|=FLG_COMPAT;
        else if(string(argv[i])==string("-n"))
            ret|=FLG_NET;
        else
        {
            ret|=FLG_FILE;
            fname=argv[i];
        }
    }
    return ret;
}
int main(int argc, char ** argv)
{
    bool nodf=false;
    if (!open_dwarf_fortress())
    {
        cout<<"Failed to find DF!"<<endl;
        cout<<"<======Limited functionality...=======>"<<endl;
        nodf=true;
        //cout<<"Quiting!"<<endl;
        //return -1;
    }
    lua::glua::Get();
    lua::RegEngine(lua::glua::Get());
    lua::RegCommon(lua::glua::Get());
    lua::RegDebuger(lua::glua::Get());
    lua::RegFunctional(lua::glua::Get());

    int flags=parseflags(argc,argv);
    if(flags&FLG_NET)
        lua::RegNetwork(lua::glua::Get());
    if(flags&FLG_COMPAT)
    {
        lua::glua::Get().push(1);
        lua::glua::Get().setglobal("COMPATMODE");
    }
    if(flags&FLG_FILE)
    {

        lua::glua::Get().push(fname);
        lua::glua::Get().setglobal("FILE");
    }
    if(flags&FLG_FIND)
    {
        try
        {
            lua::glua::Get().push(1);
            lua::glua::Get().setglobal("OFFSETSMODE");
            lua::glua::Get().loadfile("init.lua");
            lua::glua::Get().pcall();
        }
        catch(std::exception &e)
        {
            std::cout<<e.what();
            std::cout<<lua::DebugDump(lua::glua::Get())<<std::endl;
        }
        if(DF_Offsets::Get().Search())
        {
            DF_Offsets::Get().Save();
            DF_Offsets::Get().Print();
            return 0;
        }
        else
        {
            return -1;
        }
    }
    try
    {
        lua::glua::Get().loadfile("offsets.lua");
        lua::glua::Get().pcall();
    }
    catch(std::exception &e)
    {
        std::cout<<e.what()<<std::endl;
        //lua::StackDump(lua::glua::Get());
        //std::cout<<lua::DebugDump(lua::glua::Get())<<std::endl;
    }
    Debuger dbg(df_pid);
    try
    {
        DF_Offsets::Get().Load();

        dbg.Attach();
        dbg.ResumeThreads();
        dbg.Continue();
        while(dbg.Tick(100));
        PlugManager mng;
        mng.Install(DF_Offsets::Get().GetOff("CreatureGloss")+12+df_memory_base);
    }
    catch(const char *p)
    {
        std::cout<<p<<std::endl;
    }

    try
    {
        lua::glua::Get().loadfile("init.lua");
        lua::glua::Get().pcall();
    }
    catch(std::exception &e)
    {
        std::cout<<e.what()<<std::endl;
        //lua::StackDump(lua::glua::Get());
        std::cout<<lua::DebugDump(lua::glua::Get())<<endl;
        //std::cout<<"===End of stack==="<<endl;
        throw ;
    }
    resume_dwarf_fortress();
    DF_Offsets::Get().Save();
    return 0;
}

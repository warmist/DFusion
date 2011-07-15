#include "debuger_lua.h"
using namespace lua;
int suspend(lua_State *L)
{
    Debuger::GetActiveDebuger()->SuspendThreads();
    return 0;
}
int resume(lua_State *L)
{
    Debuger::GetActiveDebuger()->ResumeThreads();
    return 0;
}
int tick_dbg(lua_State *L)
{
    lua::state st(L);
    if((st.gettop()<1)||(st.is<lua::nil>(1)))
        Debuger::GetActiveDebuger()->Tick(INFINITE);
    else
        Debuger::GetActiveDebuger()->Tick(st.as<DWORD>(1));
    return 0;
}


lua_Breakpoint::lua_Breakpoint(lua_State *L,int tid):tid(tid)
{
    st=lua::state(L);
    mybp=new ll_bp(st.as<DWORD>(1),this);
    mybp->setReseting(true);
    mybp->setAutoDeactivate(true);
}
int lua_Breakpoint::IsOn(lua_State *L)
{
    st.push(mybp->isActive());
    return 1;
}
int lua_Breakpoint::SetOn(lua_State *L)
{
    bool toset=st.as<bool>(1);
    if(mybp->isActive()&& !toset)
    {
        mybp->Deactivate();
    }
    else if (!mybp->isActive() && toset)
    {
        mybp->Activate();
    }
    return 0;
}
void lua_Breakpoint::Hit()
{
    Lune<lua_Breakpoint>::GetTable(st,this);
    int table=st.gettop();
    st.getfield("OnHit");
    bool reactivate=false;
    if(st.is<lua::function>())
    {
        st.pushvalue(table);
        st.pcall(1,1);
        reactivate=st.as<bool>(false,-1);

    }
    Debuger::GetActiveDebuger()->Continue();
    if(reactivate)
    {
        //mybp->Activate();
    }
    else
        mybp->Deactivate();
    st.settop(0);
}
lua_Breakpoint::~lua_Breakpoint()
{
    delete mybp;
}
IMP_LUNE(lua_Breakpoint,Breakpoint);
LUNE_METHODS_START(lua_Breakpoint)
    {"isOn",&lua_Breakpoint::IsOn},
    {"setOn",&lua_Breakpoint::SetOn},
LUNE_METHODS_END();


void lua::RegDebuger(lua::state &L)
{

    L.newtable();

    lua_pushcfunction(L,&suspend);
    L.setfield("suspend");
    lua_pushcfunction(L,&resume);
    L.setfield("resume");
    lua_pushcfunction(L,&tick_dbg);
    L.setfield("tick");

    L.setglobal("debuger");
    Lune<lua_Breakpoint>::Register(L);
}

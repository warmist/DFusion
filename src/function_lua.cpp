#include "function_lua.h"

lua_Functional::lua_Functional(lua_State *L,int id):tblid(id)
{
    lua::state st(L);
    fc.SetFunction(st.as<DWORD>(1));
    DWvec v1;
    st.getfield("stack",2);
    st.push();
    while(st.next())
    {
        DWORD d=st.as<DWORD>();
        v1.push_back(d);
        st.pop();
    }
    st.pop();
    RegVec v2;
    st.getfield("registers",2);
    st.push();
    while(st.next())
    {
        RegData dat;
        st.push(1);
        st.gettable();
        dat.num=st.as<DWORD>();
        st.pop();
        st.push(2);
        st.gettable();
        dat.data=st.as<DWORD>();
        st.pop();

        v2.push_back(dat);

    }

    fc.SetArgs(v1);
    fc.SetArgsRegister(v2);

}
int lua_Functional::Run(lua_State *L)
{
    fc.Run(false);
    return 0;
}
int lua_Functional::isRunning(lua_State *L)
{
    lua::state st(L);
    st.push(fc.isrunning);
    return 1;
}
int lua_Functional::GetResult(lua_State *L)
{
    lua::state st(L);
    st.push(fc.GetResult());
    return 1;
}
IMP_LUNE(lua_Functional,functional);
LUNE_METHODS_START(lua_Functional)
    {"run",&lua_Functional::Run},
    {"isRunning",&lua_Functional::isRunning},
    {"getresult",&lua_Functional::GetResult},
LUNE_METHODS_END();

void lua::RegFunctional(lua::state &st)
{
    /*st.newtable();

    lua_pushcfunction(L,&LoadMod);
    L.setfield("loadmod");

    st.setglobal("functional");*/
    Lune<lua_Functional>::Register(st);

}

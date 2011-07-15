#include "filesystem_lua.h"
static int listdir(lua_State *L)
{
    lua::state st(L);
    std::string name;
    name=st.as<std::string>(1);
    boost::filesystem::directory_iterator itr(name);
    int i=0;
    st.newtable();
    do
    {
        i++;
        std::string file=itr->path().filename();
        st.push(i);
        st.push(file);
        st.settable();
        itr++;
    }
    while(itr!=boost::filesystem::directory_iterator());


    return 1;
}
void lua::RegFilesystem(lua::state &L)
{
    L.getglobal("engine");
    lua_pushcfunction(L,&listdir);
    L.setfield("dir");
    L.pop();
}

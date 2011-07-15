#include "common_lua.h"
using namespace lua;
static int GetCalls(lua_State *L)
{
    std::map <uint32_t,int> posmap;
    lua::state st(L);
    uint32_t pos;
    pos=0;
    set_hexsearch2_limits(0,-1);
    posmap.clear();
    do
    {
        uint32_t vadd;
        pos=hexsearch2(CALL,EOL);
        vadd=peekd(pos+1)+pos+5;
        //cout<<"vadd:"<<hex<<vadd<<endl;
        posmap[vadd]++;
        //cout<<"Found:"<<hex<<pos<<" value:"<<peekd(pos+3)<<endl;
        set_hexsearch2_limits(pos+5,-1);
    }
    while(pos!=0);
    st.newtable();
    for(auto it=posmap.begin();it!=posmap.end();it++)
    {
        st.push(it->first);
        st.push(it->second);
        st.settable();
    }
    return 1;
}
static int GetVecs(lua_State *L)
{
    std::map <uint32_t,int> posmap;
    posmap=DF_allvecs::Get().VectorDensityMap();
    lua::state st(L);
    st.newtable();
    for(auto it=posmap.begin();it!=posmap.end();it++)
    {
        st.push(it->first);
        st.push(it->second);
        st.settable();
    }
    return 1;
}
static int GetOffset(lua_State *L)
{
    lua::state st(L);
    std::string name=st.as<std::string>(1);
    size_t t=DF_Offsets::Get().GetOff(name);
    if(t!=0)
    {
        st.push(t);
        return 1;
    }
    else
    {
        if(DF_Offsets::Get().FindOff(name))
        {
            DF_Offsets::Get().Save();
            t=DF_Offsets::Get().GetOff(name);
            st.push(t);
            return 1;
        }
        else
        {
            st.push();
            return 1;
        }
    }
}
static int GetOffset2(lua_State *L)
{
    lua::state st(L);
    std::string name=st.as<std::string>(1);
    size_t t=DF_Offsets::Get().GetOff(name);
    if(t!=0)
    {

        st.push(t+df_memory_base);
        return 1;
    }
    else
    {
        if(DF_Offsets::Get().FindOff(name))
        {
            DF_Offsets::Get().Save();
            t=DF_Offsets::Get().GetOff(name);
            st.push(t+df_memory_base);
            return 1;
        }
        else
        {
            st.push();
            return 1;
        }

    }

}
static int NewOffset(lua_State *L)
{
    lua::state st(L);
    std::string name=st.as<std::string>(1);
    if(st.is<lua::function>(2))
    {
        new lua::Lua_Offset(st,name,2);
        return 0;
    }
    else
    {
        uint32_t off=st.as<uint32_t>(2);
        new DF_Offset_static(name,off);
        return 0;
    }
}
static int NewLazyOffset(lua_State *L)
{
    lua::state st(L);
    std::string name=st.as<std::string>(1);
    if(st.is<lua::function>(2))
    {
        new lua::Lua_Offset(st,name,2,true);
        return 0;
    }
    else
    {
        uint32_t off=st.as<uint32_t>(2);
        new DF_Offset_static(name,off);
        return 0;
    }
}
static int Find(lua_State *L)
{
    lua::state st(L);

    size_t start=st.as<size_t>(1);
    set_hexsearch2_limits(start,-1);
    tokenlist tkl;
    for(int i=2;i<=st.gettop();i++)
    {
        //std::cout<<"Calling with:"<<std::hex<<st.as<DWORD>(i)<<std::endl;
        tkl.push_back(st.as<DWORD>(i));
    }
    DWORD p=hexsearch3(tkl);
    st.push(p);
    return 1;
}
static int Save(lua_State *L)
{
    lua::state st(L);

    DF_Offsets::Get().Save();
    return 0;
}
static int GetBase(lua_State *L)
{
    lua::state st(L);
    st.push(df_memory_base);
    return 1;
}
void lua::RegCommon(lua::state &L)
{
    L.newtable();
    //L.push(&GetOffset);
    lua_pushcfunction(L,&GetOffset);
    L.setfield("get");
    lua_pushcfunction(L,&GetOffset2);
    L.setfield("getEx");
    lua_pushcfunction(L,&Find);
    L.setfield("find");
    lua_pushcfunction(L,&NewOffset);
    L.setfield("new");
    lua_pushcfunction(L,&NewLazyOffset);
    L.setfield("newlazy");
    lua_pushcfunction(L,&GetBase);
    L.setfield("base");
    lua_pushcfunction(L,&Save);
    L.setfield("save");
    lua_pushcfunction(L,&GetVecs);
    L.setfield("getvectors");
    lua_pushcfunction(L,&GetCalls);
    L.setfield("getcalls");
    L.setglobal("offsets");

    //find functionality...
    /*
        #define EOL			0x100
        #define ANYBYTE		0x101
        //#define FF_OR_00	0x102				// deprecated
        #define HERE		0x103
        #define EITHER		0x104
        #define SKIP_UP_TO	0x105
        #define RANGE_LO	0x106				// deprecated
        #define RANGE_HI	0x107				// deprecated
        #define DWORD_		0x108
        #define	ANYDWORD	0x109
        #define ADDRESS		0x10A
        #define	BYTERANGE	0x10B
        #define DWORDRANGE	0x10C
        #define JZ			0x174
        #define JNZ			0x175
        #define JCC			0x170
        #define CALL		0x1E8
        #define JUMP		0x1E9
    */
    L.push(EOL);
    L.setglobal("EOL");
    L.push(ANYBYTE);
    L.setglobal("ANYBYTE");
    L.push(HERE);
    L.setglobal("HERE");
    L.push(EITHER);
    L.setglobal("EITHER");
    L.push(SKIP_UP_TO);
    L.setglobal("SKIP_UP_TO");
    L.push(DWORD_);
    L.setglobal("DWORD_");
    L.push(ANYDWORD);
    L.setglobal("ANYDWORD");
    L.push(ADDRESS);
    L.setglobal("ADDRESS");
    L.push(BYTERANGE);
    L.setglobal("BYTERANGE");
    L.push(DWORDRANGE);
    L.setglobal("DWORDRANGE");
    L.push(JZ);
    L.setglobal("JZ");
    L.push(JNZ);
    L.setglobal("JNZ");
    L.push(JCC);
    L.setglobal("JCC");
    L.push(CALL);
    L.setglobal("CALL");
    L.push(JUMP);
    L.setglobal("JUMP");
    luaopen_bit(L);

}
bool lua::Lua_Offset::FindOff()
{

    f.Get();
    L.pcall(0,1);
    offset=L.as<uint32_t>();
    return (offset!=0);
}

#include "engine_lua.h"
static int LoadMod(lua_State *L)
{
    lua::state st(L);
    std::string modfile=st.as<std::string>(1);
    std::string modname=st.as<std::string>(2);
    OutFile::File f(modfile);
    uint32_t size=f.GetTextSize();
    uint32_t pos=PlugManager::GetInst().AddNewPlug(modname,size);
    char *buf;
    buf=new char[size];
    f.GetText(buf);
    std::cout<<"poking @:"<<std::hex<<pos<<"size :"<<size<<std::endl;
    pokearb(pos,buf,size);
    delete [] buf;
    st.push(pos);
    st.push(size);
    return 2;
}
static int LoadObj(lua_State *L)
{
    lua::state st(L);
    std::string modfile=st.as<std::string>(1);
    OutFile::File f(modfile);
    size_t s=f.GetTextSize();
    void *p=st.newuserdata(s);
    f.GetText((char*)p);
    st.push(s);
    return 2;
}
static int FindMarker(lua_State *L) // marker, void ptr, size, start
{
    lua::state st(L);
    union
    {
        unsigned char bytes[4];
        size_t mark;
    }M;
    M.mark=st.as<size_t>(1);
    unsigned char *p=(unsigned char *)lua_touserdata(L, 2);//st.as<lua::userdata>(2);
    size_t size=st.as<size_t>(3);
    size_t start=st.as<size_t>(4);
    for(size_t i=start;i<size;i++)
    {
        bool ok;
        ok=true;
        if(p[i]==M.bytes[0])
        {
            for(size_t j=0;j<4;j++)
            {
                if(p[i+j]!=M.bytes[j])
                {
                    ok=false;
                    break;
                }
            }
            if(ok)
            {
                st.push(i);
                return 1;
            }
        }
    }

    return 0;
}
static int LoadObjSymbols(lua_State *L)
{
    lua::state st(L);
    std::string modfile=st.as<std::string>(1);
    OutFile::File f(modfile);
    OutFile::vSymbol vec=f.GetSymbols();
    OutFile::Symbol S;

    st.newtable();
    for(size_t i=0;i<vec.size();i++)
    {
        st.push(i);
        S=vec[i];
        st.newtable();
        st.push(S.name);
        st.setfield("name");
        st.push(S.pos);
        st.setfield("pos");
        st.settable();
    }

    return 1;
}
static int NewMod(lua_State *L)
{
    lua::state st(L);
    std::string modname=st.as<std::string>(1);
    size_t size=st.as<size_t>(2);
    size_t loc=st.as<size_t>(3,0);
    uint32_t pos=PlugManager::GetInst().AddNewPlug(modname,size,loc);
    st.push(pos);
    return 1;
}
static int GetMod(lua_State *L)
{
    lua::state st(L);
    std::string modname=st.as<std::string>(1);
    uint32_t pos=PlugManager::GetInst().FindPlugin(modname);
    if(pos==0)
        st.push();
    else
        st.push(pos);
    return 1;
}
static int lua_peekb(lua_State *L)
{
    lua::state st(L);
    st.push(peekb(st.as<DWORD>(1)));
    return 1;
}
static int lua_peekd(lua_State *L)
{
    lua::state st(L);
    st.push(peekd(st.as<DWORD>(1)));
    return 1;
}
static int lua_peekw(lua_State *L)
{
    lua::state st(L);
    st.push(peekw(st.as<DWORD>(1)));
    return 1;
}
static int lua_peekcompare(lua_State *L)
{
    lua::state st(L);
    size_t size=st.as<size_t>(2);
    unsigned char* d=(unsigned char*)lua_touserdata(L,3);
    unsigned char* buf=new unsigned char[size];
    peekarb(st.as<DWORD>(1),buf,size);
    std::cout<<"Differences:\n";
    std::cout<<std::hex;
    for(unsigned i=0;i<size;i++)
    {
        if(buf[i]!=d[i])
        {
        std::cout<<"off:"<<i<<"was:"<<(int)d[i]<<"  is:"<<(int)buf[i]<<"\n";
        }

    }

    return 0;
}
static int lua_peekarb(lua_State *L)
{
    lua::state st(L);
    size_t size=st.as<size_t>(2);
    void *p=st.newuserdata(size);
    peekarb(st.as<DWORD>(1),p,size);
    return 1;
}
static int lua_peekstr(lua_State *L)
{
    lua::state st(L);
    char *buf;
    buf=new char[256];
    peekstr(st.as<DWORD>(1),buf,256);
    std::string tstr(buf);
    st.push(tstr);
    delete [] buf;
    return 1;
}
/*static int lua_peekarb(lua_State *L)
{
    lua::state st(L);
    st.push(peekarb(st.as<DWORD>(1)));
    return 1;
}*/
static int lua_pokeb(lua_State *L)
{
    lua::state st(L);
    pokeb(st.as<DWORD>(1),st.as<DWORD>(2));
    return 0;
}
static int lua_poked(lua_State *L)
{
    lua::state st(L);
    poked(st.as<DWORD>(1),st.as<DWORD>(2));
    return 0;
}
static int lua_pokew(lua_State *L)
{
    lua::state st(L);
    pokew(st.as<DWORD>(1),st.as<DWORD>(2));
    return 0;
}
static int lua_pokearb(lua_State *L)
{
    lua::state st(L);


    void *p=(void *)lua_touserdata(L, 2);//st.as<lua::userdata>(2);
    size_t size=st.as<size_t>(3);

    pokearb(st.as<DWORD>(1),p,size);
    return 0;
}
static int lua_pokestr(lua_State *L)
{
    lua::state st(L);
    std::string trg=st.as<std::string>(2);
    pokestr(st.as<DWORD>(1),(BYTE*)trg.c_str());
    return 0;
}
void lua::RegEngine(lua::state &L)
{
    L.newtable();
    lua_pushcfunction(L,&LoadMod);
    L.setfield("loadmod");
    lua_pushcfunction(L,&GetMod);
    L.setfield("getmod");
    lua_pushcfunction(L,&LoadObj);
    L.setfield("loadobj");

    lua_pushcfunction(L,&LoadObjSymbols);
    L.setfield("loadobjsymbols");
    lua_pushcfunction(L,&FindMarker);
    L.setfield("findmarker");
    lua_pushcfunction(L,&NewMod);
    L.setfield("newmod");
    //peeks'n'pokes
    /*
    BOOL peekarb(DWORD ea, OUT void *data, DWORD len);
    BYTE peekb(DWORD ea);
    WORD peekw(DWORD ea);
    DWORD peekd(DWORD ea);
    char *peekstr(DWORD ea, OUT char *data, DWORD maxlen);
    char *peekustr(DWORD ea, OUT char *data, DWORD maxlen);
    BOOL pokearb(DWORD ea, const void *data, DWORD len);
    BOOL pokeb(DWORD ea, BYTE data);
    BOOL pokew(DWORD ea, WORD data);
    BOOL poked(DWORD ea, DWORD data);
    BOOL pokestr(DWORD ea, const BYTE *data);
    */
    lua_pushcfunction(L,&lua_peekb);
    L.setfield("peekb");
    lua_pushcfunction(L,&lua_peekd);
    L.setfield("peekd");
    lua_pushcfunction(L,&lua_peekw);
    L.setfield("peekw");
    lua_pushcfunction(L,&lua_peekarb);
    L.setfield("peekarb");
    lua_pushcfunction(L,&lua_peekcompare);
    L.setfield("peekcompare");
    lua_pushcfunction(L,&lua_peekstr);
    L.setfield("peekstr");

    lua_pushcfunction(L,&lua_pokeb);
    L.setfield("pokeb");
    lua_pushcfunction(L,&lua_poked);
    L.setfield("poked");
    lua_pushcfunction(L,&lua_pokew);
    L.setfield("pokew");
    lua_pushcfunction(L,&lua_pokearb);
    L.setfield("pokearb");
    lua_pushcfunction(L,&lua_pokestr);
    L.setfield("pokestr");

    L.setglobal("engine");
    RegFilesystem(L);

}

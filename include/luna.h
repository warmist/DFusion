#ifndef LUNA_H
#define LUNA_H


extern "C" {
#include "lua.h"
#include "lauxlib.h"
}

#include "luaxx/luaxx.hpp"
#include <string>

using std::string;
template <typename T> class Luna {
  typedef struct { T *pT; } userdataType;
public:
  typedef int (T::*mfp)(lua_State *L);
  typedef struct { const char *name; mfp mfunc; } RegType;

  static void Register(lua_State *L) {
    lua_newtable(L);
    int methods = lua_gettop(L);

    luaL_newmetatable(L, T::className);
    int metatable = lua_gettop(L);

    // store method table in globals so that
    // scripts can add functions written in Lua.
    lua_pushstring(L, T::className);
    lua_pushvalue(L, methods);
    lua_settable(L, LUA_GLOBALSINDEX);

    lua_pushliteral(L, "__metatable");
    lua_pushvalue(L, methods);
    lua_settable(L, metatable);  // hide metatable from Lua getmetatable()

    lua_pushliteral(L, "__index");
    lua_pushcfunction(L, index_T);
    lua_settable(L, metatable);
    //lua_pushvalue(L, methods);
    //lua_settable(L, metatable);

    lua_pushliteral(L, "__newindex");
    //lua_pushvalue(L, methods);
    lua_pushcfunction(L, newindex_T);
    lua_settable(L, metatable);

    lua_pushliteral(L, "__tostring");
    lua_pushcfunction(L, tostring_T);
    lua_settable(L, metatable);

    lua_pushliteral(L, "__gc");
    lua_pushcfunction(L, gc_T);
    lua_settable(L, metatable);

    lua_newtable(L);                // mt for method table
    int mt = lua_gettop(L);
    lua_pushliteral(L, "__call");
    lua_pushcfunction(L, new_T);
    lua_pushliteral(L, "new");
    lua_pushvalue(L, -2);           // dup new_T function
    lua_settable(L, methods);       // add new_T to method table
    lua_settable(L, mt);            // mt.__call = new_T
    lua_setmetatable(L, methods);

    // fill method table with methods from class T
    for (RegType *l = T::methods; l->name; l++) {
    /* edited by Snaily: shouldn't it be const RegType *l ... ? */
        lua_pushstring(L, l->name);
        lua_pushlightuserdata(L, (void*)l);
        lua_pushcclosure(L, thunk, 1);
        lua_settable(L, methods);
            }

    lua_pop(L, 2);  // drop metatable and method table

  }

  // get userdata from Lua stack and return pointer to T object
  static T *check(lua_State *L, int narg) {
    userdataType *ud =
      static_cast<userdataType*>(luaL_checkudata(L, narg, T::className));
    if(!ud) luaL_typerror(L, narg, T::className);
    return ud->pT;  // pointer to T object
  }
   static void PushToLua(lua_State *L,T *p)
  {
        lua_newtable(L);
        int mytbl=lua_gettop(L);
        lua_pushliteral(L,"__ud");
        userdataType *ud =
          static_cast<userdataType*>(lua_newuserdata(L, sizeof(userdataType)));
        //lua::StackDump(L);
        luaL_getmetatable(L, T::className);  // lookup metatable in Lua registry
        lua_setmetatable(L,-2);
        //lua::StackDump(L);
        lua_settable(L,mytbl);
        //T *p = new(ud) T(L);  // call constructor for T objects
        //lua::StackDump(L);
        ud->pT = p;  // store pointer to object in userdata
        luaL_getmetatable(L, T::className);  // lookup metatable in Lua registry

        lua_setmetatable(L, mytbl);
        //lua::StackDump(L);
  }
private:
  Luna();  // hide default constructor
  /*static void getTable(lua_State *L,T *obj)
  {
      lua::state st(L);

  }*/
  // member function dispatcher
  static int thunk(lua_State *L) {
      //LOG<<"Size of stack:"<<lua_gettop(L)<<"\n";
    if(lua_gettop(L)<1)
        luaL_error(L,"Member function called without 'self'");
    lua_pushliteral(L,"__ud");
    lua_rawget(L,-2);//get real pointer

    lua_remove(L,-2);// remove real table... Maybe usefull e.g. to add something to it.
    //LOG<<"Size of stack after:"<<lua_gettop(L)<<"\n";
    // stack has userdata, followed by method args
    //TODO make this check work...
   T *obj = check(L, 1);  // get 'self', or if you prefer, 'this' //lets say it's okay....
   //T *obj=static_cast<userdataType*>(lua_touserdata(L,1))->pT;
    lua_remove(L, 1);  // remove self so member function args start at index 1
    // get member function from upvalue
    RegType *l = static_cast<RegType*>(lua_touserdata(L, lua_upvalueindex(1)));
    return (obj->*(l->mfunc))(L);  // call member function
  }
  static int index_T(lua_State *L){ // calls with (table, key), return value
        lua::state st(L);
        string key=st.as<string>(-1);
        if(key!="__ud")
        {
            //TODO find if class does not have that index
            lua_rawget(L,-2); //try getting from normal table
            if(st.is<lua::nil>()) //failed
            {
                st.pop(2);
                st.getglobal(T::className); //try class tables then
                st.push(key);
                st.gettable();
            }
        }
        else
        {
            st.pop(2);
            st.push();
            return 1;
        }

        return 1;
  }
  static int newindex_T(lua_State *L){
         lua::state st(L);

        if(st.as<string>(-2)!="__ud")
        {
            //TODO find if class does not have that index
            lua_rawset(L,-3); //set in normal table
        }
        else
            st.pop(2); //do nothing
        return 0;
  }
  // create a new T object and
  // push onto the Lua stack a userdata containing a pointer to T object

  static int new_T(lua_State *L) {
    lua_remove(L, 1);   // use classname:new(), instead of classname.new()

    T *obj = new T(L);  // call constructor for T objects
    PushToLua(L,obj);
    return 1;  // userdata containing pointer to T object
  }

  // garbage collection metamethod
  static int gc_T(lua_State *L) {
    //lua_getfield(L,,"__ud");
    lua_pushliteral(L,"__ud");
    lua_rawget(L,-2);
    userdataType *ud = static_cast<userdataType*>(lua_touserdata(L, 1));
    T *obj = ud->pT;
    delete obj;  // call destructor for T objects
    return 0;
  }

  static int tostring_T (lua_State *L) {
    char buff[32];
//    lua_getfield("__ud");
    lua_pushliteral(L,"__ud");
    lua_rawget(L,-2);
    userdataType *ud = static_cast<userdataType*>(lua_touserdata(L, 1));
    T *obj = ud->pT;
    sprintf(buff, "%p", obj);
    lua_pushfstring(L, "%s (%s)", T::className, buff);
    return 1;
  }

};
#define Lmethod(class, name) {#name, &class::name}
namespace lua
{

    /*class GlobalTable
    {
        state &myst;
        std::string name;
    public:
        GlobalTable(state &myst,std::string name):myst(myst),name(name)
        {
            myst.newtable();
        }
        ~GlobalTable()
        {
            myst.setglobal(name);
        }
    };*/
    /*
    usage:
    {
    lua::GlobalTable(st,"world");
    setfield...
    setfield...
    }//auto setglobal...
    */
};
#endif // LUNA_H

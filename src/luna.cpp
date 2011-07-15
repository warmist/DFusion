#include "luna.h"
/*
class Account {
  lua_Number m_balance;
public:
  static const char className[];
  static Luna<Account>::RegType methods[];

  Account(lua_State *L)      { m_balance = luaL_checknumber(L, 1); }
  int deposit (lua_State *L) { m_balance += luaL_checknumber(L, 1); return 0; }
  int withdraw(lua_State *L) { m_balance -= luaL_checknumber(L, 1); return 0; }
  int balance (lua_State *L) { lua_pushnumber(L, m_balance); return 1; }
  ~Account() { printf("deleted Account (%p)\n", this); }
};

const char Account::className[] = "Account";

#define method(class, name) {#name, &class::name}

Luna<Account>::RegType Account::methods[] = {
  method(Account, deposit),
  method(Account, withdraw),
  method(Account, balance),
  {0,0}
};


int main(int argc, char *argv[])
{
  lua_State *L = lua_open();

  luaopen_base(L);
  luaopen_table(L);
  luaopen_io(L);
  luaopen_string(L);
  luaopen_math(L);
  luaopen_debug(L);

  Luna<Account>::Register(L);

  if(argc>1) lua_dofile(L, argv[1]);

  lua_setgcthreshold(L, 0);  // collected garbage
  lua_close(L);
  return 0;
}

*/

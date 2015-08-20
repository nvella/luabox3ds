#include <stdio.h>
#include <3ds.h>

#include "lua/lua.h"
#include "lua/lualib.h"
#include "lua/lauxlib.h"

#include "luabox3ds.h"
#include "api.h"

int Api_dbgprint(lua_State *L) {
  // Lock the mutex
  svcWaitSynchronization(LuaBox_ConsoleMutex, U64_MAX);

  char *str = luaL_checkstring(LuaBox_State, 1);
  printf("%s", str);

  // Release
  svcReleaseMutex(LuaBox_ConsoleMutex);

  return 0;
}

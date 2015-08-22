#include <stdio.h>
#include <3ds.h>

#include "lua/lua.h"
#include "lua/lualib.h"
#include "lua/lauxlib.h"

#include "luabox3ds.h"
#include "api.h"

int Api_fs_exists(lua_State *l) {
  // Get path from lua
  char *path = lua_tostring(l, 1);

  // Push nil if invalid value provided
  if(path == NULL) { lua_pushnil(l); return 1; }

  // Attempt to open file
  FILE* fp = fopen(path, "r");

  // Check if file doesn't exist;
  if(fp == NULL) { lua_pushboolean(l, 0); return 1; }

  // File exists, cleanup
  lua_pushboolean(l, 1);
  fclose(fp);
  return 1;
}

#pragma once

#include "lua/lualib.h"
#include <3ds/console.h>

#define LUABOX_NAME "LuaBox3DS"
#define LUABOX_VERSION "0.0.1"

lua_State *LuaBox_State; // Global Lua state
int LuaBox_Running; // global loop control
PrintConsole LuaBox_MainConsole;

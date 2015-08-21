#pragma once

#include "lua/lualib.h"
#include <3ds/console.h>

#define LUABOX_NAME "LuaBox3DS"
#define LUABOX_VERSION "0.0.1"

#define LUABOX_THREAD_STACKSIZE (4 * 1024)

lua_State *L; // Global Lua state

int LuaBox_Running; // global loop control
PrintConsole LuaBox_MainConsole;
Handle LuaBox_ConsoleMutex;

//lua_State *LuaBox_L; // Main luathread, runs user code

// events are queued for lua interpreter
struct LuaBox_Event {
  int type;
  int value;
  struct LuaBox_Event * next;
};

#define LUABOX_EVENT_CHAR 1
#define LUABOX_EVENT_KEY  2

struct LuaBox_Event * LuaBox_EventList; // Event linked list
Handle LuaBox_EventMutex, LuaBox_EventSignal;
void LuaBox_PushEvent(int type, int value);

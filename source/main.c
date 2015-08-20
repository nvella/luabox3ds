#include <stdio.h>
#include <string.h>
#include <string.h>
#include <malloc.h>
#include <inttypes.h>

#include <3ds.h>
#include "lua/lua.h"
#include "lua/lauxlib.h"
#include "lua/lualib.h"

#include "luabox3ds.h"
#include "softkb.h"
#include "api.h"

void LuaThread(void *arg) {
	// Run Lua interpreter
	luaL_dofile(LuaBox_State, "boot.lua");
	while (1) {	svcSleepThread(10000000ULL);}
}

int main() {
	gfxInitDefault();
  consoleInit(GFX_TOP, &LuaBox_MainConsole); // Initialize console
	LuaBox_Running = 1;

	printf("%s version %s\ninitializing lua state...\n", LUABOX_NAME, LUABOX_VERSION);
	LuaBox_State = luaL_newstate();

	// Add functions to lua state
	lua_newtable(LuaBox_State);
	lua_pushstring(LuaBox_State, "print");
	lua_pushcfunction(LuaBox_State, Api_dbgprint);
	lua_settable(LuaBox_State, -3);
	lua_setglobal(LuaBox_State, "console");

	// Check if boot.lua exists
	FILE* fp = fopen("boot.lua", "r");
	if(fp == NULL) {
		printf("boot.lua does not exist\nplease create and place lua code in it for this to be useful.\npress start to exit.\n");
		while(1) {
			gspWaitForVBlank();
			hidScanInput();

			u32 kDown = hidKeysDown();

			if (kDown & KEY_START)
				break; // break in order to return to hbmenu

			// Flush and swap framebuffers
			gfxFlushBuffers();
			gfxSwapBuffers();
		}
		gfxExit();
		return 0;
	}
	fclose(fp);

	printf("initialized.\ninitializing keyboard...\n");
	SoftKb_Setup(GFX_BOTTOM, 4, 0);
	SoftKb_Draw();
	printf("initialized. use dpad and A button to type.\n");

	// Start lua thread.
	printf("starting lua thread...\n");
	svcCreateMutex(&LuaBox_ConsoleMutex, 0); // Create the console mutex
	Handle threadHandle;
	u32 *threadStack = memalign(32, LUABOX_THREAD_STACKSIZE);
	svcCreateThread(&threadHandle, LuaThread, 0, &threadStack[LUABOX_THREAD_STACKSIZE/4], 0x3f, 0);

	// Main loop
	while(LuaBox_Running) {
		gspWaitForVBlank();
		hidScanInput();

		u32 kDown = hidKeysDown();
		char out = SoftKb_Handle(kDown);
		if(out >= 32 || out == 10 || out == 7) printf("%c", out);

		if (kDown & KEY_START)
			break; // break in order to return to hbmenu

		// Flush and swap framebuffers
		gfxFlushBuffers();
		gfxSwapBuffers();
	}

	gfxExit();
	return 0;
}

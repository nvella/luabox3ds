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
	// Load the boot.lua file onto the stack as a chunk
	luaL_loadfile(L, "lua/bios.lua");

	// Continuously resume the chunk until it finishes or errors
	int args = 0;
	while(LuaBox_Running) {
		int state = lua_resume(L, NULL, args);
		args = 0;
		if(state == LUA_OK) break; // Execution complete
		if(state != LUA_YIELD) {
			// Error of some kind
			printf("# Uncaught lua error (code %i)\nTraceback:",  state);
			luaL_dostring(L, "print(debug.traceback())");
			break;
		}

		// Handle yield below
		svcWaitSynchronization(LuaBox_EventMutex, U64_MAX); // Attempt to lock event list

		if(LuaBox_EventList == NULL) {
			// Release the event lock and wait for a signal
			svcReleaseMutex(LuaBox_EventMutex);
			svcWaitSynchronization(LuaBox_EventSignal, U64_MAX); // Wait for another thread to signal us an event
			svcClearEvent(LuaBox_EventSignal); // Clear the event
			svcWaitSynchronization(LuaBox_EventMutex, U64_MAX); // Attempt to lock event list
		}

		struct LuaBox_Event * ev = LuaBox_EventList; // Grab end of list

		if(ev != NULL) {
			switch(ev->type) {
				case LUABOX_EVENT_CHAR:
					lua_pushstring(L, "char"); // Push string 'char' onto stack
					char *charPressed = malloc(2);
					charPressed[0] = (char)ev->value;
					charPressed[1] = 0x0;
					lua_pushstring(L, charPressed);
					free(charPressed);
					args = 2;
					break;
				default:
					break;
			}

			struct LuaBox_Event * next = ev->next;
			free(ev);
			LuaBox_EventList = next;
		}

		// Release the event lock
		svcReleaseMutex(LuaBox_EventMutex);
	}

	// Execution complete past this point
	printf("# bios.lua execution complete\n");
	while (1) {	svcSleepThread(10000000ULL);}
}

void LuaBox_PushEvent(int type, int value) {
	svcWaitSynchronization(LuaBox_EventMutex, U64_MAX); // Attempt to lock event list
	struct LuaBox_Event * ev = malloc(sizeof(struct LuaBox_Event)); // Allocate event
	ev->type = type;
	ev->value = value;
	ev->next = LuaBox_EventList;
	LuaBox_EventList = ev;
	svcReleaseMutex(LuaBox_EventMutex); // release event mutex

	svcSignalEvent(LuaBox_EventSignal); // Signal Lua thread if it's listening;
}

int main() {
	gfxInitDefault();
  consoleInit(GFX_TOP, &LuaBox_MainConsole); // Initialize console

	LuaBox_Running = 1;
	LuaBox_EventList = NULL; // Clear event list
	svcCreateMutex(&LuaBox_ConsoleMutex, 0); // Create the console mutex
	svcCreateMutex(&LuaBox_EventMutex, 0); // Create event list mutex
	svcCreateEvent(&LuaBox_EventSignal, 0); // Create event list mutex

	printf("%s version %s\ninitializing lua state...\n", LUABOX_NAME, LUABOX_VERSION);
	L = luaL_newstate();
	luaL_openlibs(L);

	// Install our apis
	lua_newtable(L);
	lua_pushstring(L, "exists");
	lua_pushcfunction(L, Api_fs_exists);
	lua_settable(L, -3);
	lua_setglobal(L, "fs");

	// Check if boot.lua exists
	FILE* fp = fopen("lua/bios.lua", "r");
	if(fp == NULL) {
		printf("lua/bios.lua does not exist\nplease create and place lua code in it for this to be useful.\npress start to exit.\n");
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
	Handle threadHandle;
	u32 *threadStack = memalign(32, LUABOX_THREAD_STACKSIZE);
	svcCreateThread(&threadHandle, LuaThread, 0, &threadStack[LUABOX_THREAD_STACKSIZE/4], 0x3f, 0);

	// Main loop
	while(LuaBox_Running) {
		gspWaitForVBlank();
		hidScanInput();

		u32 kDown = hidKeysDown();
		char out = SoftKb_Handle(kDown);
		if(out >= 32 || out == 10 || out == 7) {
			LuaBox_PushEvent(LUABOX_EVENT_CHAR, out);
		}

		if (kDown & KEY_START)
			break; // break in order to return to hbmenu

		// Flush and swap framebuffers
		gfxFlushBuffers();
		gfxSwapBuffers();
	}

	gfxExit();
	return 0;
}

#include <stdio.h>
#include <string.h>

#include <3ds.h>
#include "lua/lua.h"
#include "lua/lauxlib.h"
#include "lua/lualib.h"

#include "luabox3ds.h"
#include "softkb.h"

int main() {
	gfxInitDefault();
	//gfxSet3D(true); // uncomment if using stereoscopic 3D
  consoleInit(GFX_TOP, &LuaBox_MainConsole); // Initialize console
	LuaBox_Running = 1;

	printf("%s version %s\ninitializing lua state...\n", LUABOX_NAME, LUABOX_VERSION);
	LuaBox_State = luaL_newstate();
	printf("initialized.\n");

	SoftKb_Setup(GFX_BOTTOM, 0, 0);
	SoftKb_Draw();
	// Main loop
	while(LuaBox_Running) {
	gspWaitForVBlank();
	hidScanInput();
	//
	// 	// Your code goes here
	//
	u32 kDown = hidKeysDown();
	SoftKb_Handle(kDown);
	// 	if (kDown & KEY_START)
	// 		break; // break in order to return to hbmenu
	//
	// 	printf("test\n");
	//
	// 	// Flush and swap framebuffers
	gfxFlushBuffers();
	gfxSwapBuffers();
	}

	gfxExit();
	return 0;
}

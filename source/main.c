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
  consoleInit(GFX_TOP, &LuaBox_MainConsole); // Initialize console
	LuaBox_Running = 1;

	printf("%s version %s\ninitializing lua state...\n", LUABOX_NAME, LUABOX_VERSION);
	LuaBox_State = luaL_newstate();

	printf("initialized.\ninitializing keyboard...\n");
	SoftKb_Setup(GFX_BOTTOM, 4, 0);
	SoftKb_Draw();
	printf("initialized. use dpad and A button to type.\n");


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

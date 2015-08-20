#pragma once

#include <3ds/gfx.h>

#define SOFTKB_KEY_TAB "t"
#define SOFTKB_KEY_CAPS "caps"
#define SOFTKB_KEY_LSHIFT "shift"

void SoftKb_Setup(gfxScreen_t, int, int);
void SoftKb_Draw();
int SoftKb_Handle(u32);

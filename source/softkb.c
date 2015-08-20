#include "softkb.h"

#include <stdio.h>
#include <stdlib.h>

#include "luabox3ds.h"

#include <3ds/gfx.h>
#include <3ds/console.h>
#include <3ds/services/hid.h>

PrintConsole SoftKb_ConsoleWindow;

const int SoftKb_Width = 15;
const int SoftKb_Height = 4;
int SoftKb_CurX = 1;
int SoftKb_CurY = 1;
const char SoftKb_Layout[] =
{
  '~', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=',   6,
    1, 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}','\\',   0,
    2,   0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ';','\'',   4,   0,
    3,   0,   0, 'Z', 'X', 'C', 'V', 'B', 'N', 'M', ',', '.', '/',   5,   0
};
// special codes
// 0 = no key
// 1 = tab
// 2 = caps
// 3 = left shift
// 4 = enter
// 5 = right shift

u32 SoftKb_LastKeys = 0;
// used to keep track of previous button state;

void SoftKb_Setup(gfxScreen_t display, int x, int y) {
  consoleInit(display, &SoftKb_ConsoleWindow);
  consoleSetWindow(&SoftKb_ConsoleWindow, x, y, 2 * SoftKb_Width + 2, 2 * SoftKb_Height + 2);
}

void SoftKb_Draw() {
  consoleSelect(&SoftKb_ConsoleWindow);
  printf("\e[2J\e[1;1H"); // Clear screen

  for(int y = 0; y < SoftKb_Height; y++) {
    for(int x = 0; x < SoftKb_Width; x++) {
      // Go to position
      printf("\e[%i;%iH", y * 2, x * 2);

      // Print cursor
      char c = SoftKb_Layout[y * SoftKb_Width + x];
      // Print key
      if(c > 6) {
        if(x == SoftKb_CurX && y == SoftKb_CurY) {
          printf(">%c", c);
        } else {
          printf(" %c", c);
        }
      } else { // print the special key
        if(x == SoftKb_CurX && y == SoftKb_CurY) {
          printf(">");
        } else if(c > 0) {
          printf(" ");
        }
        switch(c) {
          case 1:
            printf(">");
            break;
          case 2:
            printf("cap");
            break;
          case 3:
            printf("shift");
            break;
          case 4:
            printf("ret");
            break;
          case 5:
            printf("shft");
            break;
          case 6:
            printf("<");
            break;
        }
      }
    }
  }
}

int SoftKb_Handle(u32 keyCode) {
  u32 newKeys = keyCode; // ^ SoftKb_LastKeys;
  SoftKb_LastKeys = keyCode;

  int redraw = 0;

  if(newKeys & KEY_UP)    { SoftKb_CurY = abs((SoftKb_CurY - 1) % SoftKb_Height); redraw = 1; }
  if(newKeys & KEY_DOWN)  { SoftKb_CurY = (SoftKb_CurY + 1) % SoftKb_Height;      redraw = 1; }
  if(newKeys & KEY_LEFT)  { SoftKb_CurX = abs((SoftKb_CurX - 1) % SoftKb_Width);  redraw = 1; }
  if(newKeys & KEY_RIGHT) { SoftKb_CurX = (SoftKb_CurX + 1) % SoftKb_Width;       redraw = 1; }

  if(redraw) {
    // The cursor has moved, set it to right spot if hovering over NULL-char
    while(SoftKb_Layout[SoftKb_CurY * SoftKb_Width + SoftKb_CurX] == 0) {
      if(newKeys & KEY_RIGHT) { SoftKb_CurX = (SoftKb_CurX + 1) % SoftKb_Width;     continue; }
      if(newKeys & KEY_LEFT) { SoftKb_CurX = abs((SoftKb_CurX - 1) % SoftKb_Width); continue; }
      SoftKb_CurX = (SoftKb_CurX + 1) % SoftKb_Width; // Go to the right if no specific key was pressed
    }
  }

  if(redraw) {
    SoftKb_Draw();
    consoleSelect(&LuaBox_MainConsole);
    printf("draw\n");
  }

  return 0;
}

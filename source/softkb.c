#include "softkb.h"

#include <stdio.h>
#include <stdlib.h>

#include "luabox3ds.h"

#include <3ds/gfx.h>
#include <3ds/console.h>
#include <3ds/services/hid.h>

PrintConsole SoftKb_ConsoleWindow;

const int SoftKb_Width = 15;
const int SoftKb_Height = 5;
int SoftKb_CurX = 1;
int SoftKb_CurY = 1;
char SoftKb_Layouts[] =
{
  '`', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=',   6,   0,
    1, 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '[', ']','\\',   0,
    2,   0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ';','\'',   4,   0,
    3,   0,   0, 'Z', 'X', 'C', 'V', 'B', 'N', 'M', ',', '.', '/',   5,   0,
    0,   0,   0,   0,   0,   7,   0,   0,   0,   0,   0,   0,   0,   0,   0,
  '~', '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+',   6,   0,
    1, 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '|',   0,
    2,   0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"',   4,   0,
    3,   0,   0, 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?',   5,   0,
    0,   0,   0,   0,   0,   7,   0,   0,   0,   0,   0,   0,   0,   0,   0
};

// special codes
// 0 = no key
// 1 = tab
// 2 = caps
// 3 = left shift
// 4 = enter
// 5 = right shift

int SoftKb_Shift = 0;
int SoftKb_Caps = 0;

void SoftKb_Setup(gfxScreen_t display, int x, int y) {
  consoleInit(display, &SoftKb_ConsoleWindow);
  consoleSetWindow(&SoftKb_ConsoleWindow, x, y, 2 * SoftKb_Width + 2, 2 * SoftKb_Height + 2);
}

void SoftKb_Draw() {
  consoleSelect(&SoftKb_ConsoleWindow);
  printf("\e[2J\e[1;1H"); // Clear screen

  char *currentLayout = SoftKb_Layouts;
  if(SoftKb_Shift) currentLayout += SoftKb_Width * SoftKb_Height; // Adjust for shift if enabled

  for(int y = 0; y < SoftKb_Height; y++) {
    for(int x = 0; x < SoftKb_Width; x++) {
      // Go to position
      printf("\e[%i;%iH", y * 2, x * 2);

      // Print cursor
      char c = currentLayout[y * SoftKb_Width + x];
      // Print key
      if(c > 7) {
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
            if(SoftKb_Caps) printf("\e[1m\e[33m");
            printf("cap");
            if(SoftKb_Caps) printf("\e[0m");
            break;
          case 3:
            if(SoftKb_Shift) printf("\e[1m\e[33m");
            printf("shift");
            if(SoftKb_Shift) printf("\e[0m");
            break;
          case 4:
            printf("ret");
            break;
          case 5:
            if(SoftKb_Shift) printf("\e[1m\e[33m");
            printf("shft");
            if(SoftKb_Shift) printf("\e[0m");
            break;
          case 6:
            printf("del");
            break;
          case 7:
            printf("space");
            break;
        }
      }
    }
  }

  consoleSelect(&LuaBox_MainConsole);
}

int SoftKb_Handle(u32 keyCode) {
  char *currentLayout = SoftKb_Layouts;
  if(SoftKb_Shift) currentLayout += SoftKb_Width * SoftKb_Height; // Adjust for shift if enabled

  int redraw = 0;

  if(keyCode & KEY_UP)    { SoftKb_CurY = SoftKb_CurY - 1; if(SoftKb_CurY < 0) SoftKb_CurY = SoftKb_Height - 1; redraw = 1; }
  if(keyCode & KEY_DOWN)  { SoftKb_CurY = (SoftKb_CurY + 1) % SoftKb_Height;                                    redraw = 1; }
  if(keyCode & KEY_LEFT)  { SoftKb_CurX = SoftKb_CurX - 1; if(SoftKb_CurX < 0) SoftKb_CurX = SoftKb_Width - 1;  redraw = 1; }
  if(keyCode & KEY_RIGHT) { SoftKb_CurX = (SoftKb_CurX + 1) % SoftKb_Width;                                     redraw = 1; }

  if(redraw) {
    // The cursor has moved, set it to right spot if hovering over NULL-char
    while(currentLayout[SoftKb_CurY * SoftKb_Width + SoftKb_CurX] == 0) {
      if(keyCode & KEY_RIGHT) { SoftKb_CurX = (SoftKb_CurX + 1) % SoftKb_Width;                                   continue; }
      if(keyCode & KEY_LEFT) { SoftKb_CurX = SoftKb_CurX - 1; if(SoftKb_CurX < 0) SoftKb_CurX = SoftKb_Width - 1; continue; }
      SoftKb_CurX = (SoftKb_CurX + 1) % SoftKb_Width; // Go to the right if no specific key was pressed
    }
  }

  char c = currentLayout[SoftKb_CurY * SoftKb_Width + SoftKb_CurX];
  int key = 0;
  // If user pressed A, send the key back up
  if(keyCode & KEY_A) {
    switch(c) {
      case 1: // Tab
        key = '\t';
        if(SoftKb_Shift) { SoftKb_Shift = 0; redraw = 1; } // Reset shift status
        break;
      case 2: // Caps
        SoftKb_Caps = !SoftKb_Caps;
        redraw = 1;
        break;
      case 4:
        key = '\n';
        if(SoftKb_Shift) { SoftKb_Shift = 0; redraw = 1; } // Reset shift status
        break;
      case 3: // Shift
      case 5:
        SoftKb_Shift = !SoftKb_Shift;
        redraw = 1;
        break;
      case 6: // Delete/backspace;
        key = '\b';
        if(SoftKb_Shift) { SoftKb_Shift = 0; redraw = 1; } // Reset shift status
        break;
      case 7: // Spacebar
        key = ' ';
        if(SoftKb_Shift) { SoftKb_Shift = 0; redraw = 1; } // Reset shift status
        break;
      default:
        if(!(SoftKb_Shift | SoftKb_Caps) && c >= 65 && c <= 90) { // Is letter; Don't do capital
          key = c | 32; // OR with 32 to downcase
        } else {
          key = c;
        }

        if(SoftKb_Shift) { SoftKb_Shift = 0; redraw = 1; } // Reset shift status
        break;
    }
  }

  if(redraw) SoftKb_Draw();

  return key;
}

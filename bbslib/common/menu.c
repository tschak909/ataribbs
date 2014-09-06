/**
 * menu.c - temporary crap.
 */

#include "menu.h"

#include "terminal.h"
#include "config.h"
#include "util.h"
#include <serial.h>
#include <6502.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <atari.h>
#include <stdlib.h>

char current_msg_board[32];

void _menu_display_screen(unsigned char mode)
{
  char filename[40];
  switch (mode)
    {
    case MODE_MAIN_MENU:
      strcpy(filename,"MENU>MAIN");
      break;
    }

  terminal_send_screen(filename);

}

/**
 * main entry point for menu. Exiting implies goodbye.
 */
void menu()
{
  unsigned char bQuit=0;
  unsigned char mode=MODE_MAIN_MENU;
  while (!bQuit)
    {
      _menu_display_screen(mode);
      bQuit = _menu(mode);
    }
}

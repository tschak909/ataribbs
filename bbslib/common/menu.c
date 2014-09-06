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
#include <ctype.h>

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

void _menu_confirm(unsigned char c, const char* prompt)
{
  terminal_send_char(c);
  terminal_send_right();
  terminal_send_right();
  terminal_send_right();
  terminal_send(prompt,0);
  terminal_send_eol();
  terminal_send_eol();
}

unsigned char _menu_main(unsigned char c)
{
  switch(toupper(c))
    {
    case 'G':
      _menu_confirm('G',"Goodbye");
      return 1;
    default:
      terminal_beep();
      return 0;
    }
  return 0;
}

unsigned char _is_valid_char(unsigned char mode, unsigned char c)
{
  switch(mode)
    {
    case MODE_MAIN_MENU:
      return (c=='G'||c=='g');
      break;
    }
}

unsigned char _menu(unsigned char mode)
{
  char c;
  terminal_send(">> [_]",0);
  terminal_send_left();
  terminal_send_left();

 get:  c=terminal_get_char();

  if (!_is_valid_char(mode,c))
    {
      terminal_beep();
      goto get;
    }
  else // valid char
    { 
      switch(mode)
	{
	case MODE_MAIN_MENU:
	  return _menu_main(c);
	  break;
	}
    }
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

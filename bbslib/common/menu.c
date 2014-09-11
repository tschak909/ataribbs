/**
 * menu.c - temporary crap.
 */

#include "menu.h"

#include "terminal.h"
#include "config.h"
#include "util.h"
#include "types.h"
#include "mboard.h"
#include "msg.h"
#include "user.h"
#include <serial.h>
#include <6502.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <atari.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>

unsigned char mode=MODE_MAIN_MENU;
MMUFile mmufd=0;
MMUEntry* mmuentry;
char current_mboard;
MsgFile* current_msgfile;

void _menu_display_screen(unsigned char mode)
{
  char filename[40];
  switch (mode)
    {
    case MODE_MAIN_MENU:
      strcpy(filename,"MENU>MAIN");
      break;
    case MODE_MSG_MENU:
      strcpy(filename,"MENU>MSGMAIN");
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

void _menu_msg_open()
{
  char output[80];
  terminal_close_port();
  mmuentry = calloc(1,sizeof(MMUEntry));
  mmufd = mboard_open("D1:MAIN.MMU");
  current_mboard = mboard_get_default(mmufd,mmuentry);
  current_msgfile = msg_open(mmuentry->itemFile);
  sprintf(output,"Current message board is %s",mmuentry->itemName);
  terminal_open_port();
  terminal_send(output,0);
  terminal_send_eol();
  terminal_send_eol();
  terminal_close_port();
}

void _menu_msg_close()
{
  assert(current_msgfile!=NULL);
  assert(mmuentry!=NULL);
  assert(mmufd>0);
  msg_close(current_msgfile);
  mboard_close(mmufd);
  free(mmuentry);
}

void _menu_show_board()
{
  char output[80];
  assert(mmuentry!=NULL);
  sprintf(output,"Current board changed to %s",mmuentry->itemName);
  terminal_send(output,0);
  terminal_send_eol();
  terminal_send_eol();
}

void _menu_msg_next_board()
{
  terminal_close_port();
  if (current_mboard<mboard_get_num_boards()-1)
    {
      current_mboard++;
    }
  else
    {
      current_mboard=0;
    }
  msg_close(current_msgfile);
  mboard_get(mmufd,current_mboard,mmuentry);
  current_msgfile = msg_open(mmuentry->itemFile);
  terminal_open_port();
  _menu_show_board();
}

void _menu_msg_previous_board()
{
  terminal_close_port();
  if (current_mboard>0)
    {
      current_mboard--;
    }
  else
    {
      current_mboard=mboard_get_num_boards()-1;
    }
  msg_close(current_msgfile);
  mboard_get(mmufd,current_mboard,mmuentry);
  current_msgfile = msg_open(mmuentry->itemFile);
  terminal_open_port();
  _menu_show_board();
}

void _menu_msg_header_scan()
{
  
}

unsigned char _menu_msg(unsigned char c)
{
  switch(toupper(c))
    {
    case 'N':
      _menu_confirm('N',"Next Board");
      _menu_msg_next_board();
      return 0;
    case 'P':
      _menu_confirm('P',"Previous Board");
      _menu_msg_previous_board();
      return 0;
    case 'H':
      _menu_confirm('H',"Header Scan");
      _menu_msg_header_scan();
      return 0;
    case 'G':
      _menu_confirm('G',"Goodbye");
      return 1;
    case 'X':
      _menu_confirm('X',"Exit to Main Menu");
      mode=MODE_MAIN_MENU;
      _menu_msg_close();
      return 0;
    default:
      terminal_beep();
      return 0;
    }
}

void _menu_user_list()
{
  UserRecord* record;
  char output[120];
  record = calloc(1,sizeof(UserRecord));
  terminal_close_port();
  terminal_send_screen("USERLIST");
  terminal_close_port();
  user_scan_begin();
  while (user_scan_next(record) == sizeof(UserRecord))
    {
      terminal_open_port();
      sprintf(output,"%5u %-13s %-15s",record->user_id,record->username,record->from);
      terminal_send(output,0);
      terminal_send_eol();
      memset(record,0,sizeof(UserRecord));
      terminal_close_port();
    }
  user_scan_end();
  terminal_open_port();
  free(record);
}

unsigned char _menu_main(unsigned char c)
{
  switch(toupper(c))
    {
    case 'G':
      _menu_confirm('G',"Goodbye");
      return 1;
      break;
    case 'M':
      _menu_confirm('M',"Message Board");
      mode=MODE_MSG_MENU;
      _menu_msg_open();
      return 0;
      break;
    case 'U':
      _menu_confirm('U',"User List");
      _menu_user_list();
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
      return (toupper(c)=='G' ||
	      toupper(c)=='M' ||
	      toupper(c)=='U');
      break;
    case MODE_MSG_MENU:
      return (toupper(c)=='G' ||
	      toupper(c)=='X' ||
	      toupper(c)=='N' ||
	      toupper(c)=='P' ||
	      toupper(c)=='H');
      break;
    }
  return 0;
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
	case MODE_MSG_MENU:
	  return _menu_msg(c);
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
  mode=0;
  while (!bQuit)
    {
      _menu_display_screen(mode);
      bQuit = _menu(mode);
    }
}

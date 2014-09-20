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
#include "idx.h"
#include "header.h"
#include "user.h"
#include "input.h"
#include "ledit.h"
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
long last_unread=0;

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
  last_unread=0;
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
  char output[100];
  long nummsgs;
  char* s1;
  char* s2;
  long r1, r2;
  HeaderCursor cursor;
  MsgHeader* header;
  long i;
  assert(current_msgfile!=NULL);

  header=calloc(1,sizeof(MsgHeader));
  assert(header!=NULL);
  terminal_close_port();
  nummsgs=idx_get_num_msgs(current_msgfile);
  terminal_send("<RETURN> accepts default values.",0);
  terminal_send_eol();

  sprintf(output,"Start Message (1-%lu) ",nummsgs);
  terminal_open_port();
  terminal_send(output,0);
  s1 = prompt_line(1,5);
  r1 = atol(s1);
  r1 = (r1 < nummsgs ? r1 : nummsgs);

  sprintf(output,"End Message (1-%lu)",nummsgs);

  terminal_send(output,0);
  s2 = prompt_line(1,5);
  r2 = atol(s2);
  r2 = (r2 < nummsgs ? r2 : nummsgs);

  terminal_close_port();
  cursor=header_scan_begin(current_msgfile,0);
  terminal_enable_line_counter();
  for (i=r1;i<r2;++i)
    {
      cursor=header_scan_next(current_msgfile,cursor,header);
      terminal_open_port();
      sprintf(output,"%5lu %-18s %02u-%02u-%02u %02u:%02u",header->msgId,header->from,header->stamp.year,header->stamp.month,header->stamp.day,header->stamp.hours,header->stamp.minutes);
      terminal_send(output,0);
      terminal_send_eol();
      sprintf(output,"      %-32s",header->subject);
      terminal_send(output,0);
      terminal_send_eol();
      terminal_send_eol();
      terminal_close_port();
    }
  terminal_disable_line_counter();
  free(header);
}

unsigned char _menu_valid_chars(char c, const char* validchars)
{
  unsigned char i;
  for (i=0;i<strlen(validchars);++i)
    {
      if (validchars[i]==c)
	{
	  return i;
	}
      else
	{
	  // fall through.
	}
    }
  return 255;
}

void _menu_msg_board_jump()
{
  unsigned char i;
  MMUEntry* entry;
  char output[80];
  char c=255;
  char* validchars;
  entry = calloc(1,sizeof(MMUEntry));
  validchars = calloc(1,36);
  terminal_close_port();
  for (i=0;i<mboard_get_num_boards();++i)
    {
      mboard_get(mmufd,i,entry);
      sprintf(output,"[%c] %s",entry->item,entry->itemDescription);
      validchars[i]=entry->item;
      validchars[i+1]=0;
      terminal_open_port();
      terminal_send(output,0);
      terminal_send_eol();
      terminal_close_port();
    }
  terminal_open_port();
  terminal_send_eol();
  free(entry);

  terminal_send(">> [_]",0);
  terminal_send_left();
  terminal_send_left();  

  while (_menu_valid_chars(c,validchars) == 255)
    {
      c=terminal_get_char();
      if (!_menu_valid_chars(c,validchars) == 255)
	{
	  terminal_beep();
	}
      else
	{
	  i=_menu_valid_chars(c,validchars);
	}
    }

  terminal_close_port();
  msg_close(current_msgfile);
  mboard_get(mmufd,i,mmuentry);
  terminal_open_port();
  _menu_confirm(mmuentry->item,mmuentry->itemDescription);
  terminal_close_port();
  current_msgfile = msg_open(mmuentry->itemFile);
  terminal_open_port();
  free(validchars);
}

void __msg_read(long msgId)
{
  MsgIdxEntry* idx;
  MsgHeader* header;
  char* buffer;
  size_t bodySize = 0;
  size_t i = 0;
  int abr = 0;
  
  idx=calloc(1,sizeof(MsgIdxEntry));
  assert(idx!=NULL);
  header=calloc(1,sizeof(MsgHeader));
  assert(header!=NULL);
  buffer=calloc(1,512);
  assert(buffer!=NULL);

  terminal_close_port();

  if (idx_read(current_msgfile,msgId,idx) != sizeof(MsgIdxEntry))
    {
      printf("Could not read index.");
      return;
    }

  if (header_read(current_msgfile,idx->hdrOffset,header) != sizeof(MsgHeader))
    {
      printf("Could not read header.");
      return;
    }

  // Output the header

  terminal_open_port();
  
  terminal_enable_line_counter();

  sprintf(buffer,"Message ID: %-5u",header->msgId);
  terminal_send(buffer,0);
  terminal_send_eol();

  sprintf(buffer,"From: %s",header->from);
  terminal_send(buffer,0);
  terminal_send_eol();

  sprintf(buffer,"Date/Time: 20%02u-%02u-%02u %02u:%02u:%02u",
	  header->stamp.year,
	  header->stamp.month,
	  header->stamp.day,
	  header->stamp.hours,
	  header->stamp.minutes,
	  header->stamp.seconds);
  terminal_send(buffer,0);
  terminal_send_eol();
  
  sprintf(buffer,"Subject: %s",header->subject);
  terminal_send(buffer,0);
  terminal_send_eol();

  terminal_send_eol();
  terminal_close_port();

  // Output the body.
  lseek(current_msgfile->msgfd,idx->bodyOffset,SEEK_SET);
  read(current_msgfile->msgfd,&bodySize,sizeof(size_t));

  for (i=0;i<bodySize;i=i+abr)
    {
      memset(buffer,0,512);
      if (bodySize-i<511)
	abr = read(current_msgfile->msgfd,buffer,bodySize-i);
      else
	abr = read(current_msgfile->msgfd,buffer,511);
      terminal_open_port();
      buffer[abr+1]=0;
      terminal_send(buffer,0);
      terminal_close_port();
    }
  
  terminal_open_port();
  terminal_disable_line_counter();
  terminal_send_pagination_prompt();
  terminal_close_port();

  free(idx);
  free(header);
  free(buffer);
}

void _menu_msg_board_read()
{
  char c=255;
  char validchars[6];

  validchars[0]=0x9b;
  validchars[1]=0x0d;
  validchars[2]='S';
  validchars[3]='B';
  validchars[4]='Y';
  validchars[5]=0;
 
  terminal_close_port();
  terminal_open_port();
  terminal_send("Read: [ENTER]Unread, [S]earch",0);
  terminal_send_eol();
  terminal_send("      [B]egin With, [Y]ours",0);
  terminal_send_eol();
  terminal_send_eol();
  terminal_send(">> [_]",0);
  terminal_send_left();
  terminal_send_left();
  while (c==255)
    {
      c=toupper(terminal_get_char());
      if (_menu_valid_chars(c,validchars) == 255)
	{
	  terminal_beep();
	  c=255;
	}
      else
	{
	  break;
	}
    }

  switch(c)
    {
    case 0x9b:
    case 0x0d:
      _menu_confirm('_',"Last Unread");
      __msg_read(1);
      break;
    case 'S':
      _menu_confirm('S',"Search");
      break;
    case 'B':
      _menu_confirm('B',"Begin With");
      break;
    case 'Y':
      _menu_confirm('Y',"Yours");
      break;
    }

}

void _menu_enter_message_insert_into(char* line)
{
  assert(line!=NULL);
  line[0]=1;
  while (line[0]!=0)
    {
      terminal_open_port();
      line=prompt_line(1,36);
      terminal_close_port();
      ledit_insert_at_end(line);
    }
}

void _menu_enter_message_save(char* subject)
{
  MsgHeader* entry;
  char* line;
  
  entry = calloc(1,sizeof(MsgHeader));
  strcpy(entry->from,user_get()->username);
  strcpy(entry->subject,subject);
  entry->msgId=0;
  timedate(&entry->stamp);
  msg_put_begin(current_msgfile,entry);
  msg_put_chunked(ledit_get_first_line());
  while (line = ledit_get_next_line())
    {
      msg_put_chunked(line);
    }
  msg_put_end();
  
  free(entry);
}

void _menu_enter_message()
{
  char* subject;
  char* line;
  char c=255;
  char validchars[8];

  validchars[0]=0x9b;
  validchars[1]=0x0d;
  validchars[2]='C';
  validchars[3]='S';
  validchars[4]='I';
  validchars[5]='E';
  validchars[6]='R';
  validchars[7]='A';

  terminal_close_port();
  line=calloc(1,32);
  subject=calloc(1,32);
  ledit_init();
  terminal_open_port();
  terminal_send("Name: ",0);
  terminal_send(user_get()->username,0);
  terminal_send_eol();
  terminal_send("Subject:",0);
  terminal_send_eol();
  subject = prompt_line(1,32);
  
  terminal_send_eol();
  terminal_send("Enter your message, each line seperated by <RETURN>",0);
  terminal_send_eol();
  terminal_send("<RETURN> by itself presents edit menu.",0);
  terminal_send_eol();
  terminal_send_eol();

  _menu_enter_message_insert_into(line);

  terminal_close_port();

  // The Editing menu
  while (c==255)
    {
      terminal_open_port();
      terminal_send_eol();
      terminal_send("<RETURN> Continues editing.",0);
      terminal_send_eol();
      terminal_send_eol();
      terminal_send("Options: [C]ontinue [S]ave [I]nsert",0);
      terminal_send_eol();
      terminal_send("         [E]dit [R]ead [A]bort",0);
      terminal_send_eol();
      terminal_send_eol();
      terminal_send(">> [_]",0);
      terminal_send_left();
      terminal_send_left();
      
      while (c==255)
	{
	  c=toupper(terminal_get_char());
	  if (_menu_valid_chars(c,validchars) == 255)
	    {
	      terminal_beep();
	      c=255;
	    }
	  else
	    {
	      break;
	    }
	}
      
      switch(c)
	{
	case 0x9b:
	case 0x0d:
	case 'C':
	  _menu_confirm('C',"Continue");
	  _menu_enter_message_insert_into(line);
	  c=255;
	  break;
	case 'S':
	  _menu_confirm('S',"Save");
	  terminal_send_eol();
	  terminal_send_eol();
	  terminal_send("Saving...",0);
	  terminal_close_port();
	  _menu_enter_message_save(subject);
	  terminal_open_port();
	  terminal_send("Done.",0);
	  terminal_send_eol();
	  terminal_send_eol();
	  c=0;
	  break;
	case 'I':
	  _menu_confirm('I',"Insert");
	  c=255;
	  break;
	case 'E':
	  _menu_confirm('E',"Edit");
	  c=255;
	  break;
	case 'R':
	  _menu_confirm('R',"Read");
	  c=255;
	  break;
	case 'A':
	  _menu_confirm('A',"Abort");
	  c=0;
	  break;
	}
    }

  free(line);
  free(subject);
  ledit_done();

}

unsigned char _menu_msg(unsigned char c)
{
  switch(toupper(c))
    {
    case 'J':
      _menu_confirm('J',"Jump to Board");
      _menu_msg_board_jump();
      return 0;
    case 'E':
      _menu_confirm('E',"Enter new Message");
      _menu_enter_message();
      return 0;
    case 'R':
      _menu_confirm('R',"Read Messages");
      _menu_msg_board_read();
      return 0;
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
      _menu_msg_close();
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
	      toupper(c)=='E' ||
	      toupper(c)=='X' ||
	      toupper(c)=='N' ||
	      toupper(c)=='P' ||
	      toupper(c)=='H' ||
	      toupper(c)=='J' ||
	      toupper(c)=='R');
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

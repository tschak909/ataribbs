#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <bbslib/common/config.h>
#include <bbslib/common/terminal.h>
#include "init.h"
#include "done.h"
#include "waitforcall.h"
#include <bbslib/common/util.h>
#include <bbslib/common/input.h>
#include <bbslib/common/filemenu.h>
#include <bbslib/common/user.h>
#include <bbslib/common/types.h>

PrinterFlags *config_printflags = NULL;
SerialPortFlags *config_serialportflags = NULL; 
ModemStrings *config_modemstrings = NULL;

void bbs();

/**
 * The run() loop. This is run after init() finishes.
 */
unsigned char run()
{
  int res;
  int bQuit=1;

  while (bQuit==1)
    {
      splash();
      terminal_init();
      res = waitforcall();
      switch(res)
	{
	case WAITFORCALL_CONNECTED:
	  bbs();
	  break;
	case WAITFORCALL_EXIT:
	  return 0;
	  break;
	}
      terminal_close_port();
    }
  return 0;
}

void login(const char* name)
{
  UserRecord record;
  if (user_lookup(name,&record) == 0)
    {
      char* from;
      char* email;
      char* tmp;
      char* tmp2;
      unsigned short passwordHash,passwordHashVerify;
      TimeDate firstLogon;
      char userid[40];
      timedate(&firstLogon);
      terminal_send("You seem to be a new user.",0);
      terminal_send_eol();
      terminal_send("Where are you calling from?",0);
      from = prompt_line(1,36);
      terminal_send_eol();
      terminal_send("What is your email address?",0);
      email = prompt_line(1,36);
      terminal_send_eol();
      passwordHash=0;
      passwordHashVerify=1;
      while (passwordHash != passwordHashVerify)
	{
	  terminal_send("Please enter a password:",0);
	  tmp = prompt_password_line(1,36,'*');
	  passwordHash = crc16(tmp,strlen(tmp));
	  free(tmp);
	  terminal_send_eol();
	  terminal_send("Please enter password, again:",0);
	  tmp2 = prompt_password_line(1,36,'*');
	  passwordHashVerify = crc16(tmp2,strlen(tmp2));
	  free(tmp2);
	  terminal_send_eol();
	  if (passwordHash != passwordHashVerify)
	    {
	      terminal_send("Passwords dont match. Try again.",0);
	      terminal_send_eol();
	    }
	}
      strcpy(record.username,name);
      strcpy(record.from,from);
      strcpy(record.email,email);
      record.password_hash = passwordHash;
      record.security_level=10;
      record.firstLogon=firstLogon;
      record.lastLogon=firstLogon;
      user_add(&record);
      sprintf(userid,"Your User ID is %u",record.user_id);
      terminal_send(userid,0);
      terminal_send_eol();
    }
}

void bbs()
{
  char* name;
  char* password;
  char logstring[100];
  char i;

  printf("bbs()\n\n");
  log(LOG_LEVEL_NOTICE,"Connected!");
  terminal_determine_eol();
  sleep(1);
  terminal_send_clear_screen();
  terminal_send_screen("WELCOME");
  terminal_send("This BBS is still being developed.",0);
  terminal_send_eol();
  terminal_send("For now, I just log names.",0);
  terminal_send_eol();
  terminal_send("Username: ",0);
  terminal_send_eol();
  name = prompt_line(1,32);
  terminal_send_eol();
  
  login(name);

  filemenu_show("BULLETIN");

  terminal_send("Thanks for calling... ",0);
  terminal_send_eol();
  terminal_send("More will happen soon!",0);
  terminal_send_eol();
  terminal_hang_up();
  log(LOG_LEVEL_NOTICE,"Hung up.");
  sleep(2);
}

unsigned char main(void)
{
  int retCode = 0;

  retCode = init();
  if (retCode != 0)
    {
      // Init Failed
      return retCode;
    }
  retCode = run();
  if (retCode != 0)
    {
      // Run Failed
      return retCode;
    }
  retCode = done();
  if (retCode != 0)
    {
      // Done failed
      return retCode;
    }
  return retCode;
}

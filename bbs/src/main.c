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
#include <bbslib/common/menu.h>
#include <bbslib/common/ledit.h>

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

unsigned char login(char* name)
{
  UserRecord record;
  char* uname = strupper(name);
  terminal_close_port();
  if (user_lookup(uname,&record) == 0)
    {
      char* from;
      char* email;
      char* tmp;
      char* tmp2;
      unsigned short passwordHash,passwordHashVerify;
      TimeDate firstLogon;
      char userid[40];
      timedate(&firstLogon);
      terminal_open_port();
      terminal_send("You seem to be a new user.",0);
      terminal_send_eol();
      terminal_send("Where are you calling from?",0);
      terminal_send_eol();
      from = prompt_line(1,36,NULL);
      terminal_send_eol();
      terminal_send("What is your email address?",0);
      terminal_send_eol();
      email = prompt_line(1,36,NULL);
      terminal_send_eol();
      passwordHash=0;
      passwordHashVerify=1;
      while (passwordHash != passwordHashVerify)
	{
	  terminal_send("Please enter a password:",0);
	  terminal_send_eol();
	  tmp = prompt_password_line(1,36,'*');
	  passwordHash = crc16(tmp,strlen(tmp));
	  free(tmp);
	  terminal_send_eol();
	  terminal_send("Please enter password, again:",0);
	  terminal_send_eol();
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
      terminal_close_port();
      user_add(&record);
      sprintf(userid,"Your User ID is %u",record.user_id);
      terminal_open_port();
      terminal_send(userid,0);
      terminal_send_eol();
      user_set(&record);
      return 1;
    }
  else
    {
      unsigned char retry=0;
      char* password;
      unsigned short passwordHash;
      char tmp[50];
      TimeDate td;
      terminal_open_port();
      while (retry<3)
	{
	  terminal_send("Password:",0);
	  terminal_send_eol();
	  password = prompt_password_line(1,36,'*');
	  passwordHash = crc16(password,strlen(password));
	  free(password);
	  terminal_send_eol();
	  if (passwordHash == record.password_hash)
	    {
	      sprintf(tmp,"\n\nYou last logged in on 20%02u-%02u-%02u @ %02u:%02u:%02u\n\n",record.lastLogon.year,record.lastLogon.month,record.lastLogon.day,record.lastLogon.hours,record.lastLogon.minutes,record.lastLogon.seconds);
	      timedate(&td);
	      record.lastLogon = td;
	      terminal_close_port();
	      user_update(&record);
	      user_set(&record);
	      terminal_open_port();
	      terminal_send(tmp,0);
	      terminal_send_eol();
	      return 1; 
	    }
	  else
	    {
	      terminal_send_up();
	      terminal_send_up();
	      retry++;
	    }
	}
      return 0; 
    }
}

void bbs()
{
  char* name;
  unsigned char retries=0;

  name[0]=0;
  retries=0;
  printf("bbs()\n\n");
  log(LOG_LEVEL_NOTICE,"Connected!");
  terminal_determine_eol();
  sleep(1);
  terminal_send_clear_screen();
  terminal_send_screen("WELCOME");
  terminal_send_eol();

  while ((name[0] == 0) && retries < 3)
    {
      terminal_send("Username: ",0);
      terminal_send_eol();
      name = prompt_line(1,32,NULL);
      terminal_send_eol();
      if (name[0] != 0)
  	break;
      else
  	{
  	  terminal_send_up();
  	  terminal_send_up();
	  terminal_send_up();
	  retries++;
	}
    }

  if (retries==3)
    {
      terminal_send_eol();
      terminal_send_eol();
      terminal_send_eol(); 
      goto goodbye;
    }

  if (login(name) == 1)
    {
      char login[40];
      sprintf(login,"%s logged in.",name);
      log(LOG_LEVEL_NOTICE,login);
      filemenu_show("BULLETIN");
    }

  menu();

 goodbye:  terminal_send("Thanks for calling... ",0);
  terminal_send_eol();
  terminal_send("More will happen soon!",0);
  terminal_send_eol();
  user_logoff();
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

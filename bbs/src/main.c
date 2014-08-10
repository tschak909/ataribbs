#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <bbslib/common/config.h>
#include <bbslib/common/terminal.h>
#include "init.h"
#include "done.h"
#include "waitforcall.h"
#include <bbslib/common/util.h>
#include <bbslib/common/input.h>

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
    }
  return 0;
}

void bbs()
{
  char* name;
  char* password; 
  printf("bbs()\n\n");
  log(LOG_LEVEL_NOTICE,"Connected!");
  terminal_determine_eol();
  terminal_send_screen("WELCOME");
  terminal_send("What is your name? ",0);
  name = input_line_and_echo();
  terminal_send_eol();
  printf("name is %s\n",name);
  free(name);
  terminal_send("Password? ",0);
  password = input_line_and_echo_char('*');
  terminal_send_eol();
  printf("password is %s\n",password);
  free(password);
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

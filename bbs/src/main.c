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
#include <bbslib/common/filemenu.h>

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
      terminal_close_port();
    }
  return 0;
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
  terminal_send("Who is this calling? ",0);
  terminal_send_eol();
  name = prompt_line(1,32);
  terminal_send_eol();
  sprintf(logstring,"Login by %s",name);
  log(LOG_LEVEL_NOTICE,logstring);

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

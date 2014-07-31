#include <stdio.h>
#include <unistd.h>
#include "config.h"
#include "terminal.h"
#include "init.h"
#include "done.h"
#include "waitforcall.h"
#include "util.h"

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
      res = waitforcall();
      if (res == WAITFORCALL_CONNECTED)
	{
	  bbs();
	}
    }
  return 0;
}

void bbs()
{
  printf("bbs()\n\n");
  log(LOG_LEVEL_NOTICE,"Connected!");
  terminal_send("THIS BBS IS UNDER CONSTRUCTION. PLEASE CHECK BACK LATER.\n",0);
  terminal_send_and_expect_response("+++","OK",0);
  terminal_send("ATH0\r",1);
  log(LOG_LEVEL_NOTICE,"Hung up.");
  sleep(2);
  terminal_open_port();
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

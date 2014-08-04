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
      terminal_init();
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
  char tmp[128];
  printf("bbs()\n\n");
  log(LOG_LEVEL_NOTICE,"Connected!");
  sprintf(tmp,"THIS BBS IS UNDER CONSTRUCTION. PLEASE CHECK BACK LATER.\r\n");
  terminal_send(tmp,0);
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

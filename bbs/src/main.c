#include <stdio.h>
#include "config.h"
#include "init.h"
#include "done.h"

PrinterFlags *config_printflags = NULL;
SerialPortFlags *config_serialportflags = NULL; 
ModemStrings *config_modemstrings = NULL;

/**
 * The run() loop. This is run after init() finishes.
 */
unsigned char run()
{
  return 0;
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

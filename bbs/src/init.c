/**
 * init.c - initialization routines.
 */

#include "init.h"
#include "config.h"
#include "util.h"
#include "terminal.h"

unsigned char init()
{
  config_init();
  config_save();
  config_load();
  log(LOG_LEVEL_NOTICE,"BBS Initializing. Loading config file: " FILE_BBS_CONFIG);

  if (terminal_driver_open() != 0)
    {
      fatal_error("Serial Driver not opened. Exiting.");
      return 1;
    }
  return 0;
}

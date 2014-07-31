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
  terminal_init();
  return 0;
}

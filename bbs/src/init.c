/**
 * init.c - initialization routines.
 */

#include "init.h"
#include "config.h"
#include "util.h"

int init()
{
  config_load();
  log(LOG_LEVEL_NOTICE,"BBS Initializing. Loading config file: " FILE_BBS_CONFIG);
  return 0;
}

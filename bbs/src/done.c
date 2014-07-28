/**
 * done.c - close-down routines
 */

#include "done.h"
#include "config.h"
#include "terminal.h"

int done()
{
  terminal_done();
  config_done();
  return 0;
}

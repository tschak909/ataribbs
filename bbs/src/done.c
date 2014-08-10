/**
 * done.c - close-down routines
 */

#include "done.h"
#include <bbslib/common/config.h>
#include <bbslib/common/terminal.h>

unsigned char done()
{
  terminal_done();
  config_done();
  return 0;
}

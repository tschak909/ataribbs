/**
 * terminal.c - Functions to deal with RS232 or potentially other endpoints (Dragon Cart?)
 */

#include "terminal.h"
#include "config.h"
#include "util.h"

#ifdef SERIAL_850
#include "serial_850.h"
#endif

int terminal_init()
{
  return 0;
}

int terminal_done()
{
  return 0;
}

int term_driver_open()
{
  return 0;
}

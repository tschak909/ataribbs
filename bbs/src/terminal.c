/**
 * terminal.c - Functions to deal with RS232 or potentially other endpoints (Dragon Cart?)
 */

#include "terminal.h"
#include <serial.h>
#include "config.h"
#include "util.h"
#define DRIVERNAME "D1:ATRRDEV.SER"
#define ERR_NO_DRIVER "Could not find " DRIVERNAME " - Ensure it exists."
#define ERR_NO_DEVICE "Couldn't find terminal interface hardware."
#define ERR_GENERIC_FAILURE "Serial driver Failure: 0x%x"

int terminal_init()
{
  //   log(LOG_LEVEL_NOTICE,"Opening serial driver: " DRIVERNAME);
  if (term_driver_open() != 0)
    {
      return 1;
    }
  return 0;
}

int terminal_done()
{
  ser_unload();
  return 0;
}

int term_driver_open()
{
  int driver_open_res;
  driver_open_res = ser_load_driver(DRIVERNAME);
  if (driver_open_res == SER_ERR_CANNOT_LOAD)
    {
      fatal_error(ERR_NO_DRIVER);
      return 1;
    }
  else if (driver_open_res == SER_ERR_NO_DEVICE)
    {
      fatal_error(ERR_NO_DEVICE);
      return 1;
    }
  else if (driver_open_res != SER_ERR_OK)
    {
      char err[32];
      sprintf(err,ERR_GENERIC_FAILURE,driver_open_res);
      fatal_error(err);
      return 1;
    }
}

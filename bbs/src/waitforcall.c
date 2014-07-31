/**
 * waitforcall.c - functions used to wait for call.
 */

#include "waitforcall.h"
#include "config.h"
#include "util.h"
#include "globals.h"
#include <serial.h>

unsigned char waitforcall()
{
  unsigned char i=0; // Received string from modem index.
  unsigned char rorc=0; // Matched Ring or Connect? 0 = not yet, 1 = RING, 2 = CONNECT 
  unsigned char c; // Last received modem character.
  char* match; // String to match.

 waitloop:
  while (ser_get(&c) == SER_ERR_NO_DATA)
    {
      waitforcall_check_console_switches();
      waitforcall_check_keyboard();
    }

  if ((c==config_modemstrings->ring_string[0]) && (rorc==0))
    {
      // First char, ring string.
      match = config_modemstrings->ring_string;
    }
  else if ((c==config_modemstrings->connect_string[0]) && (rorc==0))
    {
      // First char, connect string.
      match = config_modemstrings->connect_string;
    }
  
  goto waitloop;

  return 0;

}

void waitforcall_check_console_switches()
{

}

void waitforcall_check_keyboard()
{

}

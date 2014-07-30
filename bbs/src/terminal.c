/**
 * terminal.c - Functions to deal with RS232 or potentially other endpoints (Dragon Cart?)
 */

#pragma static-locals (on)

#include "terminal.h"
#include "config.h"
#include "util.h"
#include "globals.h"
#include <serial.h>
#include <6502.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define DRIVERNAME "D1:ATRRDEV.SER"
#define MODEM_RESET_STRING "\rATZ\r"
#define MODEM_RESET_RESPONSE "OK"

int terminal_init()
{
  unsigned char res;
  struct ser_params params;

  params.baudrate = config_serialportflags->scbits.serial_port_baud;
  params.databits = config_serialportflags->scbits.serial_port_data_bits;
  params.stopbits = config_serialportflags->scbits.serial_port_stop_bits;
  params.parity = config_serialportflags->scbits.serial_port_parity;
  params.handshake = SER_HS_HW; // For now, this is the only option, so...
 
  if (terminal_driver_open() != 0)
    {
      fatal_error("Serial Driver not opened. Exiting.");
      return 1;
    }
   
  if ((res = ser_open(&params)) != SER_ERR_OK)
    {
      char cErr[32];
      sprintf(cErr,"Could not open serial port - Generic Error 0x%x\n",res);
      fatal_error(cErr);
      return 1;
    }
  
  // Port is open, ready for business. Attempt a sanity check.

  printf("Port open.\n");

  if (terminal_sanity_check() != 0)
    {
      fatal_error("Modem sanity check failed. Aborting.");
      return 1;
    }

  if (terminal_init_modem() != 0)
    {
      fatal_error("Could not initialize modem. Aborting.");
      return 1;
    }

  return 0; 
}

int terminal_done()
{
  ser_unload();
  return 0;
}

int terminal_driver_open()
{
  unsigned char res = ser_load_driver(DRIVERNAME);
  if (res == SER_ERR_CANNOT_LOAD)
    {
      fatal_error("Can't load serial driver " DRIVERNAME " - aborting.");
      return 1;
    }
  else if (res == SER_ERR_NO_DEVICE)
    {
      fatal_error("Can't find interface. Is it enabled or powered on?");
      return 1;
    }
  else if (res != SER_ERR_OK)
    {
      char cErr[32];
      sprintf(cErr,"Can't open serial driver " DRIVERNAME " - Error: 0x%x",res);
      fatal_error(cErr);
    }
  return 0; 
}

int terminal_sanity_check()
{
  log(LOG_LEVEL_NOTICE,"Performing Modem sanity check.");
  return terminal_send_and_expect_response(MODEM_RESET_STRING,MODEM_RESET_RESPONSE);
}

int terminal_init_modem()
{
  log(LOG_LEVEL_NOTICE,"Initializing modem.");
  return terminal_send_and_expect_response(config_modemstrings->init_string,MODEM_RESET_RESPONSE);
}

unsigned char terminal_send(const char* sendString, char willEcho)
{
  unsigned char res;
  int i;
  for (i=0;i<strlen(sendString);++i)
    {
      res = ser_put(sendString[i]);
      if (res == SER_ERR_OVERFLOW)
	{
	  // We're overflowing, wait a moment. 
	  sleep(1);
	  ser_put(sendString[i]);
	}
      else if (res != SER_ERR_OK)
	{
	  char cErr[32];
	  sprintf(cErr,"Could not send to modem: Generic Error - 0x%x",res);
	  return 1;
	}
      if (willEcho == 1)
	{
	  char c;
	  while (ser_get(&c) == SER_ERR_NO_DATA) 
	    {
	      
	    }
	}
      // Output to screen, let's see if I want to keep this here.
      putasciichar(sendString[i]);
    }
  return 0;
}

unsigned char terminal_send_and_expect_response(const char* sendString,const char* recvString)
{
  clock_t beg, end, dur = 0;
  int i=0;

  if (terminal_send(sendString,1) != 0)
    {
      char cErr[64];
      sprintf(cErr,"Could not send string to modem: %s",sendString);
      log(LOG_LEVEL_WARNING,cErr);
      return 1;
    }

  beg = clock();
  dur = 0;

  while (dur < 5) // Remember to fix this damned conditional, when everything is ok.
    {
      char yet=1; // Only start comparing when the first byte in response is received.
      char c;
      while (ser_get(&c) == SER_ERR_NO_DATA) { }; // TODO: come back here, look at atrrdev serial driver and implement error codes.
      putasciichar(c); // see if I actually want this here.
      if (i<strlen(recvString))
	{
	  if (recvString[i]==c && yet==1)
	    {
	      yet=0; // Start comparing, first char matched.
	    }
	  if (yet==0)
	    {
	      if (recvString[i]!=c) // incorrect character matched.
		{
		  goto fail;
		}
	      else // correct character matched.
		{
		  ++i;
		}
	    }
	}
      else
	{
	  // All chars matched successfully.
	  return 0;
	}
      // Calculate timeout
      end = clock();
      dur = ((end - beg) / CLOCKS_PER_SEC);
    }
  
 fail: return 1;
}

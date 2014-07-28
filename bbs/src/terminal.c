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
#define MODEM_RESET_STRING "ATZ\r"
#define MODEM_RESET_RESPONSE "OK\r\n"
#define TERMINAL_TIMEOUT_SEC 5;

int terminal_init()
{
  unsigned char res;
  struct ser_params params;

  params.baudrate = SER_BAUD_1200;
  params.databits = config_serialportflags->scbits.serial_port_data_bits;
  params.stopbits = config_serialportflags->scbits.serial_port_stop_bits;
  params.parity = config_serialportflags->scbits.serial_port_parity;
  params.handshake = SER_HS_HW;
 
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
  return terminal_send_and_expect_response(MODEM_RESET_STRING,MODEM_RESET_RESPONSE);
}

unsigned char terminal_send(const char* sendString)
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
      // Output to screen, let's see if I want to keep this here.
      putchar(sendString[i]);
    }
  return 0;
}

unsigned char terminal_send_and_expect_response(const char* sendString,const char* recvString)
{
  clock_t beg, end, dur = 0;
  char* cResp;

  strcpy(cResp,'\0'); // Initialize response string.

  if (terminal_send(sendString) != 0)
    {
      char cErr[64];
      sprintf(cErr,"Could not send string to modem: %s",sendString);
      log(LOG_LEVEL_WARNING,cErr);
      return 1;
    }

  beg = clock();

  while (1==1)
    {
      char c;
      int i=0;
      char res = ser_get(&c);
      if (res != SER_ERR_NO_DATA)
	{
	  // We have data, process it.
	  // Append byte to string
	  int len = strlen(cResp);
	  cResp[len]=c;
	  cResp[len+1]='\0';
	  // Output to screen, figure out if i really want this here.
	  printf("%c",c);
	  if (i<strlen(recvString))
	    {
	      // do the compare.
	      if (recvString[i] != c)
		{
		  char cErr[64];
		  sprintf(cErr,"Response not matched: output: %s",cResp);
		  log(LOG_LEVEL_WARNING,cErr);
		  goto fail;
		}
	      else
		{
		  // Increment comparator index, in preparation for next byte.
		  ++i;
		}
	    }
	  else
	    {
	      // We matched, return success.
	      return 0;
	    }
	}
      else
	{
	  // We have no data, just let it ride.
	}
      end = clock();
      dur = (end - beg);
    }
  
 fail: return 1;
}

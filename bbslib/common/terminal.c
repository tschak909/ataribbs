/**
 * terminal.c - Functions to deal with RS232 or potentially other endpoints (Dragon Cart?)
 */

#pragma static-locals (on)

#include "terminal.h"
#include "config.h"
#include "util.h"
#include <serial.h>
#include <6502.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <atari.h>
#include <stdlib.h>
#include <fcntl.h>

#define DRIVERNAME "D1:ATRRDEV.SER"
#define MODEM_RESET_STRING "\rATZ\r"
#define MODEM_RESET_RESPONSE "OK"
#define MODEM_SEND_NUM_RETRIES 4
#define MODEM_RECIEVE_TIMEOUT 3
#define TERMINAL_FILE_SEND_BUFFER_SIZE 1024

extern PrinterFlags *config_printflags;
extern SerialPortFlags *config_serialportflags;
extern ModemStrings *config_modemstrings;

unsigned char terminal_port_status;
unsigned char terminal_type;

unsigned char seropen_count;

unsigned char terminal_init()
{
  unsigned char res;
  terminal_type = TERMINAL_TYPE_ASCII;
 
  if ((res = terminal_open_port()) != SER_ERR_OK)
    {
      char cErr[32];
      sprintf(cErr,"Could not open serial port - Generic Error 0x%x\n",res);
      fatal_error(cErr);
      return 1;
    }
  
  // Port is open, ready for business. Attempt a sanity check.

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

unsigned char terminal_done()
{
  terminal_close_port();
  ser_unload();
  terminal_port_status = TERMINAL_PORT_CLOSED;
  return 0;
}

unsigned char terminal_driver_open()
{
  unsigned char res;
  res = ser_load_driver(DRIVERNAME);
  seropen_count=0;
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
  terminal_port_status = TERMINAL_PORT_CLOSED;
  return 0; 
}

unsigned char terminal_open_port()
{
  struct ser_params params;
  unsigned char ret;
  params.baudrate = config_serialportflags->serial_port_baud;
  params.databits = config_serialportflags->serial_port_data_bits;
  params.stopbits = config_serialportflags->serial_port_stop_bits;
  params.parity = config_serialportflags->serial_port_parity;
  params.handshake = SER_HS_HW; // For now, this is the only option, so...
  if (terminal_port_status == TERMINAL_PORT_CLOSED)
    {
      seropen_count++;
      ret = ser_open(&params);
      if (ret = SER_ERR_OK)
	terminal_port_status = TERMINAL_PORT_OPEN;
      else
	terminal_port_status = TERMINAL_PORT_CLOSED;
      return ret;
    }
      else
    {
      ret = SER_ERR_OK; // Port already open.
    }
}

unsigned char terminal_close_port()
{
  if (terminal_port_status == TERMINAL_PORT_OPEN)
    {
      terminal_port_status = TERMINAL_PORT_CLOSED; // ??? Is there a possibility for it to be stuck open?
      seropen_count--;
      return ser_close();
    }
  else
    {
      seropen_count--;
      return ser_close(); // Port is already closed.
    }
  return 0;
}

unsigned char terminal_sanity_check()
{
  log(LOG_LEVEL_NOTICE,"Performing Modem sanity check.");
  return terminal_send_and_expect_response(MODEM_RESET_STRING,MODEM_RESET_RESPONSE,0);
}

unsigned char terminal_init_modem()
{
  log(LOG_LEVEL_NOTICE,"Initializing modem.");
  return terminal_send_and_expect_response(config_modemstrings->init_string,MODEM_RESET_RESPONSE,0);
}

unsigned char terminal_send(const char* sendString, unsigned char willEcho)
{
  unsigned char res;
  register int i;
  for (i=0;i<strlen(sendString);++i)
    {
      if (sendString[i] == '\n' && terminal_type == TERMINAL_TYPE_ASCII)
	{
	  res = ser_put(0x0a); // Fix idiotic newline translation.
	}
      else
	{
	  res = ser_put(sendString[i]);
	}
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
  terminal_flush();
  return 0;
}

unsigned char terminal_send_and_expect_response(const char* sendString,const char* recvString, unsigned char echoSend)
{
  clock_t beg, end, dur = 0;
  register char i=0;
  char retries=0; 

 retry:
  if (terminal_send(sendString,echoSend) != 0)
    {
      char cErr[64];
      sprintf(cErr,"Could not send string to modem: %s",sendString);
      log(LOG_LEVEL_WARNING,cErr);
      return 1;
    }

  beg = clock();
  dur = 0;

  while ((dur < MODEM_RECIEVE_TIMEOUT) && (retries < MODEM_SEND_NUM_RETRIES))
    {
      char yet=1; // Only start comparing when the first byte in response is received.
      char c;
      while (ser_get(&c) == SER_ERR_NO_DATA) { goto tick; }; // TODO: come back here, look at atrrdev serial driver and implement error codes.
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
		  // Sleep a bit, reset everything. Try again.
		  log(LOG_LEVEL_WARNING,"Incorrect response from MODEM, trying again.");
		  sleep(2);
		  i=0;
		  retries++;
		  yet=1;
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
	  terminal_flush();
	  return 0;
	}

    tick:      // Calculate timeout
      end = clock();
      dur = ((end - beg) / CLOCKS_PER_SEC);
    }

  if (retries < MODEM_SEND_NUM_RETRIES)
    {
      log(LOG_LEVEL_WARNING,"Timed out waiting for response. Retrying.");
      i=0;
      retries++;
      terminal_flush();
      goto retry;
    }
  
  log(LOG_LEVEL_CRITICAL,"Failed sending to modem.");
  terminal_flush();
  return 1;
}

void terminal_flush()
{
  char c;
  while (ser_get(&c) != SER_ERR_NO_DATA) { /* just spin and flush data. */}
}

void terminal_hang_up()
{
  terminal_send_and_expect_response("+++","OK",0);
  terminal_send_and_expect_response("ATH0\r","OK",1);
}

unsigned char terminal_get_char()
{
  unsigned char c;
  while (ser_get(&c) == SER_ERR_NO_DATA) { /* Put timer tick in here. */ }
   return c;
}

unsigned char terminal_get_and_echo(unsigned char i)
{
  unsigned char c = terminal_get_char();
   if (is_a_backspace(c)==1)
    {
      if (i>0)
	{
	  putasciichar(c);
	  ser_put(c);
	}
    }
  else
    {
      putasciichar(c);
      ser_put(c);
    }
  return c;
}

unsigned char terminal_get_and_echo_char(unsigned char i, unsigned char e)
{
  unsigned char c = terminal_get_char();
   if (is_a_backspace(c)==1)
    {
      if (i>0)
	{
	  putasciichar(c);
	  ser_put(c);
	}
    }
  else
    {
      if (is_a_return(c)==0)
	{
	  putasciichar(e);
	  ser_put(e);
	}
    }
  return c;
}

void terminal_send_eol()
{
  switch (terminal_type)
    {
    case TERMINAL_TYPE_ASCII:
      ser_put(0x0d);
      ser_put(0x0a);
      break;
    case TERMINAL_TYPE_ATASCII:
      ser_put(0x9b);
      break;
    }
}

void terminal_determine_eol()
{
  char c;
  terminal_send("Press <ENTER> or <RETURN>: ",0);
  while (c = terminal_get_char())
    {
      if (is_an_ascii_cr(c))
	{
	  terminal_send("ASCII Detected.",0);
	  terminal_type=TERMINAL_TYPE_ASCII;
	  terminal_send_eol();
	  return;
	}
      else if (is_an_atascii_eol(c))
	{
	  terminal_send("ATASCII Detected.",0);
	  terminal_type=TERMINAL_TYPE_ATASCII;
	  terminal_send_eol();
	  return;
	}
    }
}

void terminal_send_file(const char* filename)
{
  int fd;
  size_t abr;
  char *buf = malloc(TERMINAL_FILE_SEND_BUFFER_SIZE);

  if (!buf)
    {
      printf("terminal_send_file() - Out of memory error while allocating buffer.");
      return;
    }

  terminal_close_port();

  fd = open(filename,O_RDONLY);
  while (abr = read(fd,buf,TERMINAL_FILE_SEND_BUFFER_SIZE))
    {
      terminal_open_port();
      buf[abr+1] = '\0';
      terminal_send(buf,0);
      terminal_close_port();
    }
  free(buf);
  close(fd);
  terminal_open_port();
}

void terminal_send_screen(const char* filename)
{
  char ext[4];
  char path[20];
  if (terminal_type == TERMINAL_TYPE_ATASCII)
    {
      strcpy(ext,"ATA");
    }
  else if (terminal_type == TERMINAL_TYPE_ASCII)
    {
      strcpy(ext,"ASC");
    }
  sprintf(path,"D1:%s.%s",filename,ext);
  terminal_send_file(path);
}

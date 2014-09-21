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
#include <conio.h>

#define DRIVERNAME "D1:ATRRDEV.SER"
#define MODEM_RESET_STRING "\rATZ\r"
#define MODEM_RESET_RESPONSE "OK"
#define MODEM_SEND_NUM_RETRIES 4
#define MODEM_RECIEVE_TIMEOUT 2
#define TERMINAL_FILE_SEND_BUFFER_SIZE 256

extern PrinterFlags *config_printflags;
extern SerialPortFlags *config_serialportflags;
extern ModemStrings *config_modemstrings;

unsigned char terminal_port_status;
unsigned char terminal_type;
unsigned char seropen_count;
unsigned char terminal_line_counter;
unsigned char terminal_line_counter_enable;
unsigned char terminal_num_lines;
unsigned char ansi_state_detect;
unsigned char terminal_chat_sysop_is_speaking=0;

unsigned char terminal_init()
{
  unsigned char res;
  terminal_type = TERMINAL_TYPE_ASCII;
  terminal_reset_line_counter();
  terminal_disable_line_counter();
  terminal_num_lines=20; // FIXME: Pull from user.
 
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

void terminal_send_char(char c)
{
  ser_put(c);
  putchar(c); // simple, for now...
}

unsigned char terminal_send(const char* sendString, unsigned char willEcho)
{
  unsigned char res;
  unsigned char x;
  register int i;
  for (i=0;i<strlen(sendString);++i)
    {
      // Primitive XON/XOFF handling.
      if (ser_get(&x) == SER_ERR_NO_DATA)
	{
	  if (x==0x13)
	    {
	      while (x!=0x11)
		{
		  ser_get(&x);
		}
	    }
	  // end primitive XON/OFF
	}
      if (terminal_line_counter_enable && terminal_is_an_eol(sendString[i]))
	{
	  if (terminal_line_counter > terminal_num_lines)
	    {
	      terminal_close_port();
	      terminal_open_port();
	      terminal_send_pagination_prompt();
	    }
	  else
	    {
	      terminal_line_counter++;
	    }
	}
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
  terminal_send_eol();
  terminal_send_and_expect_response("+++","OK",0);
  terminal_send_and_expect_response("ATH0\r","OK",1);
}

unsigned char terminal_get_char()
{
  unsigned char c;
  while (ser_get(&c) == SER_ERR_NO_DATA) 
    { /* Put timer tick in here. */ 
      if (kbhit())
	{
	  terminal_flush();
	  return cgetc();
	}
    }
  if ((terminal_type == TERMINAL_TYPE_ASCII) && (ansi_state_detect==0) && (c==0x1b))
    {
      ansi_state_detect=1;
      while (ser_get(&c)==SER_ERR_NO_DATA) { }
    }
  else
    ansi_state_detect=0;

  if ((terminal_type == TERMINAL_TYPE_ASCII) && (ansi_state_detect==1) && (c==0x5b))
    {
      ansi_state_detect=2;
      while (ser_get(&c)==SER_ERR_NO_DATA) { }
    }
  else
    ansi_state_detect=0;

  if (terminal_type == TERMINAL_TYPE_ASCII && ansi_state_detect==2 && (c==0x43))
    {
      ansi_state_detect=0;
      c=0x1f;
    }
  else if (terminal_type == TERMINAL_TYPE_ASCII && ansi_state_detect==2 && (c==0x44))
    {
      ansi_state_detect=0;
      c=0x1e;
    }
  return c;
}

unsigned char terminal_get_char_chat(char* username)
{
  unsigned char c;
  while (ser_get(&c) == SER_ERR_NO_DATA) 
    { /* Put timer tick in here. */ 
      if (kbhit())
	{
	  if (!terminal_chat_sysop_is_speaking)
	    {
	      terminal_chat_sysop_is_speaking=1;
	      terminal_send_eol();
	      terminal_send_eol();
	      terminal_send("SysOp: ",0);
	    }
	  terminal_flush();
	  c=cgetc();
	  if (terminal_type == TERMINAL_TYPE_ASCII && c==0x9b)
	    {
	      ser_put(0x0a);
	      return 0x0d;
	    }
	  if (terminal_type == TERMINAL_TYPE_ASCII && is_a_left(c))
	    {
	      terminal_send_left();
	      return 0xff;
	    }
	  if (terminal_type == TERMINAL_TYPE_ASCII && is_a_right(c))
	    {
	      terminal_send_right();
	      return 0xff;
 	    }
	  if (terminal_type == TERMINAL_TYPE_ASCII && is_a_backspace(c))
	    {
	      return 0x7f;
	    }
	  return c;
	}
    }
  if (terminal_chat_sysop_is_speaking)
    {
      terminal_chat_sysop_is_speaking=0;
      terminal_send_eol();
      terminal_send_eol();
      terminal_send(username,0);
      terminal_send(": ",0);
    }
  if ((terminal_type == TERMINAL_TYPE_ASCII) && (ansi_state_detect==0) && (c==0x1b))
    {
      ansi_state_detect=1;
      while (ser_get(&c)==SER_ERR_NO_DATA) { }
    }
  else
    ansi_state_detect=0;

  if ((terminal_type == TERMINAL_TYPE_ASCII) && (ansi_state_detect==1) && (c==0x5b))
    {
      ansi_state_detect=2;
      while (ser_get(&c)==SER_ERR_NO_DATA) { }
    }
  else
    ansi_state_detect=0;

  if (terminal_type == TERMINAL_TYPE_ASCII && ansi_state_detect==2 && (c==0x43))
    {
      ansi_state_detect=0;
      c=0x1f;
    }
  else if (terminal_type == TERMINAL_TYPE_ASCII && ansi_state_detect==2 && (c==0x44))
    {
      ansi_state_detect=0;
      c=0x1e;
    }

  return c;
}

void terminal_clear_chat()
{
  terminal_chat_sysop_is_speaking=0;
}

unsigned char terminal_get_and_echo(unsigned char i, unsigned char j, unsigned char size, unsigned char rubout)
{
  unsigned char c = terminal_get_char();
  if (is_a_backspace(c)==1)
    {
      if (i>0)
	{
	  putasciichar(c);
	  ser_put(c);
	  putasciichar(rubout);
	  ser_put(rubout);
	  terminal_send_left();
	}
    }
  else if (is_a_left(c)==1)
    {
      if (i>0)
	{
	  terminal_send_left();
	}
    }
  else if (is_a_right(c)==1)
    {
      if (i<j)
	{
	  terminal_send_right();
	}
    }
  else
    {
       if (i<size)
	{
	  putasciichar(c);
	  ser_put(c);
	}
    }

  if (terminal_type == TERMINAL_TYPE_ASCII && is_an_ascii_cr(c))
    {
      terminal_send_eol();
    }

  return c;
}

unsigned char terminal_get_and_echo_char(unsigned char i, unsigned char size, unsigned char e, unsigned char rubout)
{
  unsigned char c = terminal_get_char();
   if (is_a_backspace(c)==1)
    {
      if (i>0)
	{
	  putasciichar(c);
	  ser_put(c);
	  putasciichar(rubout);
	  ser_put(rubout);
	  terminal_send_left();
	}
    }
  else
    {
      if (is_a_return(c)==0)
	{
	  if (i<size)
	    {
	      putasciichar(e);
	      ser_put(e);
	    }
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
      putchar(0x9b);
      break;
    case TERMINAL_TYPE_ATASCII:
      ser_put(0x9b);
      putchar(0x9b);
      break;
    }

  if (terminal_line_counter_enable)
    {
      if (terminal_line_counter > terminal_num_lines)
	{
	  terminal_close_port();
	  terminal_open_port();
	  terminal_send_pagination_prompt();
	}
      else
	{
	  terminal_line_counter++;
	}
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
	  terminal_send("ASCII",0);
	  terminal_type=TERMINAL_TYPE_ASCII;
	  terminal_send_eol();
	  return;
	}
      else if (is_an_atascii_eol(c))
	{
	  terminal_send("ATASCII",0);
	  terminal_type=TERMINAL_TYPE_ATASCII;
	  terminal_send_eol();
	  return;
	}
    }
}

unsigned char terminal_is_an_eol(unsigned char c)
{
  switch (terminal_type)
    {
    case TERMINAL_TYPE_ASCII:
      return is_an_ascii_cr(c);
      break;
    case TERMINAL_TYPE_ATASCII:
      return is_an_atascii_eol(c);
      break;
    }
}

void terminal_send_file(const char* filename)
{
  int fd;
  size_t abr;
  char *buf = calloc(1,TERMINAL_FILE_SEND_BUFFER_SIZE);

  if (!buf)
    {
      printf("terminal_send_file() - Out of memory error while allocating buffer.");
      return;
    }

  terminal_close_port();

  fd = open(filename,O_RDONLY);
  if (fd<0)
    {
      char msg[80];
      sprintf(msg,"terminal_send_file(): Could not open file: %s",filename);
      log(LOG_LEVEL_WARNING,msg);
      terminal_send(msg,0);
      terminal_send_eol();
      terminal_beep();
      close(fd);
      terminal_open_port();
      return;
    }
  while (abr = read(fd,buf,TERMINAL_FILE_SEND_BUFFER_SIZE))
    {
      terminal_open_port();
      buf[abr] = '\0';
      terminal_send(buf,0);
      terminal_close_port();
      memset(buf,0,TERMINAL_FILE_SEND_BUFFER_SIZE); // Clear the buffer, prevent residual crapola.
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

void terminal_send_up()
{
  switch (terminal_type)
    {
    case TERMINAL_TYPE_ASCII:
      ser_put(0x1B);
      ser_put('[');
      ser_put('A');
      putchar(0x1c);
      break;
    case TERMINAL_TYPE_ATASCII:
      ser_put(0x1c);
      putchar(0x1c);
      break;
    }
}

void terminal_send_down()
{
  switch (terminal_type)
    {
    case TERMINAL_TYPE_ASCII:
      ser_put(0x1B);
      ser_put('[');
      ser_put('B');
      putchar(0x1d);
      break;
    case TERMINAL_TYPE_ATASCII:
      ser_put(0x1d);
      putchar(0x1d);
      break;
    }
}

void terminal_send_left()
{
  switch (terminal_type)
    {
    case TERMINAL_TYPE_ASCII:
      ser_put(0x1B);
      ser_put('[');
      ser_put('D');
      putchar(0x1e);
      break;
    case TERMINAL_TYPE_ATASCII:
      ser_put(0x1e);
      putchar(0x1e);
      break;
    }
}

void terminal_send_right()
{
  switch (terminal_type)
    {
    case TERMINAL_TYPE_ASCII:
      ser_put(0x1B);
      ser_put('[');
      ser_put('C');
      putchar(0x1f);
      break;
    case TERMINAL_TYPE_ATASCII:
      ser_put(0x1f);
      putchar(0x1f);
      break;
    }
}

void terminal_send_clear_screen()
{
  switch (terminal_type)
    {
    case TERMINAL_TYPE_ASCII:
      ser_put(0x1B);
      ser_put('[');
      ser_put('2');
      ser_put('J');
      putchar(0x7d);
    case TERMINAL_TYPE_ATASCII:
      ser_put(0x7d);
      putchar(0x7d);
    }
}

void terminal_send_pagination_prompt()
{
  terminal_disable_line_counter();
  terminal_send_eol();
  terminal_send("---Pause---",0);
  terminal_get_char();
  terminal_send_eol();
  terminal_reset_line_counter();
  terminal_enable_line_counter();
  terminal_close_port();
}

void terminal_beep()
{
  switch(terminal_type)
    {
    case TERMINAL_TYPE_ASCII:
      ser_put(0x07);
      putchar(0xfd);
    case TERMINAL_TYPE_ATASCII:
      ser_put(0xfd);
      putchar(0xfd);
    }
}

void terminal_reset_line_counter()
{
  terminal_line_counter=0;
}

unsigned char terminal_get_line_counter()
{
  return terminal_line_counter;
}

void terminal_enable_line_counter()
{
  terminal_line_counter_enable=1;
}

void terminal_disable_line_counter()
{
  terminal_line_counter=0;
  terminal_line_counter_enable=0;
}

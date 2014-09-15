/**
 * util.c - miscellaneous utility functions.
 */

#include "util.h"
#include "config.h"
#include "terminal.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <atari.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <6502.h>
#include <assert.h>

#define LOG_FORMAT "%s - %s - %s\n"
#define LOG_FILE "D1:SYSTEM.LOG"

unsigned char printer_error = 0;
unsigned char logfile_error = 0;

extern PrinterFlags *config_printflags;
extern SerialPortFlags *config_serialportflags;
extern ModemStrings *config_modemstrings;

/**
 * A fatal error has occurred.
 *
 * @param msg - The message to print. 
 */
void fatal_error(const char* msg)
{
  printf("FATAL ERROR: %s",msg);
  exit(1);
}

/**
 * Write message to log
 * @param level - logging level, see the log level defines
 * @param msg - the message to log
 */
void log(char level, const char* msg)
{
  int printer;
  int logFile;
  char* logstring;
  TimeDate td;
  logstring = calloc(256,sizeof(char));
  if (!logstring)
    {
      return;
    }

  terminal_close_port();
  timedate(&td);
  if (config_printflags->printer_use == 1 && 
      config_printflags->printer_log == 1 && 
      printer_error == 0)
    {
      printer = open("P:",O_APPEND);
      sprintf(logstring,"20%02u-%02u-%02u %02u:%02u:%02u %s %s\n",td.year,td.month,td.day,td.hours,td.minutes,td.seconds,_log_level(level),msg);
      write(printer,logstring,strlen(logstring));
      close(printer);
      logFile = open(LOG_FILE,O_APPEND);
      if (logfile_error == 0)
	write(logFile,logstring,strlen(logstring));
      close(logFile);
    }
  free(logstring);
  terminal_open_port();

}

/**
 * Return the current date/time 
 * for the logs.
 */
const char* _log_date_time()
{
  return "2014-01-01";
}

/**
 * Write a log level string given a log level #
 * @param level - the log level, see the log level defines
 */
const char* _log_level(char level)
{
  switch (level)
    {
    case LOG_LEVEL_DEBUG:
      return "DEBUG";
    case LOG_LEVEL_NOTICE:
      return "NOTICE";
    case LOG_LEVEL_WARNING:
      return "WARNING";
    case LOG_LEVEL_CRITICAL:
      return "CRITICAL";
    case LOG_LEVEL_EMERGENCY:
      return "EMERGENCY";
    }
  return "UNKNOWN";
}

void putasciichar(char c)
{
  switch (c)
    {
    case 0x7:
      putchar(0xfd); // ATASCII BELL
      break;
    case 0x8:
    case 0x7f:
      putchar(CH_DEL); // ATASCII BS
      break;
    case 0x9:
      putchar(0x7f); // ATASCII TAB
      break;
    case 0xa:           // LF - ignore.
      break;
    case 0xd:
      putchar(0x9b);
      break;
    default:
      // pass everything else through.
      putchar(c);
    }
}

unsigned char is_a_left(unsigned char c)
{
  return c==0x1e; // terminal_get_char translates ansi left to atascii left
}

unsigned char is_a_right(unsigned char c)
{
  return c==0x1f; // terminal_get_char translates ansi right to atascii right.
}

unsigned char is_a_backspace(unsigned char c)
{
  return c==0x7f || c==0x8 || c==0x7e;
}

unsigned char is_a_return(unsigned char c)
{
  return c==0x0d || c==0x9b;
}

unsigned char is_an_ascii_cr(unsigned char c)
{
  return c==0x0d;
}

unsigned char is_an_atascii_eol(unsigned char c)
{
  return c==0x9b;
}

unsigned short crc16(unsigned char* data_p, unsigned char length){
    unsigned char x;
    unsigned short crc = 0xFFFF;

    while (length--){
        x = crc >> 8 ^ *data_p++;
        x ^= x>>4;
        crc = (crc << 8) ^ ((unsigned short)(x << 12)) ^ ((unsigned short)(x <<5)) ^ ((unsigned short)x);
    }
    return crc;
}

void splash()
{
  FILE* fp=fopen("D1:SPLASH.ATA","r");
  char buf[128];
  char i;
  size_t abr;

  printf("\n");

  if (!fp)
    {
      perror("splash(): ");
      fclose(fp);
      return;
    }

  while (!feof(fp))
    {
      abr = fread(buf,sizeof(char),128,fp);
      for (i=0;i<abr;++i)
	{
	  putchar(buf[i]);
	}
    }
  
  printf("\n");
  fclose(fp);
}

void timedate(TimeDate* td)
{
  struct regs r;
  assert(td!=NULL);
  r.pc = 0x703; // KERNEL
  r.y = 100;    // GETTD

  *(byte*) 0x0761 = 0x10; // DEVICE

  _sys(&r);     // Do Kernel Call.

  td->day     = *(byte*) 0x77B;
  td->month   = *(byte*) 0x77C;
  td->year    = *(byte*) 0x77D;
  td->hours   = *(byte*) 0x77E;
  td->minutes = *(byte*) 0x77F;
  td->seconds = *(byte*) 0x780;

  return;
}

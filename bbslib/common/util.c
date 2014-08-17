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
  logstring = calloc(256,sizeof(char));
  if (!logstring)
    {
      return;
    }

  terminal_close_port();

  if (config_printflags->printer_use == 1 && 
      config_printflags->printer_log == 1 && 
      printer_error == 0)
    {
      printer = open("P:",O_APPEND);
      sprintf(logstring,LOG_FORMAT,_log_date_time(),_log_level(level),msg);
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

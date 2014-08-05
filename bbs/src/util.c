/**
 * util.c - miscellaneous utility functions.
 */

#include "util.h"
#include "config.h"
#include "globals.h"
#include "terminal.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <atari.h>

#define LOG_FORMAT "%s - %s - %s\n"
#define LOG_FILE "D1:SYSTEM.LOG"

unsigned char printer_error = 0;
unsigned char logfile_error = 0;

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
  FILE *printer;
  FILE *logFile;
  if (config_printflags->pfbits.printer_use == 1 && 
      config_printflags->pfbits.printer_log == 1 && 
      printer_error == 0)
    {
      printer = fopen("P:","a");
      if (!printer)
	{
	  fatal_error("Could not open printer. Suppressing further output to printer.");
	  printer_error=1;
	}
      else
	{
	  fprintf(printer,LOG_FORMAT,_log_date_time(),_log_level(level),msg); 
	}
      fclose(printer);
      logFile = fopen(LOG_FILE,"a");
      if (!logFile)
	{
	  fatal_error("Could not write to " LOG_FILE " suppressing further output to file.");
	  logfile_error=1;
	}
      else
	{
	  if (logfile_error == 0)
	    fprintf(logFile,LOG_FORMAT,_log_date_time(),_log_level(level),msg);
	}
      fclose(logFile);
    }
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
  return c==0x7f || c==0x8;
}

unsigned char is_a_return(unsigned char c)
{
  return c==0x0d;
}

/**
 * input.c - slightly higher level input routines
 */

#include "input.h"
#include "config.h"
#include "util.h"
#include "terminal.h"
#include <string.h>

char* input_line_and_echo(unsigned char rubout)
{
  unsigned char c=0;
  unsigned char i=0;
  unsigned char buf[100];

  buf[0]='\0';

  while (is_a_return(c) == 0)
    {
      c=terminal_get_and_echo(i,rubout);
      if (is_a_backspace(c)==1)
	{
	  if (i>0)
	    {
	      --i;
	      buf[i] = '\0';
	    }
	}
      else if (is_a_return(c) == 0)
	{
	  buf[i] = c;
	  ++i;
	  buf[i] = '\0';
	}
    }
  return strdup(buf);
}

char* input_line_and_echo_char(unsigned char e,unsigned char rubout)
{
  unsigned char c=0;
  unsigned char i=0;
  unsigned char buf[100];

  buf[0]='\0';

  while (is_a_return(c) == 0)
    {
      c=terminal_get_and_echo_char(i,e,rubout);
      if (is_a_backspace(c)==1)
	{
	  if (i>0)
	    {
	      --i;
	      buf[i] = '\0';
	    }
	}
      else if (is_a_return(c) == 0)
	{
	  buf[i] = c;
	  ++i;
	  buf[i] = '\0';
	}
    }
  return strdup(buf);
}

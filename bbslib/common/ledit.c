/**
 * ledit.c - Line Editor functions
 */

#include <stdlib.h>
#include <string.h>
#include "ledit.h"

LineEditRecord* line;
int num_lines=0;

void* ledit_alloc()
{
  return calloc(1,sizeof(LineEditRecord));
}

void ledit_init()
{
  if (!line)
    {
      ledit_alloc();
    }
  else
    {
      memset(line,0,sizeof(LineEditRecord));
    }

  num_lines=0;
}

void ledit()
{

}

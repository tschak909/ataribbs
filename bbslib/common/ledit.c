/**
 * ledit.c - Line Editor functions
 */

#include <stdlib.h>
#include <string.h>
#include "ledit.h"

LineEditRecord* ledit_line;
int ledit_num_lines=0;
LineEditLinkage** linkages;
LineEditLinkage** ledit_oldlinkages;

void* ledit_alloc()
{
  return calloc(1,sizeof(LineEditRecord));
}

void * ledit_linkage_alloc(int numLines_to_add)
{
  size_t linkagePointersToAlloc;
  int oldNumLines = ledit_num_lines;
  int i;

  ledit_num_lines = ledit_num_lines+numLines_to_add;

  ledit_oldlinkages=linkages; // Save it in case realloc fails.
  linkagePointersToAlloc = sizeof(LineEditLinkage *)*ledit_num_lines;
  linkages = realloc(linkages,linkagePointersToAlloc);

  for (i=oldNumLines;i<ledit_num_lines;++i)
    {
      linkages[i]=calloc(1,sizeof(LineEditLinkage));
    }
  return linkages;
}

void ledit_init()
{
  if (!ledit_line)
    {
      ledit_alloc();
    }
  else
    {
      memset(ledit_line,0,sizeof(LineEditRecord));
    }

  ledit_num_lines=0;
}

void ledit()
{

}

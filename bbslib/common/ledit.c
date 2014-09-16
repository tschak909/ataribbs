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

void* ledit_line_alloc()
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

void ledit_line_free()
{
  free(ledit_line);
}

void ledit_linkage_free()
{
  int i;

  for (i=0;i<ledit_num_lines;++i)
    {
      free(linkages[i]);
    }

  free(linkages);
}

void ledit_init()
{
  if (!ledit_line)
    {
      ledit_line = ledit_line_alloc();
    }
  else
    {
      memset(ledit_line,0,sizeof(LineEditRecord));
    }

  ledit_num_lines=0;
  ledit_linkage_alloc(16);

}

void ledit_done()
{
  ledit_linkage_free();
  ledit_line_free();
}

void ledit()
{
  ledit_init();
  ledit_done();
}

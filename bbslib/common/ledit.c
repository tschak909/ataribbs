/**
 * ledit.c - Line Editor functions
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include "ledit.h"

LineEditRecord* ledit_line;
int ledit_num_lines=0;
LineEditLinkage** linkages;
LineEditLinkage** ledit_oldlinkages;
int ledit_fd;

int ledit_open()
{
  ledit_fd=open("D1:LEDIT.TMP",O_RDWR|O_CREAT|O_TRUNC);
  return ledit_fd;
}

void ledit_close()
{
  assert(ledit_fd>0);
  close(ledit_fd);
}

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

  if (ledit_open() < 0)
    {
      perror("Could not open D1:LEDIT.TMP");
      abort();
    }

}

void ledit_insert(const char* text)
{
  assert(ledit_fd>0);
  assert(text!=NULL);
  assert(ledit_line!=NULL);
  assert(linkages!=NULL);
  lseek(ledit_fd,0,SEEK_END);
  ledit_line->lineNo=ledit_num_lines;
  strcpy(ledit_line->line,text);
  write(ledit_fd,(LineEditRecord *)ledit_line,sizeof(LineEditRecord));
  *(linkages[ledit_num_lines])=ledit_num_lines;
  ledit_num_lines++;
}

unsigned char _ledit_get_line_offset(int line)
{
  return sizeof(LineEditRecord)*line;
}

unsigned char _ledit_get(int line)
{
  assert(ledit_fd>0);
  lseek(ledit_fd,_ledit_get_line_offset(line),SEEK_SET);
  if (read(ledit_fd,(LineEditRecord *)ledit_line,sizeof(LineEditRecord)) != sizeof(LineEditRecord))
    {
      // Failed.
      return 0;
    }
  return 1;
}

unsigned char ledit_get(int line)
{
  assert(line<ledit_num_lines);
  assert(linkages!=NULL);
  assert(linkages[line]!=NULL);
  return 0;
}

void ledit_done()
{
  ledit_close();
  ledit_linkage_free();
  ledit_line_free();
}

void ledit()
{
  ledit_init();
  ledit_insert("This is a test to see how well this actually works.");
  ledit_insert("Line 2");
  ledit_insert("Line 3 is here.");
  ledit_insert("Line 4 is right here.");
  ledit_insert("Line 5 is HERE!");
  ledit_done();
}

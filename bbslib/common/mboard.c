/**
 * mboard.c - Functions for dealing with message board groups (MMU)
 */

#include "mboard.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <assert.h>

unsigned char mboard_default;
unsigned char mboard_num_boards;

MMUFile mboard_open(const char* file)
{
  int fd = open(file,O_RDONLY);

  if (fd < 0)
    return fd;

  if (read(fd,&mboard_num_boards,sizeof(unsigned char)) != sizeof(unsigned char))
    return -1;

  if (read(fd,&mboard_default,sizeof(unsigned char)) != sizeof(unsigned char))
    return -1;

  return fd;
}

void mboard_close(MMUFile file)
{ 
  close(file);
}

MMUCursor mboard_scan_begin(MMUFile file)
{
  assert(file>0);
  return lseek(file,2,SEEK_CUR);
}

MMUCursor mboard_scan_next(MMUFile file,MMUCursor cursor, MMUEntry* entry)
{
  assert(file>0);
  return read(file,(MMUEntry *)entry,sizeof(MMUEntry)) + cursor;
}

unsigned char mboard_get(MMUFile file,char index,MMUEntry* entry)
{
  assert(file>0);
  if (lseek(file,index*sizeof(MMUEntry)+2,SEEK_SET) == index*sizeof(MMUEntry)+2)
    {
      read(file,(MMUEntry* )entry,sizeof(MMUEntry));
      return index;
    }
  return 255;
}

unsigned char mboard_get_default(MMUFile file,MMUEntry* entry)
{
  return mboard_get(file,mboard_default,entry);
}

unsigned char mboard_get_num_boards()
{
  return mboard_num_boards;
}

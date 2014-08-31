/**
 * header.c - functions to deal with headers.
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <assert.h>
#include "header.h"

long _header_offset(long msgId)
{
  return sizeof(MsgHeader) * msgId;
}

HeaderCursor header_quickscan_begin(MsgFile* file, long msgId)
{
  assert(file!=NULL);
  assert(file->hdrfd>0);

  return lseek(file->hdrfd,_header_offset(msgId),SEEK_SET);
}

HeaderCursor header_quickscan_next(MsgFile* file, HeaderCursor cursor, MsgHeader* header)
{
  assert(file!=NULL);
  assert(file->hdrfd>0);
  return read(file->hdrfd,(MsgHeader* )header,sizeof(MsgHeader)) + cursor;
}

void header_quickscan_end(MsgFile* file)
{
  // This currently doesn't do anything, Not yet.
}

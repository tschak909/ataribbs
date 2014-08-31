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

HeaderCursor header_scan_begin(MsgFile* file, long msgId)
{
  assert(file!=NULL);
  assert(file->hdrfd>0);

  return lseek(file->hdrfd,_header_offset(msgId),SEEK_SET);
}

HeaderCursor header_scan_next(MsgFile* file, HeaderCursor cursor, MsgHeader* header)
{
  assert(file!=NULL);
  assert(file->hdrfd>0);
  return read(file->hdrfd,(MsgHeader* )header,sizeof(MsgHeader)) + cursor;
}

HeaderCursor header_scan_find_from(MsgFile* file, HeaderCursor cursor, const char* from, MsgHeader* header)
{
  assert(file!=NULL);
  assert(file->hdrfd>0);
  assert(from!=NULL);
  while ((cursor=read(file->hdrfd,(MsgHeader *)header,sizeof(MsgHeader))) == sizeof(MsgHeader))
    {
      if (stricmp(from,header->from) == 0)
	{
	  // search successful
	  return cursor;
	}
    }

  // Search failed, reset to beginning.
  return -1;
}

HeaderCursor header_scan_find_network_id(MsgFile* file, HeaderCursor cursor, long networkId, MsgHeader* header)
{
  assert(file!=NULL);
  assert(file->hdrfd>0);
  while ((cursor=read(file->hdrfd,(MsgHeader *)header,sizeof(MsgHeader))) == sizeof(MsgHeader))
    {
      if (header->networkId == networkId)
	{
	  // search successful
	  return cursor;
	}
    }

  // Search failed, reset to beginning.
  return -1;
}

void header_scan_end(MsgFile* file)
{
  // This currently doesn't do anything, Not yet.
}

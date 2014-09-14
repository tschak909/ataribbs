/**
 * idx.c - functions for dealing with index files
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <assert.h>
#include "types.h"
#include "idx.h"

long idx_put_num_msgs(MsgFile* file, long nummsgs)
{
  assert(file!=NULL);
  lseek(file->idxfd,0,SEEK_SET);
  write(file->idxfd,&nummsgs,sizeof(long));
  return nummsgs;
}

long idx_get_num_msgs(MsgFile* file)
{
  long curPos;
  long nummsgs;

  assert(file!=NULL);
  
  curPos=lseek(file->idxfd,0,SEEK_END);
  
  if (curPos!=0)
    {
      lseek(file->idxfd,0,SEEK_SET);
      assert(read(file->idxfd,&nummsgs,sizeof(long)) == sizeof(long));
    }
  else
    {
      nummsgs = idx_put_num_msgs(file,0);
    }

  return nummsgs;
}

void idx_write(MsgFile* file, MsgIdxEntry* idx)
{
  assert(file!=NULL);
  assert(file->idxfd>0);
  assert(idx!=NULL);
  lseek(file->idxfd,0,SEEK_END);
  write(file->idxfd,(MsgIdxEntry *)idx,sizeof(MsgIdxEntry));
}

int idx_read(MsgFile* file, int msgId, MsgIdxEntry* idx)
{
  assert(file!=NULL);
  assert(file->idxfd>0);
  assert(idx!=NULL);
  lseek(file->idxfd,idx_offset(msgId),SEEK_SET);
  return read(file->idxfd,(MsgIdxEntry *)idx,sizeof(MsgIdxEntry));
}

long idx_offset(long msgId)
{
  return sizeof(MsgIdxEntry)*msgId+sizeof(long);
}

/**
 * msg.c - functions for dealing with whole messages 
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
#include "msg.h"
#include "idx.h"
#include "header.h"

int _msg_open(const char* msgfile, const char* extension)
{
  char tmp[32];
  strcpy(tmp,msgfile);
  strcat(tmp,extension);

  return open(tmp,O_CREAT|O_RDWR);
}

MsgFile* msg_open(const char* msgfile)
{
  MsgFile *file;

  file = calloc(1,sizeof(MsgFile));

  file->hdrfd = _msg_open(msgfile,".HDR");
  file->msgfd = _msg_open(msgfile,".MSG");
  file->idxfd = _msg_open(msgfile,".IDX");
  
  if (file->hdrfd < 0)
    return NULL;

  if (file->msgfd < 0)
    return NULL;

  if (file->idxfd < 0)
    return NULL;

  return file;

}

void msg_close(MsgFile* file)
{
  assert(file!=NULL);
  assert(file->msgfd > 0);
  assert(file->hdrfd > 0);
  assert(file->idxfd > 0);

  close(file->msgfd);
  close(file->hdrfd);
  close(file->idxfd);

  free(file);
}

void msg_write(MsgFile* file, char* body)
{
  size_t len;
  assert(file!=NULL);
  assert(file->msgfd>0);
  assert(body!=NULL);
  len=strlen(body);
  lseek(file->msgfd,0,SEEK_END);
  write(file->msgfd,&len,sizeof(size_t));
  write(file->msgfd,(char *)body,len);
}

unsigned char msg_put(MsgFile* file, MsgHeader* header, char* body)
{
  MsgIdxEntry idx;
  long nummsgs,hdrcurPos,idxcurPos,msgcurPos;
   
  assert(file!=NULL);
  assert(header!=NULL);
  assert(body!=NULL);
  assert(header->msgId==0);

  nummsgs = idx_get_num_msgs(file);
  nummsgs++;
  hdrcurPos = lseek(file->hdrfd,0,SEEK_END);
  msgcurPos = lseek(file->msgfd,0,SEEK_END);
  idxcurPos = lseek(file->idxfd,0,SEEK_END);

  /* fill out the index record */
  idx.msgId = nummsgs;
  idx.bodyOffset = msgcurPos;
  idx.hdrOffset = hdrcurPos;
  idx_write(file,&idx);
  
  /* fill out the rest of the header. */
  header->msgId = nummsgs;
  header_write(file,header);

  /** Finally, write the body. */
  msg_write(file,body);

  idx_put_num_msgs(file,nummsgs);

  return 0;

}

void msg_rewind(MsgFile* file)
{
  assert(file!=NULL);

  lseek(file->msgfd,0,SEEK_SET);
  lseek(file->hdrfd,0,SEEK_SET);
  lseek(file->idxfd,0,SEEK_SET);
}

unsigned char msg_get(MsgFile* file, long msgId, MsgHeader* header, char* body)
{
  long nummsgs;
  size_t bodySize;
  MsgIdxEntry idx;

  assert(file!=NULL);

  nummsgs = idx_get_num_msgs(file);

  assert(msgId < nummsgs);

  lseek(file->idxfd,idx_offset(msgId),SEEK_SET);
  read(file->idxfd,&idx,sizeof(MsgIdxEntry));
  lseek(file->hdrfd,idx.hdrOffset,SEEK_SET);
  assert(header!=NULL);
  read(file->hdrfd,(MsgHeader *)header,sizeof(MsgHeader));

  lseek(file->msgfd,idx.bodyOffset,SEEK_SET);
  read(file->msgfd,&bodySize,sizeof(size_t));
  assert(body!=NULL);
  read(file->msgfd,(char *)body,bodySize);
  
  return msgId;

}

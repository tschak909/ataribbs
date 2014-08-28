/**
 * flatmsg.c - routines to implement a flat message board.
 */

#ifndef FLATMSG_H
#define FLATMSG_H

typedef struct
{
  long msgId;
  long networkId;
  char from[64];
  char subject[64];
  char deleted;
} MsgHeader; 

typedef struct
{
  int msgfd;
  int hdrfd;
  int idxfd;
} MsgFile;

typedef struct
{
  long msgId;
  long hdrOffset;
  long bodyOffset;
} MsgIdxEntry;

// There isn't a message body entry struct. The message body file consists of 
// repeated sets of:
//
// 2 bytes: length (65535 bytes max message size. If it needs to be longer, um.. no.)
// X bytes: message body
// ...
// And the next one...

MsgFile* msg_open(const char* msgfile);
void msg_close(MsgFile* file);

#endif /* FLATMSG_H */

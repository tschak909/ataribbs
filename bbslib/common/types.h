/**
 * types.h - data types used by flatmsg
 */

#ifndef TYPES_H
#define TYPES_H

#include "util.h"

typedef struct
{
  int msgfd;
  int hdrfd;
  int idxfd;
} MsgFile;

typedef long HeaderCursor;
typedef struct
{
  long msgId;
  long networkId;
  TimeDate stamp;
  char from[64];
  char subject[64];
  char deleted;
} MsgHeader; 

typedef struct
{
  long msgId;
  long hdrOffset;
  long bodyOffset;
} MsgIdxEntry;

#endif /* TYPES_H */

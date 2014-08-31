/**
 * header.h - functions to deal with headers.
 */

#ifndef HEADER_H
#define HEADER_H

#include "flatmsg.h"

typedef long HeaderCursor;

HeaderCursor header_quickscan_begin(MsgFile* file, long msgId);
HeaderCursor header_quickscan_next(MsgFile* file, HeaderCursor cursor, MsgHeader* header);
void header_quickscan_end(MsgFile* file);

#endif

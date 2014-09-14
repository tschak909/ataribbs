/**
 * header.h - functions to deal with headers.
 */

#ifndef HEADER_H
#define HEADER_H

#include "msg.h"

long header_offset(long msgId);
HeaderCursor header_scan_begin(MsgFile* file, long msgId);
HeaderCursor header_scan_next(MsgFile* file, HeaderCursor cursor, MsgHeader* header);
HeaderCursor header_scan_find_from(MsgFile* file, HeaderCursor cursor, const char* from, MsgHeader* header);
HeaderCursor header_scan_find_subject(MsgFile* file, HeaderCursor cursor, const char* subject, MsgHeader* header);
HeaderCursor header_scan_find_network_id(MsgFile* file, HeaderCursor cursor, long networkId, MsgHeader* header);
void header_scan_end(MsgFile* file);

void header_write(MsgFile* file, MsgHeader* header);
int header_read(MsgFile* file, HeaderCursor cursor, MsgHeader *header);

#endif

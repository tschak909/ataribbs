/**
 * idx.h - functions dealing with the index file.
 */

#ifndef IDX_H
#define IDX_H

long idx_put_num_msgs(MsgFile* file, long nummsgs);
long idx_get_num_msgs(MsgFile* file);
void idx_write(MsgFile* file, MsgIdxEntry* idx);
long idx_offset(long msgId);

#endif

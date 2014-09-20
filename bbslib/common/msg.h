/**
 * msg.h - functions for dealing with whole messages
 */

#ifndef MSG_H
#define MSG_H

MsgFile* msg_open(const char* msgfile);
void msg_close(MsgFile* file);

void msg_rewind(MsgFile* file);

unsigned char msg_put(MsgFile* file, MsgHeader* header, char* body);
unsigned char msg_get(MsgFile* file, long msgId, MsgHeader* header, char* body);
void msg_write(MsgFile* file, char* body);

void msg_put_begin(MsgFile* file, MsgHeader* entry);
void msg_put_end();
void msg_put_chunked(char* line);


#endif /* MSG_H */

/**
 * flatmsg.c - routines to implement a flat message board.
 */

#ifndef FLATMSG_H
#define FLATMSG_H

typedef struct
{
  long msgId;
  long networkId;
  int bodyHash;
  char from[64];
  char subject[64];
  char deleted;
} MsgIDXEntry; 

// There isn't a message body entry struct. The message body file consists of 
// repeated sets of:
//
// 2 bytes: length (65535 bytes max message size. If it needs to be longer, um.. no.)
// X bytes: message body
// ...
// And the next one...

void loremIpsum(unsigned char minWords, unsigned char maxWords,
		unsigned char minSentences, unsigned char maxSentences,
		unsigned char numParagraphs, char *output);

#endif /* FLATMSG_H */

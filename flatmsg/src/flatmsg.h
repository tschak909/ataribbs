/**
 * flatmsg.c - routines to implement a flat message board.
 */

#ifndef FLATMSG_H
#define FLATMSG_H


// There isn't a message body entry struct. The message body file consists of 
// repeated sets of:
//
// 2 bytes: length (65535 bytes max message size. If it needs to be longer, um.. no.)
// X bytes: message body
// ...
// And the next one...

#endif /* FLATMSG_H */

/**
 * waitforcall.h - functions used to wait for call.
 */

#ifndef WAITFORCALL_H
#define WAITFORCALL_H

#define WAITFORCALL_NOKEY 0
#define WAITFORCALL_EXIT 0x18
#define WAITFORCALL_CONNECTED 0xff
#define WAITFORCALL_NO_CARRIER 0xfe

unsigned char waitforcall();
void waitforcall_check_console_switches();
unsigned char waitforcall_check_keyboard();
unsigned char waitforcall_answer();

#endif /* WAITFORCALL_H */

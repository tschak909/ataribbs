/**
 * waitforcall.h - functions used to wait for call.
 */

#ifndef WAITFORCALL_H
#define WAITFORCALL_H

#define WAITFORCALL_CONNECTED 3
#define WAITFORCALL_NO_CARRIER 4

unsigned char waitforcall();
void waitforcall_check_console_switches();
void waitforcall_check_keyboard();
unsigned char waitforcall_answer();

#endif /* WAITFORCALL_H */

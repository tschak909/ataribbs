/**
 * terminal.h - Functions to deal with RS232 or potentially other endpoints (Dragon Cart?)
 */

#ifndef TERMINAL_H
#define TERMINAL_H

int terminal_init();
int terminal_done();
int terminal_driver_open();
int terminal_sanity_check();
int terminal_init_modem();

char terminal_send(const char* sendString, char willEcho);
char terminal_send_and_expect_response(const char* sendString, const char* recvString);

#endif

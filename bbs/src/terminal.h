/**
 * terminal.h - Functions to deal with RS232 or potentially other endpoints (Dragon Cart?)
 */

#ifndef TERMINAL_H
#define TERMINAL_H

unsigned char terminal_init();
unsigned char terminal_done();
unsigned char terminal_driver_open();
unsigned char terminal_sanity_check();
unsigned char terminal_init_modem();

unsigned char terminal_send(const char* sendString, char willEcho);
unsigned char terminal_send_and_expect_response(const char* sendString, const char* recvString);

#endif

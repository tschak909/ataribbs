/**
 * terminal.h - Functions to deal with RS232 or potentially other endpoints (Dragon Cart?)
 */

#ifndef TERMINAL_H
#define TERMINAL_H

unsigned char terminal_init();
unsigned char terminal_done();
unsigned char terminal_driver_open();
unsigned char terminal_open_port();
unsigned char terminal_close_port();
unsigned char terminal_sanity_check();
unsigned char terminal_init_modem();
unsigned char terminal_send(const char* sendString, unsigned char willEcho);
unsigned char terminal_send_and_expect_response(const char* sendString, const char* recvString, unsigned char echoSend);
void terminal_flush();
void terminal_hang_up();
unsigned char terminal_get_char();
unsigned char terminal_get_char_chat(char* username);
void terminal_clear_chat();
unsigned char terminal_get_and_echo(unsigned char i, unsigned char j, unsigned char size, unsigned char rubout);
unsigned char terminal_get_and_echo_char(unsigned char i, unsigned char size, unsigned char c, unsigned char rubout);
void terminal_send_eol();
void terminal_determine_eol();
unsigned char terminal_is_an_eol(unsigned char c);
void terminal_send_char(char c);
void terminal_send_file(const char* filename);
void terminal_send_screen(const char* filename);
void terminal_send_up();
void terminal_send_down();
void terminal_send_left();
void terminal_send_right();
void terminal_send_clear_screen();
void terminal_send_pagination_prompt();
void terminal_beep();
void terminal_reset_line_counter();
unsigned char terminal_get_line_counter();
void terminal_enable_line_counter();
void terminal_disable_line_counter();

#define TERMINAL_PORT_OPEN 0
#define TERMINAL_PORT_CLOSED 1

#define TERMINAL_TYPE_ASCII 0x0D
#define TERMINAL_TYPE_ATASCII 0x9B

#endif

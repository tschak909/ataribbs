/**
 * input.h - slightly higher level input routines.
 */

#ifndef INPUT_H
#define INPUT_H

char* input_line_and_echo(unsigned char rubout,unsigned char size);
char* input_line_and_echo_char(unsigned char e,unsigned char rubout, unsigned char size);
char* prompt_line(unsigned char type, unsigned char size);

#endif /* INPUT_H */

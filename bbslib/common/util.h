/**
 * util.h - Utility functions.
 */

#ifndef UTIL_H
#define UTIL_H

#define LOG_LEVEL_DEBUG 0
#define LOG_LEVEL_NOTICE 1
#define LOG_LEVEL_WARNING 2
#define LOG_LEVEL_CRITICAL 3
#define LOG_LEVEL_EMERGENCY 4

void putasciichar(char c);
void fatal_error(const char* msg);
void log(char level, const char* msg);
const char* _log_level(char level);
const char* _log_date_time();
unsigned char is_a_backspace(unsigned char c);
unsigned char is_a_return(unsigned char c);
unsigned char is_an_ascii_cr(unsigned char c);
unsigned char is_an_atascii_eol(unsigned char c);
unsigned short crc16(unsigned char* data_p, unsigned char length);

#endif /* UTIL_H */

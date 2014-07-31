/**
 * config.h - configuration file code
 */

#ifndef CONFIG_H
#define CONFIG_H
#define FILE_BBS_CONFIG "D1:BBS.CNF"
#include <stdio.h>

typedef union 
{
  struct 
  {
    unsigned int printer_use : 1;
    unsigned int printer_log : 1;
    unsigned int printer_bbs_output : 1;
  } pfbits;
  unsigned int printer_flags;
} PrinterFlags;

typedef union
{
  struct
  {
    unsigned int serial_port_baud : 5;
    unsigned int serial_port_data_bits : 2;
    unsigned int serial_port_stop_bits : 1;
    unsigned int serial_port_parity : 3;
    unsigned int serial_handshake_mode : 2;
  } scbits;
  unsigned int serial_port_flags;
} SerialPortFlags;

typedef struct
{
  char init_string[48];
  char ring_string[8];
  char connect_string[16];
} ModemStrings;

unsigned char config_init();
unsigned char config_save();
unsigned char config_load();
void config_done();

#endif /* CONFIG_H */

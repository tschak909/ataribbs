/**
 * config.c - Configuration functions
 */

#include "config.h"
#include "util.h"
#include <serial.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

extern PrinterFlags *config_printflags;
extern SerialPortFlags *config_serialportflags;
extern ModemStrings *config_modemstrings;

unsigned char config_init()
{
  if (!(config_printflags = calloc(1,sizeof(PrinterFlags))))
    {
      fatal_error("Could not allocate memory for printer flags.");
      return 1;
    }

  if (!(config_serialportflags = calloc(1,sizeof(SerialPortFlags))))
    {
      fatal_error("Could not allocate memory for serial port settings.");
      return 1;
    }

  if (!(config_modemstrings = calloc(1,sizeof(ModemStrings))))
    {
      fatal_error("Could not allocate memory for modem strings.");
      return 1;
    }
}

void config_done()
{

  if (config_printflags)
    free(config_printflags);

  if (config_serialportflags)
    free(config_serialportflags);

  if (config_modemstrings)
    free(config_modemstrings);

}

unsigned char config_save()
{
  int configfd;
  configfd = open(FILE_BBS_CONFIG,O_CREAT|O_RDWR);
  /**
   * Write printer flags
   */
  if (write(configfd,(PrinterFlags *)config_printflags,sizeof(PrinterFlags)) != sizeof(PrinterFlags))
    {
      fatal_error("Could not write printer flags to " FILE_BBS_CONFIG " - Disk full? ");
      return 1;
    }
  if (write(configfd,(SerialPortFlags *)config_serialportflags,sizeof(SerialPortFlags)) != sizeof(SerialPortFlags))
    {
      fatal_error("Could not write serial port flags to " FILE_BBS_CONFIG " - Disk full? ");
      return 1;
    }
  if (write(configfd,(ModemStrings *)config_modemstrings,sizeof(ModemStrings)) != sizeof(ModemStrings))
    {
      fatal_error("Could not write modem strings to " FILE_BBS_CONFIG " - Disk full? ");
      return 1;
    }
  close(configfd);

  return 0;
}

unsigned char config_load()
{
  int configfd;
  configfd = open(FILE_BBS_CONFIG,O_RDONLY);
  if (read(configfd,(PrinterFlags *)config_printflags,sizeof(PrinterFlags)) != sizeof(PrinterFlags))
    {
      fatal_error("Could not read printer values from configuration file. File may be truncated.");
      return 1;
    }
  if (read(configfd,(SerialPortFlags *)config_serialportflags,sizeof(SerialPortFlags)) != sizeof(SerialPortFlags))
    {
      fatal_error("Could not read serial port values from config file. File may be truncated");
      return 1;
    }
  if (read(configfd,(ModemStrings *)config_modemstrings,sizeof(ModemStrings)) != sizeof(ModemStrings))
    {
      fatal_error("Could not read modem strings from config file. File may be truncated");
      return 1;
    }

#ifdef CONFIG_TEST
  printf("Configuration values:\n");
  printf("---------------------\n");
  printf("Printer: Use Printer: %d\n",config_printflags->pfbits.printer_use);
  printf("Printer: Use for Logging: %d\n",config_printflags->pfbits.printer_log);
  printf("Printer: Use for BBS call output: %d\n",config_printflags->pfbits.printer_bbs_output);
  printf("\n\n");
  printf("Serial port: Baud: 0x%x\n",config_serialportflags->scbits.serial_port_baud);
  printf("Serial port: Data Bits 0x%x\n",config_serialportflags->scbits.serial_port_data_bits);
  printf("Serial port: Parity: 0x%x\n",config_serialportflags->scbits.serial_port_parity);
  printf("Serial port: Handshake mode: 0x%x\n",config_serialportflags->scbits.serial_handshake_mode);
  printf("\n\n");
  printf("Modem Strings: Init String: %s\n",config_modemstrings->init_string);
  printf("Modem Strings: Ring String: %s\n",config_modemstrings->ring_string);
  printf("Modem Strings: Answer String: %s\n",config_modemstrings->answer_string);
  printf("Modem Strings: Connect String: %s\n",config_modemstrings->connect_string);
  printf("Modem Strings: Hung-up String: %s\n",config_modemstrings->hungup_string);
  printf("\n\n");
#endif CONFIG_TEST

  close(configfd);

  return 0;
}

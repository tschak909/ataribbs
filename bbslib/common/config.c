/**
 * config.c - Configuration functions
 */

#include "config.h"
#include "util.h"
#include <serial.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern PrinterFlags *config_printflags;
extern SerialPortFlags *config_serialportflags;
extern ModemStrings *config_modemstrings;

unsigned char config_init()
{
  if (!(config_printflags = malloc(sizeof(PrinterFlags))))
    {
      fatal_error("Could not allocate memory for printer flags.");
      return 1;
    }

  if (!(config_serialportflags = malloc(sizeof(SerialPortFlags))))
    {
      fatal_error("Could not allocate memory for serial port settings.");
      return 1;
    }

  if (!(config_modemstrings = malloc(sizeof(ModemStrings))))
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
  FILE* pFile;
  printf("config_save()\n");
#ifdef CONFIG_TEST
  config_printflags->printer_flags = 0;
  config_printflags->pfbits.printer_use = 1;
  config_printflags->pfbits.printer_log = 1;
  config_printflags->pfbits.printer_bbs_output = 1;

  config_serialportflags->serial_port_flags = 0;
  config_serialportflags->scbits.serial_port_baud = SER_BAUD_56_875; // 115200 with extended codes.
  config_serialportflags->scbits.serial_port_data_bits = SER_BITS_8;
  config_serialportflags->scbits.serial_port_stop_bits = SER_STOP_1;
  config_serialportflags->scbits.serial_port_parity = SER_PAR_NONE;
  config_serialportflags->scbits.serial_handshake_mode = SER_HS_HW;

  strcpy(config_modemstrings->init_string,"ATZ\r");
  strcpy(config_modemstrings->ring_string,"RING");
  strcpy(config_modemstrings->answer_string,"ATA\r");
  strcpy(config_modemstrings->connect_string,"CONNECT 115200");
  strcpy(config_modemstrings->hungup_string,"NO CARRIER");

#endif
  pFile = fopen(FILE_BBS_CONFIG,"w+");
  /**
   * Write printer flags
   */
  if (!pFile)
    {
      fatal_error("Could not open " FILE_BBS_CONFIG " for writing.\n ");
      return 1;
    }
  if (fwrite((unsigned int *)config_printflags->printer_flags,sizeof(char),1,pFile) != 1)
    {
      fatal_error("Could not write printer flags to " FILE_BBS_CONFIG " - Disk full? ");
      return 1;
    }
  if (fwrite((unsigned int *)config_serialportflags->serial_port_flags,sizeof(unsigned int),1,pFile) != 1)
    {
      fatal_error("Could not write serial port flags to " FILE_BBS_CONFIG " - Disk full? ");
      return 1;
    }
  if (fwrite((ModemStrings *)config_modemstrings,sizeof(ModemStrings),1,pFile) != 1)
    {
      fatal_error("Could not write modem strings to " FILE_BBS_CONFIG " - Disk full? ");
      return 1;
    }
  fclose(pFile);

  return 0;
}

unsigned char config_load()
{
  FILE *pFile;
  pFile = fopen(FILE_BBS_CONFIG,"r");
  if (!pFile)
    {
      fatal_error("Could not open " FILE_BBS_CONFIG " for reading.\n");
      return 1;
    }
  if (fread((unsigned int *)config_printflags->printer_flags,sizeof(char),1,pFile) < 1)
    {
      fatal_error("Could not read printer values from configuration file. File may be truncated.");
      return 1;
    }
  if (fread((unsigned int *)config_serialportflags->serial_port_flags,sizeof(unsigned int),1,pFile) < 1)
    {
      fatal_error("Could not read serial port values from config file. File may be truncated");
      return 1;
    }
  if (fread((ModemStrings *)config_modemstrings,sizeof(ModemStrings),1,pFile) < 1)
    {
      fatal_error("Could not read modem strings from config file. File may be truncated");
      return 1;
    }

#ifdef CONFIG_TEST
  /*  printf("Configuration values:\n");
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
  printf("\n\n"); */
#endif CONFIG_TEST

  return 0;
}

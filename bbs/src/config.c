/**
 * config.c - Configuration functions
 */

#include "config.h"
#include "util.h"
#include "globals.h"
#include <serial.h>
#include <stdio.h>

#define CONFIG_TEST 1

int config_save()
{
  FILE* pFile;
  printf("config_save()\n");
#ifdef CONFIG_TEST
  // Temporary code
  config_printflags.printer_flags = 0;
  config_printflags.pfbits.printer_use = 1;
  config_printflags.pfbits.printer_log = 1;
  config_printflags.pfbits.printer_bbs_output = 1;

  config_serialportflags.serial_port_flags = 0;
  config_serialportflags.scbits.serial_port_baud = SER_BAUD_19200;
  config_serialportflags.scbits.serial_port_data_bits = SER_BITS_8;
  config_serialportflags.scbits.serial_port_stop_bits = SER_STOP_1;
  config_serialportflags.scbits.serial_port_parity = SER_PAR_NONE;
  config_serialportflags.scbits.serial_handshake_mode = SER_HS_HW;

  // End Temporary code
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
  if (fwrite(&config_printflags.printer_flags,sizeof(char),1,pFile) != 1)
    {
      fatal_error("Could not write printer flags to " FILE_BBS_CONFIG " - Disk full? ");
      return 1;
    }
  if (fwrite(&config_serialportflags.serial_port_flags,sizeof(unsigned int),1,pFile) != 1)
    {
      fatal_error("Could not write serial port flags to " FILE_BBS_CONFIG " - Disk full? ");
      return 1;
    }
  fclose(pFile);

  return 0;
}

int config_load()
{
  FILE *pFile;
  printf("config_load()\n");
  pFile = fopen(FILE_BBS_CONFIG,"r");
  if (!pFile)
    {
      fatal_error("Could not open " FILE_BBS_CONFIG " for reading.\n");
      return 1;
    }
  if (fread(&config_printflags.printer_flags,sizeof(char),1,pFile) < 1)
    {
      fatal_error("Could not read printer values from configuration file. File may be truncated.");
      return 1;
    }
  if (fread(&config_serialportflags.serial_port_flags,sizeof(unsigned int),1,pFile) < 1)
    {
      fatal_error("Could not read serial port values from config file. File may be truncated");
      return 1;
    }

#ifdef CONFIG_TEST
  printf("Configuration values:\n");
  printf("---------------------\n");
  printf("Printer: Use Printer: %d\n",config_printflags.pfbits.printer_use);
  printf("Printer: Use for Logging: %d\n",config_printflags.pfbits.printer_log);
  printf("Printer: Use for BBS call output: %d\n",config_printflags.pfbits.printer_bbs_output);
  printf("\n\n");
  printf("Serial port: Baud: 0x%x\n",config_serialportflags.scbits.serial_port_baud);
  printf("Serial port: Data Bits 0x%x\n",config_serialportflags.scbits.serial_port_data_bits);
  printf("Serial port: Parity: 0x%x\n",config_serialportflags.scbits.serial_port_parity);
  printf("Serial port: Handshake mode: 0x%x\n",config_serialportflags.scbits.serial_handshake_mode);
  printf("\n\n");
#endif CONFIG_TEST

  return 0;
}

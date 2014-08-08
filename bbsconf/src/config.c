/**
 * config.c - Configuration functions
 */

#include "config.h"
#include <serial.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern PrinterFlags *config_printflags;
extern SerialPortFlags *config_serialportflags;
extern ModemStrings *config_modemstrings;

unsigned char config_save()
{
  FILE* pFile;
  pFile = fopen(FILE_BBS_CONFIG,"w+");
  /**
   * Write printer flags
   */
  if (!pFile)
    {
      perror("Could not open " FILE_BBS_CONFIG " for writing.\n ");
      return 1;
    }
  if (fwrite((unsigned int *)config_printflags->printer_flags,sizeof(char),1,pFile) != 1)
    {
      perror("Could not write printer flags to " FILE_BBS_CONFIG " - Disk full? ");
      return 1;
    }
  if (fwrite((unsigned int *)config_serialportflags->serial_port_flags,sizeof(unsigned int),1,pFile) != 1)
    {
      perror("Could not write serial port flags to " FILE_BBS_CONFIG " - Disk full? ");
      return 1;
    }
  if (fwrite((ModemStrings *)config_modemstrings,sizeof(ModemStrings),1,pFile) != 1)
    {
      perror("Could not write modem strings to " FILE_BBS_CONFIG " - Disk full? ");
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
      perror("Could not open " FILE_BBS_CONFIG " for reading.\n");
      return 1;
    }
  if (fread((unsigned int *)config_printflags->printer_flags,sizeof(char),1,pFile) < 1)
    {
      perror("Could not read printer values from configuration file. File may be truncated.");
      return 1;
    }
  if (fread((unsigned int *)config_serialportflags->serial_port_flags,sizeof(unsigned int),1,pFile) < 1)
    {
      perror("Could not read serial port values from config file. File may be truncated");
      return 1;
    }
  if (fread((ModemStrings *)config_modemstrings,sizeof(ModemStrings),1,pFile) < 1)
    {
      perror("Could not read modem strings from config file. File may be truncated");
      return 1;
    }

  return 0;
}

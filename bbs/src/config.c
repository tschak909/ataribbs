/**
 * config.c - Configuration functions
 */

#include "config.h"
#include "util.h"
#include <stdio.h>

extern PrinterFlags config_printflags;

int config_save()
{
  FILE* pFile;
#ifdef CONFIG_TEST
  // Temporary code
  config_printflags.printer_flags = 0;
  config_printflags.pfbits.printer_use = 1;
  config_printflags.pfbits.printer_log = 1;
  config_printflags.pfbits.printer_bbs_output = 1;
  // End Temporary code
#endif
  pFile = fopen(FILE_BBS_CONFIG,"w");

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
  fclose(pFile);

  return 0;
}

int config_load()
{
  FILE *pFile;
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

#ifdef CONFIG_TEST
  printf("Configuration values:\n");
  printf("---------------------\n");
  printf("Printer: Use Printer: %d\n",config_printflags.pfbits.printer_use);
  printf("Printer: Use for Logging: %d\n",config_printflags.pfbits.printer_log);
  printf("Printer: Use for BBS call output: %d\n",config_printflags.pfbits.printer_bbs_output);
  printf("\n\n");
#endif CONFIG_TEST

  return 0;
}

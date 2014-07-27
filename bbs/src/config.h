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

int config_save();
int config_load();

#endif /* CONFIG_H */

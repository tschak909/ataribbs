/**
 * mboard.h - functions to handle groups of message boards (MMU)
 */

#ifndef MBOARD_H
#define MBOARD_H

typedef long MMUCursor;
typedef int MMUFile;

typedef struct
{
  char item;
  char itemName[32];
  char itemDescription[64];
  char itemFile[17];
} MMUEntry;

MMUFile mboard_open(const char* file);
void mboard_close(MMUFile file);
MMUCursor mboard_scan_begin(MMUFile file);
MMUCursor mboard_scan_next(MMUFile file,MMUCursor cursor,MMUEntry* entry);
unsigned char mboard_get(MMUFile file, char item, MMUEntry* entry);
unsigned char mboard_get_default(MMUFile file,MMUEntry* entry);
unsigned char mboard_get_num_boards();

#endif

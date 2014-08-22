/**
 * filemenu.h - functions to display a menu of text files.
 */

#ifndef FILEMENU_H
#define FILEMENU_H

typedef struct
{
  char item;
  char itemName[32];
  char itemFile[17];
} FileMenuEntry;

unsigned char filemenu_show(const char* filename);
unsigned char filemenu_item_index(char c);

#endif

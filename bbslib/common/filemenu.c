/**
 * filemenu.c - functions to display a file menu
 */

#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "filemenu.h"
#include "terminal.h"
#include "config.h"
#include "util.h"

unsigned char filemenu_show(const char* filename)
{
  int menufd;
  char itemFileWithExt[13];
  char prompt[40];
  char selected_item;
  unsigned char num_entries;
  unsigned char i;
  FileMenuEntry** menuentries;

  sprintf(itemFileWithExt,"%s.FMU",filename);

  menufd = open(itemFileWithExt,O_RDONLY);
  if (menufd == -1)
    {
      printf("Could not open %s",itemFileWithExt);
      return 0x1b; // as if ESC were pressed.
    }

  read(menufd,&num_entries,sizeof(unsigned char));
  read(menufd,&prompt,40);

  menuentries = calloc(num_entries,sizeof(FileMenuEntry));
  if (!menuentries)
    {
      terminal_send("Could not allocate memory for menu.",0);
      log(LOG_LEVEL_NOTICE,"Could not allocate memory for menu.");
      return 0x1b; // as if ESC were pressed.
    }

  read(menufd,(FileMenuEntry *)menuentries, sizeof(FileMenuEntry)*num_entries);
  close(menufd);

  while (selected_item != 0x1b)
    {
      terminal_send_screen(filename);
      // selected_item = terminal_show_prompt(prompt);
      for (i=0;i<num_entries;++i)
	{
	  if (menuentries[i]->item == selected_item)
	    {
	      // terminal_show_selection(menuentries[i]->itemName);
	      free(menuentries);
	      return selected_item;
	    }
	}
      if (selected_item != 0x1b)
	{
	  // terminal_beep();
	}
    }
}

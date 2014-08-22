/**
 * fmenue.c - File Menu Editor
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <conio.h>
#include <errno.h>
#include <string.h>
#include <bbslib/common/filemenu.h>

const char* filemenu_items="1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZ"; // 36 entries.

unsigned char filemenu_item_index(char c)
{
  char *e;
  e = strchr(filemenu_items,c);
  return (unsigned char)(e-filemenu_items);
}

const char* stringOption(const char* prompt, const char* defaultOption)
{
  char str[80];
  printf("%s\n(default: %s):\n",prompt,defaultOption);
  fgets(str,80,stdin);
  if (str[0] == 0x9b)
    {
      putchar(0x1c);
      printf("Returning Default option: %s\n\n",defaultOption);
      return strdup(defaultOption);
    }
  else
    {
      char *ret;
      ret = sscanf(str,"%s",ret);
      return strdup(ret);
    }
}

unsigned char yesNoOption(const char* prompt, char defaultOption)
{
  unsigned char c;
  printf("%s (Y/N)? ",prompt);
  while (c=cgetc())
    {
      switch(c)
	{
	case 'Y':
	case 'y':
	  printf("Yes.\n");
	  return 1;
	case 'N':
	case 'n':
	  printf("No.\n");
	  return 0;
	case 0x9B:
	  if (defaultOption=='Y')
	    {
	      printf("Yes.\n");
	      return 1;
	    }
	  else if (defaultOption=='N')
	    {
	      printf("No.\n");
	      return 0;
	    }
	default:
	  printf("%c",0xfd);
	}
    }
}

void fmenue_edit_entry(char item, FileMenuEntry* entry)
{
  
  strcpy(entry->itemFile,stringOption("Entry Filename without extension. e.g. BULLETIN1",entry->itemFile));
  strcpy(entry->itemName,stringOption("Entry Title. e.g. Bulletin #1",entry->itemName));
  entry->item = item;

}

unsigned char fmenue_load(int fd, FileMenuEntry** entries)
{
  unsigned char num_entries;
  FileMenuEntry* entry;

  entry = calloc(1,sizeof(entry));
  num_entries = read(fd,&num_entries,sizeof(unsigned char));

  printf("\nLoading %u menu entries...",num_entries);

  while (read(fd, (FileMenuEntry *)entry,sizeof(FileMenuEntry)) == sizeof(FileMenuEntry))
    {
      entries[filemenu_item_index(entry->item)] = entry;
    }

  printf("ok.\n\n");

  free(entry);

  return num_entries;
}

void fmenue_show_entries(FileMenuEntry** entries)
{
  unsigned char i;

  printf("\f\nSelect a menu item to edit. ^C exits.\n\n");
  for (i=0;i<35;i=i+2)
    {
      printf("[%c] %15s [%c] %15s\n",
	     filemenu_items[i],
	     entries[i]->itemFile,
	     filemenu_items[i+1],
	     entries[i+1]->itemFile);
    }

  printf("\n");

}

int fmenue_edit(int fd)
{
  FileMenuEntry** entries;
  unsigned char num_entries;
  char key;
  unsigned char i;

  entries = calloc(36,sizeof(FileMenuEntry *));

  for (i=0;i<37;++i)
    {
      entries[i]=calloc(1,sizeof(FileMenuEntry));
    }

  if (!entries)
    {
      perror("fmenue_edit()");
      return errno;
    }

  // If seek is greater than 0 bytes, then the file exists. Let's load it.
  if (lseek(fd, 0, SEEK_END) != 0)
    {
      lseek(fd, 0, SEEK_SET);
      num_entries = fmenue_load(fd,entries); // If it doesn't load, no big deal. 
    }

  while (key!=0x03) // ^C Exits.
    {
      fmenue_show_entries(entries);
      printf("Select menu item [_]");
      key=cgetc();
      if (key!=0x03)
	fmenue_edit_entry(key,entries[filemenu_item_index(key)]);
    }

  for (i=0;i<37;++i)
    {
      free(entries[i]);
    }

  free(entries);

  return 0;

}

int fmenue(char* filename)
{
  int fd;
  char prompt[48];

  fd = open(filename,O_RDWR|O_CREAT|O_EXCL);
  if (errno == EEXIST)
    return fmenue_edit(fd);
  else
    {
      sprintf(prompt,"\nFile %s does not exist.\nCreate it?",filename);
      cursor(1);
      if (yesNoOption(prompt,'N') == 0)
	{
	  close(fd);
	  unlink(filename);
	  return 0;
	}
      else
	return fmenue_edit(fd);
    }
}

int show_usage(char* argv[])
{
  printf("\nMenu filename not specified.\n\nUsage: %s <file.fmu>\n\n",argv[0]);
  return 1;
}

int main(int argc, char* argv[])
{
  if (argc!=2)
    return show_usage(argv);
  else
    return fmenue(argv[1]);
}

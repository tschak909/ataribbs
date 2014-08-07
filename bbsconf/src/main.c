#include <stdio.h>
#include <unistd.h>
#include <conio.h>
#include <serial.h>
#include <string.h>

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
	  return c;
	case 'N':
	case 'n':
	  printf("No.\n");
	  return c;
	case 0x9B:
	  printf("Default: %c\n",defaultOption);
	  return defaultOption;
	default:
	  printf("%c",0xfd);
	}
    }
}

unsigned char baudRateOption()
{
  unsigned char c;
  printf("\n");
  printf("Select desired Baud Rate:\n");
  printf("-------------------------\n");
  printf("(A) 300 bps\n");
  printf("(B) 1200 bps\n");
  printf("(C) 2400 bps\n");
  printf("(D) 9600 bps\n");
  printf("(E) 57600 bps (Emulation)\n");
  printf("(F) 115200 bps (Emulation)\n");
  printf("\n");
  printf("Baud Rate (A-F)? ");
  while (c=cgetc())
    {
      switch(c)
	{
	case 'A':
	case 'a':
	  printf("300 bps.");
	  return SER_BAUD_300;
	case 'B':
	case 'b':
	  printf("1200 bps.");
	  return SER_BAUD_1200;
	case 'C':
	case 'c':
	  printf("2400 bps.");
	  return SER_BAUD_2400;
	case 'D':
	case 'd':
	  printf("9600 bps.");
	  return SER_BAUD_9600;
	case 'E':
	case 'e':
	  printf("57600 bps (Emulation).");
	  return SER_BAUD_45_5;
	case 'F':
	case 'f':
	case 0x9b:
	  printf("115200 bps (Emulation).");
	  return SER_BAUD_56_875;
	default:
	  printf("%c",0xfd);
	}
    }
}

unsigned char dataBitsOption()
{
  char c;
  printf("\n");
  printf("Select # of data bits\n");
  printf("---------------------\n");
  printf("(A) 7 bits\n");
  printf("(B) 8 bits (recommended)\n");
  printf("\n");
  printf("Select Desired Data Bits Size (A-D)? ");
  while (c=cgetc())
    {
      switch(c)
	{
	case 'A':
	case 'a':
	  printf("7 bits.\n");
	  return SER_BITS_7;
	case 'B':
	case 'b':
	case 0x9b:
	  printf("8 bits. (recommended)\n");
	  return SER_BITS_8;
	default:
	  printf("%c",0xfd);
	}
    }
}

unsigned char stopBitsOption()
{
  char c;
  printf("\n");
  printf("Select # of stop bits\n");
  printf("---------------------\n");
  printf("(A) 1 stop bit. (Recommended)\n");
  printf("(B) 2 stop bits.\n");
  printf("\n");
  printf("Select # of stop bits (A-B)? ");
  while (c=cgetc())
    {
      switch(c)
	{
	case 'A':
	case 'a':
	case 0x9b:
	  printf("1 stop bit (Recommended)\n");
	  return SER_STOP_1;
	case 'B':
	case 'b':
	  printf("2 stop bits.\n");
	  return SER_STOP_2;
	default:
	  printf("%c",0xfd);
	}
    }
}

unsigned char parityOption()
{
  char c;
  printf("\n");
  printf("Select Parity\n");
  printf("-------------\n");
  printf("(A) None (Recommended)\n");
  printf("(B) Odd\n");
  printf("(C) Even\n");
  printf("(D) Mark\n");
  printf("(E) Space\n");
  printf("\n");
  printf("Select Parity (A-E)? ");
  while (c=cgetc())
    {
      switch(c)
	{
	case 'A':
	case 'a':
	case 0x9b:
	  printf("None.\n");
	  return SER_PAR_NONE;
	case 'B':
	case 'b':
	  printf("Odd.\n");
	  return SER_PAR_ODD;
	case 'C':
	case 'c':
	  printf("Even.\n");
	  return SER_PAR_EVEN;
	case 'D':
	case 'd':
	  printf("Mark.\n");
	  return SER_PAR_MARK;
	case 'E':
	case 'e':
	  printf("Space.\n");
	  return SER_PAR_SPACE;
	default:
	  printf("%c",0xfd);
	}
    }
}

const char* stringOption(const char* prompt, const char* defaultOption)
{
  char str[80];
  printf("%s (default: %s)\n",prompt,defaultOption);
  scanf("%s",str);
  if (str == "\n")
    {
      putchar(0x1c);
      printf("Returning Default option: %s",defaultOption);
      return strdup(defaultOption);
    }
  else
    {
      return strdup(str);
    }
}

void printer_options_header()
{
  printf("\n");
  printf("Printer Options\n");
  printf("---------------\n");
  printf("\n");
}

void modem_strings_header()
{
  printf("\n");
  printf("Modem Strings\n");
  printf("-------------\n");
  printf("\n");
}

void header()
{
//printf("                                        ")
  printf("\n");
  printf("AtariBBS Configurator v0.1\n");
  printf("--------------------------\n");
  printf("\n");
}

unsigned char main(int argc, char* argv[])
{
  cursor(1);
  header();
  printer_options_header();
  yesNoOption("Use printer",'Y');
  yesNoOption("Use for log output",'Y');
  yesNoOption("Use for BBS output",'Y');
  baudRateOption();
  dataBitsOption();
  stopBitsOption();
  parityOption();
  modem_strings_header();
  stringOption("Default Modem Reset String","ATZ\r");
  stringOption("Default Modem Init String","ATZ\r");
  stringOption("Default Modem Answer String","ATA\r");
  stringOption("Default Modem Ring String","RING");
  stringOption("Default Modem Connect String","CONNECT 115200");
  stringOption("Default Modem Hangup String","NO CARRIER");
  return 0;
}

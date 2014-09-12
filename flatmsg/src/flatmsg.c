/**
 * flatmsg.c - routines to implement a flat message board.
 */

#include "flatmsg.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <assert.h>
#include <conio.h>
#include <bbslib/common/config.h>
#include <bbslib/common/util.h>
#include <bbslib/common/types.h>
#include <bbslib/common/msg.h>
#include <bbslib/common/idx.h>
#include <bbslib/common/header.h>

PrinterFlags *config_printflags = NULL;
SerialPortFlags *config_serialportflags = NULL; 
ModemStrings *config_modemstrings = NULL;

void _loremIpsum(unsigned char minWords, unsigned char maxWords,
	       unsigned char minSentences, unsigned char maxSentences,
	       unsigned char numParagraphs, char *output)
{
  const char* words[] = {
    "lorem", "ipsum", "dolor", "sit", "amet", "consectetuer",
    "adipiscing", "elit", "sed", "diam", "nonummy", "nibh", "euismod",
    "tincidunt", "ut", "laoreet", "dolore", "magna", "aliquam", "erat"};

  int numSentences = rand() % (maxSentences-minSentences)+minSentences+1;
  int numWords = rand() % (maxWords-minWords)+minWords+1;
  int p,s,w;

  srand(clock());

  for(p=0; p<numParagraphs;++p) 
    {
      strcat(output,"\n");
      for(s = 0; s < numSentences; s++)
	{
	  for(w=0; w < numWords; w++)
	    {
	      if (w > 0) 
		{
		  strcat(output," "); 
		}
	      strcat(output,words[rand() % 20]);
	    }
	  strcat(output,". ");
	}
      strcat(output,"\n");
    }
}

void _randomName(char* output)
{
  const char* first[] = {
    "THOMAS", "MICHAEL", "DONNA", "GINA", "PETER", "STEPHEN", "LANA", "TROY", "TODD", "ROY", "DANIEL", "SCOTT", "VICTOR", "WILEY", "RICHARD", "GREG", "ANTHONY"
  };

  const char* last[] = {
    "BROWN", "CHERRYHOMES", "BINNEY", "EDWARDS", "TESSEROT", "THOMAS", "VOS", "LEVY", "BENSON", "O'NEAL", "SHEPARDINI", "HARGHIS", "NORTON", "CUMIA", "CASAMENTO" 
  };

  srand(clock());

  strcat(output,first[rand() % 17]);
  strcat(output," ");
  strcat(output,last[rand() % 15]);
}

void test_write(MsgFile* file)
{
  MsgHeader *entry;
  long nummsgs = idx_get_num_msgs(file);
  long i;
  char* name;
  char* body;

  entry = calloc(1,sizeof(MsgHeader));
  name = calloc(1,50);
  body = calloc(1,8192);

  printf("This test appends 255 test messages to\n");
  printf("the MSGTEST board. It generates both\n ");
  printf("the from and body content randomly.\n");
  printf("\n");
  printf("This test takes a while. Turn on Warp\n");
  printf("Speed, or watch TV.\n");
  printf("\n");

  for (i=0;i<255;++i)
    {
      memset(name,0,50);
      memset(body,0,8192);
      _randomName(name);
      strcpy(entry->from,name);
      sprintf(entry->subject,"Test Message %lu",i+nummsgs);
      entry->msgId=0;
      timedate(&entry->stamp);
      _loremIpsum(3,10,2,10,8,body);
      printf("W: #: %lu F: %s L: %u\n",i+nummsgs,name,strlen(body));
      msg_put(file,entry,body);
    }

  free(entry);
  free(name);
  free(body);

  printf("\n");
  printf("Done.");
  printf("\n");

  return;
}

void test_load_all(MsgFile* file)
{
  MsgHeader* header;
  char* body;
  char c=0;
  long nummsgs;
  size_t i;
  clock_t b,e,d;

  nummsgs = idx_get_num_msgs(file);
  header=malloc(sizeof(MsgHeader));
  body=malloc(8192);
  if (!body)
    {
      perror("malloc body failed.");
      return;
    }

  printf("Benchmarking full msg load...");

  b=clock();

  for (i=0;i<nummsgs;++i)
    {
      memset(body,0,8192);
      msg_get(file,i,header,body);
      putchar('.'); 
    }

  e=clock();
  d=(e-b)/CLOCKS_PER_SEC;

  printf("%u msgs. %lu secs.\n",nummsgs,d);

  free(header);
  free(body);

  return;
}

void output_header(MsgHeader* header)
{
  assert(header!=NULL);
  printf("Message #%lu\n",header->msgId);
  printf("Network ID: %lu\n",header->networkId);
  printf("From: %s\n",header->from);
  printf("Subject: %s\n",header->subject);
  printf("\n");
}

void output_body(char *body)
{
  size_t i;
  assert(body!=NULL);
  for (i=0;i<strlen(body);++i)
    {
      putchar(body[i]);
    }
}

void test_show_msg(MsgFile* file)
{
  MsgHeader header;
  char* body;
  char reqMsgNum[16];
  long msgNum;

  body = calloc(1,8192);
  assert(file!=NULL);
  assert(body!=NULL);
  
  printf("Display message in MSGTEST board.\n");
  printf("<RETURN> exits.\n\n");

  reqMsgNum[0]=0x0;

  while (reqMsgNum[0] != 0x9B)
    {
      printf("Message #? ");
      fgets(reqMsgNum,16,stdin);
      if (reqMsgNum[0] == 0x9B)
	break;
      msgNum = atol(reqMsgNum);
      msg_get(file,msgNum,&header,body);
      output_header(&header);
      output_body(body);
    }
  
  free(body);
}

void test_show_headers(MsgFile* file)
{
  MsgHeader header;
  long nummsgs = idx_get_num_msgs(file);
  long i;
  HeaderCursor cursor;
  clock_t b,e,d;

  printf("Benchmarking header scan of %lu msgs...\n",nummsgs);

  b=clock();

  for (i=0;i<nummsgs;++i)
    {
      cursor=header_scan_next(file,cursor,&header);
      putchar('.');
    }

  e=clock();
  d=(e-b) / CLOCKS_PER_SEC;

  printf("%lu secs\n",d);

  cursor=header_scan_begin(file,0);

  for (i=0;i<nummsgs;++i)
    {
      cursor=header_scan_next(file,cursor,&header);
      printf("ID: %lu FROM: %s\n",header.msgId,header.from);
    }

}

void test_search_from(MsgFile* file)
{
  char from[64];
  MsgHeader header;
  HeaderCursor cursor;
  long nummsgs = idx_get_num_msgs(file);
  clock_t b,e,d;
  assert(file!=NULL);
  from[0]=0x0;
  while (from[0]!=0x9b)
    {
      printf("\n \nEnter from value to search \n <RETURN> exits:");
      fgets(from,64,stdin);
      if (from[0]==0x9b)
	break;
      from[strlen(from)-1]='\0';  // strip away the $@#($#@ newline.
      b=clock();
      cursor = header_scan_begin(file,0);
      while(cursor!=-1)
	{
	  cursor=header_scan_find_from(file,cursor,from,&header);
	  if (cursor!=-1)
	    printf("ID #: %lu - FROM: %s\n",header.msgId,header.from);
	}
      e=clock();
      d=(e-b)/CLOCKS_PER_SEC;
      printf("\n \n %lu msgs searched in %lu secs\n",nummsgs,d);
    }

}

void test_search_subject(MsgFile* file)
{
  char subject[64];
  MsgHeader header;
  HeaderCursor cursor;
  long nummsgs = idx_get_num_msgs(file);
  clock_t b,e,d;
  assert(file!=NULL);
  subject[0]=0x0;
  while (subject[0]!=0x9b)
    {
      printf("\n \nEnter subject value to search \n <RETURN> exits:");
      fgets(subject,64,stdin);
      if (subject[0]==0x9b)
	break;
      subject[strlen(subject)-1]='\0';  // strip away the $@#($#@ newline.
      b=clock();
      cursor = header_scan_begin(file,0);
      while(cursor!=-1)
	{
	  cursor=header_scan_find_subject(file,cursor,subject,&header);
	  if (cursor!=-1)
	    printf("ID #: %lu - FROM: %s\n",header.msgId,header.from);
	}
      e=clock();
      d=(e-b)/CLOCKS_PER_SEC;
      printf("\n \n %lu msgs searched in %lu secs\n",nummsgs,d);
    }
}

void test_show_index(MsgFile* file)
{
  long i=0;
  char c=0;
  long nummsgs=idx_get_num_msgs(file);
  MsgIdxEntry idx;
  clock_t b,e,d;
  
  printf("Benchmarking index read");
  
  b=clock();
  for (i=0;i<nummsgs;++i)
    {
      lseek(file->idxfd,idx_offset(i),SEEK_SET);
      read(file->idxfd,&idx,sizeof(MsgIdxEntry));
      putchar('.');
    }
  
  e=clock();
  d=(e-b)/CLOCKS_PER_SEC;

  printf("Read %lu msgs in %lu secs.\n\n",nummsgs,d);
  
  printf("Press any key to show idx dump.");
  c=cgetc();

  for (i=0;i<nummsgs;++i)
    {
      lseek(file->idxfd,idx_offset(i),SEEK_SET);
      read(file->idxfd,&idx,sizeof(MsgIdxEntry));
      printf("Msg ID #%lu\n",idx.msgId);
      printf("Header offset: %lu\n",idx.hdrOffset);
      printf("Body offset: %lu\n",idx.bodyOffset);
      printf("\n");
    }

}

int main(int argc, char* argv[])
{
  char c=0;
  MsgFile* file = msg_open("D1:MSGTEST");
  
  printf("\n");
  printf("For these tests, D1:MSGTEST will be\n");
  printf("created. If it does not exist, otherwise\n");
  printf("it will be loaded as an existing board.\n");
  printf("Please ensure that the target disk has\n");
  printf("at least 1 megabyte of free space left.\n");

  while (c!=0x1b)
    {       
      printf("\n \n %s - test harness\n \n",argv[0]);
      printf("(1) msg: Write MSGTEST board\n");
      printf("(2) msg: Load all msgs.\n");
      printf("(3) msg: Show one msg.\n");
      printf("(4) hdr: Show All Headers\n");
      printf("(5) hdr: Search by from\n");
      printf("(6) hdr: Search by subject\n");
      printf("(7) idx: show index record\n");
      
      printf("(ESC) Exit.\n \n");
      
      printf("Which test? ");
      
      cursor(1);
    g:      c=cgetc();

      switch(c)
	{
	case '1':
	  printf("Write MSGTEST.\n");
	  test_write(file);
	  break;
	case '2':
	  printf("Load All Msgs.\n");
	  test_load_all(file);
	  break;
	case '3':
	  printf("Show one msg.\n");
	  test_show_msg(file);
	  break;
	case '4':
	  printf("Show all headers.\n");
	  test_show_headers(file);
	  break;
	case '5':
	  printf("Search by from\n");
	  test_search_from(file);
	  break;
	case '6':
	  printf("Search by subject.\n");
	  test_search_subject(file);
	  break;
	case '7':
	  printf("Show index record\n");
	  test_show_index(file);
	  break;
	case 0x1b:
	  printf("Exiting.\n");
	  msg_close(file);
	  return 0;
	  break;
	default:
	  putchar(0xfd);
	  goto g;
	}
    }

  msg_close(file);

  return 0;

}

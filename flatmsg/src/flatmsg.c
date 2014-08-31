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

int main(int argc, char* argv[])
{
  /*
    char* output;
    output = calloc(1,8192);
    loremIpsum(3,10,2,10,4,output);
    printf("%s",output);
  */

  /*
  char* output;
  char i=0;

  output=calloc(1,50);

  for (i=0;i<20;++i)
    {
      memset(output,0,50);

      randomName(output);
      printf("%s \n",output);
    }

  free(output);
  */

  /*
  MsgFile *file;
  MsgHeader *entry;
  unsigned char i;
  char* name;
  char* body;

  file = msg_open("D1:MSGTEST");

  entry = calloc(1,sizeof(MsgHeader));
  name = calloc(1,50);
  body = calloc(1,8192);

  for (i=0;i<255;++i)
    {
      memset(name,0,50);
      memset(body,0,8192);
      _randomName(name);
      strcpy(entry->from,name);
      sprintf(entry->subject,"Test Message %u",i);
      entry->msgId=0;
      _loremIpsum(3,10,2,10,8,body);
      printf("W: #: %u F: %s L: %u\n",i,name,strlen(body));
      msg_put(file,entry,body);
    }

  msg_close(file);

  return 0;
  */
  
  /* MsgFile* file;
  MsgHeader* header;
  char* body;
  long nummsgs;
  size_t i;
  clock_t b,e,d;

  file = msg_open("D1:MSGTEST");
  nummsgs = _get_num_msgs(file);
  header=malloc(sizeof(MsgHeader));
  body=malloc(8192);
  if (!body)
    {
      perror("malloc body failed.");
      return 1;
    }

  printf("Loading all messages...");

  b = clock();

  for (i=0;i<nummsgs;++i)
    {
      // memset(body,0,8192);
      msg_get(file,i,header,body);
      
      printf("\n\n");
      printf("Message #%lu of %lu\n",header->msgId,nummsgs);
      printf("Network ID: %lu\n",header->networkId);
      printf("From: %s\n",header->from);
      printf("Subject: %s\n",header->subject);
      printf("Body length: %u",strlen(body));
      printf("\n\n");
    }

  e = clock();
  d = (e-b) / CLOCKS_PER_SEC;

  printf("time spent: %lu sec.\n",d);

  free(header);
  free(body);

  msg_close(file);
  
  return 0;
  */

  /*MsgFile *file;
  MsgIdxEntry* idx;
  long nummsgs;
  long i;
  size_t abr;
  
  idx = calloc(1,sizeof(MsgIdxEntry));

  file = msg_open("D1:MSGTEST");

  abr = read(file->idxfd,&nummsgs,sizeof(long));

  printf("abr: %u\n",abr);

  printf("%lu total messages in index.\n",nummsgs);

  for (i=0;i<2;++i)
    {
      abr = read(file->idxfd,(MsgIdxEntry *)idx,sizeof(MsgIdxEntry));
      printf("abr: %u\n",abr);
      printf("Message %lu of %lu\n",i,nummsgs);
      printf("Msg ID: %lu\n",idx->msgId);
      printf("Header Offset: %lu\n",idx->hdrOffset);
      printf("Body Offset: %lu\n",idx->bodyOffset);
      printf("\n\n");
    }
  
  free(idx);
  msg_close(file);

  return 0;
 
  */

  /*
  MsgFile* file = msg_open("D1:MSGTEST");
  long nummsgs = _get_num_msgs(file);
  HeaderCursor cursor = header_scan_begin(file,0);
  MsgHeader header;
  unsigned char i;
  clock_t b, e, d;
  
  printf("Doing quickscan of all messages...");

  b=clock();

  while ((cursor=header_scan_find_network_id(file,cursor,0,&header)) != -1)
    {
      printf("M#: %lu - From: %s",header.msgId,header.from);
    }

  header_scan_end(file);

  e=clock();

  d=(e-b) / CLOCKS_PER_SEC;

  printf("quickscan took: %lu secs.\n",d);

  return 0;
  */

  return 0;

}

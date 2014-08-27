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

MsgFile* msg_open(const char* msgfile)
{
  char idxpath[32];
  char msgpath[32];
  MsgFile *file;

  file = calloc(1,sizeof(MsgFile));

  strcpy(idxpath,msgfile);
  strcpy(msgpath,msgfile);
  strcat(idxpath,".IDX");
  strcat(msgpath,".MSG");

  file->idxfd = open(idxpath,O_CREAT|O_RDWR);
  file->msgfd = open(msgpath,O_CREAT|O_RDWR);
  
  if (file->idxfd < 0)
    return NULL;

  if (file->msgfd < 0)
    return NULL;

  return file;

}

void msg_close(MsgFile* file)
{

  assert(file!=NULL);
  assert(file->msgfd > 0);
  assert(file->idxfd > 0);

  close(file->msgfd);
  close(file->idxfd);

  free(file);
}

unsigned char msg_put(MsgFile* file, MsgIDXEntry* entry, char* body)
{

  assert(file!=NULL);
  assert(entry!=NULL);
  assert(entry->msgId==0);
   
  return 0;

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

  MsgFile *file;

  file = msg_open("D1:MSGTEST");
  msg_close(file);

  return 0;
}

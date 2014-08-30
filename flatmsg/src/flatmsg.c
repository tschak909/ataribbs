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
  char hdrpath[32];
  char msgpath[32];
  char idxpath[32];
  MsgFile *file;

  file = calloc(1,sizeof(MsgFile));

  strcpy(hdrpath,msgfile);
  strcpy(msgpath,msgfile);
  strcpy(idxpath,msgfile);
  strcat(hdrpath,".HDR");
  strcat(msgpath,".MSG");
  strcat(idxpath,".IDX");

  file->hdrfd = open(hdrpath,O_CREAT|O_RDWR);
  file->msgfd = open(msgpath,O_CREAT|O_RDWR);
  file->idxfd = open(idxpath,O_CREAT|O_RDWR);
  
  if (file->hdrfd < 0)
    return NULL;

  if (file->msgfd < 0)
    return NULL;

  if (file->idxfd < 0)
    return NULL;

  return file;

}

void msg_close(MsgFile* file)
{
  assert(file!=NULL);
  assert(file->msgfd > 0);
  assert(file->hdrfd > 0);
  assert(file->idxfd > 0);

  close(file->msgfd);
  close(file->hdrfd);
  close(file->idxfd);

  free(file);
}

long _put_num_msgs(MsgFile* file, long nummsgs)
{
  assert(file!=NULL);
  lseek(file->idxfd,0,SEEK_SET);
  write(file->idxfd,&nummsgs,sizeof(long));
  return nummsgs;
}

long _get_num_msgs(MsgFile* file)
{
  long curPos;
  long nummsgs;

  assert(file!=NULL);
  
  curPos=lseek(file->idxfd,0,SEEK_END);
  
  if (curPos!=0)
    {
      lseek(file->idxfd,0,SEEK_SET);
      assert(read(file->idxfd,&nummsgs,sizeof(long)) == sizeof(long));
      lseek(file->idxfd,0,SEEK_END);
    }
  else
    {
      nummsgs = _put_num_msgs(file,0);
    }

  return nummsgs;
}

unsigned char msg_put(MsgFile* file, MsgHeader* entry, char* body)
{
  long nummsgs;
  long msgcurPos,hdrcurPos,idxcurPos;
  size_t bodylen;
  MsgIdxEntry* idx;

  assert(file!=NULL);
  assert(entry!=NULL);
  assert(body!=NULL);
  assert(entry->msgId==0);
 
  idx = calloc(1,sizeof(MsgHeader));

  nummsgs = _get_num_msgs(file);
  nummsgs++;
  hdrcurPos = lseek(file->hdrfd,SEEK_END,0);
  msgcurPos = lseek(file->msgfd,SEEK_END,0);
  idxcurPos = lseek(file->idxfd,SEEK_END,0);

  idx->msgId = nummsgs;
  idx->bodyOffset = msgcurPos;
  idx->hdrOffset = hdrcurPos;
  
  entry->msgId = nummsgs;

  bodylen=strlen(body);

  write(file->hdrfd,(MsgHeader *)entry,sizeof(MsgHeader));
  write(file->msgfd,&bodylen,sizeof(size_t));
  write(file->msgfd,(char *)body,strlen(body));  
  write(file->idxfd,(MsgIdxEntry *)idx,sizeof(MsgIdxEntry));
  _put_num_msgs(file,nummsgs);

  free(idx);

  return 0;

}

void _msg_rewind(MsgFile* file)
{
  assert(file!=NULL);

  lseek(file->msgfd,0,SEEK_SET);
  lseek(file->hdrfd,0,SEEK_SET);
  lseek(file->idxfd,0,SEEK_SET);
}

unsigned char msg_get(MsgFile* file, long msgId, MsgHeader* header, char* body)
{
  long nummsgs;
  size_t bodySize;
  MsgIdxEntry idx;

  assert(file!=NULL);

  nummsgs = _get_num_msgs(file);

  _msg_rewind(file);

  assert(msgId < nummsgs);

  lseek(file->idxfd,sizeof(MsgIdxEntry)*msgId,SEEK_SET);
  read(file->idxfd,&idx,sizeof(MsgIdxEntry));
  lseek(file->hdrfd,idx.hdrOffset,SEEK_SET);
  assert(header!=NULL);
  read(file->hdrfd,(MsgHeader *)header,sizeof(MsgHeader));

  lseek(file->msgfd,idx.bodyOffset,SEEK_SET);
  read(file->msgfd,&bodySize,sizeof(size_t));
  assert(body!=NULL);
  read(file->msgfd,(char *)body,bodySize);
  
  return msgId;

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
  MsgHeader *entry;
  unsigned char i;
  char* name;
  char* body;

  file = msg_open("D1:MSGTEST");

  entry = calloc(1,sizeof(MsgHeader));
  name = calloc(1,50);
  body = calloc(1,8192);

  for (i=0;i<2;++i)
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
  

  /* MsgFile* file;
  MsgHeader* header;
  char* body;
  long nummsgs;
  size_t i;

  file = msg_open("D1:MSGTEST");
  nummsgs = _get_num_msgs(file);
  header=malloc(sizeof(MsgHeader));
  body=malloc(8192);
  if (!body)
    {
      perror("malloc body failed.");
      return 1;
    }
  msg_get(file,2,header,body);
  
  printf("\n\n");
  printf("Message #%u of %u\n",header->msgId,nummsgs);
  printf("Network ID: %u\n",header->networkId);
  printf("From: %s\n",header->from);
  printf("Subject: %s\n",header->subject);
  printf("Body length: %u",strlen(body));
  printf("\n\n");

  free(header);
  free(body);

  msg_close(file);
  */

  
  /* MsgFile *file;
  MsgIdxEntry* idx;
  long nummsgs;
  long i;
  
  idx = calloc(1,sizeof(MsgIdxEntry));

  file = msg_open("D1:MSGTEST");

  read(file->idxfd,&nummsgs,sizeof(long));

  printf("%lu total messages in index.\n",nummsgs);

  for (i=0;i<2;++i)
    {
      read(file->idxfd,(MsgIdxEntry *)idx,sizeof(MsgIdxEntry));
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
}

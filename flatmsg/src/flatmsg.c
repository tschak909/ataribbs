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

int _msg_open(const char* msgfile, const char* extension)
{
  char tmp[32];
  strcpy(tmp,msgfile);
  strcat(tmp,extension);

  return open(tmp,O_CREAT|O_RDWR);
}

MsgFile* msg_open(const char* msgfile)
{
  MsgFile *file;

  file = calloc(1,sizeof(MsgFile));

  file->hdrfd = _msg_open(msgfile,".HDR");
  file->msgfd = _msg_open(msgfile,".MSG");
  file->idxfd = _msg_open(msgfile,".IDX");
  
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
    }
  else
    {
      nummsgs = _put_num_msgs(file,0);
    }

  return nummsgs;
}

void _msg_write_idx(MsgFile* file, MsgIdxEntry* idx)
{
  assert(file!=NULL);
  assert(file->idxfd>0);
  assert(idx!=NULL);
  lseek(file->idxfd,0,SEEK_END);
  write(file->idxfd,(MsgIdxEntry *)idx,sizeof(MsgIdxEntry));
}

void _msg_write_body(MsgFile* file, char* body)
{
  size_t len;
  assert(file!=NULL);
  assert(file->msgfd>0);
  assert(body!=NULL);
  len=strlen(body);
  lseek(file->msgfd,0,SEEK_END);
  write(file->msgfd,&len,sizeof(size_t));
  write(file->msgfd,(char *)body,len);
}


void _msg_write_header(MsgFile* file, MsgHeader* header)
{
  assert(file!=NULL);
  assert(file->hdrfd>0);
  assert(header!=NULL);
  lseek(file->hdrfd,0,SEEK_END);
  write(file->hdrfd,(MsgHeader *)header,sizeof(MsgHeader));
}

unsigned char msg_put(MsgFile* file, MsgHeader* header, char* body)
{
  MsgIdxEntry idx;
  long nummsgs,hdrcurPos,idxcurPos,msgcurPos;
   
  assert(file!=NULL);
  assert(header!=NULL);
  assert(body!=NULL);
  assert(header->msgId==0);

  nummsgs = _get_num_msgs(file);
  nummsgs++;
  hdrcurPos = lseek(file->hdrfd,0,SEEK_END);
  msgcurPos = lseek(file->msgfd,0,SEEK_END);
  idxcurPos = lseek(file->idxfd,0,SEEK_END);

  /* fill out the index record */
  idx.msgId = nummsgs;
  idx.bodyOffset = msgcurPos;
  idx.hdrOffset = hdrcurPos;
  _msg_write_idx(file,&idx);
  
  /* fill out the rest of the header. */
  header->msgId = nummsgs;
  _msg_write_header(file,header);

  /** Finally, write the body. */
  _msg_write_body(file,body);

  _put_num_msgs(file,nummsgs);

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

  assert(msgId < nummsgs);

  lseek(file->idxfd,sizeof(MsgIdxEntry)*msgId+sizeof(long),SEEK_SET);
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

  for (i=0;i<1;++i)
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

  MsgFile* file;
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
  
  return 0;
  
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

}

/**
 * user.c - user back-end functions.
 */

#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <bbslib/common/config.h>
#include <bbslib/common/util.h>
#include <unistd.h>
#include "user.h"

#define TRUE 1
#define FALSE 0

PrinterFlags *config_printflags = NULL;
SerialPortFlags *config_serialportflags = NULL; 
ModemStrings *config_modemstrings = NULL;

unsigned short _user_name_to_hash(const char* username)
{
  char buf[40];
  strcpy(buf,username);
  return crc16(buf,strlen(buf));
}

unsigned int _user_numusers_get()
{
  int fd;
  unsigned int numusers;
  fd = open(FILE_NUMUSERS,O_RDONLY);
  
  if (read(fd,&numusers,sizeof(unsigned int)) != sizeof(unsigned int))
    {
      perror("read()");
      close(fd);
      return FALSE;
    }
  close(fd);
  return numusers;
}

unsigned int _user_numusers_set(unsigned int numusers)
{
  int fd = open(FILE_NUMUSERS,O_RDWR|O_CREAT);

  if (write(fd,&numusers,sizeof(unsigned int)) != sizeof(unsigned int))
    {
      return 0;
    }

  close(fd);

  return numusers;
}

unsigned int _user_numusers_create()
{
  return _user_numusers_set(0);
}

unsigned int _user_numusers_inc()
{
  unsigned int numusers;
  numusers = _user_numusers_get();
  numusers++;
  return _user_numusers_set(numusers);
} 

unsigned char user_add(UserRecord* record)
{
  UserIndexRecord idx;
  UserRecord *lookup;
  unsigned int numusers = 0;
  int datfd; 
  int idxfd;
  long datoffset;

  if (user_lookup(record->username,lookup) == TRUE)
    {
      // User exists.
      return FALSE;
    }

  datfd = open(FILE_USER_DAT,O_CREAT|O_RDWR);
  idxfd = open(FILE_USER_IDX,O_CREAT|O_RDWR);

  numusers = _user_numusers_inc(); // first call of this will return 1
  datoffset = lseek(datfd,0,SEEK_END);
  lseek(idxfd,0,SEEK_END);
  record->user_id = numusers;
  idx.username_hash = _user_name_to_hash(record->username);
  idx.offset = datoffset;

  if (write(datfd,(UserRecord *)record,sizeof(UserRecord)) != sizeof(UserRecord))
    {
      close(datfd);
      close(idxfd);
      return FALSE;
    }

  if (write(idxfd,&idx,sizeof(UserIndexRecord)) != sizeof(UserIndexRecord))
    {
      close(datfd);
      close(idxfd);
      return FALSE;
    }

  close(datfd);
  close(idxfd);
  return TRUE;
}

long _find_user_offset(const char* username)
{
  UserIndexRecord *idx;
  int idxfd;
  unsigned int username_hash = _user_name_to_hash(username);
  long offset = -1;

  idx = calloc(1,sizeof(UserIndexRecord));
  if (!idx)
    {
      // Could not allocate.
      return -1;
    }

  idxfd = open(FILE_USER_IDX,O_RDONLY);
  if (idxfd == -1)
    {
      // Could not open user index.
      return -1;
    }

  while (read(idxfd,(UserIndexRecord *)idx,sizeof(UserIndexRecord)) == sizeof(UserIndexRecord))
    {
      if (username_hash == idx->username_hash)
	{
	  offset = idx->offset;
	}
    }
  
  close(idxfd);
  free(idx);

  return offset;
}

unsigned char user_lookup(const char* username, UserRecord* record)
{
  int datfd;
  unsigned int username_hash = _user_name_to_hash(username);
  long offset = _find_user_offset(username);
  off_t seeked_offset;

  if (offset == -1)
    {
      // User not found.
      return FALSE;
    }

  datfd = open(FILE_USER_DAT,O_RDONLY);
  if (datfd == -1)
    {
      // Could not open user file.
      return FALSE;
    }

  seeked_offset = lseek(datfd,offset,SEEK_SET);
  if (seeked_offset!=offset)
    {
      // Could not seek to user record.
      return FALSE;
    }
  
  read(datfd,(UserRecord *)record, sizeof(UserRecord));
  close(datfd);
  return TRUE;
}

unsigned char user_update(UserRecord* record)
{
  return 0;
}

int main()
{
  UserRecord *rec;

  printf("Deleting user files for test...");
  unlink(FILE_USER_DAT);
  unlink(FILE_USER_IDX);
  unlink(FILE_NUMUSERS);
  printf("ok.\n");

  _user_numusers_create();

  rec = calloc(1,sizeof(UserRecord));
  if (!rec)
    {
      fatal_error("Could not allocate user record.");
    }

  strcpy(rec->username,"TSCHAK909");
  rec->password_hash = _user_name_to_hash("hal9000");
  strcpy(rec->from,"Outer Space");
  rec->security_level=255;
  rec->birthday=123456;
  strcpy(rec->email,"thom.cherryhomes@gmail.com");

  printf("Adding user %s... ",rec->username);
  if (user_add(rec) == FALSE)
    {
      printf("FAIL.\n");
      return FALSE;
    }
  else
    {
      printf("PASS.\n");
    }

  printf("Check duplicate during add...");
  if (user_add(rec) == FALSE)
    {
      printf("PASS.\n");
    }
  else
    {
      printf("FAIL.\n");
      return FALSE;
    }
  
  free(rec);

  rec = calloc(1,sizeof(UserRecord));

  strcpy(rec->username,"flashjazzcat");
  rec->password_hash = _user_name_to_hash("shibby");
  strcpy(rec->from,"Just Past Outer Space");
  rec->security_level=20;
  rec->birthday=552277;
  strcpy(rec->email,"flashjazzcat@atariage.com");

  printf("Attempting second user add...");

  if (user_add(rec) == FALSE)
    {
      printf("FAIL.\n");
      return FALSE;
    }
  else
    {
      printf("PASS.\n");
    }

  free(rec);
  rec = calloc(1,sizeof(UserRecord));

  printf("Attempting user lookup of TSCHAK909...");

  if (user_lookup("TSCHAK909",rec) == FALSE)
    {
      printf("FAIL.\n");
      return FALSE;
    }
  else
    {
      printf("PASS.\n");
      printf("-----\n");
      printf("user id: %u\n",rec->user_id);
      printf("from: %s\n",rec->from);
    }

  printf("Attempting password hash verify\n");
  printf("for TSCHAK909...");

  if (rec->password_hash != _user_name_to_hash("hal9000"))
    {
      printf("FAIL.\n");
      return FALSE;
    }
  else
    {
      printf("PASS.\n");
    }

  free(rec);
  return TRUE;

}

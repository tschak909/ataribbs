/**
 * user.c - user back-end functions.
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <bbslib/common/config.h>
#include <bbslib/common/util.h>
#include <unistd.h>
#include "user.h"

PrinterFlags *config_printflags = NULL;
SerialPortFlags *config_serialportflags = NULL; 
ModemStrings *config_modemstrings = NULL;

unsigned short _user_name_to_hash(const char* username)
{
  char buf[40];
  strcpy(buf,username);
  return crc16(buf,strlen(buf));
}

unsigned char user_add(UserRecord* record)
{
  FILE *fp;
  long offset;
  unsigned int numusers = 0;
  UserIndexRecord idx;

  if (!record)
    {
      fatal_error("Refusing to add NULL user.");
    }

  fp = fopen(FILE_NUMUSERS,"r");
  if (!fp)
    {
      fclose(fp);
      fp = fopen(FILE_NUMUSERS,"w+");
      if (!fp)
	{
	  fatal_error("Could not create " FILE_NUMUSERS);
	}
      if (fwrite(&numusers,sizeof(unsigned int),1,fp) != 1)
	{
	  fatal_error("Could not write initial value to " FILE_NUMUSERS);
	}
      fclose(fp);
    }
  else
    {
      if (fread(&numusers,sizeof(unsigned int),1,fp) != 1)
	{
	  fatal_error("Could not read value from " FILE_NUMUSERS);
	}
      fclose(fp);
    }

  fp = fopen(FILE_USER_DAT,"w+");
  fseek(fp,0,SEEK_END);

  if (!fp)
    {
      fatal_error("Could not open " FILE_USER_DAT);
    }

  offset = ftell(fp);

  numusers++;
  record->user_id = numusers;

  if (fwrite((UserRecord *)record,sizeof(UserRecord),1,fp) != 1)
    {
      fatal_error("Could not write user record to " FILE_USER_DAT);
    }
  
  fclose(fp);

  fp = fopen(FILE_USER_IDX,"w+");
  if (!fp)
    {
      fatal_error("Could not open " FILE_USER_IDX);
    }

  idx.user_id = numusers;
  idx.username_hash = _user_name_to_hash(record->username);
  idx.offset = offset;

  fseek(fp,0,SEEK_END);

  if (fwrite((UserIndexRecord *)idx,sizeof(UserIndexRecord),1,fp) != 1)
    {
      fatal_error("Could not write index record to " FILE_USER_IDX);
    }

  fclose(fp);

  fp = fopen(FILE_NUMUSERS,"w");
  if (!fp)
    {
      fatal_error("Could not write new value to " FILE_NUMUSERS);
    }
  
  if (fwrite(&numusers,sizeof(unsigned int),1,fp) != 1)
    {
      fatal_error("Could not write new value to " FILE_NUMUSERS);
    }

  fclose(fp);

  return 0;
}

void _user_dump()
{
  FILE *fp;
  UserRecord *rec;
  UserIndexRecord *idx;
  unsigned int numusers;
  rec = calloc(1,sizeof(UserRecord));
  idx = calloc(1,sizeof(UserIndexRecord));
  fp = fopen(FILE_USER_DAT,"r");
  while (!feof(fp))
    {
      fread((UserRecord *)rec, sizeof(UserRecord),1,fp);
      printf("User ID: %u\n",rec->user_id);
      printf("Username: %s\n",rec->username);
      printf("Password Hash: 0x%04x",rec->password_hash);
      printf("From: %s\n",rec->from);
      printf("Security Level: %u\n",rec->security_level);
      printf("Birthday: %lu\n",rec->birthday);
      printf("Email: %s\n",rec->email);
      printf("\n---\n");
    }
  fclose(fp);
  fp = fopen(FILE_USER_IDX,"r");
  while (!feof(fp))
    {
      fread((UserIndexRecord *)idx, sizeof(UserIndexRecord),1,fp);
      printf("User ID: %u\n",idx->user_id);
      printf("Username Hash: 0x%04x\n",idx->username_hash);
      printf("Offset in File: %lu\n",idx->offset);
      printf("\n--\n");
    }
  fclose(fp);

  fp = fopen(FILE_NUMUSERS,"r");
  while (!feof(fp))
    {
      fread(&numusers, sizeof(unsigned int),1,fp);
      printf("Num Users: %u\n",numusers);
    }
  fclose(fp);

  free(rec);
  free(idx);

}

unsigned char user_lookup(const char* username, UserRecord* record)
{
  return 0;
}

void main()
{
  UserRecord *rec;


  unlink(FILE_USER_DAT);
  unlink(FILE_USER_IDX);
  unlink(FILE_NUMUSERS);
  

  rec = calloc(1,sizeof(UserRecord));
  if (!rec)
    {
      fatal_error("Could not allocate user record.");
    }

  strcpy(rec->username,"TSCHAK909");
  rec->password_hash = _user_name_to_hash("password");
  strcpy(rec->from,"Outer Space");
  rec->security_level=255;
  rec->birthday=123456;
  strcpy(rec->email,"thom.cherryhomes@gmail.com");

  user_add(rec);

  free(rec);
  rec = calloc(1,sizeof(UserRecord));

  strcpy(rec->username,"flashjazzcat");
  rec->password_hash = _user_name_to_hash("shibby");
  strcpy(rec->from,"Just Past Outer Space");
  rec->security_level=20;
  rec->birthday=552277;
  strcpy(rec->email,"flashjazzcat@atariage.com");

  user_add(rec);
 
  _user_dump();

}

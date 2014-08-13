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

unsigned int _user_numusers_get()
{
  FILE *fp;
  unsigned int numusers;
  fp = fopen(FILE_NUMUSERS,"r");
  
  if (!fp)
    {
       fclose(fp);
      return 0;
    }
  
  if (fread(&numusers,sizeof(unsigned int),1,fp) != 1)
    {
      fclose(fp);
      return 0;
    }
  fclose(fp);
  return numusers;
}

unsigned int _user_numusers_set(unsigned int numusers)
{
  FILE *fp = fopen(FILE_NUMUSERS,"w");
  if (!fp)
    {
      return 0;
    }

  if (fwrite(&numusers,sizeof(unsigned int),1,fp) != 1)
    {
      return 0;
    }

  return numusers;
}

unsigned int _user_numusers_create()
{
  return _user_numusers_set(1);
}

unsigned int _user_numusers_inc()
{
  unsigned int numusers;

  numusers = _user_numusers_get();
  if (numusers == 0)
    {
      return _user_numusers_create();
    }
  
  numusers++;
  return _user_numusers_set(numusers);
  
} 

unsigned char user_add(UserRecord* record)
{
  UserIndexRecord idx;
  unsigned int numusers = _user_numusers_inc(); // first call of this will return 1
  FILE *datfp = fopen(FILE_USER_DAT,"w+");
  FILE *idxfp = fopen(FILE_USER_IDX,"w+");
  long datoffset;
  fseek(datfp,0,SEEK_END);
  fseek(idxfp,0,SEEK_END);
  datoffset = ftell(datfp);
  record->user_id = numusers;
  idx.username_hash = _user_name_to_hash(record->username);
  idx.offset = datoffset;
  fwrite((UserRecord *)record,sizeof(UserRecord),1,datfp);
  fwrite(&idx,sizeof(UserIndexRecord),1,idxfp);
  fclose(datfp);
  fclose(idxfp);
  return 0;
}

void _user_dump()
{
  FILE *fp;
  FILE *printer;
  UserRecord *rec;
  UserIndexRecord *idx;
  unsigned int numusers;
  rec = calloc(1,sizeof(UserRecord));
  idx = calloc(1,sizeof(UserIndexRecord));
  printer = fopen("P:","w");
  fp = fopen(FILE_USER_DAT,"r");
  while (!feof(fp))
    {
      fread((UserRecord *)rec, sizeof(UserRecord),1,fp);
      fprintf(printer,"User ID: %u\n",rec->user_id);
      fprintf(printer,"Username: %s\n",rec->username);
      fprintf(printer,"Password Hash: 0x%04x\n",rec->password_hash);
      fprintf(printer,"From: %s\n",rec->from);
      fprintf(printer,"Security Level: %u\n",rec->security_level);
      fprintf(printer,"Birthday: %lu\n",rec->birthday);
      fprintf(printer,"Email: %s\n",rec->email);
      fprintf(printer,"\n---\n");
    }
  fclose(fp);
  fprintf(printer,"\n\n--\n\n--\n\n");
  fp = fopen(FILE_USER_IDX,"r");
  while (!feof(fp))
    {
      fread((UserIndexRecord *)idx, sizeof(UserIndexRecord),1,fp);
      fprintf(printer,"Username Hash: 0x%04x\n",idx->username_hash);
      fprintf(printer,"Offset in File: %lu\n",idx->offset);
      fprintf(printer,"\n--\n");
    }
  fclose(fp);

  fp = fopen(FILE_NUMUSERS,"r");
  while (!feof(fp))
    {
      fread(&numusers, sizeof(unsigned int),1,fp);
      fprintf(printer,"Num Users: %u\n",numusers);
    }
  fclose(fp);
  fclose(printer);

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

  free(rec);

}

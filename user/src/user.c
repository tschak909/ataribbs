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
  FILE *fp;
  unsigned int numusers;
  fp = fopen(FILE_NUMUSERS,"w+");
  
  if (!fp)
    {
      perror("fopen()");
      fclose(fp);
      return FALSE;
    }

  rewind(fp);

  if (fread(&numusers,sizeof(unsigned int),1,fp) != 1)
    {
      perror("fread()");
      fclose(fp);
      return FALSE;
    }
  fclose(fp);
  return numusers;
}

unsigned int _user_numusers_set(unsigned int numusers)
{
  FILE *fp = fopen(FILE_NUMUSERS,"w+");
  if (!fp)
    {
      return 0;
    }

  if (fwrite(&numusers,sizeof(unsigned int),1,fp) != 1)
    {
      return 0;
    }

  fclose(fp);

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
  FILE *datfp = fopen(FILE_USER_DAT,"w+");
  FILE *idxfp = fopen(FILE_USER_IDX,"w+");
  long datoffset;

  if (!datfp || !idxfp)
    {
      // Couldn't open files
      return FALSE;
    }

  if (user_lookup(record->username,lookup) == TRUE)
    {
      // User exists.
      return FALSE;
    }

  numusers = _user_numusers_inc(); // first call of this will return 1
  fseek(datfp,0,SEEK_END);
  fseek(idxfp,0,SEEK_END);
  datoffset = ftell(datfp);
  record->user_id = numusers;
  idx.username_hash = _user_name_to_hash(record->username);
  idx.offset = datoffset;

  if (fwrite((UserRecord *)record,sizeof(UserRecord),1,datfp) != 1)
    {
      fclose(datfp);
      fclose(idxfp);
      return FALSE;
    }

  if (fwrite(&idx,sizeof(UserIndexRecord),1,idxfp) != 1)
    {
      fclose(datfp);
      fclose(idxfp);
      return FALSE;
    }

  fclose(datfp);
  fclose(idxfp);
  return TRUE;
}

unsigned char user_lookup(const char* username, UserRecord* record)
{
  UserIndexRecord *idx;
  FILE *idxfp = fopen(FILE_USER_IDX,"r");
  FILE *datfp = fopen(FILE_USER_DAT,"r");
  unsigned int username_hash = _user_name_to_hash(username);

  if (!idxfp || !datfp)
    {
      fclose(idxfp);
      fclose(datfp);
      return FALSE;
    }

  idx = calloc(1,sizeof(UserIndexRecord));

  while (!feof(idxfp))
    {
      fread((UserIndexRecord *)idx, sizeof(UserIndexRecord),1,idxfp);
      if (username_hash == idx->username_hash)
	{
	  fseek(datfp,idx->offset,SEEK_SET);
	  fread((UserRecord *)record, sizeof(UserRecord),1,datfp);
	  fclose(idxfp);
	  fclose(datfp);
	  free(idx);
	  return TRUE;
	}
    }
  record = NULL;
  fclose(idxfp);
  fclose(datfp);
  free(idx);
  return FALSE;
}

int main()
{
  UserRecord *rec;
  unsigned int userid;

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

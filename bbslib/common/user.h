/**
 * user.h - user back-end functions.
 */

#ifndef USER_H
#define USER_H

#define FILE_USER_DAT "D1:USER.DAT"
#define FILE_USER_IDX "D1:USER.IDX"
#define FILE_NUMUSERS "D1:NUMUSERS"

#include "types.h"

typedef struct
{
  unsigned int user_id;
  char username[40];
  unsigned short password_hash;
  char from[40];
  char security_level;
  TimeDate birthday;
  char email[64];
  TimeDate firstLogon;
  TimeDate lastLogon;
} UserRecord;

typedef struct
{
  unsigned short username_hash;
  long offset;
} UserIndexRecord;

unsigned char user_add(UserRecord* record);
unsigned char user_lookup(const char* username, UserRecord* record);
unsigned char user_update(UserRecord* record);
void user_scan_begin();
void user_scan_end();
int user_scan_next(UserRecord* record);
void user_set(UserRecord* record);
UserRecord* user_get();
void user_logoff();

#endif USER_H

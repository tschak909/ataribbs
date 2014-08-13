/**
 * user.h - user back-end functions.
 */

#ifndef USER_H
#define USER_H

#define FILE_USER_DAT "D1:USER.DAT"
#define FILE_USER_IDX "D1:USER.IDX"
#define FILE_NUMUSERS "D1:NUMUSERS"

typedef struct
{
  unsigned int user_id;
  char username[40];
  unsigned short password_hash;
  char from[40];
  char security_level;
  long birthday;
  char email[64];
} UserRecord;

typedef struct
{
  unsigned short username_hash;
  long offset;
} UserIndexRecord;

unsigned char user_add(UserRecord* record);
unsigned char user_lookup(const char* username, UserRecord* record);

#endif USER_H

/**
 * ledit.h - Line Editor Functions
 */
#ifndef LEDIT_H
#define LEDIT_H

#define LEDIT_LINE_SIZE 40

typedef struct {
  int lineNo;
  char line[LEDIT_LINE_SIZE];
} LineEditRecord;

struct LineEditLinkage; 

typedef struct LineEditLinkage {
  struct LineEditLinkage* prev;
  struct LineEditLinkage* next;
  int lineNo;
} LineEditLinkage;

#endif /* LEDIT_H */

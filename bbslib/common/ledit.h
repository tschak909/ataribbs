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

struct LineEditNode;

typedef struct LineEditNode {
  struct LineEditNode* prev;
  int lineNo;
  struct LineEditNode* next;
} LineEditNode;

#endif /* LEDIT_H */

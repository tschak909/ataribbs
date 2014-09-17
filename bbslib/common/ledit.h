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

typedef int LineEditLinkage;

void ledit_init();
void ledit_done();
void ledit_insert(const char* text);
unsigned char ledit_get(int line);

void ledit();

#endif /* LEDIT_H */

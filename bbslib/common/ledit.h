/**
 * ledit.h - Line Editor Functions
 */
#ifndef LEDIT_H
#define LEDIT_H

#define LEDIT_LINE_SIZE 40

typedef struct LineEditRecord {
  int lineNo;
  char line[LEDIT_LINE_SIZE];
} LineEditRecord;

typedef struct LineEditNode {
  struct LineEditNode* prev;
  int lineNo;
  struct LineEditNode* next;
} LineEditNode;

void ledit_init();
void ledit_done();
void ledit_insert_at_end(char* line);
void ledit_debug();
void ledit_insert_at_beginning(char* text);
void ledit_insert_after_line(int lineNo, char* line);
char* ledit_get_first_line();
char* ledit_get_next_line();

// void ledit_insert_line_replace(int lineNo, const char* text);
#endif /* LEDIT_H */

/**
 * ledit.c - Line Editor functions
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include "ledit.h"

#define LEDIT_TEMP_FILE "LEDIT.TMP"

LineEditNode* ledit_head = NULL; 
LineEditNode* ledit_tail = NULL;
LineEditNode* ledit_ptr = NULL;
int ledit_node_count = 0;

LineEditRecord* ledit_line = NULL;
int leditfd;
int ledit_line_count = 0;

LineEditNode* ledit_create_node(int lineNo)
{
  LineEditNode* newNode;
  newNode=(LineEditNode *)malloc(sizeof(LineEditNode));

  if (!newNode)
    {
      perror("could not allocate memory");
      abort();
    }

  newNode->lineNo = lineNo;
  newNode->next = NULL;
  newNode->prev = NULL;
  return newNode;

}

void ledit_create_initial_nodes()
{
  ledit_head = (LineEditNode*)malloc(sizeof(LineEditNode));
  if (!ledit_head)
    {
      perror("Could not allocate head node");
      abort();
    }

  ledit_tail = (LineEditNode*)malloc(sizeof(LineEditNode));
  if (!ledit_tail)
    {
      perror("Could not allocate tail node");
      abort();
    }

  ledit_head->lineNo=ledit_tail->lineNo=0;
  ledit_head->next=ledit_tail;
  ledit_tail->prev=ledit_head;
  ledit_head->prev=ledit_tail->next=NULL;
}

void ledit_delete_initial_nodes()
{
  if (ledit_head)
    {
      free(ledit_head);
      ledit_head = NULL;
    }

  if (ledit_tail)
    {
      free(ledit_tail);
      ledit_tail = NULL;
    }
}

void ledit_insert_node(int lineNo, int pos)
{
  LineEditNode* newnode;
  LineEditNode* temp1;
  int i=1;

  newnode=ledit_create_node(lineNo);
  temp1=ledit_head;

  while (temp1)
    {
      if (i==pos)
	{
	  newnode->next = temp1->next;
	  newnode->prev = temp1;
	  temp1->next->prev = newnode;
	  temp1->next = newnode;
	  ledit_node_count++;
	  break;
	}
      temp1=temp1->next;
      i++;
    }
  return;
}

void ledit_insert_node_at_start(int lineNo)
{
  LineEditNode* newnode = ledit_create_node(lineNo);
  newnode->next = ledit_head->next;
  newnode->prev = ledit_head;
  ledit_head->next->prev = newnode;
  ledit_head->next = newnode;
  ledit_node_count++;
}

void ledit_insert_node_at_end(int lineNo)
{
  LineEditNode* newnode = ledit_create_node(lineNo);
  newnode->next=ledit_tail;
  newnode->prev=ledit_tail->prev;
  ledit_tail->prev->next = newnode;
  ledit_tail->prev = newnode;
  ledit_node_count++;
}

void ledit_delete_node(int pos)
{
  LineEditNode* temp1;
  LineEditNode* temp2;
  int i=1;
  temp1 = ledit_head->next;
  while (temp1 != ledit_tail)
    {
      if (i==pos)
	{
	  temp2=temp1;
	  temp1->prev->next = temp1->next;
	  temp1->next->prev = temp1->prev;
	  free(temp2);
	  ledit_node_count--;
	  return;
	}
      temp1 = temp1->next;
      ++i;
    }
  perror("Given node is not present in list.");
  return;
}

void ledit_delete_list()
{
  LineEditNode* temp2;
  LineEditNode* temp1 = ledit_head->next;
  while (temp1!=ledit_tail)
    {
      temp1->prev->next=temp1->next;
      temp1->next->prev=temp1->prev;
      temp2=temp1;
      free(temp1);
      temp1=temp2->next;
    }
  ledit_node_count=0;
  return;
}

void ledit_search_node(int lineNo)
{
  LineEditNode* temp = ledit_head->next;
  while (temp!=ledit_tail)
    {
      if (temp->lineNo==lineNo)
	{
	  // Node is present
	  return;
	}
      temp=temp->next;
    }
  // Node is not present
  return;
}

void ledit_alloc_line_buffer()
{
  ledit_line = calloc(1,sizeof(LineEditRecord));
  if (!ledit_line)
    {
      perror("Could not allocate line buffer.");
      abort();
    }
  return;
}

void ledit_dealloc_line_buffer()
{
  free(ledit_line);
}

void ledit_open_temp()
{
  unlink(LEDIT_TEMP_FILE);
  leditfd = open(LEDIT_TEMP_FILE,O_RDWR|O_CREAT|O_TRUNC);
  if (leditfd<0)
    {
      perror("Could not open " LEDIT_TEMP_FILE);
      abort();
    }
}

void ledit_close_temp()
{
  close(leditfd);
}

void ledit_insert_at_beginning(char* line)
{
  assert(leditfd>0);
  assert(ledit_line!=NULL);
  memset(ledit_line,0,sizeof(LineEditRecord));
  lseek(leditfd,0,SEEK_END);
  ledit_line->lineNo=ledit_line_count;
  strcpy(ledit_line->line,line);

  if (write(leditfd,(LineEditRecord* )ledit_line,sizeof(LineEditRecord)) != sizeof(LineEditRecord))
    {
      perror("Could not write line to " LEDIT_TEMP_FILE);
      abort();
    }

  ledit_insert_node_at_start(ledit_line_count);
  ledit_line_count++;
}

void ledit_insert_at_end(char* line)
{
  assert(leditfd>0);
  assert(ledit_line!=NULL);
  memset(ledit_line,0,sizeof(LineEditRecord));
  lseek(leditfd,0,SEEK_END);
  ledit_line->lineNo=ledit_line_count;
  strcpy(ledit_line->line,line);

  if (write(leditfd,(LineEditRecord* )ledit_line,sizeof(LineEditRecord)) != sizeof(LineEditRecord))
    {
      perror("Could not write line to " LEDIT_TEMP_FILE);
      abort();
    }

  ledit_insert_node_at_end(ledit_line_count);
  ledit_line_count++;
}

void ledit_insert_after_line(int lineNo, char* line)
{
  assert(leditfd>0);
  assert(ledit_line!=NULL);
  memset(ledit_line,0,sizeof(LineEditRecord));
  lseek(leditfd,0,SEEK_END);
  ledit_line->lineNo=ledit_line_count;
  strcpy(ledit_line->line,line);

  if (write(leditfd,(LineEditRecord* )ledit_line,sizeof(LineEditRecord)) != sizeof(LineEditRecord))
    {
      perror("Could not write line to " LEDIT_TEMP_FILE);
      abort();
    }

  ledit_insert_node(ledit_line_count,lineNo);
  ledit_line_count++;
}

long ledit_get_line_offset(int lineNo)
{
  return sizeof(LineEditRecord)*lineNo;
}

void ledit_read_line(int lineNo, char* buf)
{
  assert(buf!=NULL);
  assert(leditfd>0);
  lseek(leditfd,ledit_get_line_offset(lineNo),SEEK_SET);
  if (read(leditfd,(LineEditRecord* )ledit_line,sizeof(LineEditRecord)) != sizeof(LineEditRecord))
    {
      perror("couldn't read from temp file.");
      abort();
    }
  strcpy(buf,ledit_line->line);
  return;
}

void ledit_debug()
{
  LineEditNode* ptr = ledit_head->next;

  if (ledit_head->next == ledit_tail)
    {
      perror("Nothing in list");
      return;
    }

  while(ptr!=ledit_tail)
    {
      printf("%5u\n",ptr->lineNo);
      ptr = ptr->next;
    }

}

char* ledit_get_line(int lineNo)
{
  LineEditNode* ptr = ledit_head->next;
  int i=0;
  char buf[40];
  assert(lineNo<ledit_node_count);
  while(ptr!=ledit_tail)
    {
      if (i==lineNo)
	{
	  ledit_read_line(ptr->lineNo,buf);
	  return strdup(buf);
	  break;
	}
      else
	{
	  ++i;
	  ptr = ptr->next;
	}
    }
  return NULL;
}

char* ledit_get_next_line()
{
  char buf[40];
  assert(ledit_ptr!=NULL);

  if (ledit_ptr==ledit_tail)
    return NULL;

  ledit_read_line(ledit_ptr->lineNo,buf);  
  ledit_ptr=ledit_ptr->next;

  return strdup(buf);
}

char* ledit_get_first_line()
{
  ledit_ptr = ledit_head->next;
  return ledit_get_next_line();
}

void ledit_replace_node(int oldLineNo, int newLineNo)
{
  LineEditNode* current_node;
  int i=0;

  current_node = ledit_head->next;

  while (current_node!=ledit_tail)
    {
      if (current_node->lineNo==oldLineNo)
	{
	  current_node->lineNo=newLineNo;
	  return;
	}
      current_node=current_node->next;
    }
  printf("Node not present.");
  return;
}

void ledit_replace_line(int lineNo, const char* text)
{
  assert(leditfd>0);
  assert(ledit_line!=NULL);
  lseek(leditfd,0,SEEK_END);
  ledit_line->lineNo=ledit_line_count;
  strcpy(ledit_line->line,text);

  if (write(leditfd,(LineEditRecord* )ledit_line,sizeof(LineEditRecord)) != sizeof(LineEditRecord))
    {
      perror("Could not write line to " LEDIT_TEMP_FILE);
      abort();
    }

  ledit_replace_node(lineNo,ledit_line_count);
  ledit_line_count++;  
}

int ledit_get_line_count()
{
  return ledit_node_count;
}

void ledit_init()
{
  ledit_open_temp();
  ledit_create_initial_nodes();
  ledit_alloc_line_buffer();
}

void ledit_done()
{
  ledit_close_temp();
  ledit_delete_list();
  ledit_delete_initial_nodes();
  ledit_dealloc_line_buffer();
  ledit_line_count=0;
  ledit_node_count=0;
}

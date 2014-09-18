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

LineEditNode* ledit_head = NULL; 
LineEditNode* ledit_tail = NULL;
int ledit_node_count = 0;

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

void ledit_insert(int lineNo, int pos)
{
  LineEditNode* newnode;
  LineEditNode* temp1;
  int i;

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

void ledit_insert_at_start(int lineNo)
{
  LineEditNode* newnode = ledit_create_node(lineNo);
  newnode->next = ledit_head->next;
  newnode->prev = ledit_tail->prev;
  ledit_tail->prev->next = newnode;
  ledit_tail->prev = newnode;
  ledit_node_count++;
}

void ledit_insert_at_end(int lineNo)
{
  LineEditNode* newnode = ledit_create_node(lineNo);
  newnode->next=ledit_tail;
  newnode->prev=ledit_tail->prev;
  ledit_tail->prev->next = newnode;
  ledit_tail->prev = newnode;
  ledit_node_count++;
}

void delete(int lineNo)
{
  LineEditNode* temp1;
  LineEditNode* temp2;
  temp1 = ledit_head->next;
  while (temp1 != ledit_tail)
    {
      if (temp1->lineNo == lineNo)
	{
	  temp2=temp1;
	  temp1->prev->next = temp1->next;
	  temp1->next->prev = temp1->prev;
	  free(temp2);
	  ledit_node_count--;
	  return;
	}
      temp1 = temp1->next;
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

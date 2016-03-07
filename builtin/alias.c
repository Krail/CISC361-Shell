#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "alias.h"


void printAliasTable(struct alias *head) {
  struct alias *node = head;
  while (node != NULL) {
    printf("%s\t%s\n", node->alias, node->command);
    node = node->next;
  }
}

char* getAlias(struct alias *head, const char *alias_match) {
  struct alias *node = head;
  while (node != NULL) {
    if (strcmp(node->alias, alias_match) == 0) return node->command;
    node = node->next;
  }
  return NULL;
}

struct alias* setAlias(struct alias *head, const char *alias, const char *command) {
  struct alias *node = head;
  struct alias *new_node = malloc(sizeof(struct alias));
  new_node->alias = malloc(sizeof(alias)+1);
  strcpy(new_node->alias, alias);
  new_node->command = malloc(sizeof(command)+1);
  strcpy(new_node->command, command);
  new_node->next = NULL;
  if (node == NULL) head = new_node;
  else {
    while (node->next != NULL) {
      if (strcmp(node->alias, alias) == 0) {
        free(node->command);
        node->command = new_node->command;
        free(new_node->alias);
        free(new_node);
        return head;
      }
      node = node->next;
    }
    if (strcmp(node->alias, alias) == 0) {
      free(node->command);
      node->command = new_node->command;
      free(new_node->alias);
      free(new_node);
      return head;
    }
    node->next = new_node;
  }
  return head;
}

void freeAliasTable(struct alias *head) {
  struct alias *node = head;
  while (node != NULL) {
    free(node->alias);
    free(node->command);
    node = node->next;
  }
  free(head);
}


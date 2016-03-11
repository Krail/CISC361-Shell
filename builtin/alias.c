#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "alias.h"


/* Print the given alias table */
void printAliasTable(struct alias *head) {
  struct alias *node = head;
  while (node != NULL) {
    printf("%s\t%s\n", node->alias, node->command);
    node = node->next;
  }
}

/* Get aliased command */
char* getAlias(struct alias *head, const char *alias_match) {
  struct alias *node = head;
  while (node != NULL) {
		// Match?
		if (strcmp(node->alias, alias_match) == 0) return node->command;
    node = node->next;
  }
	// No alias found with that name
  return NULL;
}

/* Set command to this alias, overwrite if needed */
struct alias* setAlias(struct alias *head, const char *alias, const char *command) {
  struct alias *node = head;
  struct alias *new_node = malloc(sizeof(struct alias));
  new_node->alias = malloc((strlen(alias)+1)*sizeof(char));
  strcpy(new_node->alias, alias);
  new_node->command = malloc((strlen(command)+1)*sizeof(char));
  strcpy(new_node->command, command);
  new_node->next = NULL;
  if (head == NULL) head = new_node;
  else {
    while (node->next != NULL) {
			// Match?
      if (strcmp(node->alias, alias) == 0) {
        free(node->command);
        node->command = new_node->command;
        free(new_node->alias);
        free(new_node);
        return head;
      }
      node = node->next;
    }
		// Need to check again for last node
    if (strcmp(node->alias, alias) == 0) {
      free(node->command);
      node->command = new_node->command;
      free(new_node->alias);
      free(new_node);
      return head;
    }
    node->next = new_node;
  }
	// Return head
  return head;
}

/* Free alias table */
void freeAliasTable(struct alias *head) {
  struct alias *node = head;
	struct alias *tmp;
  while (node != NULL) {
    free(node->alias);
    free(node->command);
		tmp = node->next;
		free(node);
    node = tmp;
  }
}


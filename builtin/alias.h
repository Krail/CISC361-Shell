#ifndef ALIAS_H
#define ALIAS_H

//  char **ALIAS_TABLE;
//  int NUM_ALIASES = 0;

  struct alias {
    char *alias;
    char *command;
    struct alias *next;
  };

  void printAliasTable(struct alias *head);

  char* getAlias(struct alias *head, const char *alias);

  struct alias* setAlias(struct alias *head, const char *alias, const char *command);

  void freeAliasTable(struct alias *head);

#endif /* ALIAS_H */


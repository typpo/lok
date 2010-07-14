#ifndef DB_H
#define DB_H

#include <sqlite3.h>

// info for showing things in menu
typedef struct {
    int id;
    char *title;
    char *text;
    char *edited;
    char *added;
} lok_item;

// ** db functions

int db_start(char *path, char *key);

void db_shutdown();

void free_notes(char **buf);

int db_create_table();

int fetch_notes(int n, lok_item **buf);

int db_insert_note(char *title, char *text);

int db_edit_note(int id, char *title, char *text);
#endif

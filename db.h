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

void db_free_notes(lok_item *buf, int num_items);

int db_create_table();

int db_fetch_notes(int limit, lok_item **buf, int *num_fetched);

int db_insert_note(char *title, char *text);

int db_edit_note(int id, char *title, char *text);
#endif

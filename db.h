#ifndef DB_H
#define DB_H

#include <sqlite3.h>

// info pertaining to a single note
typedef struct {
    char *id;
    char *title;
    char *text;
    char *edited;
    char *added;
} lok_item_t;

// ** db functions

int db_start(char *path, char *key);

void db_shutdown();

void db_free_notes(lok_item_t *buf, int num_items);

int db_create_table();

int db_fetch_notes(int limit, lok_item_t **buf, int *num_fetched);

int db_insert_note(char *title, char *text);

int db_edit_note(char *id, char *title, char *text);
#endif

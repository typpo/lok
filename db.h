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

int db_start(char *path, sqlite3 *handle);

void db_shutdown(sqlite3 *handle);

void free_notes(char **buf);

int db_create_table();

int fetch_notes(sqlite3 * handle, int n, lok_item **buf);

int db_insert_note(sqlite3 *handle, char *title, char *text);

int db_edit_note(sqlite3 *handle, int id, char *title, char *text);
#endif

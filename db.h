#ifndef DB_H
#define DB_H

#include <sqlite3.h>

// ** db functions

int db_start(char *path, sqlite3 *handle);

void db_shutdown(sqlite3 *handle);

int db_create_table();

int db_fetch_notes(sqlite3 *handle, int n, char **buf);

int db_insert_note(sqlite3 *handle, char *title, char *text);

int db_edit_note(sqlite3 *handle, int id, char *title, char *text);
#endif

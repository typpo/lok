#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <string.h>
#include "db.h"

int db_start(char *path, sqlite3 * handle)
{
	if (sqlite3_open(path, &handle)) {
		fprintf(stderr, "Couldn't load database: %s\n",
			sqlite3_errmsg(handle));
		sqlite3_close(handle);
		return -1;
	}
	return 0;
}

void db_shutdown(sqlite3 * handle)
{
	sqlite3_close(handle);
}

int create_table(sqlite3 * handle)
{
	char *query =
	    "CREATE TABLE IF NOT EXISTS notes(id INTEGER PRIMARY KEY,title TEXT NOT NULL,text TEXT NOT NULL, edited DATE NOT NULL, added DATE NOT NULL)";
	return sqlite3_exec(handle, query, 0, 0, 0);
}

int insert_note(sqlite3 * handle, char *title, char *text)
{
	// prepare the query
	sqlite3_stmt *stmt;
	char *query =
	    "INSERT INTO notes VALUES (NULL, ?, ?, datetime('now'), datetime('now'))";
	if (sqlite3_prepare_v2(handle, query, -1, &stmt, 0)) {
		return -1;
	}
	if (sqlite3_bind_text(stmt, 1, title, -1, SQLITE_STATIC)) {
		return -1;
	}
	if (sqlite3_bind_text(stmt, 2, text, -1, SQLITE_STATIC)) {
		return -1;
	}
	// execute it
	if (sqlite3_step(stmt)) {
		return -1;
	}

    if (sqlite3_finalize(stmt)) {
        return -1;
    }
	return 0;
}

int edit_note(sqlite3 * handle, int id, char *title, char *text)
{
	// prepare the query
	sqlite3_stmt *stmt;
	char *query =
        "UPDATE notes SET title=?, text=?, edited=datetime('now') WHERE id=?";
	if (sqlite3_prepare_v2(handle, query, -1, &stmt, 0)) {
		return -1;
	}
	if (sqlite3_bind_int(stmt, 1, id)) {
		return -1;
	}
	if (sqlite3_bind_text(stmt, 2, title, -1, SQLITE_STATIC)) {
		return -1;
	}
	if (sqlite3_bind_text(stmt, 3, text, -1, SQLITE_STATIC)) {
		return -1;
	}
	// execute it
	if (sqlite3_step(stmt)) {
		return -1;
	}

    if (sqlite3_finalize(stmt)) {
        return -1;
    }

	return 0;
}

// allocates memory for buf
int fetch_notes(sqlite3 * handle, int n, lok_item **buf)
{
	sqlite3_stmt *stmt;
	// TODO make limit 50 a constant, possibly change order to added
	char *query = "SELECT * FROM notes ORDER BY edited DESC LIMIT 50";
	int ret = sqlite3_prepare_v2(handle, query, -1, &stmt, 0);
	if (ret) {
		return -1;
	}

	buf = (lok_item**) calloc(50, sizeof(lok_item*));
    ret = 0;
    for (int i=0;ret==SQLITE_ROW;i++) {
		ret = sqlite3_step(stmt);

        const int id = (const int)sqlite3_column_int(stmt, 0);
        const char *tmp_title = (const char *)sqlite3_column_text(stmt, 1);
        const char *tmp_text = (const char *)sqlite3_column_text(stmt, 2);
        const char *tmp_edited = (const char *)sqlite3_column_text(stmt, 3);
        const char *tmp_added = (const char *)sqlite3_column_text(stmt, 4);

        char *title = malloc(sizeof(char)*strlen(tmp_title));
        char *text = malloc(sizeof(char)*strlen(tmp_text));
        char *edited = malloc(sizeof(char)*strlen(tmp_edited));
        const char *added = malloc(sizeof(char)*strlen(tmp_added));

        strcpy(title, tmp_title);
        strcpy(text, tmp_text);
        strcpy(edited, tmp_edited);
        strcpy(added, tmp_added);

        lok_item *item = malloc(sizeof(lok_item));
        item->id = id;
        item->title = title;
        item->edited = edited;
        item->added = added;
    }

	if (sqlite3_finalize(stmt)) {
        return -1;
    }
    return 0;
}

// frees 2d array of chars, which is how notes are recorded by fetch_notes
void free_notes(char **buf)
{
    int n = sizeof(buf)/sizeof(buf[0]);
    for (int i=0; i < n; i++) {
        free(buf[i]);
    }
    free(buf);
}
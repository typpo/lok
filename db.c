#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <string.h>
#include "db.h"

// DB connection
sqlite3 *handle;

int db_start(char *path, char *key)
{
	if (sqlite3_open(path, &handle)) {
		fprintf(stderr, "Couldn't load database: %s\n",
			sqlite3_errmsg(handle));
		sqlite3_close(handle);
		return -1;
	}
#ifndef NO_CRYPTO
    if (sqlite3_key(handle, key, strlen(key))) {
		fprintf(stderr, "Couldn't load database with key: %s\n",
			sqlite3_errmsg(handle));
		sqlite3_close(handle);
		return -1;
    }
#endif

    printf("Started db connection\n");
	return 0;
}

void db_shutdown()
{
	if (sqlite3_close(handle)) {
		fprintf(stderr, "Couldn't close database: %s\n",
			sqlite3_errmsg(handle));
    }
}

int db_create_table()
{
	char *query =
	    "CREATE TABLE IF NOT EXISTS notes(id INTEGER PRIMARY KEY,title TEXT NOT NULL,text TEXT NOT NULL, edited DATE NOT NULL, added DATE NOT NULL)";
	return sqlite3_exec(handle, query, 0, 0, 0);
}

int db_insert_note(char *title, char *text)
{
	sqlite3_stmt *stmt;
	char *query =
	    "INSERT INTO notes VALUES (NULL, ?, ?, datetime('now'), datetime('now'))";
    int ret = sqlite3_prepare_v2(handle, query, -1, &stmt, 0)
        || sqlite3_bind_text(stmt, 1, title, -1, SQLITE_STATIC)
        || sqlite3_bind_text(stmt, 2, text, -1, SQLITE_STATIC)
        || sqlite3_step(stmt);

    if (sqlite3_finalize(stmt)) {
        return -1;
    }
	return ret;
}

int db_edit_note(char *id, char *title, char *text)
{
	sqlite3_stmt *stmt;
	char *query =
        "UPDATE notes SET title=?, text=?, edited=datetime('now') WHERE id=?";
	int ret = sqlite3_prepare_v2(handle, query, -1, &stmt, 0)
        || sqlite3_bind_text(stmt, 1, title, -1, SQLITE_STATIC)
        || sqlite3_bind_text(stmt, 2, text, -1, SQLITE_STATIC)
        || sqlite3_bind_text(stmt, 3, id, -1, SQLITE_STATIC)
        || sqlite3_step(stmt);

    if (sqlite3_finalize(stmt)) {
        return -1;
    }
	return ret;
}

// allocates memory for buf
int db_fetch_notes(int limit, lok_item **buf, int *num_notes)
{
	sqlite3_stmt *stmt;
	// TODO make limit 50 a constant, possibly change order to added
	char *query = "SELECT * FROM notes ORDER BY edited DESC LIMIT 50";
	if (sqlite3_prepare_v2(handle, query, -1, &stmt, 0)) {
        printf("Invalid key.\n");
		return -1;
	}

	lok_item *ptr = malloc(50*sizeof(lok_item));
    *buf = ptr;
    int i;
    // walk through each row in results
    for (i=0;sqlite3_step(stmt)==SQLITE_ROW;i++) {
        const char *id = (const char *)sqlite3_column_text(stmt, 0);
        const char *title = (const char *)sqlite3_column_text(stmt, 1);
        const char *text = (const char *)sqlite3_column_text(stmt, 2);
        const char *edited = (const char *)sqlite3_column_text(stmt, 3);
        const char *added = (const char *)sqlite3_column_text(stmt, 4);

        // allocate pointers
        ptr->id = malloc((strlen(id)+1)*sizeof(char));
        ptr->title = malloc((strlen(title)+1)*sizeof(char));
        ptr->text = malloc((strlen(text)+1)*sizeof(char));
        ptr->added = malloc((strlen(added)+1)*sizeof(char));
        ptr->edited = malloc((strlen(edited)+1)*sizeof(char));

        // copy query results
        strcpy(ptr->id, id);
        strcpy(ptr->title, title);
        strcpy(ptr->edited, edited);
        strcpy(ptr->added, added);
        strcpy(ptr->text, text);

        ptr++;
    }
    // record number of notes
    *num_notes = i;

	if (sqlite3_finalize(stmt)) {
        return -1;
    }
    return 0;
}

// frees 2d array of chars, which is how notes are recorded by fetch_notes
void db_free_notes(lok_item *buf, int num_items)
{
    for (int i=0; i < num_items; i++) {
        free(buf[i].id);
        free(buf[i].title);
        free(buf[i].edited);
        free(buf[i].added);
        free(buf[i].text);
    }
    free(buf);
}

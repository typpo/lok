#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>

int db_start(char *path, sqlite3 *handle)
{
    if (sqlite3_open(path, &handle)) {
        fprintf(stderr, "Couldn't load database: %s\n", sqlite3_errmsg(handle));
        sqlite3_close(handle);
        return -1;
    }
    return 0;
}

void db_shutdown(sqlite3 *handle)
{
    sqlite3_close(handle);
}

int create_table(sqlite3* handle)
{
    char *query = "CREATE TABLE IF NOT EXISTS notes(id INTEGER PRIMARY KEY,title TEXT NOT NULL,text TEXT NOT NULL, date DATE NOT NULL)";
    return sqlite3_exec(handle, query, 0,0,0);
}

int insert_note(sqlite3 *handle, char *title, char *text)
{
    // prepare the query
    sqlite3_stmt *stmt;
    char *query = "INSERT INTO notes VALUES (NULL, ?, ?, datetime('now'))";
    if (sqlite3_prepare_v2(handle, query, -1, &stmt, 0)) {
        return -1;
    }
    if (sqlite3_bind_text(stmt, 1, title, -1, SQLITE_STATIC)) {
        return -2;
    }
    if (sqlite3_bind_text(stmt, 2, text, -1, SQLITE_STATIC)) {
        return -3;
    }
    // execute it
    if (sqlite3_step(stmt)) {
        return -4;
    }
    return 0;
}

int edit_note(sqlite3 *handle, int id, char *title, char *text)
{
    // prepare the query
    sqlite3_stmt *stmt;
    char *query = "UPDATE notes SET title=?, text=? WHERE id=?";
    if (sqlite3_prepare_v2(handle, query, -1, &stmt, 0)) {
        return -1;
    }
    if (sqlite3_bind_int(stmt, 1, id)) {
        return -2;
    }
    if (sqlite3_bind_text(stmt, 2, title, -1, SQLITE_STATIC)) {
        return -3;
    }
    if (sqlite3_bind_text(stmt, 3, text, -1, SQLITE_STATIC)) {
        return -4;
    }
    // execute it
    if (sqlite3_step(stmt)) {
        return -5;
    }
    return 0;
}

int fetch_notes(sqlite3 *handle, int n, char **buf)
{
    sqlite3_stmt *stmt;
    char *query = "SELECT * FROM notes ORDER BY date DESC LIMIT 50";
    int retval = sqlite3_prepare_v2(handle, query, -1, &stmt, 0);
    if (retval) {
        return -1;
    }

    int cols = sqlite3_column_count(stmt);
    int i = 0;
    // TODO make limit 50 a constant
    buf = (char **) calloc(50, sizeof(char *));
    do {
        retval = sqlite3_step(stmt);
        if (retval == SQLITE_ROW) {
            for (int col=0; col < cols; col++) {
                const char *val = (const char*)sqlite3_column_text(stmt, col);
                printf("%s = %s\t", sqlite3_column_name(stmt, col), val);
                buf[i] = malloc(sizeof(char) * 10);
            }
            printf("\n");
            i++;
        }
    } while (retval == SQLITE_ROW);

    return 0;
}

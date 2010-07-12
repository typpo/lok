#ifndef MAIN_H
#define MAIN_H

// ** Startup functions

void usage();

void getpass(char *buf);

void init();

// ** curses functions

void start_main_window();

void loop(WINDOW *menu_win, MENU *menu);

void print_in_middle(WINDOW * win, int starty, int startx, int width,
		     char *string, chtype color);

// ** db functions

int create_table();

int fetch_notes(sqlite3 *handle, int n, char **buf);

int insert_note(char *title, char *text);

int edit_note(int id, char *title, char *text);


#endif

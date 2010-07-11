#ifndef MAIN_H
#define MAIN_H
void usage();

void getpass(char *buf);

int create_table();

int fetch_notes(sqlite3 *handle, int n, char **buf);

void print_in_middle(WINDOW * win, int starty, int startx, int width,
		     char *string, chtype color);

void init();

void start();

void loop(WINDOW *menu_win, MENU *menu);
#endif

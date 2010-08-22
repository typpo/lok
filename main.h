#ifndef MAIN_H
#define MAIN_H

#include "db.h"

// ** Startup functions

void usage();

int loadview();

void input_key(char *buf);

// ** Shutdown functions

void shutdown();

// ** DB interaction functions

int init_db(char *dbfile);

void do_add();

void do_edit(int index);

void titleFromInput(char *input, char *title, int len);

// ** curses functions
void init_curses();

void start_main_window(lok_item *notes, int num_notes);

void loop(WINDOW * menu_win, MENU * menu);

void print_centered(WINDOW * win, int starty, int startx, int width,
		     char *string, chtype color);

#endif

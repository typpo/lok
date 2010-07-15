#ifndef MAIN_H
#define MAIN_H

#include "db.h"

// ** Startup functions

void usage();

void input_key(char *buf);

void init_curses();

// ** curses functions

void start_main_window(lok_item *notes, int num_notes);

void loop(WINDOW *menu_win, MENU *menu);

void print_centered(WINDOW * win, int starty, int startx, int width,
		     char *string, chtype color);

#endif

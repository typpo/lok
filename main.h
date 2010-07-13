#ifndef MAIN_H
#define MAIN_H

// ** Startup functions

void usage();

void input_key(char *buf);

void init();

// ** curses functions

void start_main_window();

void loop(WINDOW *menu_win, MENU *menu);

void print_centered(WINDOW * win, int starty, int startx, int width,
		     char *string, chtype color);

#endif

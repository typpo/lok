#ifndef MAIN_H
#define MAIN_H

// info for showing things in menu
typedef struct lok_item {
    int id;
    char *title;
    char *last_edited;
    char *created;
} lok_item;

// ** Startup functions

void usage();

void input_key(char *buf);

void init();

// ** curses functions

void start_main_window();

void loop(WINDOW *menu_win, MENU *menu);

void print_in_middle(WINDOW * win, int starty, int startx, int width,
		     char *string, chtype color);

#endif

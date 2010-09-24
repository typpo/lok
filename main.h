#ifndef MAIN_H
#define MAIN_H

#include <curses.h>
#include <menu.h>
#include "db.h"

// menu references
typedef struct {
    MENU *menu;
    WINDOW *window;
    ITEM **items;
    int num_items;
} menu_handle_t;

// ** Startup functions

void usage();

char *input_key();

// ** Shutdown functions

void shutdown();

// ** DB interaction functions

int init_db(char *dbfile);

void do_add();

void do_edit(int index);

void titleFromInput(char *input, char *title);

int load_notes();

int reload_notes();

// ** curses functions
void init_curses();

int start_main_window();

void create_menu();

void clear_menu();

void loop();

void print_centered(WINDOW * win, int starty, int startx, int width,
		     char *string, chtype color);

#endif

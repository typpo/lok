#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <curses.h>
#include <menu.h>
#include "main.h"
#include "db.h"

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

// maximum characters in key
#define MAX_KEY_LEN 30

// number of notes to display at once
#define MAX_DISPLAY 50

// passkey
char key[MAX_KEY_LEN];

int main(int argc, char **argv)
{
	/*
	   if (argc != 2) {
	   usage();
	   return 1;
	   }
	 */
//    init();

	// get key
//    printw("Enter your key: ");
//    refresh();
//    input_key(key);

	// connect to db
	if (db_start("test.db", "testkey") < 0) {
		// TODO shutdown ncurses
        printf("Something fucked up\n");
		return 1;
	}
	//insert_note("testsubj", "testtext");
	//fetch_notes(handle, 0, NULL);

	// start main view
	//start_main_window();
	db_shutdown();
	return 0;
}

void input_key(char *buf)
{
	char key;
	char *ptr = buf;
	char *max = buf + MAX_KEY_LEN - 1;
	do {
		if (ptr >= max) {
			printf("\nReached key maximum\n");
			break;
		}

		key = getch();
		switch (key) {
		case (char) KEY_BACKSPACE:
			// backspace
			if (ptr > buf) {
				*(--ptr) = 0;
			}
			break;
		default:
			if (key > 31 && key < 127) {
				*(ptr++) = key;
			}
		}
	} while (key != '\n');
	*ptr = 0;

	// TODO get hash
}

void usage()
{
	printf("lok your_sqlite.db");
}

void init()
{
	// initialize curses
	initscr();
	start_color();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);
	init_pair(1, COLOR_RED, COLOR_BLACK);
	init_pair(2, COLOR_CYAN, COLOR_BLACK);
}

void start_main_window()
{
	// create menu window
	WINDOW *menu_win = newwin(24, 80, 0, 0);
	keypad(menu_win, TRUE);

	// create menu items
	int n_choices = 20;	//ARRAY_SIZE(choices);
	ITEM **items = (ITEM **) calloc(n_choices, sizeof(ITEM *));
	char **item_names = (char **) calloc(n_choices, sizeof(char *));
	for (int i = 0; i < n_choices; i++) {
		item_names[i] = malloc(sizeof(char) * 10);
		snprintf(item_names[i], 10, "%d!", i);
		items[i] = new_item(item_names[i], "");
	}

	// create menu
	MENU *menu = new_menu((ITEM **) items);

	// set menu main window and subwindow
	set_menu_win(menu, menu_win);

	// menu subwin height, width, and y,x coords
	set_menu_sub(menu, derwin(menu_win, 20, 78, 3, 1));

	// number of lines to show in menu, row to start al
	// keep height menu subwin height - 1
	set_menu_format(menu, 19, 1);

	/* Set menu mark to the string " * " */
	set_menu_mark(menu, " * ");

	// print border
	box(menu_win, 0, 0);

	// print heading
	print_centered(menu_win, 1, 0, 80, "lok", COLOR_PAIR(1));
	mvwhline(menu_win, 2, 1, ACS_HLINE, 78);

	// show menu
	post_menu(menu);
	wrefresh(menu_win);
	refresh();


	// process input input
	loop(menu_win, menu);

	// clean up
	unpost_menu(menu);
	free_menu(menu);
	for (int i = 0; i < n_choices; i++) {
		free_item(items[i]);
		free(item_names[i]);
	}
	free(items);
	free(item_names);
    db_shutdown();
	endwin();
}

// Loop for processing user input
void loop(WINDOW * menu_win, MENU * menu)
{
	int c;
	while ((c = wgetch(menu_win)) != 'q') {
		switch (c) {
		case KEY_DOWN:
		case 'j':
			menu_driver(menu, REQ_DOWN_ITEM);
			break;
		case KEY_UP:
		case 'k':
			menu_driver(menu, REQ_UP_ITEM);
			break;
		case KEY_NPAGE:
			menu_driver(menu, REQ_SCR_DPAGE);
			break;
		case KEY_PPAGE:
			menu_driver(menu, REQ_SCR_UPAGE);
			break;
		}
		wrefresh(menu_win);
	}
}

// Prints text in the middle of a window, taken from ncurses docs
void print_centered(WINDOW * win, int starty, int startx, int width,
		    char *string, chtype color)
{
	int length, x, y;
	float temp;

	if (win == NULL)
		win = stdscr;
	getyx(win, y, x);
	if (startx != 0)
		x = startx;
	if (starty != 0)
		y = starty;
	if (width == 0)
		width = 80;

	length = strlen(string);
	temp = (width - length) / 2;
	x = startx + (int) temp;
	wattron(win, color);
	mvwprintw(win, y, x, "%s", string);
	wattroff(win, color);
	refresh();
}

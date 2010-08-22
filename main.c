#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <curses.h>
#include <menu.h>
#include "main.h"
#include "db.h"
#include "writing.h"

// Maximum number of notes to load into display
#define MAX_DISPLAY 50

// Maximum number of characters in a note title.
// Longer titles are truncated.
#define MAX_TITLE_LEN 10

// Path to temporary file for editing
#define TEMP_FILE "tmp"

// Maximum number of characters in database password
#define MAX_KEY_LEN 30

// User-entered password for the database
char key[MAX_KEY_LEN];

// Stored notes information
lok_item *notes;
int num_notes;

int main(int argc, char **argv)
{
	/*
	   if (argc != 2) {
	   usage();
	   return 1;
	   }
	 */

	// Initialize backend
	// TODO use command line arg
	if (init_db("test.db")) {
        printf("DB connection failed.\n");
		return 1;
    }
    printf("db connection success.\n");

	// Start curses
	init_curses();
	// get key
    // printw("Enter your key: ");
    // refresh();
    // input_key(key);
	
	// get notes
	if (db_fetch_notes(0, &notes, &num_notes)) {
        printf("Couldn't get notes from db.\n");
		return 1;
	}

	// start view
	start_main_window(notes, num_notes);

	// Finish
	shutdown();

	return 0;
}

// Prints complex instructions on how to use this program.
void usage()
{
	printf("lok your_sqlite.db");
}

// Establishes a connection a specified sqlite database and makes sure the
// notes table exists.
int init_db(char *dbfile)
{
	// connect to db
	if (db_start(dbfile, "testkey") < 0) {
		return 1;
	}
	// make sure table exists
	db_create_table();

	return 0;
}

// Starts the curses environment.
void init_curses()
{
	initscr();
	start_color();
	cbreak();
	noecho();
	nonl();
	init_pair(1, COLOR_RED, COLOR_BLACK);
}

// Frees the notes list and closes the connection to the database.
void shutdown()
{
	// free notes list
	db_free_notes(notes, num_notes);
	db_shutdown();
}

// Reads user input while hiding what the user types.
// Used for password input.
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

	// TODO hash the key and get rid of the plaintext one
}

// Loads the notes view in the curses display.
// Blocks for the duration of the program.
void start_main_window(lok_item * notes, int num_notes)
{
	// create menu window
	WINDOW *menu_win = newwin(24, 80, 0, 0);
	keypad(menu_win, TRUE);

	// create menu items
	ITEM **items = (ITEM **) calloc(num_notes + 1, sizeof(ITEM *));
	for (int i = 0; i < num_notes; i++) {
		items[i] = new_item(notes[i].title, notes[i].id);
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

	// menu mark 
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

	// Process user input. This call will block for the duration
    // of the program.
	loop(menu_win, menu);

	// clean up curses
	unpost_menu(menu);
	free_menu(menu);
	for (int i = 0; i < num_notes; i++) {
		free_item(items[i]);
	}
	free(items);
	endwin();
}

// Loop for processing user input.
// Blocks for the duration of the program.
void loop(WINDOW * menu_win, MENU * menu)
{
	int c;
    int index;
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
		case 'a':	// add
			do_add();
			break;
		case 'e':	// edit
            index = item_index(current_item(menu));
            if (index > -1) {
                do_edit(index);
            }
			break;
		}
		wrefresh(menu_win);
	}
}

// Handles switch to vim and back to curses for adding a new note.
void do_add()
{
	def_prog_mode();
	endwin();

	// add
	char *input;
	if (editor_do(TEMP_FILE, "", &input) < 0) {
		// error of some kind
		if (!input) {
			free(input);
		}
		refresh();
		reset_prog_mode();
		return;
	}

    // read title
    char title[MAX_TITLE_LEN];
    titleFromInput(input, title, MAX_TITLE_LEN);

	db_insert_note(title, input);
	free(input);

	refresh();
	reset_prog_mode();
}

// Handles switch to vim and back to curses for editing an existing note.
void do_edit(int index)
{
	// save and close curses
	def_prog_mode();
	endwin();

	// edit
	char *input = 0;

	// let user perform edit
    printf("Editing %d\n", index);
	int edit_result = editor_do(TEMP_FILE, notes[index].text, &input);
	if (edit_result < 0) {
		goto restore;
	} else if (edit_result != 0) {
        // read title
        char title[MAX_TITLE_LEN];
        titleFromInput(input, title, MAX_TITLE_LEN);

        // write to db
		db_edit_note(notes[index].id, title, input);
	}
	// if edit_result is 0, then no change has been made.
	free(input);

      restore:
	// restores curses ui
	refresh();
	reset_prog_mode();
}

// Takes the first line from input and puts it in title, for a maximum of 
// len characters.
void titleFromInput(char *input, char *title, int len)
{
	// TODO what if there's no newline?
	int newline = strcspn(input, "\n");
	strncpy(title, input, newline < len ? newline : len);
}

// Prints text in the middle of a window.
// This function is taken from ncurses docs.
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

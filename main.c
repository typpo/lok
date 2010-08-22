#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <curses.h>
#include <menu.h>
#include "main.h"
#include "db.h"
#include "writing.h"

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

// maximum characters in key
#define MAX_KEY_LEN 30

// number of notes to display at once
#define MAX_DISPLAY 50

// name of temporary file for read/writes
#define TEMP_FILE "tmp"

// passkey
char key[MAX_KEY_LEN];

// stored notes
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
	// get key
//    printw("Enter your key: ");
//    refresh();
//    input_key(key);
    
    // Initialize backend
    if (init_db())
        return -1;

    // Start UI
    init_curses();
    loadview();

    // Finish
    shutdown();

	return 0;
}

int init_db()
{
	// connect to db
	if (db_start("test.db", "testkey") < 0) {
		return 1;
	}

    // make sure table exists
    db_create_table();

    // insert test row
    // db_insert_note("Placeholder", "Placeholder\n\nsome text");
    return 0;
}
void init_curses()
{
	// initialize curses
	initscr();
	start_color();
	cbreak();
	noecho();
    nonl();
	init_pair(1, COLOR_RED, COLOR_BLACK);
}

int loadview()
{
    // get notes
	if (db_fetch_notes(0, &notes, &num_notes) < 0) {
        return -1;
    }

	// start view
	start_main_window(notes, num_notes);
    return 0;
}

void shutdown()
{
    // free notes list
    db_free_notes(notes, num_notes);
	db_shutdown();
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

void start_main_window(lok_item *notes, int num_notes)
{
	// create menu window
	WINDOW *menu_win = newwin(24, 80, 0, 0);
	keypad(menu_win, TRUE);

	// create menu items
	ITEM **items = (ITEM **) calloc(num_notes+1, sizeof(ITEM *));
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

	// process input input
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
        case 'a':   // add
            do_add();
            break;
        case 'e':   // edit
            do_edit(item_index(current_item(menu)));
            break;
		}
		wrefresh(menu_win);
	}
}

void do_add() {
    def_prog_mode();
    endwin();

    // add
    char *input;
    if (editor_do(TEMP_FILE, "", &input) < 0) {
        // error of some kind
        printf("Nothing added\n");
        if (!input) {
            free(input);
        }
        refresh();
        reset_prog_mode();
        return;
    }
    printf("Something added\n");

    // grab title
    // TODO what if there's no newline?
    // TODO generalize this (edit does it too)
    int newline = strcspn(input, "\n");
    char title[newline+1];
    strncpy(title, input, newline);

    db_insert_note(title, input);
    free(input);

    refresh();
    reset_prog_mode();
}

void do_edit(int index) {
    // save and close curses
    def_prog_mode();
    endwin();

    // edit
    printf("Editing\n");
    char *input = 0;

    // let user perform edit
    int edit_result = editor_do(TEMP_FILE, notes[index].text, &input);
    if (edit_result < 0) {
        printf("Error editing\n");
        goto restore;
    }
    else if (edit_result != 0) {
        printf("Processing edit...");
        // grab title
        int newline = strcspn(input, "\n");
        // TODO check if no input
        /*
        if (newline < 1) {
            printf("No input");
            char *title = "No title";
        }
        else {
        */
            char title[newline+1];
            strncpy(title, input, newline);
        //}

        db_edit_note(notes[index].id, title, input);
    }
    // if edit_result is 0, then no change has been made.
    free(input);

    restore:
    // restores curses ui
    refresh();
    reset_prog_mode();
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

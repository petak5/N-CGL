#include <stdlib.h>
#include <ncurses.h>
#include <signal.h>

void exit_handler() {
	endwin();
}

int main(void)
{
    initscr();
	cbreak(); //disable line buffering so stuff is draw even without \n
	noecho();
	clear();
	curs_set(0); //disable cursor

	signal(SIGSEGV, exit_handler);

	WINDOW * win = newwin(LINES, COLS, 0, 0); //make the window full screen
	nodelay(win, TRUE); //make input non-blocking
	keypad(win, TRUE); //Needs to be set to true or mouse events just come in as garbage
	mousemask(BUTTON1_RELEASED | REPORT_MOUSE_POSITION, NULL);
	mouseinterval(0);


	refresh();

	FILE* logFile = fopen("log.txt", "w");
	if (logFile == NULL) 
    { 
		endwin();
        exit(-1);
    }

	fprintf(logFile, "Started, has mouse: %d\n", has_mouse());
	fflush(logFile);

	while(true) {
		int ch = wgetch(win);
    	if(ch == KEY_MOUSE) {
			MEVENT mouseEvent;
			if(getmouse(&mouseEvent) == OK) {
				fprintf(logFile, "Mouse (%d, %d, %d)\n", mouseEvent.x, mouseEvent.y, mouseEvent.z);
				fflush(logFile);

				wmove(win, mouseEvent.y, mouseEvent.x);
				waddch(win, 'A'); //Test print
			}
		} else if(ch != ERR) {
			fprintf(logFile, "Event %d\n", ch);
			fflush(logFile);
		}

		//box(win, 0, '*');
		//mvwprintw(stdscr, 1, 1, "Hi there :)");
		wrefresh(stdscr);
	}


    return 0;
}

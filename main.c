#include <unistd.h>
#include <stdlib.h>
#include <ncurses.h>
#include <signal.h>

void exit_handler() {
	endwin();
}

void processInput(char (*grid)[LINES][COLS], WINDOW *win);
void clear_grid(char (* grid)[LINES][COLS]);
void print_grid(char (* grid)[LINES][COLS], WINDOW * win);

FILE* logFile;

#define ALIVE_CHAR '*'
#define DEAD_CHAR ' '

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

	logFile = fopen("log.txt", "w");
	if (logFile == NULL) 
    { 
		endwin();
        exit(-1);
    }

	char grid[LINES][COLS];

	clear_grid(&grid);

	while(TRUE)
	{
		processInput(&grid, win);
		print_grid(&grid, win);
		wrefresh(win);
	}

	fprintf(logFile, "Started, has mouse: %d\n", has_mouse());
	fflush(logFile);

    return 0;
}

void processInput(char (*grid)[LINES][COLS], WINDOW *win) {
	int ch = wgetch(win);
	if(ch == KEY_MOUSE) {
		MEVENT mouseEvent;
		if(getmouse(&mouseEvent) == OK) {
			fprintf(logFile, "Mouse (%d, %d, %d)\n", mouseEvent.x, mouseEvent.y, mouseEvent.z);
			fflush(logFile);

			(* grid)[mouseEvent.y][mouseEvent.x] = ALIVE_CHAR;
		}
	}
}

void clear_grid(char (* grid)[LINES][COLS])
{
	(* grid)[0][0] = 0;
	for (int i = 0; i < LINES; i++) {
		for (int j = 0; j < COLS; j++) {
			(* grid)[i][j] = DEAD_CHAR;
		}
	}
}

void print_grid(char (* grid)[LINES][COLS], WINDOW * win)
{
	for (int i = 0; i < LINES; i++) {
		for (int j = 0; j < COLS; j++) {
			mvwprintw(win, i, j, "%c", (* grid)[i][j]);
		}
	}
}

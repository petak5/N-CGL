#include <unistd.h>
#include <ncurses.h>

void clear_grid(char (* grid)[LINES][COLS]);
void print_grid(char (* grid)[LINES][COLS], WINDOW * win);


int main(void)
{
    initscr();
	cbreak(); //disable line buffering so stuff is draw even without \n
	noecho();
	clear();

	WINDOW * win = newwin(LINES, COLS, 0, 0); //make the window full screen
	refresh();	

	char grid[LINES][COLS];

	clear_grid(&grid);

	grid[3][5] = ' ';

	while(TRUE)
	{
		print_grid(&grid, win);
		wrefresh(win);
		sleep(2);	// 2 seconds
	}

	endwin();

    return 0;
}

void clear_grid(char (* grid)[LINES][COLS])
{
	(* grid)[0][0] = 0;
	for (int i = 0; i < LINES; i++) {
		for (int j = 0; j < COLS; j++) {
			(* grid)[i][j] = '*';
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

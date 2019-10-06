#include <ncurses.h>

int main(void)
{
    initscr();
	cbreak(); //disable line buffering so stuff is draw even without \n
	noecho();
	clear();

	WINDOW * win = newwin(LINES, COLS, 0, 0); //make the window full screen
	refresh();

	box(win, 0, '*');
	mvwprintw(win, 1, 1, "Hi there :)");
	wrefresh(win);
	
	getch();


	endwin();

    return 0;
}

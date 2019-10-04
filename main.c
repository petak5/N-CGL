#include <ncurses.h>

int main(void)
{
    initscr();
	noecho();

	int height, width, start_y, start_x;
	height = 10;
	width = 20;
	start_y = start_x = 10;

	WINDOW * win = newwin(height, width, start_y, start_x);
	refresh();

	box(win, 0, '*');
	mvwprintw(win, 1, 1, "Hi there :)");
	wrefresh(win);
	
	getch();


	endwin();

    return 0;
}

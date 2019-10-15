#define _DEFAULT_SOURCE
#include <unistd.h>
#include <stdlib.h>
#include <ncurses.h>
#include <signal.h>
#include <sys/time.h>
#include <string.h>

void exit_handler() {
	endwin();
}

void processInput(char (*grid)[LINES][COLS], WINDOW *win);
void clear_grid(char (* grid)[LINES][COLS]);
void iterate_grid(char (* grid)[LINES][COLS]);
void print_grid(char (* grid)[LINES][COLS], WINDOW * win);
int neighbours_count(char (* grid)[LINES][COLS], int y, int x);

FILE* logFile;

#define ALIVE_CHAR '*'
#define DEAD_CHAR ' '

int simulation_interval_millis = 20;

int paused;

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


	/* A glider
	grid[10][10] = ALIVE_CHAR;
	grid[12][10] = ALIVE_CHAR;
	grid[12][11] = ALIVE_CHAR;
	grid[11][11] = ALIVE_CHAR;
	grid[11][12] = ALIVE_CHAR;
	*/

	struct timeval lastUpdate, currentTime;
	memset(&lastUpdate, 0, sizeof(lastUpdate));

	while(TRUE)
	{
		processInput(&grid, win);
		print_grid(&grid, win);
		wrefresh(win);

		gettimeofday(&currentTime, NULL);
		float millisDiff = (currentTime.tv_sec - lastUpdate.tv_sec) * 1000.0f + (currentTime.tv_usec - lastUpdate.tv_usec) / 1000.0f;
		
		if(millisDiff > simulation_interval_millis && !paused) {
			memcpy(&lastUpdate, &currentTime, sizeof(currentTime));
			iterate_grid(&grid);

			fprintf(logFile, "Diff: %f\n", millisDiff);
			fflush(logFile);
		}

		//TODO Maybe: Replace usleep since it's deprecated
		usleep(16000); //cap it at 60 fps (1000 millis / 60 = 16.66 ms)
	}

    return 0;
}

void processInput(char (*grid)[LINES][COLS], WINDOW *win) {
	int ch = wgetch(win);
	if(ch == KEY_MOUSE) {
		MEVENT mouseEvent;
		if(getmouse(&mouseEvent) == OK) {
			fprintf(logFile, "Mouse (%d, %d, %d)\n", mouseEvent.x, mouseEvent.y, mouseEvent.z);
			fflush(logFile);

			
			char *cell = &(* grid)[mouseEvent.y][mouseEvent.x];
			*cell = *cell == ALIVE_CHAR ? DEAD_CHAR : ALIVE_CHAR;
		}
	} else if(ch == ' ') {
		paused = !paused;
	} else if(ch == '-') {
		if(simulation_interval_millis < 1) { //if it's 0, then doubling it is useless
			simulation_interval_millis = 1;
		} else {
			simulation_interval_millis *= 2; //Slow it down
		}
	} else if(ch == '=') {
		simulation_interval_millis /= 2; //Speed it up
	} else if(ch == 'q') {
		endwin();
		exit(0);
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

void iterate_grid(char (* grid)[LINES][COLS])
{
	// An array consisting of the number of alive cells for each cell
	char temp_count[LINES][COLS];

	// Get the alive neighbour cells counts populated into the array
	for (int i = 0; i < LINES; i++) {
		for (int j = 0; j < COLS; j++) {
			temp_count[i][j] = neighbours_count(grid, i, j);
		}
	}

	// Apply the rules and update the board
	for (int i = 0; i < LINES; i++) {
		for (int j = 0; j < COLS; j++) {
			if (temp_count[i][j] < 2) {
				(* grid)[i][j] = DEAD_CHAR;
			}
			if (temp_count[i][j] > 3) {
				(* grid)[i][j] = DEAD_CHAR;
			}
			if (temp_count[i][j] == 3) {
				(* grid)[i][j] = ALIVE_CHAR;
			}
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

/* Returns the number of alive cels next to [x, y] cell */
int neighbours_count(char (* grid)[LINES][COLS], int y, int x)
{
	int count = 0;

	/* Horizontal */
	// Left
	if (x > 0) {
		if ((* grid)[y][x - 1] == ALIVE_CHAR) {
			count++;
		}
	}
	// Right
	if (x < COLS) {
		if ((* grid)[y][x + 1] == ALIVE_CHAR) {
			count++;
		}
	}
	/* Vertical */
	// Top
	if (y > 0) {
		if ((* grid)[y - 1][x] == ALIVE_CHAR) {
			count++;
		}
	}
	// Bottom
	if (y < LINES) {
		if ((* grid)[y + 1][x] == ALIVE_CHAR) {
			count++;
		}
	}
	/* Diagonal */
	// Top Left
	if ((x > 0) && (y > 0)) {
		if ((* grid)[y - 1][x - 1] == ALIVE_CHAR) {
			count++;
		}
	}
	// Top Right
	if ((x < COLS) && (y > 0)) {
		if ((* grid)[y - 1][x + 1] == ALIVE_CHAR) {
			count++;
		}
	}
	// Bottom Left
	if ((x > 0) && (y <LINES)) {
		if ((* grid)[y + 1][x - 1] == ALIVE_CHAR) {
			count++;
		}
	}
	// Bottm Right
	if ((x < COLS) && (y < LINES)) {
		if ((* grid)[y + 1][x + 1] == ALIVE_CHAR) {
			count++;
		}
	}

	return count;
}

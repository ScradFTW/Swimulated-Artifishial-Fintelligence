#define WIDTH 30
#define LENGTH 30
#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3
#define SLEEP_CYCLE 5000000

#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

char map[WIDTH][LENGTH];

typedef struct fish_props
{
    int x;
    int y;
    int px;
    int py;
    int direction;
    int pDirection;
    int happiness;
    int hunger;
    unsigned long long lifecycles;
} fish;

void initMap()
{
    for (int x = 0; x < WIDTH; x++)
	for (int y = 0; y < LENGTH; y++)
	    if (y == 0 || y == LENGTH - 1 || x == WIDTH - 1)
		map[x][y] = '+';
	    else
		map[x][y] = ' ';

}

int nearBounds(int x, int y)
{
    for (int n = 0; n <= 1; n++)
	for (int i = 0; i <= 1; i++)
	    if (map[x+n][y+i] != ' ')
		return 0;
	
    return 1;
}

void printFish(fish* pf)
{
    mvaddch(pf->py, pf->px, ' ');
    
    switch (pf->pDirection)
    {
    case UP:
	mvaddch(pf->py + 1, pf->px, ' ');
	break;

    case DOWN:
	mvaddch(pf->py - 1, pf->px, ' ');
	break;

    case LEFT:
	mvaddch(pf->py, pf->px + 1, ' ');
	break;

    case RIGHT:
	mvaddch(pf->py, pf->px - 1, ' ');
	break;
    }

    mvaddch(pf->y, pf->x, 'o');


    switch(pf->direction)
    {
    case UP:
	mvaddch(pf->y + 1, pf->x, '^');
	break;

    case DOWN:
	mvaddch(pf->y - 1, pf->x, 'v');
	break;

    case LEFT:
	mvaddch(pf->y, pf->x + 1, '<');
	break;

    case RIGHT:
	mvaddch(pf->y, pf->x - 1, '>');
	break;
    }
    
    move(0, 0);
}

void printMap()
{    
    for (int x = 0; x < WIDTH; x++)
    {
	for (int y = 0; y < LENGTH; y++)
	    printw("%c", map[x][y]);

	printw("\n");
    }
    
}

void nextPosition(fish* pf)
{
    int x = pf->x;
    int y = pf->y;
    int x_dir = 0;
    int y_dir = 0;
    srand(time(NULL));
	
    x_dir = (rand() % 3) - 1;
    y_dir = (rand() % 3) - 1;

    x += x_dir;
    y += y_dir;

    if (x == WIDTH - 2 || y == LENGTH - 2 
	|| x == 1 || y == 0)
    {
	x_dir *= -2;
	y_dir *= -2;
	x += x_dir;
	y += y_dir;
    }

    if (map[x][y] != ' ')
	exit(1);
    
    pf->px = pf->x;
    pf->py = pf->y;
    pf->pDirection = pf->direction;

    pf->x = x;
    pf->y = y;
    if (x_dir == 1)
	pf->direction = RIGHT;
    else if (x_dir == -1)
	pf->direction = LEFT;

    else if (y_dir == 1)
	pf->direction = DOWN;
    else if (y_dir == -1)
	pf->direction = UP;

    pf->lifecycles++;
}	


void death(fish* pf)
{
    pf->direction = DOWN;

    while (map[pf->x][pf->y--] == ' ')
	printFish(pf);

}


int main()
{    
    initscr();
    raw();
    
    char ch;
    fish pf;
    pf.x = 10;
    pf.y = 10;
    pf.happiness = 8;
    pf.direction = UP;
    pf.lifecycles = 0;
    pf.hunger = 0;

    initMap();
    
    printMap(map);
    
    for (;;) 
    {
	nextPosition(&pf);
	printFish(&pf);

	//if f is pressed, drop food
       
	if (pf.happiness)
	    usleep(SLEEP_CYCLE/pf.happiness);
	else
	    death(&pf);

	refresh();
    }
    
    

    getch();
    endwin();
    return EXIT_SUCCESS;
}

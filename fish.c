#define WIDTH 100
#define LENGTH 100
#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3

#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>

int oppositeDirection(int x_dir, int y_dir, int pDir)
{
    
    if ((x_dir == 1 && pDir == LEFT)
	|| (x_dir == -1 && pDir == RIGHT)
	|| (y_dir == 1 && pDir == UP)
	|| (y_dir == -1 && pDir == DOWN))
	return 1;

    return 0;
}

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
    int lifecycles;
} fish;

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

void printMap(char map[WIDTH][LENGTH])
{    
    for (int x = 0; x < WIDTH; x++)
    {
	for (int y = 0; y < LENGTH; y++)
	    printw("%c", map[x][y]);

	printw("\n");
    }
    
}

void nextPosition(fish* pf, char map[WIDTH][LENGTH])
{
    int x_dir;
    int y_dir;

    for (;;)
    {	
	x_dir = (random() % 3) - 1;
	y_dir = (random() % 3) - 1;

	if (!oppositeDirection(x_dir, y_dir, pf->direction))
	    break;
    }

    pf->px = pf->x;
    pf->py = pf->y;
    pf->pDirection = pf->direction;

    pf->x += x_dir;
    pf->y += y_dir;
    
    if (x_dir == 1)
	pf->direction = RIGHT;
    else if (x_dir == -1)
	pf->direction = LEFT;

    if (y_dir == 1)
	pf->direction = DOWN;
    else if (y_dir == -1)
	pf->direction = UP;
	

    if (map[pf->x][pf->y] != ' ')
    {
	pf->x = pf->px;
	pf->y = pf->py;
	pf->direction = pf->pDirection;
	nextPosition(pf, map);
    }

    pf->lifecycles++;
}


void death(fish* pf, char map[WIDTH][LENGTH])
{
    pf->direction = DOWN;

    while (map[pf->x][pf->y--] == ' ')
     	printFish(pf);

}


int main()
{    
    initscr();
    raw();
    
    fish pf;
    pf.x = 10;
    pf.y = 10;
    pf.happiness = 8;
    pf.direction = UP;
    pf.lifecycles = 0;
    pf.hunger = 0;

    char map[WIDTH][LENGTH] = { 
#include "initMap.txt" 
    }; 
    
    printMap(map);
    
    for (;;) 
    {
	nextPosition(&pf, map);
	printFish(&pf);

	if (pf.happiness)
	    usleep(5000000/pf.happiness);
	else
	    death(&pf, map);

	refresh();
    }
    
    

    getch();
    endwin();
    return EXIT_SUCCESS;
}

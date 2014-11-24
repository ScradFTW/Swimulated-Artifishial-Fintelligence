#define WIDTH 30
#define HEIGHT 30
#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3
#define SLEEP_CYCLE 280000
#define TRUE 1
#define FALSE 0
#define BOOL int

#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include "food.h"

char map[WIDTH][HEIGHT];

typedef struct location_props
{
    int x;
    int y;
} location;

typedef struct food_list
{
    int x;
    int y;
    struct food_list* next;
    struct food_list* prev;
} food;

BOOL addFood(food* fd, int x_pos)
{
    food* newFd = (food*) malloc(sizeof(food));
    food* current;

    newFd->x = x_pos;
    newFd->y = 0;
    newFd->next = NULL;
    newFd->prev = NULL;
    

    current = fd;
    while (TRUE)
    {
	if (current->next == NULL)
	{
	    current->next = newFd;
	    newFd->prev = current;
	    break;
	}
	else
	    current = current->next;
    }
    

    fd = current;

    return TRUE;
}

BOOL deleteFood(food* fd)
{
    if (fd == NULL)
	return FALSE;
 
    if (fd->prev != NULL && fd->next != NULL) //node inbetween two others       
    {
	fd->next->prev = fd->next;
	fd->prev->next = fd->prev;
    }
    else if (fd->prev != NULL && fd->next == NULL) //last node
	fd->prev->next = NULL;
    else if (fd->prev == NULL && fd->next != NULL) //first node
	fd->next->prev = NULL;


    free(fd);
    return TRUE;
}

BOOL foodAt(food* fd, int pfx, int pfy)
{
    food* current = fd;
    while (current != NULL && pfx != 0 && pfy != 0)
    {
	if (current->x == pfx && current->y == pfy)
	    return TRUE;
	
	current = current->next;
    }
	
    return FALSE;
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
    unsigned long long lifecycles;
} fish;


void initMap()
{
    for (int x = 0; x < WIDTH; x++)
        for (int y = 0; y < HEIGHT; y++)
            if (y == 0 || y == HEIGHT - 1 || x == WIDTH - 1)
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

    move(1, 32);
    printw("Life cycles:  %d", pf->lifecycles);

    mvaddch(32, 0, ' ');
}

void printMap()
{
    for (int x = 0; x < WIDTH; x++)
    {
        for (int y = 0; y < HEIGHT; y++)
            printw("%c", map[x][y]);

        printw("\n");
    }
}

void nextPosition(fish* pf, int set_x_dir, int set_y_dir)
{
    int x = pf->x;
    int y = pf->y;
    int x_dir = 0;
    int y_dir = 0;
    srand(time(NULL));

    if (set_x_dir == 0 && set_y_dir == 0)
    {
	x_dir = (rand() % 3) - 1;
	y_dir = (rand() % 3) - 1;

	x += x_dir;
	y += y_dir;	
    }
    else
    {
	x += set_x_dir;
	y += set_y_dir;
    }

    if (x == WIDTH - 2 || y == HEIGHT - 2
	|| x == 1 || y == 0)
    {
        x_dir *= -2;
        y_dir *= -2;
        x += x_dir;
        y += y_dir;
    }

    pf->px = pf->x;
    pf->py = pf->y;
    pf->pDirection = pf->direction;

    pf->x = x;
    pf->y = y;
    if (x_dir == 1)
        pf->direction = RIGHT;
    else if (x_dir == -1)
        pf->direction = LEFT;

    if (y_dir == 1)
        pf->direction = DOWN;
    else if (y_dir == -1)
        pf->direction = UP;

    pf->lifecycles++;
}

location* pathTo(fish* pf, int fdx, int fdy)
{
    location* loc = malloc(sizeof(location));
    
    if (pf->y == fdy)
    {
	if (pf->x > fdx)
	{
	    loc->x = -1;
	    loc->y = 1;
	    return loc;
	}
	else if (pf->x < fdx)
	{
	    loc->x = 1;
	    loc->y = 1;
	    return loc;
	}
    }
    else if (pf->y > fdy)
    {
	if (pf->x == fdx)
	{
	    loc->x = 0;
	    loc->y = -1;
	    return loc;
	}
	else if (pf->x > fdx)
	{
	    loc->x = -1;
	    loc->y = -1;
	    return loc;
	}	
	else if (pf->x < fdx)
	{
	    loc->x = 1;
	    loc->y = -1;
	    return loc;
	}
    }
	
    return NULL;
}

location* findFood(fish* pf, food* fd)
{
    int searchWidth = HEIGHT - pf->y;

    for (int h = pf->y; h >= 0; h--)
    {
	for (int w = 0 - searchWidth; w < searchWidth; w++)
	{
	    if (foodAt(fd, pf->x + w, h))
	    {
		location* loc;
		return loc = pathTo(pf, pf->x + w, h);
	    }
	}

	searchWidth++;
    }


    return NULL;
}

int dropFood(food* fList, fish* pf)
{
    food* current = fList;
    if (current == NULL)
	return FALSE;

    while (current != NULL)
    {
	if (current->y != 0)
	    mvaddch(current->y - 1, current->x, ' ');
	
	if (current->x > 0)
	    mvaddch(current->y++, current->x, '*');

	if (current->y >= HEIGHT - 1 
	    || (pf->x == current->x && pf->y == current->y))
	{
	    mvaddch(current->y - 1, current->x, ' ');
	    deleteFood(current);
	}

	current = current->next;
    }

	

    return TRUE;
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
    cbreak();
    timeout(1);

    char ch;
    food* fList = malloc(sizeof(food));
    fish pf;
    int randX;
    location* loc;

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
	if ((loc = findFood(&pf, fList)) != NULL)
	    nextPosition(&pf, loc->x, loc->y);
	else
	    nextPosition(&pf, 0 , 0);

	ch = getch();
        if (ch == 'f')
        {
	    randX = (rand() % (WIDTH - 4)) + 3;
	    addFood(fList, randX);
        }
        else if (ch == 'q')
        {
            nocbreak();
            return EXIT_SUCCESS;
        }

	dropFood(fList, &pf);
	printFish(&pf);
	
	usleep(SLEEP_CYCLE);
        refresh();

    }

    getch();
    endwin();
    return EXIT_SUCCESS;
}

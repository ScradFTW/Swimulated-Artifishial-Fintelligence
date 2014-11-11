#define WIDTH 30
#define LENGTH 30
#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3
#define SLEEP_CYCLE 3500000
#define TRUE 1
#define FALSE 0
#define BOOL int

#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <math.h>

char map[WIDTH][LENGTH];

/*
 * Keeps track of the x and y position of a piece of food
 * and whether it exists or not.
 */ 
typedef struct food_props
{
    int x;
    int y;
    BOOL exists;
} food;

/*
 * 
 *
 *
 */ 
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

    move(1, 32);
    printw("Life cycles:  %d", pf->lifecycles);

    mvaddch(32, 0, ' ');
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

int distance(food* fd, fish* pf)
{
    int x_dist = abs(fd->x - pf->x);
    int y_dist = abs(fd->y - pf->y);

    return (int)sqrt(pow(x_dist, 2) + pow(y_dist, 2));
}

BOOL reachable(food* fd, fish* pf)
{
    if (fd->y > pf->y)
	return FALSE;
    else if (fd->y - pf->y >= pf->x - fd->x)
	return TRUE;

    return TRUE;
}

BOOL findFood(food fd[WIDTH], fish* pf, int fCount)
{
    int sd = 0;
    int d; 
    int closestFood = -1;

    for (int i = 0; i < fCount; i++)
	if (reachable(&fd[i], pf) 
	    && (d = distance(&fd[i], pf)) < sd)
	{
	    sd = d;
	    closestFood = i;
	}

    food* nearestFood = &fd[closestFood];

    if (closestFood == -1)
	return FALSE;

    
    if (pf->x > nearestFood->x && pf->x - nearestFood->x == sd)
	pf->x--;
    else if (nearestFood->x > pf->x && nearestFood->x - pf->x == sd)
	pf->x++;
    
    return TRUE;
}

int dropFood(food fd[WIDTH], fish* pf, int fCount)
{
    for (int i = 0; i < fCount; i++)
    {
	if ((fd[i].x == pf->x && fd[i].y == pf->y)
	    || fd[i].y >= WIDTH - 2)
	{
	    mvaddch(fd[i].y, fd[i].x, ' ');
	    fd[i].exists = FALSE;
	    fCount--;
	}

	mvaddch(fd[i].y, fd[i].x, ' ');

	if (fd[i].exists)
	{
	    fd[i].y++;
	    mvaddch(fd[i].y, fd[i].x, '*');
	}
    }

    return fCount;
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
    int fCount = 0;
    food fd_ary[WIDTH];
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

        //drop food when f is pressed
        ch = getch();
        if (ch == 'f')
        {
	    mvprintw(0, 0, "%d", fCount);
	    srand(time(NULL));
	    fd_ary[fCount].exists = TRUE;
	    fd_ary[fCount].x = (rand() % WIDTH - 2) + 3;
	    fd_ary[fCount].y = 0;
	    fCount++;
	    
        }
        else if (ch == 'q')
        {
            nocbreak();
            return EXIT_SUCCESS;
        }

        if (fCount > 0)
            fCount = dropFood(fd_ary, &pf, fCount);

        printFish(&pf);

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

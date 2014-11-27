#define WIDTH 40
#define HEIGHT 40
#define MAX_FOOD_SIZE WIDTH * HEIGHT
#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3
#define MAX_HAPPINESS 10
#define SLEEP_CYCLE 40000
#define TRUE 1
#define FALSE 0
#define BOOL int

#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <math.h>

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
} food;

BOOL addFood(food* fList[MAX_FOOD_SIZE], int x_pos)
{
    for (int i = 0; i < MAX_FOOD_SIZE; i++)
	if (fList[i] == NULL)
	{
	    fList[i] = malloc(sizeof(food));
	    fList[i]->x = x_pos;
	    fList[i]->y = 0;
	    return TRUE;
	}
    
    return FALSE;
}

BOOL deleteFood(food* fList[MAX_FOOD_SIZE], int i)
{
    if (fList[i] == NULL)
	return FALSE;
    else
	fList[i] = NULL;

    for (int n = i; fList[n + 1] != NULL; n++)
	fList[n] = fList[n + 1];

    return TRUE;
}

BOOL foodAt(food* fList[MAX_FOOD_SIZE], int fdx, int fdy)
{
    for (int i = 0; fList[i] != NULL; i++)
	if (fList[i]->x == fdx && fList[i]->y == fdy)
	    return TRUE;
	
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
    unsigned long long lifecycles;
} fish;


void initMap()
{
    for (int x = 0; x < WIDTH; x++)
        for (int y = 0; y < HEIGHT; y++)
            if (x == 0 || x == WIDTH - 1 || y == HEIGHT - 1)
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

    mvaddch(HEIGHT + 2, 0, ' ');
}

void printMap()
{
    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
            printw("%c", map[x][y]);

        printw("\n");
    }
}

void nextPosition(fish* pf, int set_x_dir, int set_y_dir)
{
    int pfx = pf->x;
    int pfy = pf->y;
    int x_dir = 0;
    int y_dir = 0;
    srand(time(NULL));

    if (set_x_dir == 0 && set_y_dir == 0)
    {
	x_dir = (rand() % 3) - 1;
	y_dir = (rand() % 3) - 1;	
    }
    else
    {       
	x_dir = set_x_dir;
	y_dir = set_y_dir;
    }

    pfx += x_dir;
    pfy += y_dir;

    if (pfx == WIDTH - 1 || pfy == HEIGHT - 1
	|| pfx == 1 || pfy == 0)
    {
        x_dir *= -2;
        y_dir *= -2;
        pfx += x_dir;
        pfy += y_dir;
    }

    pf->px = pf->x;
    pf->py = pf->y;
    pf->pDirection = pf->direction;

    pf->x = pfx;
    pf->y = pfy;
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
    int pfx = pf->x;
    int pfy = pf->y;
    location* loc = malloc(sizeof(location));
    
#ifdef DEBUGF
    mvprintw(7, WIDTH + 5, "                                                  ");
    mvprintw(8, WIDTH + 5, "                                                  ");
    mvprintw(7, WIDTH + 5, "Food:   fdx = %d,   fdy = %d", fdx, fdy);
    mvprintw(8, WIDTH + 5, "Fish: pf->x = %d, pf->y = %d", pfx, pfy); 
    move(HEIGHT + 1, 0);
#endif

    if (pfy == fdy)
    {
	if (pfx > fdx)
	{
	    loc->x = -1;
	    loc->y = 1;
	    return loc;
	}
	else if (pfx < fdx)
	{
	    loc->x = 1;
	    loc->y = 1;
	    return loc;
	}
	else
	    loc->x = 0;
	loc->y = -1;
    }
    else if (pfy > fdy)
    {
	if (pfx == fdx)
	{
	    loc->x = 0;
	    loc->y = -1;
	    return loc;
	}
	else if (pfx > fdx)
	{
	    loc->x = -1;
	    loc->y = -1;
	    return loc;
	}	
	else if (pfx < fdx)
	{
	    loc->x = 1;
	    loc->y = -1;
	    return loc;
	}
    }
	
    return NULL;
}

location* findFood(fish* pf, food* fList[MAX_FOOD_SIZE])
{
    if (fList[0] == NULL)
	return NULL;

    int pfy = pf->y;
    int pfx = pf->x;
    int searchWidth = HEIGHT - 1 - pf->y;
    location* loc;

    for (int h = pfy; h >= 0; h--)
    {
	for (int w = pfx - searchWidth; w < pfx + searchWidth; w++)
	{
	    if (w > 1 && w < WIDTH - 1
		&& foodAt(fList, w, h))
		return loc = pathTo(pf, w, h);

#ifdef DEBUGF
	    if (w > 1 && w < WIDTH - 1)
		mvaddch(h, w, '#');
#endif
	}

	searchWidth++;
    }
		 
    return NULL;
}

void dropFood(food* fList[MAX_FOOD_SIZE], fish* pf)
{
    int pfx = pf->x;
    int pfy = pf->y;
    int fdx, fdy;

    if (fList[0] == NULL)
	return ;

    for (int i = 0; fList[i] != NULL; i++)
    {
	fList[i]->y++;
	fdx = fList[i]->x;
	fdy = fList[i]->y;

	if (fdy > 1)
	    mvaddch(fdy - 1, fdx, ' ');
	

	if (fdy > HEIGHT - 2)
	{
	    deleteFood(fList, i);
	    if (fList[0] == NULL)
		break;
	}
	else if (fdx == pfx && (fdy == pfy || fdy + 1 == pfy))
	{
	    deleteFood(fList, i--);
	    if (pf->happiness < MAX_HAPPINESS)
		pf->happiness++;

	    pf->lifecycles = 0;

	    if (fList[0] == NULL)
		break;
	}
	else if (fdy > 1)
	    mvaddch(fdy, fdx, '*');
	
	move(HEIGHT + 1, 0);
    }
}

int death(fish* pf)
{
    
    while (TRUE)
    {
	
	if (pf->y < WIDTH - 2)
	    nextPosition(pf, 0, 1);
	else
	{
	    mvprintw(32, 0, "You have forgotten to feed your fish "
		     "and it has passed away.");

	    break;
	}

	printFish(pf);
	refresh();
	usleep(SLEEP_CYCLE);
    }
    return EXIT_SUCCESS;
}

void printFishStats(fish* pf)
{
    char* face = malloc(5 * sizeof(char));
    int happiness = pf->happiness;

    mvprintw(3, WIDTH + 2, "Happiness: [");
    for (int h = 0; h < MAX_HAPPINESS; h++)
    {
	if (h < happiness - 1)
	    mvprintw(3, WIDTH + 14 + h, "|");
	else
	    mvprintw(3, WIDTH + 14 + h, " ");
    }

    mvprintw(3, WIDTH + 23, "]");
    
    switch (happiness)
    {
    case 0:
	face = "X(";
	break;

    case 1:
    case 2:
	face = ":'(";
	break;

    case 3:
    case 4:
	face = ":(";
	break;

    case 5:
    case 6:
	face = ":|";
	break;

    case 7:
    case 8:
	face = ":)";
	break;

    case 9:
	face = ":D";
	break;

    case 10:
	face = "!:D";
    }

    mvprintw(3, WIDTH + 24, "%4s", face);
    
    move(HEIGHT + 2, 0);
}

int main()
{
    initscr();
    raw();
    cbreak();
    timeout(100);

    char ch;
    food* fList[MAX_FOOD_SIZE] = {0};

    fish pf;
    int randX;
    location* loc;

    pf.x = WIDTH - 10;
    pf.y = HEIGHT - 10;
    pf.happiness = 7;
    pf.direction = UP;
    pf.lifecycles = 0;

    initMap();
    printMap(map);

    for (;;)
    {
	if (pf.happiness <= 0)
	    return death(&pf);

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

	printFishStats(&pf);

	if ((loc = findFood(&pf, fList)) != NULL)
	    nextPosition(&pf, loc->x, loc->y);
	else
	    nextPosition(&pf, 0 , 0);

	if (pf.lifecycles == 100)
	{
	    pf.lifecycles = 0;
	    pf.happiness--;
	}

	dropFood(fList, &pf);

	printFish(&pf);
	
	usleep(SLEEP_CYCLE);
	refresh();
    }

    nocbreak();
    endwin();
    return EXIT_SUCCESS;
}

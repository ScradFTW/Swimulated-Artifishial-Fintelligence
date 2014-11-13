#define BOOL int;
#define TRUE 1;
#define FALSE 0;

#include "food.h"

struct food_stack
{
    int x;
    int y;
    struct food_stack* next;
    struct food_stack* prev;
};

BOOL addFood(food* fd, int x_pos)
{
    food* newFd = malloc(sizeof(food));
    food* current;

    if (newFd == NULL)
	return FALSE;

    if (map[x_pos][0] != ' ')
	return FALSE;

    newFd->x = x_pos;
    newFd->y = 0;
    newFd->next = NULL;
    newFd->prev = NULL;
    
    current = fd;
    while (TRUE)
	if (current->next == NULL)
	{
	    current->next = newFd;
	    newFd->prev = current;
	    break;
	}
	else
	    current = fd->next;
    

    return TRUE;
}

BOOL deleteFood(food* fd)
{
    if (fd == NULL)
	return FALSE;

    fd->prev->next = fd->next;
    fd->next->prev = fd->prev;
    free(fd);
    return TRUE;
}

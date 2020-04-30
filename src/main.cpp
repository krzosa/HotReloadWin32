#include "main.h"
#include "include/SDL.h"


int clamp(int min, int val, int max)
{
    if (val > max) return max;
    else if (val < min ) return min;
    return val;
}

void moveRect(user_input *input, SDL_Rect *rect)
{
    rect->x += (input->stickX / 9000 );
    rect->y += (input->stickY / 9000 );
	rect->x += input->right;
    rect->x -= input->left;
    rect->y -= input->up;
    rect->y += input->down;
    rect->y = clamp(0, rect->y, SCREEN_HEIGHT - rect->h);
    rect->x = clamp(0, rect->x, SCREEN_WIDTH - rect->w);
}

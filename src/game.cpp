#include "game.h"
#include "include/SDL.h"


internal int clamp(int min, int val, int max)
{
    if (val > max) return max;
    else if (val < min ) return min;
    return val;
}

bool moveRect(user_input *input, SDL_Rect *rect)
{
    rect->x += (input->stickX) * 5;
    rect->y += (input->stickY) * 5;
	rect->x += input->right * input->stickRange;
    rect->x -= input->left * input->stickRange;
    rect->y -= input->up * input->stickRange;
    rect->y += input->down * input->stickRange;
    // rect->y = clamp(0, rect->y, SCREEN_HEIGHT - rect->h);
    // rect->x = clamp(0, rect->x, SCREEN_WIDTH - rect->w);

    return 1;
}

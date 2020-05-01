#include "game.h"
#include "include/SDL.h"


internal int clamp(int min, int val, int max)
{
    if (val > max) return max;
    else if (val < min ) return min;
    return val;
}

internal void renderGradient(graphics_buffer* buffer, int offsetX, int offsetY)
{
    Uint8 *Row = (Uint8 *)buffer->pixels;    
    for(int Y = 0; Y < buffer->height; ++Y)
    {
        Uint32 *Pixel = (Uint32 *)Row;
        for(int X = 0; X < buffer->width; ++X)
        {
            Uint8 Blue = (Uint8)X + offsetX;
            Uint8 Green = (Uint8)Y + offsetY;

            // RGBA 
            *Pixel++ = ((Green << 8) | Blue << 16);
        }
        
        Row += 4 * buffer->width;
    }

}

void UpdateAndRender(graphics_buffer* buffer, user_input *input, game_state *gameState)
{
    gameState->player.x += (input->stickX) * 5 ;
    gameState->player.y += (input->stickY) * 5 ;
	gameState->player.x += input->right * input->stickRange;
    gameState->player.x -= input->left * input->stickRange;
    gameState->player.y -= input->up * input->stickRange;
    gameState->player.y += input->down * input->stickRange;
    renderGradient(buffer, gameState->player.x, gameState->player.y);
    // rect->y = clamp(0, rect->y, SCREEN_HEIGHT - rect->h);
    // rect->x = clamp(0, rect->x, SCREEN_WIDTH - rect->w);
}

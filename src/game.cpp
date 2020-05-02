#include "game.h"
#include "include/SDL.h"

internal void renderGradient(graphics_buffer* buffer, int offsetX, int offsetY)
{
    Uint8 *row = (Uint8 *)buffer->pixels;    
    for(int Y = 0; Y < buffer->height; ++Y)
    {
        Uint32 *pixel = (Uint32 *)row;
        for(int X = 0; X < buffer->width; ++X)
        {
            // so to my understanding when it reaches 255 it wraps around to 0 and starts counting again
            // TODO: try to code this myself using different types
            Uint8 color1 = (Uint8)X + offsetX;
            Uint8 color2 = (Uint8)Y + offsetY;

            // RGBA 
            *pixel++ = ((color2 << 8) | color1 << 8);
        }
        
        row += 4 * buffer->width;
    }
}

internal void renderRectangle(graphics_buffer* buffer, int x, int y, int width, int height, color color)
{
    Uint8 *Row = (Uint8 *)buffer->pixels; 
    int pitch = buffer->bytesPerPixel * buffer->width;
    Row = Row + (y * pitch);
    for(int Y = 0; Y < height; Y++)
    {
        Uint32 *Pixel = (Uint32*)Row + x;
        for(int X = 0; X < width; X++)
        {
            *Pixel++ = (color.red) | (color.green << 8) | (color.blue << 16) ;
        }
        Row = Row + pitch;
    }
}

void UpdateAndRender(graphics_buffer* buffer, user_input *input, game_state *gameState)
{
    /* Update player */
    int speed = 3;
    gameState->player.x += (input->stickX) * speed;
    gameState->player.y += (input->stickY) * speed;
	gameState->player.x += input->right * input->stickRange * speed;
    gameState->player.x -= input->left * input->stickRange * speed;
    gameState->player.y -= input->up * input->stickRange * speed;
    gameState->player.y += input->down * input->stickRange * speed;

    color colorClear = {0, 150, 150, 0};
    color color = {0, 250, 150, 0};

    renderRectangle(buffer, 0, 0, buffer->width, buffer->height, colorClear);
    // renderGradient(buffer, gameState->player.x, gameState->player.y);
    renderRectangle(buffer, gameState->player.x, gameState->player.y, 40, 40, color);
    
}

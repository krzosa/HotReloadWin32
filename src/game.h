#pragma once
#include "external_include\SDL_stdinc.h"

#define NO_STDIO_REDIRECT
#define internal static 
#define global_variable static

struct user_input
{
    bool up;
    bool down;
    bool left;
    bool right;
    bool button1;
    bool button2;
    bool special1;

    int stickX;
    int stickY;
    // for example if set to 4, stick only can
    // have values from 0 to 4
    int stickRange;
    // int deadzone;
};

struct graphics_buffer
{
    void* pixels;
    int bytesPerPixel;
    int width;
    int height;
};

struct player
{
    int x;
    int y;
    int width;
    int height;
};

struct game_state
{
    player player;
};

struct color
{
    Uint8 red;
    Uint8 green;
    Uint8 blue;
    Uint8 alpha;
};

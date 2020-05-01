#pragma once

#define NO_STDIO_REDIRECT
#define SCREEN_WIDTH 700
#define SCREEN_HEIGHT 960
#define internal static 
#define global_variable static

struct user_input
{
    bool up;
    bool down;
    bool left;
    bool right;
    bool a;
    bool b;
    int stickX;
    int stickY;
};
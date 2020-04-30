#include "include/SDL.h"
#include <cstring>
#include <windows.h>
#include "main.h"
#define SDL_MAIN_HANDLED

/* TODO
    Joystick check every now and then (every 4 secs lets say) 
    Normalize joystick / keyboard controls (movement speed)
    Add fps lock, fps counter 
    Add delta time for fps independent stuff
    Loading wav files and stuff like that 
    */

/* NOTES:
    Before copying temp dll to main dll make sure to NULL all the pointers that accessed something from the dll
    Compile with /LD flag and also tell the compiler which functions to export with /link /EXPORT:function ...   */


const int SAMPLE_RATE = 44100;

/* explanation: https://hero.handmade.network/episode/code/day006/ */
#define MOVE_RECT(name) void name(user_input *input, SDL_Rect *rect)
/* this expands to a function definition: move_rect(user_input *input, SDL_Rect *rect) */
typedef MOVE_RECT(move_rect);
/* this expands to a function: moveRectStub(user_input *input, SDL_Rect *rect) {}  */
MOVE_RECT(moveRectStub)
{
}

global_variable int global_loop = 1;


internal void
SDLAudioCallback(void *userData, Uint8 *audioBuffer, int length_)
{
    // Sint16 *buffer = (Sint16*)audioBuffer;
    // int &sample_nr(*(int*)userData);
    // int length = length_ / 2;
    // int freq = 5000;

    // for(int i = 0; i < length; i++, sample_nr++)
    // {
    //     double time = (double)sample_nr / (double)SAMPLE_RATE;
    //     buffer[i] = (Sint16)(5000 * sin(2.0f * M_PI * 441.0f * time)); // render 441 HZ sine wave
    //     // buffer[i] = (Sint16)(freq * time);
    //     if(sample_nr == SAMPLE_RATE) sample_nr = 0;
    //     // SDL_Log("%d\n", freq);
    // }
}

struct win32_game_code
{
    HMODULE library;
    move_rect* move_rect;
    bool isValid;
};


internal void Win32UnloadGameCode(win32_game_code *GameCode)
{
    if(GameCode->library)
    {
        FreeLibrary(GameCode->library);
        GameCode->library = 0;
        GameCode->move_rect = NULL;
    }

    GameCode->isValid = false;
}

internal win32_game_code Win32LoadGameCode(char* mainDllPath, char* tempDllPath)
{
    win32_game_code Result;
    CopyFile((LPCSTR)mainDllPath, (LPCSTR)tempDllPath, FALSE);

    Result.library = LoadLibraryA("main_temp.dll");
    if(Result.library)
    {
        Result.move_rect = (move_rect *)
            GetProcAddress(Result.library, "moveRect");

        Result.isValid = (Result.move_rect != 0);
    }

    if(Result.isValid == 0)
    {
        Result.move_rect = moveRectStub;
    }

    return Result;
}

internal void handleInput(SDL_Event* event, user_input* input)
{
    while (SDL_PollEvent(event)) {
    switch (event->type)
            {
                case(SDL_QUIT):
                {
                    global_loop = 0;
                } break;
                case(SDL_KEYDOWN):
                {
                    if (event->key.keysym.sym == SDLK_w)
                    {
                        input->up = 1;
                    }
                    else if (event->key.keysym.sym == SDLK_s)
                    {
                        input->down = 1;
                    }
                    else if (event->key.keysym.sym == SDLK_a)
                    {
                        input->left = 1;
                    }
                    else if (event->key.keysym.sym == SDLK_d)
                    {
                        input->right = 1;
                    }
                    else if (event->key.keysym.sym == SDLK_d)
                    {
                        input->right = 1;
                    }
                    else if (event->key.keysym.sym == SDLK_z)
                    {
                        input->a = 1;
                    }
                    else if (event->key.keysym.sym == SDLK_x)
                    {
                        input->b = 1;
                    }
                    else if (event->key.keysym.sym == SDLK_ESCAPE)
                    {
                        global_loop = 0;
                    }
                } break;
                case(SDL_KEYUP):
                {
                    if (event->key.keysym.sym == SDLK_w)
                    {
                        input->up = 0;
                    }
                    else if (event->key.keysym.sym == SDLK_s)
                    {
                        input->down = 0;
                    }
                    else if (event->key.keysym.sym == SDLK_a)
                    {
                        input->left = 0;
                    }
                    else if (event->key.keysym.sym == SDLK_d)
                    {
                        input->right = 0;
                    }
                    else if (event->key.keysym.sym == SDLK_d)
                    {
                        input->right = 0;
                    }
                    else if (event->key.keysym.sym == SDLK_z)
                    {
                        input->a = 0;
                    }
                    else if (event->key.keysym.sym == SDLK_x)
                    {
                        input->b = 0;
                    }
                } break; 
                case(SDL_CONTROLLERBUTTONDOWN):
                {
                    if (event->cbutton.button == SDL_CONTROLLER_BUTTON_A)
                    {
                        input->a = 1;
                    }
                    else if (event->cbutton.button == SDL_CONTROLLER_BUTTON_B)
                    {   
                        input->b = 1;
                    }
                } break;        
                case(SDL_CONTROLLERBUTTONUP):
                {
                    if (event->cbutton.button == SDL_CONTROLLER_BUTTON_A)
                    {
                        input->a = 0;
                    }
                    else if (event->cbutton.button == SDL_CONTROLLER_BUTTON_B)
                    {   
                        input->b = 0;
                    }

                } break;        
                case(SDL_CONTROLLERAXISMOTION):
                {
                    if(event->caxis.axis == SDL_CONTROLLER_AXIS_LEFTX )
                    {
                        input->stickX = event->caxis.value;
                    }
                    else if (event->caxis.axis == SDL_CONTROLLER_AXIS_LEFTY )
                    {
                        input->stickY = event->caxis.value;
                    }
                } break;        
            }
    }
}

int main()
{
    SDL_Window* window = NULL;
    SDL_Surface* screenSurface = NULL;
    SDL_GameController* gGameController = NULL;

    char* mainDllPath = SDL_GetBasePath();
    char* tempDllPath = SDL_GetBasePath();
    // TODO: Bulletproof these pointers ? Should they also be freed ? (probably not)
    strcat(mainDllPath, "main.dll");
    strcat(tempDllPath, "main_temp.dll");
    SDL_LogDebug(SDL_LOG_CATEGORY_SYSTEM, "%s, %s", mainDllPath, tempDllPath);

    if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER ) < 0 )
    {
        SDL_LogError( SDL_LOG_CATEGORY_APPLICATION, 
            "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
        global_loop = 0;
    }
    
    window = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
    if(!window)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
        global_loop = 0;  
    } 

    user_input input = {};
    input.deadzone = 8000;
    // Note(Karol): Should this be inside loop. Maybe every 120 frames?
    if( SDL_NumJoysticks() < 1 )
    {
        SDL_LogDebug( SDL_LOG_CATEGORY_INPUT, "Warning: No joysticks connected!\n" );
        input.isGamePadConnected = false;
    }
    else
    {
        gGameController = SDL_GameControllerOpen( 0 );
        if( gGameController == NULL )
        {
            SDL_LogDebug( SDL_LOG_CATEGORY_INPUT, "Warning: Unable to open game controller! SDL Error: %s\n", SDL_GetError() );
            input.isGamePadConnected = false;
        }
        else input.isGamePadConnected = true;
    }

    

    int sampleNumber = 5;
    SDL_AudioSpec audioSettings;
    audioSettings.freq = SAMPLE_RATE;
    audioSettings.format = AUDIO_S16SYS;
    audioSettings.channels = 2;    /* 1 = mono, 2 = stereo */
    audioSettings.samples = 2048;  /* Good low-latency value for callback */
    audioSettings.callback = SDLAudioCallback;
    audioSettings.userdata = &sampleNumber;

    /* Open the audio device, forcing the desired format */
    if ( SDL_OpenAudio(&audioSettings, NULL) < 0 ) {SDL_LogError(SDL_LOG_CATEGORY_AUDIO, "Couldn't open audio: %s\n", SDL_GetError());}

    SDL_PauseAudio(0);
    SDL_Event event;
    move_rect* moveRect;
    win32_game_code gameCode;
    SDL_Rect rect;
    rect.x = 10;
    rect.y = 10;
    rect.w = 20;
    rect.h = 50;
    
    int gameCodeLoadIterator = 0;
    screenSurface = SDL_GetWindowSurface( window );
    gameCode = Win32LoadGameCode(mainDllPath, tempDllPath);
    while (global_loop) {
        if (gameCodeLoadIterator++ == 250)
        {
            Win32UnloadGameCode(&gameCode);
            gameCode = Win32LoadGameCode(mainDllPath, tempDllPath); 
            gameCodeLoadIterator = 0;
        }
        handleInput(&event, &input);

        gameCode.move_rect(&input, &rect);
        SDL_FillRect( screenSurface, NULL, SDL_MapRGB( screenSurface->format, 0x55, 0xFF, 0xFF ) );
        SDL_FillRect( screenSurface, &rect, SDL_MapRGB( screenSurface->format, 0xFF, 0x55, 0x55 ));
        SDL_UpdateWindowSurface( window );
    }

    
    // TODO: is this necessary?
    SDL_DestroyWindow( window );
    SDL_Quit();
    return 0;
}
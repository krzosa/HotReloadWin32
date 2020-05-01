#define SDL_MAIN_HANDLED
#include "include/SDL.h"
#include "include/SDL_mixer.h"
#include <windows.h>
#include "game.h"

/* TODO
    Joystick check every now and then (every 4 secs lets say) 
    Normalize joystick / keyboard controls (movement speed)
    Add fps lock, fps counter 
    Add delta time for fps independent stuff
    Loading wav files and stuff like that 
    Looping feature
    Instantenous dll loading

    */

/* NOTES:
    Before copying temp dll to main dll make sure to NULL all the pointers that accessed something from the dll
    Compile with /LD flag and also tell the compiler which functions to export with /link /EXPORT:function ...   */


/* explanation: https://hero.handmade.network/episode/code/day006/ */
#define MOVE_RECT(name) void name(user_input *input, SDL_Rect *rect)
/* this expands to a function definition: move_rect(user_input *input, SDL_Rect *rect) */
typedef MOVE_RECT(move_rect);
/* this expands to a function: moveRectStub(user_input *input, SDL_Rect *rect) {}  */
MOVE_RECT(moveRectStub)
{
}

global_variable int global_loop = 1;

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

    Result.library = LoadLibraryA(tempDllPath);
    if(Result.library)
    {
        Result.move_rect = (move_rect *)
            GetProcAddress(Result.library, "moveRect");

        Result.isValid = (Result.move_rect != 0);
    }

    if(Result.isValid == 0)
    {
        Result.move_rect = moveRectStub;
        SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION,"FAILED to load function from a DLL");
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
    SDL_Event event;

    char* basePath = SDL_GetBasePath();
    char dataPath[MAX_PATH];
    char mainDllPath[MAX_PATH];
    char tempDllPath[MAX_PATH];

    strcpy(dataPath, basePath);
    strcpy(mainDllPath, basePath);
    strcpy(tempDllPath, basePath);

    strcat(dataPath, "data\\"); 
    strcat(mainDllPath, "game.dll");
    strcat(tempDllPath, "game_temp.dll");
    SDL_LogDebug(SDL_LOG_CATEGORY_SYSTEM, "mainDLLPath: %s\n tempDLLPath: %s\n dataPath: %s", mainDllPath, tempDllPath, dataPath);

    char musPath[MAX_PATH];
    char effectPath[MAX_PATH];
    strcpy(musPath, dataPath);
    strcpy(effectPath, dataPath);
    strcat(musPath, "whateverwhenever.mp3");
    strcat(effectPath, "confirm_style_1_001.wav");


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


    if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) == -1)
    {
        SDL_LogError(SDL_LOG_CATEGORY_AUDIO, "FAILED to open audio SDL_OpenAudio: %s\n", SDL_GetError());
    }


    // Note(Karol): Should this be inside loop. Maybe every 120 frames?
    if( SDL_NumJoysticks() < 1 )
    {
        SDL_LogDebug( SDL_LOG_CATEGORY_INPUT, "Warning: No joysticks connected!\n" );
    }
    else
    {
        gGameController = SDL_GameControllerOpen( 0 );
        if( gGameController == NULL )
        {
            SDL_LogDebug( SDL_LOG_CATEGORY_INPUT, "Warning: Unable to open game controller! SDL Error: %s\n", SDL_GetError() );
        }
    }

    Mix_Music* TTNG = Mix_LoadMUS(musPath);
    Mix_Chunk* effect = Mix_LoadWAV(effectPath);
    user_input input = {};
    move_rect* moveRect;
    win32_game_code gameCode;
    SDL_Rect rect = {10, 10, 50, 50};
    
    // Mix_VolumeMusic(10);
    // Mix_PlayMusic(TTNG, 0);
    Mix_PlayChannel( -1, effect, 0 );

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
}
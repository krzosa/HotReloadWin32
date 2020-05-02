#define SDL_MAIN_HANDLED
#include "include/SDL.h"
#include <windows.h>
#include "game.h"

#define MILLISECONDS_BETWEEN_FRAMES 15
#define SCREEN_WIDTH 700
#define SCREEN_HEIGHT 960

global_variable SDL_Window* window = NULL;
global_variable int global_loop = 1;

#define UPDATE_AND_RENDER(name) void name(graphics_buffer* buffer, user_input *input, game_state *gameState)
typedef UPDATE_AND_RENDER(UpdateAndRender);
UPDATE_AND_RENDER(UpdateAndRenderStub)
{
}

struct win32_game_code
{
    HMODULE library;
    FILETIME lastDllWriteTime;
    UpdateAndRender* UpdateAndRender;
    bool isValid;
};

internal FILETIME Win32GetLastWriteTime(char* file)
{
    FILETIME lastWriteTime = {};
    WIN32_FIND_DATAA data;
    HANDLE dllFileHandle = FindFirstFileA(file, &data);
    if(dllFileHandle != INVALID_HANDLE_VALUE)
    {
        FindClose(dllFileHandle);
        lastWriteTime = data.ftLastWriteTime;
    }

    return lastWriteTime;
}

internal void Win32UnloadGameCode(win32_game_code *GameCode)
{
    if(GameCode->library)
    {
        FreeLibrary(GameCode->library);
        GameCode->library = 0;
        GameCode->UpdateAndRender = UpdateAndRenderStub;
    }

    GameCode->isValid = false;
}

internal win32_game_code Win32LoadGameCode(char* mainDllPath, char* tempDllPath)
{
    win32_game_code Result;
    Result.lastDllWriteTime = Win32GetLastWriteTime(tempDllPath);

    CopyFileA((LPCSTR)mainDllPath, (LPCSTR)tempDllPath, FALSE);
    Result.library = LoadLibraryA(tempDllPath);
    Result.isValid = 1;
    if(Result.library)
    {
        Result.UpdateAndRender = (UpdateAndRender*)
            GetProcAddress(Result.library, "UpdateAndRender");

        Result.isValid = (Result.UpdateAndRender != 0);
    }

    if(Result.isValid == 0)
    {
        Result.UpdateAndRender = UpdateAndRenderStub;
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"FAILED to load function from a DLL");
    }

    return Result;
}

internal void handleInput(SDL_Event* event, user_input* input, SDL_GameController* gGameController)
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
                        input->button1 = 1;
                    }
                    else if (event->key.keysym.sym == SDLK_x)
                    {
                        input->button2 = 1;
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
                        input->button1 = 0;
                    }
                    else if (event->key.keysym.sym == SDLK_x)
                    {
                        input->button2 = 0;
                    }
                } break; 
                case(SDL_CONTROLLERBUTTONDOWN):
                {
                    if (event->cbutton.button == SDL_CONTROLLER_BUTTON_A)
                    {
                        input->button1 = 1;
                    }
                    else if (event->cbutton.button == SDL_CONTROLLER_BUTTON_B)
                    {   
                        input->button2 = 1;
                    }
                    else if (event->cbutton.button == SDL_CONTROLLER_BUTTON_RIGHTSHOULDER)
                    {   
                        input->special1 = 1;
                    }
                } break;        
                case(SDL_CONTROLLERBUTTONUP):
                {
                    if (event->cbutton.button == SDL_CONTROLLER_BUTTON_A)
                    {
                        input->button1 = 0;
                    }
                    else if (event->cbutton.button == SDL_CONTROLLER_BUTTON_B)
                    {   
                        input->button2 = 0;
                    }
                    else if (event->cbutton.button == SDL_CONTROLLER_BUTTON_RIGHTSHOULDER)
                    {   
                        input->special1 = 0;
                    }

                } break;        
                case(SDL_CONTROLLERAXISMOTION):
                {
                    int controllerStickMaxValue = 32767;
                    if(event->caxis.axis == SDL_CONTROLLER_AXIS_LEFTX )
                    {
                        // Normalizing stick value to range <0, 4> 
                        input->stickX = event->caxis.value / (controllerStickMaxValue / input->stickRange);
                    }
                    else if (event->caxis.axis == SDL_CONTROLLER_AXIS_LEFTY )
                    {
                        input->stickY = event->caxis.value / (controllerStickMaxValue / input->stickRange);
                    }
                } break;        
                case(SDL_CONTROLLERDEVICEADDED):
                {
                    SDL_Log("Controller %d connected", event->cdevice.which); 
                    gGameController = SDL_GameControllerOpen( 0 );
                    
                } break;
                case(SDL_CONTROLLERDEVICEREMOVED):
                {
                    SDL_Log("Controller %d disconnected", event->cdevice.which);
                    if(event->cdevice.which == 0)
                    {
                        gGameController = NULL;
                    }
                } break;
                case(SDL_WINDOWEVENT):
                {
                    if(event->window.event == SDL_WINDOWEVENT_FOCUS_GAINED)
                    {
                        SDL_SetWindowBordered(window, SDL_TRUE);
                        SDL_SetWindowOpacity(window, 1);
                        // SDL_Log("SDL_WINDOWEVENT_FOCUS_GAINED");
                    }
                    else if(event->window.event == SDL_WINDOWEVENT_FOCUS_LOST)
                    {
                        SDL_SetWindowBordered(window, SDL_FALSE);
                        SDL_SetWindowOpacity(window, 0.5);
                        // SDL_Log("SDL_WINDOWEVENT_FOCUS_LOST");
                    }
                } break;
            }
    }
}

int main()
{
    SDL_Event event;
    SDL_GameController* gGameController = NULL;

    char* basePath = SDL_GetBasePath();
    char  dataPath[MAX_PATH];
    char  mainDllPath[MAX_PATH];
    char  tempDllPath[MAX_PATH];

    strcpy_s(dataPath, basePath);
    strcpy_s(mainDllPath, basePath);
    strcpy_s(tempDllPath, basePath);

    strcat_s(dataPath, "data\\"); 
    strcat_s(mainDllPath, "game.dll");
    strcat_s(tempDllPath, "game_temp.dll");

    if( SDL_Init(SDL_INIT_EVERYTHING) < 0 )
    {
        SDL_LogError( SDL_LOG_CATEGORY_APPLICATION, 
            "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
        global_loop = 0;
    }
    
    window = SDL_CreateWindow( "Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_ALWAYS_ON_TOP );
    if(!window)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "FAILED to create window: %s\n", SDL_GetError() );
        global_loop = 0;  
    } 

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer)
    {
        SDL_LogError(SDL_LOG_CATEGORY_RENDER, "FAILED to create renderer");
        global_loop = 0;
    }

    win32_game_code gameCode = {};
    gameCode = Win32LoadGameCode(mainDllPath, tempDllPath);

    user_input input = {};
    input.stickRange = 4;

    game_state gameState = {};
    gameState.player.x = 10;
    gameState.player.y = 10; 
    gameState.player.width = 50;
    gameState.player.height = 50;

    graphics_buffer buffer = {};
    buffer.bytesPerPixel = 4;

    SDL_GetWindowSize(window, &buffer.width, &buffer.height);
    SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING,
                                            buffer.width, buffer.height);
    // Windows function for allocating memory
    buffer.pixels = VirtualAlloc(NULL, buffer.width * buffer.height * buffer.bytesPerPixel, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

    Uint32 prevIterationTimer = 0;
    while (global_loop) {
        Uint32 timerStart = SDL_GetTicks();
        Uint32 betweenFramesTime = timerStart - prevIterationTimer; // delta time
        prevIterationTimer = SDL_GetTicks();

        // check for last write to the dll and if it changed, reload the dll
        FILETIME dllFileWriteTime = Win32GetLastWriteTime(mainDllPath);
        if (CompareFileTime(&dllFileWriteTime, &gameCode.lastDllWriteTime) != 0)
        {
            Win32UnloadGameCode(&gameCode);
            gameCode = Win32LoadGameCode(mainDllPath, tempDllPath); 
        }

        handleInput(&event, &input, gGameController);

        gameCode.UpdateAndRender(&buffer, &input, &gameState);
        SDL_UpdateTexture(texture, 0, buffer.pixels, buffer.width * buffer.bytesPerPixel);
        SDL_RenderCopy(renderer, texture, 0, 0); 
        SDL_RenderPresent(renderer);

        Uint32 timerStop = SDL_GetTicks();
        Uint32 iterationTime = timerStop - timerStart;

        // calculate how much we should delay a frame to keep steady framerate
        if (iterationTime < MILLISECONDS_BETWEEN_FRAMES)
        {
            SDL_Delay(MILLISECONDS_BETWEEN_FRAMES - iterationTime);
        }
    }
    // Windows handles memory deallocation on exit so its not needed
}
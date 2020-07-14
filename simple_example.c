
/* This code is not supposed to work, these are all the important functions but in a simpler to read format */



/* this macro creates two type definitions UpdateAndRender(graphics_buffer* buffer, user_input *input, game_state *gameState) 
    and UpdateAndRenderStub(graphics_buffer* buffer, user_input *input, game_state *gameState)
    {

    }
    stub does nothing, its like nulling a pointer but we have confidence that it wont break anything if something happens 
*/
#define UPDATE_AND_RENDER(name) void name(graphics_buffer* buffer, user_input *input, game_state *gameState)
typedef UPDATE_AND_RENDER(UpdateAndRender);
UPDATE_AND_RENDER(UpdateAndRenderStub)
{
}

/* Searches for a file, extracts properties and returns the time
    the file was last written to  */
FILETIME Win32GetLastWriteTime(char* file)
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

/* Unloads the dll and nulls the pointers to functions from the dll */
void Win32UnloadGameCode(win32_game_code *GameCode)
{
    if(GameCode->library)
    {
        FreeLibrary(GameCode->library);
        GameCode->library = 0;
        GameCode->UpdateAndRender = UpdateAndRenderStub;
    }

    GameCode->isValid = false;
}

/* Copies the main_dll to a temp_dll, loads temp_dll, returns a struct containing pointer
    to the lib, functions and last write time */
win32_game_code Win32LoadGameCode(char* mainDllPath, char* tempDllPath)
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


int main()
{
    /* Get absolute paths to dll */
    char* basePath = SDL_GetBasePath();
    char  mainDllPath[MAX_PATH];
    char  tempDllPath[MAX_PATH];

    strcpy_s(mainDllPath, basePath);
    strcpy_s(tempDllPath, basePath);

    strcat_s(mainDllPath, "game.dll");
    strcat_s(tempDllPath, "game_temp.dll");

    /* Passing absolute paths of dlls to the load function */
    win32_game_code gameCode = {};
    gameCode = Win32LoadGameCode(mainDllPath, tempDllPath);

    while(1)
    {
        /* Check for last write to the dll and if it changed, reload the dll */
        FILETIME dllFileWriteTime = Win32GetLastWriteTime(mainDllPath);
        if (CompareFileTime(&dllFileWriteTime, &gameCode.lastDllWriteTime) != 0)
        {
            /* Unload game code so we can write to the temp_dll, copy main_dll to temp_dll and load game code */
            Win32UnloadGameCode(&gameCode);
            gameCode = Win32LoadGameCode(mainDllPath, tempDllPath); 
        }

        /* Call function from the dll */
        gameCode.UpdateAndRender(&graphicsBuffer, &input, &gameState, &etc..);
    }
}
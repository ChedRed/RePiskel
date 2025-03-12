#ifdef _WIN32
#include <Windows.h>
#endif
#define _USE_MATH_DEFINES
#include "TextHelp.h"
#include "StringFix.h"
#include <iostream>

#ifdef _WIN32
#define SDL_MODKEY SDL_SCANCODE_LCTRL
#else
#define SDL_MODKEY SDL_SCANCODE_LGUI
#endif


std::string SourcePath = "Resources/Content/";
#ifdef __APPLE__
SourcePath = "Content/";
#endif


// Setup default values
std::string SettingsPath = (SDL_GetBasePath()) + std::string("/Resources/Content/Settings");

Vector2 WindowSize = {960, 540};
SDL_FRect WindowWorkspace = {0, 36, WindowSize.x, WindowSize.y - 36};
SDL_FRect CanvasWorkspace = {0, 36, WindowSize.x, WindowSize.y - 36};

Vector2 Resolution = {16, 16};



/* Main! */
int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);


    // Pre-parse settings file
    std::string RawFileData = std::string((char *)SDL_LoadFile(SettingsPath.c_str(), nullptr));
    std::vector<std::string> FileData = SplitString(RawFileData, '\n');


    // Parse settings file
    std::vector<std::string> Settings_Resolution = SplitString(FileData[0], ',');
    Resolution = {(float)std::stoi(Settings_Resolution[0]), (float)std::stoi(Settings_Resolution[1])};


    return 0;
}



#ifdef _WIN32
/* Windows window subsystem :( */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow){
    return main(__argc, __argv);
}
#endif

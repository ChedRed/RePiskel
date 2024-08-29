#include <iostream>

#include <SDL3/SDL.h>

SDL_Event e;
bool loop = true;

struct duo {
    int x;
    int y;
};

int border = 10
duo leftmargin = { 100.0f, 100.0f, 200.0f, 150.0f };
SDL_FRect rightmargin = { 100.0f, 100.0f, 200.0f, 150.0f };
SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Set color to red
SDL_RenderRect(renderer, &rect);

int main() {
    std::cout << "Initializing SDL3" << std::endl;
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window * window = SDL_CreateWindow("RePiskel",1000,650, SDL_WINDOW_RESIZABLE | SDL_WINDOW_MOUSE_CAPTURE);
    SDL_Renderer * renderer = SDL_CreateRenderer(window,NULL);
    SDL_SetWindowMinimumSize(window,800,500);
    std::cout << "Success! Initializing loop" << std::endl;
    while (loop) {
        while (SDL_PollEvent(&e) != 0){
            if (e.type == 256) loop = false;
        }
        SDL_RenderPresent(renderer);
    }

    return 0; }

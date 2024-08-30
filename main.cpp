#include <iostream>

#include <SDL3/SDL.h>

SDL_Event e;
bool loop = true;


/* New 'data types' */
struct deltadata { double fps; double now; double then; double deltime; };
struct vec2 { int x; int y; };
struct fvec2 { float x; float y; };
struct duo { int a; int b; };


deltadata dtdata;
fvec2 mouse = {0,0};
vec2 windowsize = {1000,650};
duo margin = {200,200};
int border = 10;
SDL_Color gridA = {.r=76,.g=76,.b=76,.a=255};
SDL_Color gridB = {.r=85,.g=85,.b=85,.a=255};
SDL_FRect grid = {0,0,8,8};


SDL_FRect leftmargin = { 0,0,margin.a,windowsize.y };
SDL_FRect rightmargin = { windowsize.x-margin.b,0,margin.b,windowsize.y };


int main() {
    std::cout << "Initializing SDL3" << std::endl;
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window * window = SDL_CreateWindow("RePiskel",windowsize.x,windowsize.y, SDL_WINDOW_RESIZABLE | SDL_WINDOW_MOUSE_CAPTURE);
    SDL_Renderer * renderer = SDL_CreateRenderer(window,NULL);
    SDL_SetWindowMinimumSize(window,800,500);
    SDL_SetRenderVSync(renderer, 1);
    std::cout << "Success! Initializing loop" << std::endl;
    while (loop) {

        /* Clear renderer, get mouse pos, and update FPS */
        SDL_SetRenderDrawColor(renderer, 127, 127, 127, 255);
        SDL_RenderClear(renderer);
        SDL_GetMouseState(&mouse.x, &mouse.y);
        dtdata.then = dtdata.now;
        dtdata.now = SDL_GetPerformanceCounter();
        dtdata.deltime = (dtdata.now - dtdata.then) / (double)SDL_GetPerformanceFrequency();
        dtdata.fps = (1/dtdata.deltime);


        /* Poll inputs */
        while (SDL_PollEvent(&e) != 0){
            switch (e.type) {
                case SDL_EVENT_QUIT:
                    loop = false;
                    break;
                case SDL_EVENT_WINDOW_RESIZED:
                    SDL_GetWindowSize(window,&windowsize.x,&windowsize.y);
                    leftmargin.h=windowsize.y;
                    rightmargin.h=windowsize.y;
                    rightmargin.x = windowsize.x-margin.b;
            }
        }

        SDL_SetRenderDrawColor(renderer, gridA.r,gridA.g,gridA.b,gridA.a);
        for (int y = 0; y < (int)(windowsize.y/8)+1; y++) {
            grid.y = (y*8);
            for (int x = 0; x < (int)(windowsize.x/8); x++) {
                grid.x = (x*16)-((y%2)*8);
                SDL_RenderFillRect(renderer, &grid);
            }
        }
        SDL_SetRenderDrawColor(renderer, gridB.r,gridB.g,gridB.b,gridB.a);
        for (int y = 0; y < (int)(windowsize.y/8)+1; y++) {
            grid.y = (y*8);
            for (int x = 0; x < (int)(windowsize.x/8); x++) {
                grid.x = (x*16)-((y%2)*8)+8;
                SDL_RenderFillRect(renderer, &grid);
            }
        }

        /* Render margins */
        SDL_SetRenderDrawColor(renderer, 29, 29, 29, 255);
        SDL_RenderFillRect(renderer, &leftmargin);
        SDL_RenderFillRect(renderer, &rightmargin);
        SDL_RenderPresent(renderer);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0; }

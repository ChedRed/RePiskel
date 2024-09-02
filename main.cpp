#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>


/* SDL setup variables */
SDL_Event e;
bool loop = true;


/* New 'data types' */
struct deltadata { double fps; double now; double then; double deltime; };
struct vec2 { int x; int y; };
struct fvec2 { float x; float y; };
struct duo { float a; float b; };


/* Display setup variables */
deltadata dtdata;
vec2 windowsize = { 1000,650 };


/* Mouse setup variables */
fvec2 mouse = { 0,0 };
fvec2 scroll = { 0,0 };


/* Art setup variables */
SDL_Color gridC = { .r=76,.g=76,.b=76,.a=255 };
SDL_FRect grid = { 0,0,8,8 };
float zoom = 1;


/* UI */
duo margin = { 240,260 };
SDL_FRect tirect = { 0,0,0,0 };
SDL_FRect nameborder = { 0,0,windowsize.x,36 };
SDL_FRect leftmargin = { 0,0,margin.a,windowsize.y };
SDL_FRect rightmargin = { windowsize.x-margin.b,0,margin.b,windowsize.y };
SDL_FRect bottomargin = { 0,36,windowsize.x,8 };
SDL_FRect topmargin = { 0,windowsize.y-8,windowsize.x,8 };


/* Canvas */
SDL_FRect canvas = { margin.a,44,windowsize.x-margin.a-margin.b,windowsize.y-52 };
SDL_FRect upbar = { margin.a,44,canvas.w,(canvas.h>canvas.w)?-(canvas.w-canvas.h)/2:0 };
SDL_FRect downbar = { margin.a,44+canvas.h,canvas.w,(canvas.h>canvas.w)?(canvas.w-canvas.h)/2:0 };
SDL_FRect leftbar = { margin.a,44,(canvas.w>canvas.w)?(canvas.h-canvas.w)/2:0,canvas.h };
SDL_FRect rightbar = { margin.a,44,(canvas.w>canvas.w)?-(canvas.h-canvas.w)/2:0,canvas.h };


/* Diagnostic tools */
float maxdeltime = 0;
float elapsedtime = 0;


/* Main! */
int main() {


    /* Initialize SDL, create window and renderer */
    std::cout << "Initializing SDL3" << std::endl;
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window * window = SDL_CreateWindow("RePiskel",windowsize.x,windowsize.y, SDL_WINDOW_RESIZABLE | SDL_WINDOW_MOUSE_CAPTURE);
    SDL_Renderer * renderer = SDL_CreateRenderer(window,NULL);
    SDL_SetWindowMinimumSize(window,800,450);
    SDL_SetRenderVSync(renderer, 1);
    std::cout << "Success! Initializing loop" << std::endl;


    /* Initialize SDL_ttf, create font object */
    TTF_Init();
    char fontPath[256];
    snprintf(fontPath, sizeof(fontPath), "%s%s", SDL_GetBasePath(), "../Resources/FreeSans.ttf");
    TTF_Font * font = TTF_OpenFont(fontPath, 24);


    /* Create title text */
    SDL_Surface * pretitle = TTF_RenderText_Blended(font, "New Piskel", (SDL_Color){ .r=255,.g=255,.b=255,.a=255 });
    SDL_Texture * title = SDL_CreateTextureFromSurface(renderer, pretitle);
    SDL_GetTextureSize(title, &tirect.w, &tirect.h);
    tirect.x = (int)(windowsize.x/2)-(int)(tirect.w/2);
    tirect.y = 18-(int)(tirect.h/2);
    SDL_DestroySurface(pretitle);


    /* Main loop */
    while (loop) {


        /* Get mouse pos and get FPS */
        dtdata.then = dtdata.now;
        dtdata.now = SDL_GetPerformanceCounter();
        dtdata.deltime = (dtdata.now - dtdata.then) / (double)SDL_GetPerformanceFrequency();
        dtdata.fps = (1/dtdata.deltime);


        /* Run deltime diagnostics */
        elapsedtime += dtdata.deltime;
        maxdeltime=(dtdata.deltime > maxdeltime)?dtdata.deltime:maxdeltime;
        if (elapsedtime > 5){
            elapsedtime = 0;
            std::cout << "Highest dt in the past second: " << maxdeltime << std::endl;
            maxdeltime = 0;
        }


        /* Clear renderer and draw background grid */
        SDL_SetRenderDrawColor(renderer, 85, 85, 85, 255);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, gridC.r,gridC.g,gridC.b,gridC.a);
        for (int y = 0; y < (int)(windowsize.y/8)+1; y++) {
            grid.y = (y*8);
            for (int x = 0; x < (int)(windowsize.x/8); x++) {
                grid.x = (x*16)-((y%2)*8);
                SDL_RenderFillRect(renderer, &grid); }}


        /* Update mouse */
        SDL_GetMouseState(&mouse.x, &mouse.y);
        if (scroll.x != 0) { scroll.x = 0; }
        if (scroll.y != 0) { scroll.y = 0; }


        /* Poll inputs */
        while (SDL_PollEvent(&e) != 0){
            switch (e.type) {
                case SDL_EVENT_QUIT:
                    loop = false;
                    break;
                case SDL_EVENT_WINDOW_RESIZED:
                    SDL_GetWindowSize(window,&windowsize.x,&windowsize.y);
                    leftmargin.h = windowsize.y;
                    rightmargin.h = windowsize.y;
                    rightmargin.x = windowsize.x-margin.b;
                    nameborder.w = windowsize.x;
                    tirect.x = (int)(windowsize.x/2)-(int)(tirect.w/2);
                    bottomargin.w = windowsize.x;
                    topmargin.w = windowsize.x;
                    topmargin.y = windowsize.y-8;
                    canvas = (SDL_FRect){ .x=margin.a,.y=44,.w=windowsize.x-margin.a-margin.b,.h=windowsize.y-52 };
                    upbar = (SDL_FRect){ .x=margin.a,.y=44,.w=canvas.w,.h=(canvas.h>canvas.w)?-(canvas.w-canvas.h)/2:0 };
                    downbar = (SDL_FRect){ .x=margin.a,.y=44+canvas.h,.w=canvas.w,.h=(canvas.h>canvas.w)?(canvas.w-canvas.h)/2:0 };
                    leftbar = (SDL_FRect){ .x=margin.a,.y=44,.w=(canvas.w>canvas.h)?-(canvas.h-canvas.w)/2:0,.h=canvas.h };
                    rightbar = (SDL_FRect){ .x=margin.a+canvas.w,.y=44,.w=(canvas.w>canvas.h)?(canvas.h-canvas.w)/2:0,.h=canvas.h };
                case SDL_EVENT_MOUSE_WHEEL:
                    scroll.x = e.wheel.x;
                    scroll.y = e.wheel.y;
            }
        }


        /* Render margins */
        SDL_SetRenderDrawColor(renderer, 29, 29, 29, 255);
        SDL_RenderFillRect(renderer, &leftmargin);
        SDL_RenderFillRect(renderer, &rightmargin);


        /* Render borders */
        SDL_SetRenderDrawColor(renderer, 0,0,0,255);
        SDL_RenderFillRect(renderer, &nameborder);
        SDL_SetRenderDrawColor(renderer, 29,29,29,255);
        SDL_RenderFillRect(renderer, &bottomargin);
        SDL_RenderFillRect(renderer, &topmargin);


        /* Render canvas */
        SDL_SetRenderDrawColor(renderer, 160,160,160,255);
        SDL_RenderFillRect(renderer, &upbar);
        SDL_RenderFillRect(renderer, &downbar);
        SDL_RenderFillRect(renderer, &leftbar);
        SDL_RenderFillRect(renderer, &rightbar);


        /* Render title text */
        SDL_RenderTexture(renderer, title, NULL, &tirect);


        /* Push render content */
        SDL_RenderPresent(renderer);
    }


    /* Exit properly */
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0; }

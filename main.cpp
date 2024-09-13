#include <ostream>
#include <vector>
#include <optional>
#include <string>
#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>


/* SDL setup variables */
SDL_Event e;
bool loop = true;
bool focus = true;


/* New 'data types' */
struct deltadata { double fps; double now; double then; double deltime; };
struct vec2 { int x; int y; };
struct fvec2 { float x; float y; };
struct duo { float a; float b; };


/* Display setup variables */
deltadata dtdata;
vec2 windowsize = { 1000, 650 };
SDL_Color gridC = { .r=76, .g=76, .b=76, .a=255 };
SDL_FRect grid = { 0, 0, 8, 8 };


/* Mouse setup variables */
fvec2 mouse = { 0, 0 };
fvec2 scroll = { 0, 0 };
bool mousedowned = false;
fvec2 lastmouse = { 0,0 };
fvec2 framelastmouse = { 0,0 };
Uint32 mousebitmask;


/* UI */
duo margin = { 240, 260 };
SDL_FRect tirect;
SDL_FRect nameborder = { 0, 0, (float)windowsize.x, 36 };
SDL_FRect leftmargin = { 0, 0, margin.a, (float)windowsize.y };
SDL_FRect rightmargin = { windowsize.x-margin.b, 0, margin.b, (float)windowsize.y };
SDL_FRect bottomargin = { 0, 36, (float)windowsize.x, 8 };
SDL_FRect topmargin = { 0, (float)windowsize.y-8, (float)windowsize.x, 8 };


/* UI elements */
float toolsuiwidth = 96;
SDL_FRect toolsrect = { 8, 0, 0, 0 };
int currentool = 0;
SDL_FRect toolshoveredrect = { 0,0,48,48 };
SDL_FRect toolselectedrect = { 0,0,48,48 };
std::string toolnames[18] = {"Pen","Line","Eraser","Mirror","Dither","Lighten","Fill","Multi-Fill","","Rectangle","Circle","","Rectangle Select","Lasso Select","Magic Select","Grab","Gridlock","Pick Color"};
SDL_FRect cursizerectinborder;
SDL_FRect cursizerectborder;
SDL_FRect cursizerect;
float cursize = 1;
SDL_FRect cursizetextrect;


/* Canvas */
fvec2 resolution = { 16, 16 };
fvec2 drawresolution = { 0, 0 };
fvec2 resratio = { (resolution.x<resolution.y)?(float)resolution.x/resolution.y:1, (resolution.x>resolution.y)?(float)resolution.y/resolution.x:1 };
fvec2 canvascenter = { (margin.a/2)+((windowsize.x-margin.b)/2), (22)+(((float)windowsize.y-8)/2) };
SDL_FRect precanvas = { margin.a, 44, windowsize.x-margin.a-margin.b, (float)windowsize.y-52 };
SDL_FRect oldprecanvas = precanvas;
fvec2 canvasize = { resratio.x*((resolution.x<resolution.y)?precanvas.h:precanvas.w), resratio.y*((resolution.x>resolution.y)?precanvas.h:precanvas.w) };
fvec2 oldcanvasize = canvasize;
SDL_FRect canvas = { canvascenter.x-(canvasize.x/2), canvascenter.y-(canvasize.y/2), canvasize.x, canvasize.y };
std::vector<SDL_Texture *> sprite;
int frame = 0;


/* Canvas borders */
SDL_FRect upbar = { margin.a, 44, precanvas.w, (precanvas.h-canvas.h)/2 };
SDL_FRect downbar = { margin.a, (float)windowsize.y-8, precanvas.w, -(precanvas.h-canvas.h)/2 };
SDL_FRect leftbar = { margin.a, 44, (precanvas.w-canvas.w)/2, precanvas.h };
SDL_FRect rightbar = { margin.a, 44, -(precanvas.w-canvas.w)/2, precanvas.h };


/* Canvas drawing variables */
void*pixels;
int pitch;


/* Limit function */
double limit(double value, std::optional<double> min = NULL, std::optional<double> max = NULL) { return ((min!=NULL)?((max!=NULL)?(value>max?max:(value<min?min:value)):(value<min?min:value)):(max!=NULL)?(value>max?max:value):value).value(); }


/* Contained function */
bool contained(fvec2 point, SDL_FRect container) { return ((container.w>0)?point.x>container.x:point.x<container.x) && ((container.h>0)?point.y>container.y:point.y<container.y) && ((container.w>0)?point.x<container.x+container.w:point.x>container.x+container.w) && ((container.h>0)?point.y<container.y+container.h:point.y>container.y+container.h); }


/* Main! */
int main() {


    /* Initialize SDL, create window and renderer */
    std::cout << "Initializing SDL3" << std::endl;
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window * window = SDL_CreateWindow("RePiskel", windowsize.x, windowsize.y, SDL_WINDOW_RESIZABLE | SDL_WINDOW_MOUSE_CAPTURE);
    SDL_Renderer * renderer = SDL_CreateRenderer(window, NULL);
    SDL_SetWindowMinimumSize(window, 960, 540);
    SDL_SetRenderVSync(renderer, 1);
    std::cout << "Success! Initializing loop" << std::endl;


    /* Initialize SDL_ttf, create font object */
    TTF_Init();
    char tempath[256];
    snprintf(tempath, sizeof(tempath), "%s%s", SDL_GetBasePath(), "../Resources/FreeSans.ttf");
    TTF_Font * font = TTF_OpenFont(tempath, 24);


    /* Create title text */
    SDL_Surface * pretitle = TTF_RenderText_Blended(font, "New Piskel", (SDL_Color){ .r=255, .g=255, .b=255, .a=255 });
    SDL_Texture * title = SDL_CreateTextureFromSurface(renderer, pretitle);
    SDL_GetTextureSize(title, &tirect.w, &tirect.h);
    tirect.x = ((float)windowsize.x/2)-((float)tirect.w/2);
    tirect.y = 18-(tirect.h/2);
    SDL_DestroySurface(pretitle);


    /* Load textures */
    snprintf(tempath, sizeof(tempath), "%s%s", SDL_GetBasePath(), "../Resources/tools.bmp");
    SDL_Surface * pretools = SDL_LoadBMP(tempath);
    SDL_Texture * tools = SDL_CreateTextureFromSurface(renderer, pretools);
    SDL_GetTextureSize(tools, &toolsrect.w, &toolsrect.h);
    toolsrect = (SDL_FRect){ .x=toolsrect.x, .y=((float)windowsize.y/2)-(toolsrect.h/(toolsrect.h/toolsuiwidth)), .w=toolsuiwidth, .h=toolsuiwidth*(toolsrect.h/toolsrect.w) };
    toolshoveredrect = (SDL_FRect){ .x=0,.y=0,toolsrect.w/3,toolsrect.w/3 };
    toolselectedrect = (SDL_FRect){ .x=0,.y=0,toolsrect.w/3,toolsrect.w/3 };
    cursizerectborder = (SDL_FRect){ .x=toolsrect.x, .y=toolsrect.y, .w=toolsrect.w, .h=-toolsrect.w };
    cursizerectinborder = (SDL_FRect){ .x=toolsrect.x+4, .y=toolsrect.y-4, .w=toolsrect.w-8, .h=-toolsrect.w+8 };
    snprintf(tempath, sizeof(tempath), "%s%s", SDL_GetBasePath(), "../Resources/border.bmp");
    SDL_Surface * pretoolsborder = SDL_LoadBMP(tempath);
    SDL_Texture * toolsborder = SDL_CreateTextureFromSurface(renderer, pretoolsborder);
    SDL_SetTextureScaleMode(toolsborder, SDL_SCALEMODE_NEAREST);
    sprite.push_back(SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, resolution.x, resolution.y));
    SDL_SetTextureScaleMode(sprite[0], SDL_SCALEMODE_NEAREST);
    SDL_Surface * tempcursizetextrect = TTF_RenderText_Blended(font, "1x", (SDL_Color){ .r=255, .g=255, .b=255, .a=255 });
    SDL_Texture * cursizetextrecture = SDL_CreateTextureFromSurface(renderer, tempcursizetextrect);
    SDL_DestroySurface(tempcursizetextrect);
    SDL_GetTextureSize(cursizetextrecture, &cursizetextrect.w, &cursizetextrect.h);
    cursizetextrect = (SDL_FRect){ .x=cursizerectborder.x+((cursizerectborder.w-cursizetextrect.w)/2), .y=cursizerectborder.y+cursizerectborder.h-cursizetextrect.h, .w=cursizetextrect.w, .h=cursizetextrect.h };


    /* Remove unecessary data */
    SDL_DestroySurface(pretoolsborder);
    SDL_DestroySurface(pretools);


    /* Main loop */
    while (loop) {


        /* Get mouse pos and get FPS */
        dtdata.then = dtdata.now;
        dtdata.now = SDL_GetPerformanceCounter();
        dtdata.deltime = (dtdata.now - dtdata.then) / (double)SDL_GetPerformanceFrequency();
        dtdata.fps = (1/dtdata.deltime);


        /* Clear renderer and draw background grid */
        SDL_SetRenderDrawColor(renderer, 85, 85, 85, 255);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, gridC.r, gridC.g, gridC.b, gridC.a);
        for (int y = 0; y < (int)(windowsize.y/8)+1; y++) {
            grid.y = (y*8);
            for (int x = 0; x < (int)(windowsize.x/8); x++) {
                grid.x = (x*16)-((y%2)*8);
                SDL_RenderFillRect(renderer, &grid); }}


        /* Update mouse */
        mousebitmask = SDL_GetMouseState(&mouse.x, &mouse.y);
        scroll.x = 0;
        scroll.y = 0;


        /* Poll inputs */
        while (SDL_PollEvent(&e) != 0){
            switch (e.type) {
                case SDL_EVENT_QUIT:
                    loop = false;
                    break;


                /* Resize window */
                case SDL_EVENT_WINDOW_RESIZED:
                    SDL_GetWindowSize(window, &windowsize.x, &windowsize.y);


                    /* Reset ui */
                    leftmargin.h = windowsize.y;
                    rightmargin.h = windowsize.y;
                    rightmargin.x = windowsize.x-margin.b;
                    nameborder.w = windowsize.x;
                    tirect.x = ((float)windowsize.x/2)-((float)tirect.w/2);
                    bottomargin.w = windowsize.x;
                    topmargin.w = windowsize.x;
                    topmargin.y = windowsize.y-8;


                    /* Reset canvas */
                    precanvas = (SDL_FRect){ .x=margin.a, .y=44, .w=windowsize.x-margin.a-margin.b, .h=(float)windowsize.y-52 };
                    canvascenter = (fvec2){ .x=(margin.a/2)+((windowsize.x-margin.b)/2), .y=22+(((float)windowsize.y-8)/2) };
                    canvasize = (fvec2){ .x=resratio.x*((precanvas.w>precanvas.h)?precanvas.h:precanvas.w), .y=resratio.y*((precanvas.w>precanvas.h)?precanvas.h:precanvas.w) };
                    canvas = (SDL_FRect){ .x=canvas.x, .y=canvas.y, .w=canvas.w*(canvasize.x/oldcanvasize.x), .h=canvas.h*(canvasize.y/oldcanvasize.y) };
                    oldprecanvas=precanvas;
                    oldcanvasize=canvasize;


                    /* Reset canvas borders */
                    upbar = (SDL_FRect){ .x=margin.a, .y=44, .w=precanvas.w, .h=canvas.y-precanvas.y };
                    downbar = (SDL_FRect){ .x=margin.a, .y=(float)windowsize.y-8, .w=precanvas.w, .h=((canvas.y+canvas.h)-(precanvas.y+precanvas.h)) };
                    leftbar = (SDL_FRect){ .x=margin.a, .y=44, .w=-((canvas.x+canvas.w)-(precanvas.x+precanvas.w)), .h=precanvas.h };
                    rightbar = (SDL_FRect){ .x=margin.a+precanvas.w, .y=44, .w=((canvas.x+canvas.w)-(precanvas.x+precanvas.w)), .h=precanvas.h };


                    /* Reset UI */
                    toolsrect.y=((float)windowsize.y/2)-toolsrect.w;
                    cursizerectborder.y=toolsrect.y;
                    cursizerectinborder.y=toolsrect.y-4;


                    /* Reset pen size text */
                    cursizetextrect = (SDL_FRect){ .x=cursizerectborder.x+((cursizerectborder.w-cursizetextrect.w)/2), .y=cursizerectborder.y+cursizerectborder.h-cursizetextrect.h, .w=cursizetextrect.w, .h=cursizetextrect.h };


                /* Zoom canvas */
                case SDL_EVENT_MOUSE_WHEEL:

                    /* Reset scroll */
                    scroll.x = e.wheel.x;
                    scroll.y = e.wheel.y;


                    /* Reset canvas */
                    if (contained(mouse, precanvas)){
                        precanvas = (SDL_FRect){ .x=margin.a, .y=44, .w=windowsize.x-margin.a-margin.b, .h=(float)windowsize.y-52 };
                        canvas = (SDL_FRect){ .x=(canvas.w<precanvas.w)?canvascenter.x-((canvas.w*(1+(scroll.y/100)))/2):((canvas.x-mouse.x)*(1+(scroll.y/100)))+mouse.x, .y=(canvas.h<precanvas.h)?canvascenter.y-((canvas.h*(1+(scroll.y/100)))/2):((canvas.y-mouse.y)*(1+(scroll.y/100)))+mouse.y, .w=canvas.w*(1+(scroll.y/100)), .h=canvas.h*(1+(scroll.y/100)) };
                        if (canvas.w>precanvas.w) {
                            (canvas.x>precanvas.x)?canvas.x=precanvas.x:canvas.x;
                            (canvas.x+canvas.w<precanvas.x+precanvas.w)?canvas.x=precanvas.x-(canvas.w-precanvas.w):canvas.x;
                        }
                        if (canvas.h>precanvas.h) {
                            (canvas.y>precanvas.y)?canvas.y=precanvas.y:canvas.y;
                            (canvas.y+canvas.h<precanvas.y+precanvas.h)?canvas.y=precanvas.y-(canvas.h-precanvas.h):canvas.y;
                        }


                        /* Reset canvas borders */
                        upbar = (SDL_FRect){ .x=margin.a, .y=44, .w=precanvas.w, .h=canvas.y-precanvas.y };
                        downbar = (SDL_FRect){ .x=margin.a, .y=(float)windowsize.y-8, .w=precanvas.w, .h=((canvas.y+canvas.h)-(precanvas.y+precanvas.h)) };
                        leftbar = (SDL_FRect){ .x=margin.a, .y=44, .w=-((canvas.x+canvas.w)-(precanvas.x+precanvas.w)), .h=precanvas.h };
                        rightbar = (SDL_FRect){ .x=margin.a+precanvas.w, .y=44, .w=((canvas.x+canvas.w)-(precanvas.x+precanvas.w)), .h=precanvas.h };
                    }


                    /* Reset pen size */
                    if (contained(mouse, cursizerectborder)){
                        if ((int)cursize!=(int)limit(cursize+(scroll.y/10),1)){
                            SDL_DestroyTexture(cursizetextrecture);
                            char tempchar[256];
                            snprintf(tempchar, sizeof(tempchar), "%d%s", ((int)limit(cursize+(scroll.y/10),1)), "x");
                            tempcursizetextrect = TTF_RenderText_Blended(font, tempchar, (SDL_Color){ .r=255, .g=255, .b=255, .a=255 });
                            cursizetextrecture = SDL_CreateTextureFromSurface(renderer, tempcursizetextrect);
                            SDL_GetTextureSize(cursizetextrecture, &cursizetextrect.w, &cursizetextrect.h);
                            SDL_DestroySurface(tempcursizetextrect);
                            remove(tempchar);
                        }
                        cursize=limit(cursize+(scroll.y/10),1);


                        /* Reset pen size text */
                        cursizetextrect = (SDL_FRect){ .x=cursizerectborder.x+((cursizerectborder.w-cursizetextrect.w)/2), .y=cursizerectborder.y+cursizerectborder.h-cursizetextrect.h, .w=cursizetextrect.w, .h=cursizetextrect.h };
                    }




                /* Reduce FPS if unfocussed */
                case SDL_EVENT_WINDOW_FOCUS_GAINED:
                    focus = true;
                    break;
                case SDL_EVENT_WINDOW_FOCUS_LOST:
                    focus = false;
                    break;


                /* Mouse use event */
                case SDL_EVENT_MOUSE_BUTTON_DOWN:


                    /* Interact with UI */
                    if (contained(mouse,toolsrect) && !(toolnames[((int)((mouse.x-toolsrect.x)/(toolsrect.w/3)))+((int)((mouse.y-toolsrect.y)/(toolsrect.w/3))*3)]=="")) currentool=((int)((mouse.x-toolsrect.x)/(toolsrect.w/3)))+((int)((mouse.y-toolsrect.y)/(toolsrect.w/3))*3);
                    if (contained(mouse,canvas)) lastmouse = mouse;
            }
        }


        /* Update canvas texture if necessary */
        if (mousebitmask & SDL_BUTTON_LMASK || mousebitmask & SDL_BUTTON_RMASK) {
            SDL_LockTexture(sprite[frame], NULL, &pixels, &pitch);
                if (currentool == 0) {
                    /* Draw line from framelastmouse to mouse */
                }
            SDL_UnlockTexture(sprite[frame]);
        }


        /* Render canvas */
        SDL_SetRenderDrawColor(renderer, 160, 160, 160, 255);
        SDL_RenderFillRect(renderer, &upbar);
        SDL_RenderFillRect(renderer, &downbar);
        SDL_RenderFillRect(renderer, &leftbar);
        SDL_RenderFillRect(renderer, &rightbar);
        SDL_RenderTexture(renderer, sprite[frame], NULL, &canvas);


        /* Render margins */
        SDL_SetRenderDrawColor(renderer, 29, 29, 29, 255);
        SDL_RenderFillRect(renderer, &leftmargin);
        SDL_RenderFillRect(renderer, &rightmargin);


        /* Render borders */
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderFillRect(renderer, &nameborder);
        SDL_SetRenderDrawColor(renderer, 29, 29, 29, 255);
        SDL_RenderFillRect(renderer, &bottomargin);
        SDL_RenderFillRect(renderer, &topmargin);


        /* Render UI */
        SDL_SetRenderDrawColor(renderer, 68, 68, 68, 255);
        if (contained(mouse, toolsrect) && toolnames[((int)((mouse.x-toolsrect.x)/(toolsrect.w/3)))+((int)((mouse.y-toolsrect.y)/(toolsrect.w/3))*3)] != "") {
            toolselectedrect = (SDL_FRect){ .x=(float)((int)((mouse.x-toolsrect.x)/(toolsrect.w/3))*(toolsrect.w/3))+toolsrect.x, .y=(float)((int)((mouse.y-toolsrect.y)/(toolsrect.w/3))*(toolsrect.w/3))+toolsrect.y, .w=toolselectedrect.w, .h=toolselectedrect.h };
            SDL_RenderFillRect(renderer, &toolselectedrect);
        }
        toolshoveredrect.x=(currentool%3)*(toolsrect.w/3)+toolsrect.x;
        toolshoveredrect.y=((int)(currentool/3))*(toolsrect.w/3)+toolsrect.y;
        SDL_RenderTexture(renderer, toolsborder, NULL, &toolshoveredrect);
        SDL_RenderTexture(renderer, tools, NULL, &toolsrect);
        SDL_SetRenderDrawColor(renderer, 58, 58, 58, 255);
        SDL_RenderFillRect(renderer, &cursizerectborder);
        SDL_SetRenderDrawColor(renderer, 15, 15, 15, 255);
        SDL_RenderFillRect(renderer, &cursizerectinborder);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        cursizerect = (SDL_FRect){ .x=(float)limit(-((canvas.w/resolution.x)*(int)cursize)/2+cursizerectborder.x+(cursizerectborder.w/2),cursizerectinborder.x), .y=(float)limit(cursizerectinborder.y-(cursizerectinborder.w/2)+((canvas.w/resolution.y)/2*(int)cursize),NULL,cursizerectinborder.y), .w=(float)limit(canvas.w/resolution.x*(int)cursize,NULL,cursizerectinborder.w), .h=-(float)limit(canvas.h/resolution.y*(int)cursize,NULL,-cursizerectinborder.h) };
        SDL_RenderFillRect(renderer, &cursizerect);


        /* Render UI text */
        SDL_RenderTexture(renderer, cursizetextrecture, NULL, &cursizetextrect);


        /* Render title text */
        SDL_RenderTexture(renderer, title, NULL, &tirect);


        /* Push render content */
        SDL_RenderPresent(renderer);


        /* Update mouse variables */
        framelastmouse = mouse;


        /* Wait if unfocussed */
        if (!focus) SDL_Delay((Uint32)1000);
    }


    /* Exit properly */
    SDL_DestroyTexture(title);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0; }

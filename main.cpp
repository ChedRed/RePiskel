#include "SDL3/SDL_blendmode.h"
#include "SDL3/SDL_error.h"
#include <iostream>
#include <cmath>
#include <string>
#include <vector>
#include <algorithm>
#include <optional>
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


/* Simple math things */
#define M_EIGHTHPI (((2*M_PI)/360)*22.5)


/* Display setup variables */
deltadata dtdata;
vec2 windowsize = { 1000, 650 };
SDL_Color gridC = { .r=76, .g=76, .b=76, .a=255 };
SDL_FRect grid = { 0, 0, 8, 8 };


/* Input setup variables */
fvec2 mouse = { 0, 0 };
fvec2 scroll = { 0, 0 };
bool mousedowned = false;
fvec2 lastmouse = { 0, 0 };
fvec2 framelastmouse = { 0, 0 };
Uint32 mousebitmask;
const Uint8 * keystates = SDL_GetKeyboardState(NULL);
bool oldshift = false;
bool oldmousedown = false;


/* UI */
duo margin = { 240, 260 };
SDL_FRect tirect;
SDL_FRect nameborder = { 0, 0, (float)windowsize.x, 36 };


/* Canvas */
fvec2 resolution = { 16, 16 };
fvec2 drawresolution = { 0, 0 };
fvec2 resratio = { (resolution.x<resolution.y)?(float)resolution.x/resolution.y:1, (resolution.x>resolution.y)?(float)resolution.y/resolution.x:1 };
fvec2 canvascenter = { (margin.a/2)+((windowsize.x-margin.b)/2), (22)+(((float)windowsize.y-8)/2) };
SDL_FRect precanvas = { margin.a, 44, windowsize.x-margin.a-margin.b, (float)windowsize.y-52 };
fvec2 canvasize = { resratio.x*((resolution.x<resolution.y)?precanvas.h:precanvas.w), resratio.y*((resolution.x>resolution.y)?precanvas.h:precanvas.w) };
fvec2 oldcanvasize = canvasize;
SDL_FRect canvas = { canvascenter.x-(canvasize.x/2), canvascenter.y-(canvasize.y/2), canvasize.x, canvasize.y };
SDL_Texture * presprite;
void* prespritepixels;
int prespritepitch;
std::vector<SDL_Texture * > sprite;
SDL_Texture * cursorture;
SDL_FRect cursorturect = { 0, 0, 1, 1 };
SDL_FRect spriterect;
int frame = 0;
bool skiprespriterender = false;


/* Canvas drawing variables */
void*pixels;
int pitch;
SDL_Color leftcolor = (SDL_Color){ .r=0, .g=0, .b=0, .a=255 };
SDL_Color rightcolor = (SDL_Color){ .r=0, .g=0, .b=0, .a=0 };


/* Custom SDL3 items */
SDL_BlendMode straightdarken = SDL_ComposeCustomBlendMode(SDL_BLENDFACTOR_SRC_ALPHA, SDL_BLENDFACTOR_ONE, SDL_BLENDOPERATION_REV_SUBTRACT, SDL_BLENDFACTOR_ZERO, SDL_BLENDFACTOR_ONE, SDL_BLENDOPERATION_REV_SUBTRACT);


/* Undo variables */
int unlimit = 512;
int undupdateorder = 1;
std::vector<std::vector<SDL_Texture * > > undotextures;
bool resizeundo = false;
bool undupdatequeued = false;
bool undupdatequeuedb = false;


/* UI elements */
float toolsuiwidth = 96;
SDL_FRect toolsrect = { 8, 0, 0, 0 };
int currentool = 0;
SDL_FRect toolshoveredrect = { 0, 0, 48, 48 };
SDL_FRect toolselectedrect = { 0, 0, 48, 48 };
std::string toolnames[18] = {"Pen", "Line", "Eraser", "Mirror", "Dither", "Lighten", "Fill", "Multi-Fill", "", "Rectangle", "Circle", "", "Rectangle Select", "Lasso Select", "Magic Select", "Grab", "Gridlock", "Pick Color"};
SDL_FRect cursizerectinborder;
SDL_FRect cursizerectborder;
SDL_FRect cursizerect;
float cursize = 1;
SDL_FRect cursizetextrect;
SDL_FRect selectedcolorect = { 0, 0, 36, 36 };
SDL_FRect selectedcolorealrect = { 0, 0, selectedcolorect.w-8, selectedcolorect.h-8 };
SDL_Vertex leftcoloralphapreview[] = {
    {{selectedcolorealrect.x, selectedcolorealrect.y}, {(float)leftcolor.r/255, (float)leftcolor.g/255, (float)leftcolor.b/255, (float)leftcolor.a/255}, {0, 0}},
    {{selectedcolorealrect.x+selectedcolorealrect.w, selectedcolorealrect.y}, {(float)leftcolor.r/255, (float)leftcolor.g/255, (float)leftcolor.b/255, (float)leftcolor.a/255}, {0, 0}},
    {{selectedcolorealrect.x, selectedcolorealrect.y+selectedcolorealrect.y}, {(float)leftcolor.r/255, (float)leftcolor.g/255, (float)leftcolor.b/255, (float)leftcolor.a/255}, {0, 0}}
};
SDL_Vertex leftcolorpreview[] = {
    {{selectedcolorealrect.x+selectedcolorealrect.w, selectedcolorealrect.y}, {(float)leftcolor.r/255, (float)leftcolor.g/255, (float)leftcolor.b/255, 1}, {0, 0}},
    {{selectedcolorealrect.x+selectedcolorealrect.w, selectedcolorealrect.y+selectedcolorealrect.h}, {(float)leftcolor.r/255, (float)leftcolor.g/255, (float)leftcolor.b/255, 1}, {0, 0}},
    {{selectedcolorealrect.x, selectedcolorealrect.y+selectedcolorealrect.y}, {(float)leftcolor.r/255, (float)leftcolor.g/255, (float)leftcolor.b/255, 1}, {0, 0}}
};
SDL_Vertex rightcoloralphapreview[] = {
    {{selectedcolorealrect.x, selectedcolorealrect.y}, {(float)leftcolor.r/255, (float)rightcolor.g/255, (float)rightcolor.b/255, (float)rightcolor.a/255}, {0, 0}},
    {{selectedcolorealrect.x+selectedcolorealrect.w, selectedcolorealrect.y}, {(float)rightcolor.r/255, (float)rightcolor.g/255, (float)rightcolor.b/255, (float)rightcolor.a/255}, {0, 0}},
    {{selectedcolorealrect.x, selectedcolorealrect.y+selectedcolorealrect.y}, {(float)rightcolor.r/255, (float)rightcolor.g/255, (float)rightcolor.b/255, (float)rightcolor.a/255}, {0, 0}}
};
SDL_Vertex rightcolorpreview[] = {
    {{selectedcolorealrect.x+selectedcolorealrect.w, selectedcolorealrect.y}, {(float)rightcolor.r/255, (float)rightcolor.g/255, (float)rightcolor.b/255, 1}, {0, 0}},
    {{selectedcolorealrect.x+selectedcolorealrect.w, selectedcolorealrect.y+selectedcolorealrect.h}, {(float)rightcolor.r/255, (float)rightcolor.g/255, (float)rightcolor.b/255, 1}, {0, 0}},
    {{selectedcolorealrect.x, selectedcolorealrect.y+selectedcolorealrect.y}, {(float)rightcolor.r/255, (float)rightcolor.g/255, (float)rightcolor.b/255, 1}, {0, 0}}
};
SDL_FRect colorselector = { 0, 0, 250, 200 };
bool colorselectorvisible = false;


/* Outer border */
SDL_Vertex visualborders[] = {
    {{0, 36}, {(float)29/255, (float)29/255, (float)29/255, 1}, {0, 0}},
    {{0, (float)windowsize.y}, {(float)29/255, (float)29/255, (float)29/255, 1}, {0, 0}},
    {{margin.a, 36}, {(float)29/255, (float)29/255, (float)29/255, 1}, {0, 0}},
    {{margin.a, 44}, {(float)29/255, (float)29/255, (float)29/255, 1}, {0, 0}},
    {{margin.a, (float)windowsize.y-8}, {(float)29/255, (float)29/255, (float)29/255, 1}, {0, 0}},
    {{margin.a, (float)windowsize.y}, {(float)29/255, (float)29/255, (float)29/255, 1}, {0, 0}},
    {{windowsize.x-margin.b, 36}, {(float)29/255, (float)29/255, (float)29/255, 1}, {0, 0}},
    {{windowsize.x-margin.b, 44}, {(float)29/255, (float)29/255, (float)29/255, 1}, {0, 0}},
    {{windowsize.x-margin.b, (float)windowsize.y-8}, {(float)29/255, (float)29/255, (float)29/255, 1}, {0, 0}},
    {{windowsize.x-margin.b, (float)windowsize.y}, {(float)29/255, (float)29/255, (float)29/255, 1}, {0, 0}},
    {{(float)windowsize.x, 36}, {(float)29/255, (float)29/255, (float)29/255, 1}, {0, 0}},
    {{(float)windowsize.x, (float)windowsize.y}, {(float)29/255, (float)29/255, (float)29/255, 1}, {0, 0}}
};
int visualborderindicies[] = { 0, 1, 2, 2, 1, 5, 2, 3, 7, 7, 6, 2, 4, 5, 9, 9, 8, 4, 6, 9, 10, 10, 9, 11 };


/* Canvas border */
SDL_Vertex canvasborders[] = {
    {{precanvas.x, precanvas.y}, {(float)160/255, (float)160/255, (float)160/255, 1}, {0, 0}},
    {{canvas.x, canvas.y}, {(float)160/255, (float)160/255, (float)160/255, 1}, {0, 0}},
    {{canvas.x, canvas.y+canvas.h}, {(float)160/255, (float)160/255, (float)160/255, 1}, {0, 0}},
    {{precanvas.x, precanvas.y+precanvas.h}, {(float)160/255, (float)160/255, (float)160/255, 1}, {0, 0}},
    {{precanvas.x+precanvas.w, precanvas.y}, {(float)160/255, (float)160/255, (float)160/255, 1}, {0, 0}},
    {{canvas.x+precanvas.w, canvas.y}, {(float)160/255, (float)160/255, (float)160/255, 1}, {0, 0}},
    {{canvas.x+precanvas.w, canvas.y+canvas.h}, {(float)160/255, (float)160/255, (float)160/255, 1}, {0, 0}},
    {{precanvas.x+precanvas.w, precanvas.y+precanvas.h}, {(float)160/255, (float)160/255, (float)160/255, 1}, {0, 0}}
};
int canvasborderindicies[] = { 0, 1, 3, 3, 2, 1, 2, 3, 7, 7, 6, 2, 6, 7, 4, 4, 5, 6, 5, 4, 1, 1, 0, 4 };


/* Limit function */
double limit(double value, std::optional<double> min = NULL, std::optional<double> max = NULL) { return ((min!=NULL)?((max!=NULL)?(value>max?max:(value<min?min:value)):(value<min?min:value)):(max!=NULL)?(value>max?max:value):value).value(); }


/* Contained function */
bool contained(fvec2 point, SDL_FRect container) { return ((container.w>0)?point.x>container.x:point.x<container.x) && ((container.h>0)?point.y>container.y:point.y<container.y) && ((container.w>0)?point.x<container.x+container.w:point.x>container.x+container.w) && ((container.h>0)?point.y<container.y+container.h:point.y>container.y+container.h); }


/* Max/Min functions */
double max(double a, double b) { return (a>b)?a:b; }
double min(double a, double b) { return (a<b)?a:b; }


/* Secondary line function for dithering */
void ditherline(SDL_Renderer * renderer, SDL_Color cola, SDL_Color colb, vec2 start, vec2 end, bool primary) {
    vec2 distance = { abs(end.x-start.x), abs(end.y-start.y) };
    vec2 mirror = { (start.x<end.x)?1:-1, (start.y<end.y) ?1:-1 };
    int err = distance.x-distance.y;

    while (true) {
        if (primary){
            ((start.x+start.y)%2==0)?SDL_SetRenderDrawColor(renderer, cola.r, cola.g, cola.b, cola.a):SDL_SetRenderDrawColor(renderer, colb.r, colb.g, colb.b, colb.a);
        }
        else{
            ((start.x+start.y)%2==1)?SDL_SetRenderDrawColor(renderer, cola.r, cola.g, cola.b, cola.a):SDL_SetRenderDrawColor(renderer, colb.r, colb.g, colb.b, colb.a);
        }
        SDL_RenderPoint(renderer, start.x, start.y);

        if (start.x==end.x && start.y==end.y) break;

        if (2*err>-distance.y) {
            err -= distance.y;
            start.x += mirror.x;
        }
        if (2*err<distance.x) {
            err += distance.x;
            start.y += mirror.y;
        }
    }
}


/* Tertiary line function for lighten */
void lightenline(SDL_Renderer * renderer, vec2 start, vec2 end, SDL_Rect bound, bool darken, bool add) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 16);
    if (add) (darken)?SDL_SetRenderDrawBlendMode(renderer, straightdarken):SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_ADD);
    else SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    vec2 distance = { abs(end.x-start.x), abs(end.y-start.y) };
    vec2 mirror = { (start.x<end.x)?1:-1, (start.y<end.y) ?1:-1 };
    int err = distance.x-distance.y;

    while (true) {

        if (contained((fvec2){ (float)start.x,(float)start.y }, (SDL_FRect){ bound.x, bound.y, bound.w, bound.h })) { SDL_RenderPoint(renderer, start.x, start.y) };
        std::cout << SDL_GetError() << std::endl;

        if (start.x==end.x && start.y==end.y) break;

        if (2*err>-distance.y) {
            err -= distance.y;
            start.x += mirror.x;
        }
        if (2*err<distance.x) {
            err += distance.x;
            start.y += mirror.y;
        }
    }
}


/* Main! */
int main() {
    undotextures.resize(1);

    /* Initialize SDL, create window and renderer */
    std::cout << "Initializing SDL3" << std::endl;
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window * window = SDL_CreateWindow("RePiskel", windowsize.x, windowsize.y, SDL_WINDOW_RESIZABLE | SDL_WINDOW_MOUSE_CAPTURE);
    SDL_Renderer * renderer = SDL_CreateRenderer(window, NULL);
    SDL_SetWindowMinimumSize(window, 960, 540);
    SDL_SetRenderVSync(renderer, 1);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
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
    toolsrect = (SDL_FRect){toolsrect.x, ((float)windowsize.y/2)-(toolsrect.h/(toolsrect.h/toolsuiwidth)), toolsuiwidth, toolsuiwidth*(toolsrect.h/toolsrect.w) };
    toolshoveredrect = (SDL_FRect){0, 0, toolsrect.w/3, toolsrect.w/3 };
    toolselectedrect = (SDL_FRect){0, 0, toolsrect.w/3, toolsrect.w/3 };
    cursizerectborder = (SDL_FRect){toolsrect.x, toolsrect.y, toolsrect.w, -toolsrect.w };
    cursizerectinborder = (SDL_FRect){toolsrect.x+4, toolsrect.y-4, toolsrect.w-8, -toolsrect.w+8 };
    snprintf(tempath, sizeof(tempath), "%s%s", SDL_GetBasePath(), "../Resources/border.bmp");
    SDL_Surface * pretoolsborder = SDL_LoadBMP(tempath);
    SDL_Texture * toolsborder = SDL_CreateTextureFromSurface(renderer, pretoolsborder);
    SDL_SetTextureScaleMode(toolsborder, SDL_SCALEMODE_NEAREST);
    snprintf(tempath, sizeof(tempath), "%s%s", SDL_GetBasePath(), "../Resources/colorselector.bmp");
    SDL_Surface * tempcolorselector = SDL_LoadBMP(tempath);
    SDL_Texture * colorselector = SDL_CreateTextureFromSurface(renderer, tempcolorselector);
    SDL_SetTextureScaleMode(colorselector, SDL_SCALEMODE_NEAREST);
    sprite.push_back(SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, resolution.x, resolution.y));
    presprite = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, resolution.x, resolution.y);
    cursorture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, resolution.x, resolution.y);
    SDL_SetRenderTarget(renderer, sprite[frame]);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
    SDL_SetRenderTarget(renderer, presprite);
    SDL_RenderClear(renderer);
    SDL_SetRenderTarget(renderer, NULL);
    undotextures[frame].push_back(SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, resolution.x, resolution.y));
    SDL_SetRenderTarget(renderer, undotextures[frame][0]);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
    SDL_SetRenderTarget(renderer, NULL);
    SDL_SetTextureScaleMode(presprite, SDL_SCALEMODE_NEAREST);
    SDL_SetTextureScaleMode(sprite[0], SDL_SCALEMODE_NEAREST);
    SDL_SetTextureScaleMode(cursorture, SDL_SCALEMODE_NEAREST);
    SDL_GetTextureSize(presprite, &spriterect.w, &spriterect.h);
    spriterect = (SDL_FRect){0, 0, spriterect.w, spriterect.h };
    SDL_Surface * tempcursizetextrect = TTF_RenderText_Blended(font, "1x", (SDL_Color){ .r=255, .g=255, .b=255, .a=255 });
    SDL_Texture * cursizetextrecture = SDL_CreateTextureFromSurface(renderer, tempcursizetextrect);
    SDL_DestroySurface(tempcursizetextrect);
    SDL_GetTextureSize(cursizetextrecture, &cursizetextrect.w, &cursizetextrect.h);
    cursizetextrect = (SDL_FRect){cursizerectborder.x+((cursizerectborder.w-cursizetextrect.w)/2), cursizerectborder.y+cursizerectborder.h-cursizetextrect.h, cursizetextrect.w, cursizetextrect.h };


    /* Setup variables that require data from textures */
    selectedcolorealrect = (SDL_FRect){8+((selectedcolorect.w-selectedcolorealrect.w)/2), toolsrect.y+toolsrect.h+8+((selectedcolorect.h-selectedcolorealrect.h)/2), selectedcolorealrect.w, selectedcolorealrect.h };
    leftcoloralphapreview[0].position.x=selectedcolorealrect.x;
    leftcoloralphapreview[0].position.y=selectedcolorealrect.y;
    leftcoloralphapreview[1].position.x=selectedcolorealrect.x+selectedcolorealrect.w;
    leftcoloralphapreview[1].position.y=selectedcolorealrect.y;
    leftcoloralphapreview[2].position.x=selectedcolorealrect.x;
    leftcoloralphapreview[2].position.y=selectedcolorealrect.y+selectedcolorealrect.h;
    leftcolorpreview[0].position.x=selectedcolorealrect.x+selectedcolorealrect.w;
    leftcolorpreview[0].position.y=selectedcolorealrect.y;
    leftcolorpreview[1].position.x=selectedcolorealrect.x+selectedcolorealrect.w;
    leftcolorpreview[1].position.y=selectedcolorealrect.y+selectedcolorealrect.h;
    leftcolorpreview[2].position.x=selectedcolorealrect.x;
    leftcolorpreview[2].position.y=selectedcolorealrect.y+selectedcolorealrect.h;
    selectedcolorealrect = (SDL_FRect){toolsrect.w-selectedcolorect.w+8+((selectedcolorect.w-selectedcolorealrect.w)/2), toolsrect.y+toolsrect.h+8+((selectedcolorect.h-selectedcolorealrect.h)/2), selectedcolorealrect.w, selectedcolorealrect.h };
    rightcoloralphapreview[0].position.x=selectedcolorealrect.x;
    rightcoloralphapreview[0].position.y=selectedcolorealrect.y;
    rightcoloralphapreview[1].position.x=selectedcolorealrect.x+selectedcolorealrect.w;
    rightcoloralphapreview[1].position.y=selectedcolorealrect.y;
    rightcoloralphapreview[2].position.x=selectedcolorealrect.x;
    rightcoloralphapreview[2].position.y=selectedcolorealrect.y+selectedcolorealrect.h;
    rightcolorpreview[0].position.x=selectedcolorealrect.x+selectedcolorealrect.w;
    rightcolorpreview[0].position.y=selectedcolorealrect.y;
    rightcolorpreview[1].position.x=selectedcolorealrect.x+selectedcolorealrect.w;
    rightcolorpreview[1].position.y=selectedcolorealrect.y+selectedcolorealrect.h;
    rightcolorpreview[2].position.x=selectedcolorealrect.x;
    rightcolorpreview[2].position.y=selectedcolorealrect.y+selectedcolorealrect.h;


    /* Remove unecessary data */
    SDL_DestroySurface(pretoolsborder);
    SDL_DestroySurface(pretools);
    SDL_DestroySurface(tempcolorselector);


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
                    nameborder.w = windowsize.x;
                    tirect.x = ((float)windowsize.x/2)-((float)tirect.w/2);


                    /* Reset canvas */
                    precanvas = (SDL_FRect){margin.a, 44, windowsize.x-margin.a-margin.b, (float)windowsize.y-52 };
                    canvascenter = (fvec2){(margin.a/2)+((windowsize.x-margin.b)/2), 22+(((float)windowsize.y-8)/2) };
                    canvasize = (fvec2){resratio.x*((precanvas.w>precanvas.h)?precanvas.h:precanvas.w), resratio.y*((precanvas.w>precanvas.h)?precanvas.h:precanvas.w) };
                    canvas = (SDL_FRect){canvas.x, canvas.y, canvas.w*(canvasize.x/oldcanvasize.x), canvas.h*(canvasize.y/oldcanvasize.y) };
                    if (canvas.w<=precanvas.w) canvas.x=(precanvas.x+((precanvas.w-canvas.w)/2));
                    if (canvas.h<=precanvas.h) canvas.y=(precanvas.y+((precanvas.h-canvas.h)/2));
                    oldcanvasize=canvasize;


                    /* Reset UI */
                    toolsrect.y=((float)windowsize.y/2)-toolsrect.w;
                    cursizerectborder.y=toolsrect.y;
                    cursizerectinborder.y=toolsrect.y-4;


                    /* Reset pen size text */
                    cursizetextrect = (SDL_FRect){cursizerectborder.x+((cursizerectborder.w-cursizetextrect.w)/2), cursizerectborder.y+cursizerectborder.h-cursizetextrect.h, cursizetextrect.w, cursizetextrect.h };


                    /* Reset color preview position */
                    selectedcolorealrect = (SDL_FRect){8+((selectedcolorect.w-selectedcolorealrect.w)/2), toolsrect.y+toolsrect.h+8+((selectedcolorect.h-selectedcolorealrect.h)/2), selectedcolorealrect.w, selectedcolorealrect.h };
                    leftcoloralphapreview[0].position.x=selectedcolorealrect.x;
                    leftcoloralphapreview[0].position.y=selectedcolorealrect.y;
                    leftcoloralphapreview[1].position.x=selectedcolorealrect.x+selectedcolorealrect.w;
                    leftcoloralphapreview[1].position.y=selectedcolorealrect.y;
                    leftcoloralphapreview[2].position.x=selectedcolorealrect.x;
                    leftcoloralphapreview[2].position.y=selectedcolorealrect.y+selectedcolorealrect.h;
                    leftcolorpreview[0].position.x=selectedcolorealrect.x+selectedcolorealrect.w;
                    leftcolorpreview[0].position.y=selectedcolorealrect.y;
                    leftcolorpreview[1].position.x=selectedcolorealrect.x+selectedcolorealrect.w;
                    leftcolorpreview[1].position.y=selectedcolorealrect.y+selectedcolorealrect.h;
                    leftcolorpreview[2].position.x=selectedcolorealrect.x;
                    leftcolorpreview[2].position.y=selectedcolorealrect.y+selectedcolorealrect.h;
                    selectedcolorealrect = (SDL_FRect){toolsrect.w-selectedcolorect.w+8+((selectedcolorect.w-selectedcolorealrect.w)/2), toolsrect.y+toolsrect.h+8+((selectedcolorect.h-selectedcolorealrect.h)/2), selectedcolorealrect.w, selectedcolorealrect.h };
                    rightcoloralphapreview[0].position.x=selectedcolorealrect.x;
                    rightcoloralphapreview[0].position.y=selectedcolorealrect.y;
                    rightcoloralphapreview[1].position.x=selectedcolorealrect.x+selectedcolorealrect.w;
                    rightcoloralphapreview[1].position.y=selectedcolorealrect.y;
                    rightcoloralphapreview[2].position.x=selectedcolorealrect.x;
                    rightcoloralphapreview[2].position.y=selectedcolorealrect.y+selectedcolorealrect.h;
                    rightcolorpreview[0].position.x=selectedcolorealrect.x+selectedcolorealrect.w;
                    rightcolorpreview[0].position.y=selectedcolorealrect.y;
                    rightcolorpreview[1].position.x=selectedcolorealrect.x+selectedcolorealrect.w;
                    rightcolorpreview[1].position.y=selectedcolorealrect.y+selectedcolorealrect.h;
                    rightcolorpreview[2].position.x=selectedcolorealrect.x;
                    rightcolorpreview[2].position.y=selectedcolorealrect.y+selectedcolorealrect.h;


                    /* Reset visual borders */
                    visualborders[1].position.y = (float)windowsize.y;
                    visualborders[4].position.y = (float)windowsize.y-8;
                    visualborders[5].position.y = (float)windowsize.y;
                    visualborders[6].position.x = windowsize.x-margin.b;
                    visualborders[7].position.x = windowsize.x-margin.b;
                    visualborders[8].position = (SDL_FPoint){ windowsize.x-margin.b, (float)windowsize.y-8 };
                    visualborders[9].position = (SDL_FPoint){ windowsize.x-margin.b, (float)windowsize.y };
                    visualborders[10].position.x = (float)windowsize.x;
                    visualborders[11].position = (SDL_FPoint){ (float)windowsize.x, (float)windowsize.y };


                    /* Reset canvas borders */
                    canvasborders[0].position = (SDL_FPoint){ precanvas.x, precanvas.y };
                    canvasborders[1].position = (SDL_FPoint){ canvas.x, canvas.y };
                    canvasborders[2].position = (SDL_FPoint){ canvas.x, canvas.y+canvas.h };
                    canvasborders[3].position = (SDL_FPoint){ precanvas.x, precanvas.y+precanvas.h };
                    canvasborders[4].position = (SDL_FPoint){ precanvas.x+precanvas.w, precanvas.y };
                    canvasborders[5].position = (SDL_FPoint){ canvas.x+canvas.w, canvas.y };
                    canvasborders[6].position = (SDL_FPoint){ canvas.x+canvas.w, canvas.y+canvas.h };
                    canvasborders[7].position = (SDL_FPoint){ precanvas.x+precanvas.w, precanvas.y+precanvas.h };


                /* Zoom canvas */
                case SDL_EVENT_MOUSE_WHEEL:


                    /* Reset scroll */
                    scroll.x = e.wheel.x;
                    scroll.y = e.wheel.y;


                    /* Reset canvas */
                    if (keystates[SDL_SCANCODE_LCTRL]){
                        canvas = (SDL_FRect){(canvas.w<precanvas.w)?canvascenter.x-((canvas.w*(1+(scroll.y/100)))/2):((canvas.x-mouse.x)*(1+(scroll.y/100)))+mouse.x, (canvas.h<precanvas.h)?canvascenter.y-((canvas.h*(1+(scroll.y/100)))/2):((canvas.y-mouse.y)*(1+(scroll.y/100)))+mouse.y, canvas.w*(1+(scroll.y/100)), canvas.h*(1+(scroll.y/100)) };
                        if (canvas.w>precanvas.w) {
                            (canvas.x>precanvas.x)?canvas.x=precanvas.x:canvas.x;
                            (canvas.x+canvas.w<precanvas.x+precanvas.w)?canvas.x=precanvas.x-(canvas.w-precanvas.w):canvas.x;
                        }
                        if (canvas.h>precanvas.h) {
                            (canvas.y>precanvas.y)?canvas.y=precanvas.y:canvas.y;
                            (canvas.y+canvas.h<precanvas.y+precanvas.h)?canvas.y=precanvas.y-(canvas.h-precanvas.h):canvas.y;
                        }
                        canvasborders[0].position = (SDL_FPoint){ precanvas.x, precanvas.y };
                        canvasborders[1].position = (SDL_FPoint){ canvas.x, canvas.y };
                        canvasborders[2].position = (SDL_FPoint){ canvas.x, canvas.y+canvas.h };
                        canvasborders[3].position = (SDL_FPoint){ precanvas.x, precanvas.y+precanvas.h };
                        canvasborders[4].position = (SDL_FPoint){ precanvas.x+precanvas.w, precanvas.y };
                        canvasborders[5].position = (SDL_FPoint){ canvas.x+canvas.w, canvas.y };
                        canvasborders[6].position = (SDL_FPoint){ canvas.x+canvas.w, canvas.y+canvas.h };
                        canvasborders[7].position = (SDL_FPoint){ precanvas.x+precanvas.w, precanvas.y+precanvas.h };
                    }


                    /* Reset pen size */
                    else{
                        if ((int)cursize!=(int)limit(cursize+(scroll.y/10), 1, max(resolution.x, resolution.y))){
                            SDL_DestroyTexture(cursizetextrecture);
                            char tempchar[256];
                            snprintf(tempchar, sizeof(tempchar), "%d%s", (int)limit(cursize+(scroll.y/10), 1, max(resolution.x, resolution.y)), "x");
                            tempcursizetextrect = TTF_RenderText_Blended(font, tempchar, (SDL_Color){ .r=255, .g=255, .b=255, .a=255 });
                            cursizetextrecture = SDL_CreateTextureFromSurface(renderer, tempcursizetextrect);
                            SDL_DestroySurface(tempcursizetextrect);
                            SDL_GetTextureSize(cursizetextrecture, &cursizetextrect.w, &cursizetextrect.h);
                            SDL_DestroySurface(tempcursizetextrect);
                            remove(tempchar);
                        }
                        cursize=limit(cursize+(scroll.y/10), 1, max(resolution.x, resolution.y));


                        /* Reset pen size text */
                        cursizetextrect = (SDL_FRect){cursizerectborder.x+((cursizerectborder.w-cursizetextrect.w)/2), cursizerectborder.y+cursizerectborder.h-cursizetextrect.h, cursizetextrect.w, cursizetextrect.h };
                    }




                /* Reduce FPS if unfocussed */
                case SDL_EVENT_WINDOW_FOCUS_GAINED:
                    focus = true;
                    break;
                case SDL_EVENT_WINDOW_FOCUS_LOST:
                    focus = false;
                    break;


                /* Interact with UI */
                case SDL_EVENT_MOUSE_BUTTON_DOWN:
                    if (contained(mouse, toolsrect) && !(toolnames[((int)((mouse.x-toolsrect.x)/(toolsrect.w/3)))+((int)((mouse.y-toolsrect.y)/(toolsrect.w/3))*3)]=="")) currentool=((int)((mouse.x-toolsrect.x)/(toolsrect.w/3)))+((int)((mouse.y-toolsrect.y)/(toolsrect.w/3))*3);
                    lastmouse = mouse;
                    break;


                /* Update canvas */
                case SDL_EVENT_MOUSE_BUTTON_UP:
                    if ((mousebitmask & SDL_BUTTON_LMASK || mousebitmask & SDL_BUTTON_RMASK) && currentool == 1 && contained(lastmouse, canvas)){
                        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
                        SDL_SetRenderTarget(renderer, sprite[frame]);
                        if (mousebitmask & SDL_BUTTON_LMASK) SDL_SetRenderDrawColor(renderer, leftcolor.r, leftcolor.g, leftcolor.b, leftcolor.a);
                        else if (mousebitmask & SDL_BUTTON_RMASK) SDL_SetRenderDrawColor(renderer, rightcolor.r, rightcolor.g, rightcolor.b, rightcolor.a);
                        fvec2 roundlastmouse = (fvec2){(float)(int)(((lastmouse.x-((canvas.w/resolution.x)*((int)cursize-1)/2))-canvas.x)/(canvas.w/resolution.x)), (float)(int)(((lastmouse.y-((canvas.h/resolution.y)*((int)cursize-1)/2))-canvas.y)/(canvas.h/resolution.y)) };
                        fvec2 roundmouse = (fvec2){(float)(int)(((mouse.x-((canvas.w/resolution.x)*((int)cursize-1)/2))-canvas.x)/(canvas.w/resolution.x)), (float)(int)(((mouse.y-((canvas.h/resolution.y)*((int)cursize-1)/2))-canvas.y)/(canvas.h/resolution.y)) };
                        fvec2 fakeroundmouse = roundmouse;
                        if (keystates[SDL_SCANCODE_LSHIFT]) fakeroundmouse = (fvec2){ (float)(sin(round(atan2(roundmouse.x-roundlastmouse.x, roundmouse.y-roundlastmouse.y)/M_EIGHTHPI)*M_EIGHTHPI)*sqrt(((roundmouse.x-roundlastmouse.x)*(roundmouse.x-roundlastmouse.x))+((roundmouse.y-roundlastmouse.y)*(roundmouse.y-roundlastmouse.y))))+roundlastmouse.x, (float)(cos(round(atan2(roundmouse.x-roundlastmouse.x, roundmouse.y-roundlastmouse.y)/M_EIGHTHPI)*M_EIGHTHPI)*sqrt(((roundmouse.x-roundlastmouse.x)*(roundmouse.x-roundlastmouse.x))+((roundmouse.y-roundlastmouse.y)*(roundmouse.y-roundlastmouse.y))))+roundlastmouse.y };
                        for (int y = 0; y < (int)cursize; y++) {
                            for (int x = 0; x < (int)cursize; x++) {
                                SDL_RenderLine(renderer, roundlastmouse.x+x, roundlastmouse.y+y, fakeroundmouse.x+x, fakeroundmouse.y+y );
                            }
                        }
                        SDL_SetRenderTarget(renderer, presprite);
                        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
                        SDL_RenderClear(renderer);
                        SDL_SetRenderTarget(renderer, NULL);
                        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
                    }
                    if ((mousebitmask & SDL_BUTTON_LMASK || mousebitmask & SDL_BUTTON_RMASK) && currentool == 5 && contained(lastmouse, canvas)) {
                        if (keystates[SDL_SCANCODE_LSHIFT] || oldshift) {
                            SDL_SetRenderTarget(renderer, sprite[frame]);
                            SDL_SetTextureBlendMode(presprite, (keystates[SDL_SCANCODE_LCTRL])?SDL_SetRenderDrawBlendMode(renderer, straightdarken):SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_ADD));
                            SDL_RenderTexture(renderer, presprite, NULL, &spriterect);
                            SDL_SetTextureBlendMode(presprite, SDL_BLENDMODE_BLEND);
                            SDL_SetRenderTarget(renderer, NULL);
                            skiprespriterender = true;
                        }
                    }
                    if (contained(lastmouse, canvas)) undupdatequeued = true;


                case SDL_EVENT_KEY_DOWN:


                    /* Undo/Redo */
                    if (keystates[SDL_SCANCODE_LGUI]) {
                        if (e.key.key == SDLK_Z) {
                            if (keystates[SDL_SCANCODE_LSHIFT]){
                                if (undupdateorder > 1) {
                                    undupdateorder -= 1;
                                    SDL_SetRenderTarget(renderer, sprite[frame]);
                                    SDL_SetTextureBlendMode(undotextures[frame][undotextures[frame].size()-undupdateorder], SDL_BLENDMODE_NONE);
                                    SDL_RenderTexture(renderer, undotextures[frame][undotextures[frame].size()-undupdateorder], NULL, NULL);
                                    SDL_SetTextureBlendMode(undotextures[frame][undotextures[frame].size()-undupdateorder], SDL_BLENDMODE_BLEND);
                                    SDL_SetRenderTarget(renderer, NULL);
                                }
                            }
                            else {
                                if (undotextures[frame].size() > 1) {
                                    if (undotextures[frame].size() - undupdateorder > 0) undupdateorder += 1;
                                    SDL_SetRenderTarget(renderer, sprite[frame]);
                                    SDL_SetTextureBlendMode(undotextures[frame][undotextures[frame].size()-undupdateorder], SDL_BLENDMODE_NONE);
                                    SDL_RenderTexture(renderer, undotextures[frame][undotextures[frame].size()-undupdateorder], NULL, NULL);
                                    SDL_SetTextureBlendMode(undotextures[frame][undotextures[frame].size()-undupdateorder], SDL_BLENDMODE_BLEND);
                                    SDL_SetRenderTarget(renderer, NULL);
                                    resizeundo = true;
                                }
                            }
                        }
                    }


                    /* Reset canvas size */
                    if (e.key.key == SDLK_0) {
                        canvas = (SDL_FRect){ canvascenter.x-(canvasize.x/2), canvascenter.y-(canvasize.y/2), canvasize.x, canvasize.y };
                        canvasborders[0].position = (SDL_FPoint){ precanvas.x, precanvas.y };
                        canvasborders[1].position = (SDL_FPoint){ canvas.x, canvas.y };
                        canvasborders[2].position = (SDL_FPoint){ canvas.x, canvas.y+canvas.h };
                        canvasborders[3].position = (SDL_FPoint){ precanvas.x, precanvas.y+precanvas.h };
                        canvasborders[4].position = (SDL_FPoint){ precanvas.x+precanvas.w, precanvas.y };
                        canvasborders[5].position = (SDL_FPoint){ canvas.x+canvas.w, canvas.y };
                        canvasborders[6].position = (SDL_FPoint){ canvas.x+canvas.w, canvas.y+canvas.h };
                        canvasborders[7].position = (SDL_FPoint){ precanvas.x+precanvas.w, precanvas.y+precanvas.h };
                    }
            }
        }


        /* Update canvas texture if necessary */
        if ((mousebitmask & SDL_BUTTON_LMASK || mousebitmask & SDL_BUTTON_RMASK) && contained(lastmouse, canvas)) {
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);


                /* Draw line from framelastmouse to mouse */
                if (currentool == 0) {
                    SDL_SetRenderTarget(renderer, sprite[frame]);
                    (mousebitmask & SDL_BUTTON_LMASK)?SDL_SetRenderDrawColor(renderer, leftcolor.r, leftcolor.g, leftcolor.b, leftcolor.a):SDL_SetRenderDrawColor(renderer, rightcolor.r, rightcolor.g, rightcolor.b, rightcolor.a);
                    for (int y = 0; y < (int)cursize; y++) {
                        for (int x = 0; x < (int)cursize; x++) {
                            SDL_RenderLine(renderer, (int)(((framelastmouse.x-((canvas.w/resolution.x)*((int)cursize-1)/2))-canvas.x)/(canvas.w/resolution.x))+x, (int)(((framelastmouse.y-((canvas.h/resolution.y)*((int)cursize-1)/2))-canvas.y)/(canvas.h/resolution.y))+y, (int)(((mouse.x-((canvas.w/resolution.x)*((int)cursize-1)/2))-canvas.x)/(canvas.w/resolution.x))+x, (int)(((mouse.y-((canvas.h/resolution.y)*((int)cursize-1)/2))-canvas.y)/(canvas.h/resolution.y))+y);
                        }
                    }
                    SDL_SetRenderTarget(renderer, NULL);
                }


                /* Draw line from lastmouse to mouse */
                else if (currentool == 1) {
                    SDL_SetRenderTarget(renderer, presprite);
                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
                    SDL_RenderClear(renderer);
                    SDL_SetRenderDrawColor(renderer, 160, 215, 240, 153);
                    fvec2 roundlastmouse = (fvec2){(float)(int)(((lastmouse.x-((canvas.w/resolution.x)*((int)cursize-1)/2))-canvas.x)/(canvas.w/resolution.x)), (float)(int)(((lastmouse.y-((canvas.h/resolution.y)*((int)cursize-1)/2))-canvas.y)/(canvas.h/resolution.y)) };
                    fvec2 roundmouse = (fvec2){(float)(int)(((mouse.x-((canvas.w/resolution.x)*((int)cursize-1)/2))-canvas.x)/(canvas.w/resolution.x)), (float)(int)(((mouse.y-((canvas.h/resolution.y)*((int)cursize-1)/2))-canvas.y)/(canvas.h/resolution.y)) };
                    fvec2 fakeroundmouse = roundmouse;
                    if (keystates[SDL_SCANCODE_LSHIFT]) fakeroundmouse = (fvec2){ (float)(sin(round(atan2(roundmouse.x-roundlastmouse.x, roundmouse.y-roundlastmouse.y)/M_EIGHTHPI)*M_EIGHTHPI)*sqrt(((roundmouse.x-roundlastmouse.x)*(roundmouse.x-roundlastmouse.x))+((roundmouse.y-roundlastmouse.y)*(roundmouse.y-roundlastmouse.y))))+roundlastmouse.x, (float)(cos(round(atan2(roundmouse.x-roundlastmouse.x, roundmouse.y-roundlastmouse.y)/M_EIGHTHPI)*M_EIGHTHPI)*sqrt(((roundmouse.x-roundlastmouse.x)*(roundmouse.x-roundlastmouse.x))+((roundmouse.y-roundlastmouse.y)*(roundmouse.y-roundlastmouse.y))))+roundlastmouse.y };
                    for (int y = 0; y < (int)cursize; y++) {
                        for (int x = 0; x < (int)cursize; x++) {
                            SDL_RenderLine(renderer, roundlastmouse.x+x, roundlastmouse.y+y, fakeroundmouse.x+x, fakeroundmouse.y+y );
                        }
                    }
                    SDL_SetRenderTarget(renderer, NULL);
                }


                /* Erase from framelastmouse to mouse */
                else if (currentool == 2) {
                    SDL_SetRenderTarget(renderer, sprite[frame]);
                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
                    for (int y = 0; y < (int)cursize; y++) {
                        for (int x = 0; x < (int)cursize; x++) {
                            SDL_RenderLine(renderer, (int)(((mouse.x-((canvas.w/resolution.x)*((int)cursize-1)/2))-canvas.x)/(canvas.w/resolution.x))+x, (int)(((mouse.y-((canvas.h/resolution.y)*((int)cursize-1)/2))-canvas.y)/(canvas.h/resolution.y))+y, (int)((mouse.x-canvas.x)/(canvas.w/resolution.x)), (int)((mouse.y-canvas.y)/(canvas.h/resolution.y)));
                        }
                    }
                    SDL_SetRenderTarget(renderer, NULL);
                }


                /* Draw line from framelastmouse to mouse mirrored */
                else if (currentool == 3) {
                    SDL_SetRenderTarget(renderer, sprite[frame]);
                    (mousebitmask & SDL_BUTTON_LMASK)?SDL_SetRenderDrawColor(renderer, leftcolor.r, leftcolor.g, leftcolor.b, leftcolor.a):SDL_SetRenderDrawColor(renderer, rightcolor.r, rightcolor.g, rightcolor.b, rightcolor.a);
                    for (int y = 0; y < (int)cursize; y++) {
                        for (int x = 0; x < (int)cursize; x++) {
                            SDL_RenderLine(renderer, (int)(((framelastmouse.x-((canvas.w/resolution.x)*((int)cursize-1)/2))-canvas.x)/(canvas.w/resolution.x))+x, (int)(((framelastmouse.y-((canvas.h/resolution.y)*((int)cursize-1)/2))-canvas.y)/(canvas.h/resolution.y))+y, (int)(((mouse.x-((canvas.w/resolution.x)*((int)cursize-1)/2))-canvas.x)/(canvas.w/resolution.x))+x, (int)(((mouse.y-((canvas.h/resolution.y)*((int)cursize-1)/2))-canvas.y)/(canvas.h/resolution.y))+y);
                        }
                    }
                    if (keystates[SDL_SCANCODE_LSHIFT]) {
                        for (int y = 0; y < (int)cursize; y++) {
                            for (int x = 0; x < (int)cursize; x++) {
                                SDL_RenderLine(renderer, (int)(((framelastmouse.x-((canvas.w/resolution.x)*((int)cursize-1)/2))-canvas.x)/(canvas.w/resolution.x))+x, -(int)(((framelastmouse.y-((canvas.h/resolution.y)*((int)cursize-1)/2))-canvas.y)/(canvas.h/resolution.y))+y+resolution.y-(int)cursize, (int)(((mouse.x-((canvas.w/resolution.x)*((int)cursize-1)/2))-canvas.x)/(canvas.w/resolution.x))+x, -(int)(((mouse.y-((canvas.h/resolution.y)*((int)cursize-1)/2))-canvas.y)/(canvas.h/resolution.y))+y+resolution.y-(int)cursize);
                            }
                        }
                        for (int y = 0; y < (int)cursize; y++) {
                            for (int x = 0; x < (int)cursize; x++) {
                                SDL_RenderLine(renderer, -(int)(((framelastmouse.x-((canvas.w/resolution.x)*((int)cursize-1)/2))-canvas.x)/(canvas.w/resolution.x))+x+resolution.x-(int)cursize, -(int)(((framelastmouse.y-((canvas.h/resolution.y)*((int)cursize-1)/2))-canvas.y)/(canvas.h/resolution.y))+y+resolution.y-(int)cursize, -(int)(((mouse.x-((canvas.w/resolution.x)*((int)cursize-1)/2))-canvas.x)/(canvas.w/resolution.x))+x+resolution.x-(int)cursize, -(int)(((mouse.y-((canvas.h/resolution.y)*((int)cursize-1)/2))-canvas.y)/(canvas.h/resolution.y))+y+resolution.y-(int)cursize);
                            }
                        }
                        for (int y = 0; y < (int)cursize; y++) {
                            for (int x = 0; x < (int)cursize; x++) {
                                SDL_RenderLine(renderer, -(int)(((framelastmouse.x-((canvas.w/resolution.x)*((int)cursize-1)/2))-canvas.x)/(canvas.w/resolution.x))+x+resolution.x-(int)cursize, (int)(((framelastmouse.y-((canvas.h/resolution.y)*((int)cursize-1)/2))-canvas.y)/(canvas.h/resolution.y))+y, -(int)(((mouse.x-((canvas.w/resolution.x)*((int)cursize-1)/2))-canvas.x)/(canvas.w/resolution.x))+x+resolution.x-(int)cursize, (int)(((mouse.y-((canvas.h/resolution.y)*((int)cursize-1)/2))-canvas.y)/(canvas.h/resolution.y))+y);
                            }
                        }
                    }
                    else if (keystates[SDL_SCANCODE_LCTRL]) {
                        for (int y = 0; y < (int)cursize; y++) {
                            for (int x = 0; x < (int)cursize; x++) {
                                SDL_RenderLine(renderer, (int)(((framelastmouse.x-((canvas.w/resolution.x)*((int)cursize-1)/2))-canvas.x)/(canvas.w/resolution.x))+x, -(int)(((framelastmouse.y-((canvas.h/resolution.y)*((int)cursize-1)/2))-canvas.y)/(canvas.h/resolution.y))+y+resolution.y-(int)cursize, (int)(((mouse.x-((canvas.w/resolution.x)*((int)cursize-1)/2))-canvas.x)/(canvas.w/resolution.x))+x, -(int)(((mouse.y-((canvas.h/resolution.y)*((int)cursize-1)/2))-canvas.y)/(canvas.h/resolution.y))+y+resolution.y-(int)cursize);
                            }
                        }
                    }
                    else {
                        for (int y = 0; y < (int)cursize; y++) {
                            for (int x = 0; x < (int)cursize; x++) {
                                SDL_RenderLine(renderer, -(int)(((framelastmouse.x-((canvas.w/resolution.x)*((int)cursize-1)/2))-canvas.x)/(canvas.w/resolution.x))+x+resolution.x-(int)cursize, (int)(((framelastmouse.y-((canvas.h/resolution.y)*((int)cursize-1)/2))-canvas.y)/(canvas.h/resolution.y))+y, -(int)(((mouse.x-((canvas.w/resolution.x)*((int)cursize-1)/2))-canvas.x)/(canvas.w/resolution.x))+x+resolution.x-(int)cursize, (int)(((mouse.y-((canvas.h/resolution.y)*((int)cursize-1)/2))-canvas.y)/(canvas.h/resolution.y))+y);
                            }
                        }
                    }
                    SDL_SetRenderTarget(renderer, NULL);
                }


                /* Draw dithered line from framelastmouse to mouse */
                else if (currentool == 4) {
                    SDL_SetRenderTarget(renderer, sprite[frame]);
                    for (int y = 0; y < (int)cursize; y++) {
                        for (int x = 0; x < (int)cursize; x++) {
                            ditherline(renderer, leftcolor, rightcolor, (vec2){(int)(((framelastmouse.x-((canvas.w/resolution.x)*((int)cursize-1)/2))-canvas.x)/(canvas.w/resolution.x))+x, (int)(((framelastmouse.y-((canvas.h/resolution.y)*((int)cursize-1)/2))-canvas.y)/(canvas.h/resolution.y))+y}, (vec2){(int)(((mouse.x-((canvas.w/resolution.x)*((int)cursize-1)/2))-canvas.x)/(canvas.w/resolution.x))+x, (int)(((mouse.y-((canvas.h/resolution.y)*((int)cursize-1)/2))-canvas.y)/(canvas.h/resolution.y))+y}, (mousebitmask & SDL_BUTTON_LMASK));
                        }
                    }
                    SDL_SetRenderTarget(renderer, NULL);
                }


                /* Increase or decrease a pixel's brightness */
                else if (currentool == 5) {
                    if (keystates[SDL_SCANCODE_LSHIFT]) {
                        SDL_SetRenderTarget(renderer, presprite);
                        if (!oldmousedown || !oldshift) {
                            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
                            SDL_RenderClear(renderer);
                        }
                        for (int y = 0; y < (int)cursize; y++) {
                            for (int x = 0; x < (int)cursize; x++) {
                                lightenline(renderer, (vec2){ (int)(((framelastmouse.x-((canvas.w/resolution.x)*((int)cursize-1)/2))-canvas.x)/(canvas.w/resolution.x))+x, (int)(((framelastmouse.y-((canvas.h/resolution.y)*((int)cursize-1)/2))-canvas.y)/(canvas.h/resolution.y))+y }, (vec2){ (int)(((mouse.x-((canvas.w/resolution.x)*((int)cursize-1)/2))-canvas.x)/(canvas.w/resolution.x))+x, (int)(((mouse.y-((canvas.h/resolution.y)*((int)cursize-1)/2))-canvas.y)/(canvas.h/resolution.y))+y }, keystates[SDL_SCANCODE_LCTRL], false);
                            }
                        }
                        SDL_SetRenderTarget(renderer, NULL);
                    }
                    else {
                        SDL_SetRenderTarget(renderer, sprite[frame]);
                        if (oldshift) {
                            SDL_RenderTexture(renderer, presprite, NULL, &spriterect);
                        }
                        else {
                            SDL_SetRenderTarget(renderer, presprite);
                            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
                            SDL_RenderClear(renderer);
                            SDL_SetRenderTarget(renderer, sprite[frame]);
                        }
                        for (int y = 0; y < (int)cursize; y++) {
                            for (int x = 0; x < (int)cursize; x++) {
                                lightenline(renderer, (vec2){ (int)(((framelastmouse.x-((canvas.w/resolution.x)*((int)cursize-1)/2))-canvas.x)/(canvas.w/resolution.x))+x, (int)(((framelastmouse.y-((canvas.h/resolution.y)*((int)cursize-1)/2))-canvas.y)/(canvas.h/resolution.y))+y }, (vec2){ (int)(((mouse.x-((canvas.w/resolution.x)*((int)cursize-1)/2))-canvas.x)/(canvas.w/resolution.x))+x, (int)(((mouse.y-((canvas.h/resolution.y)*((int)cursize-1)/2))-canvas.y)/(canvas.h/resolution.y))+y }, keystates[SDL_SCANCODE_LCTRL], true);
                            }
                        }
                        SDL_SetRenderTarget(renderer, NULL);
                    }
                }
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        }


        /* Update undo list */
        if (undupdatequeued){
            if (resizeundo) {
                undotextures[frame].erase(undotextures[frame].end()-undupdateorder+1, undotextures[frame].end());
                undupdateorder = 1;
            }
            undotextures[frame].push_back(SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, resolution.x, resolution.y));
            SDL_SetRenderTarget(renderer, undotextures[frame][undotextures[frame].size()-1]);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
            SDL_RenderClear(renderer);
            SDL_RenderTexture(renderer, sprite[frame], NULL, &spriterect);
            SDL_SetRenderTarget(renderer, NULL);
            if (undotextures[frame].size() > unlimit) {
                std::shift_left(undotextures[frame].begin(), undotextures[frame].end(), 1);
                undupdateorder += 1;
            }
            undupdatequeued = false;
        }


        /* Render canvas */
        SDL_RenderTexture(renderer, sprite[frame], NULL, &canvas);
        if ((mousebitmask & SDL_BUTTON_LMASK || mousebitmask & SDL_BUTTON_RMASK) && currentool == 1 && contained(lastmouse, canvas)) {
            SDL_RenderTexture(renderer, presprite, NULL, &canvas);
            SDL_SetRenderTarget(renderer, presprite);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
            SDL_RenderClear(renderer);
            SDL_SetRenderTarget(renderer, NULL);
        }
        if ((mousebitmask & SDL_BUTTON_LMASK || mousebitmask & SDL_BUTTON_RMASK) && (currentool == 4 || currentool == 5) && contained(lastmouse, canvas)) {
            if (skiprespriterender) skiprespriterender = false;
            else SDL_RenderTexture(renderer, presprite, NULL, &canvas);
        }
        if (contained(mouse, canvas) && !(mousebitmask & SDL_BUTTON_LMASK || mousebitmask & SDL_BUTTON_RMASK) && focus){
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
            SDL_SetRenderTarget(renderer, cursorture);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
            SDL_RenderClear(renderer);
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 51);
            cursorturect = (SDL_FRect){(float)(int)((((mouse.x-canvas.x)/canvas.w)*resolution.x)-((float)(int)(cursize-1)/2)), (float)(int)((((mouse.y-canvas.y)/canvas.h)*resolution.y)-((float)(int)(cursize-1)/2)), (float)(int)cursize, (float)(int)cursize };
            SDL_RenderFillRect(renderer, &cursorturect);
            SDL_SetRenderTarget(renderer, NULL);
            SDL_RenderTexture(renderer, cursorture, NULL, &canvas);
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        }


        /* Render margins */
        SDL_RenderGeometry(renderer, NULL, canvasborders, 8, canvasborderindicies, 24);
        SDL_RenderGeometry(renderer, NULL, visualborders, 12, visualborderindicies, 24);


        /* Render borders */
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderFillRect(renderer, &nameborder);


        /* Render UI */
        SDL_SetRenderDrawColor(renderer, 68, 68, 68, 255);
        if (contained(mouse, toolsrect) && toolnames[((int)((mouse.x-toolsrect.x)/(toolsrect.w/3)))+((int)((mouse.y-toolsrect.y)/(toolsrect.w/3))*3)] != "") {
            toolselectedrect = (SDL_FRect){(float)((int)((mouse.x-toolsrect.x)/(toolsrect.w/3))*(toolsrect.w/3))+toolsrect.x, (float)((int)((mouse.y-toolsrect.y)/(toolsrect.w/3))*(toolsrect.w/3))+toolsrect.y, toolselectedrect.w, toolselectedrect.h };
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
        cursizerect = (SDL_FRect){(float)limit(-((canvas.w/resolution.x)*(int)cursize)/2+cursizerectborder.x+(cursizerectborder.w/2), cursizerectinborder.x), (float)limit(cursizerectinborder.y-(cursizerectinborder.w/2)+((canvas.w/resolution.y)/2*(int)cursize), NULL, cursizerectinborder.y), (float)limit(canvas.w/resolution.x*(int)cursize, NULL, cursizerectinborder.w), -(float)limit(canvas.h/resolution.y*(int)cursize, NULL, -cursizerectinborder.h) };
        SDL_RenderFillRect(renderer, &cursizerect);
        selectedcolorect = (SDL_FRect){8, toolsrect.y+toolsrect.h+8, selectedcolorect.w, selectedcolorect.h };
        (contained(mouse, selectedcolorect))?SDL_SetRenderDrawColor(renderer, 136, 136, 136, 255):SDL_SetRenderDrawColor(renderer, 68, 68, 68, 255);
        SDL_RenderFillRect(renderer, &selectedcolorect);
        selectedcolorect = (SDL_FRect){toolsrect.w-selectedcolorect.w+8, toolsrect.y+toolsrect.h+8, selectedcolorect.w, selectedcolorect.h };
        (contained(mouse, selectedcolorect))?SDL_SetRenderDrawColor(renderer, 136, 136, 136, 255):SDL_SetRenderDrawColor(renderer, 68, 68, 68, 255);
        SDL_RenderFillRect(renderer, &selectedcolorect);
        SDL_SetRenderDrawColor(renderer, leftcolor.r, leftcolor.g, leftcolor.b, leftcolor.a);
        selectedcolorealrect = (SDL_FRect){8+((selectedcolorect.w-selectedcolorealrect.w)/2), toolsrect.y+toolsrect.h+8+((selectedcolorect.h-selectedcolorealrect.h)/2), selectedcolorealrect.w, selectedcolorealrect.h };
        SDL_RenderTexture(renderer, colorselector, NULL, &selectedcolorealrect);
        SDL_RenderGeometry(renderer, NULL, leftcoloralphapreview, 3, NULL, 0);
        SDL_RenderGeometry(renderer, NULL, leftcolorpreview, 3, NULL, 0);
        SDL_SetRenderDrawColor(renderer, rightcolor.r, rightcolor.g, rightcolor.b, rightcolor.a);
        selectedcolorealrect = (SDL_FRect){toolsrect.w-selectedcolorect.w+8+((selectedcolorect.w-selectedcolorealrect.w)/2), toolsrect.y+toolsrect.h+8+((selectedcolorect.h-selectedcolorealrect.h)/2), selectedcolorealrect.w, selectedcolorealrect.h };
        SDL_RenderTexture(renderer, colorselector, NULL, &selectedcolorealrect);
        SDL_RenderGeometry(renderer, NULL, rightcoloralphapreview, 3, NULL, 0);
        SDL_RenderGeometry(renderer, NULL, rightcolorpreview, 3, NULL, 0);


        /* Render UI text */
        SDL_RenderTexture(renderer, cursizetextrecture, NULL, &cursizetextrect);


        /* Render title text */
        SDL_RenderTexture(renderer, title, NULL, &tirect);


        /* Push render content */
        SDL_RenderPresent(renderer);


        /* Update input variables */
        framelastmouse = mouse;
        oldshift = keystates[SDL_SCANCODE_LSHIFT];
        oldmousedown = (mousebitmask & SDL_BUTTON_LMASK || mousebitmask & SDL_BUTTON_RMASK);


        /* Wait if unfocussed */
        if (!focus) SDL_Delay((Uint32)250);
        SDL_Delay((Uint32)5);
    }


    /* Exit properly */
    SDL_DestroyTexture(title);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0; }

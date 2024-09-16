#include <cmath>
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
std::vector<SDL_Texture *> sprite;
SDL_Texture * cursorture;
SDL_FRect cursorturect = { 0, 0, 1, 1 };
SDL_FRect spriterect;
int frame = 0;


/* Canvas borders */
SDL_FRect upbar = { margin.a, 44, precanvas.w, (precanvas.h-canvas.h)/2 };
SDL_FRect downbar = { margin.a, (float)windowsize.y-8, precanvas.w, -(precanvas.h-canvas.h)/2 };
SDL_FRect leftbar = { margin.a, 44, (precanvas.w-canvas.w)/2, precanvas.h };
SDL_FRect rightbar = { margin.a, 44, -(precanvas.w-canvas.w)/2, precanvas.h };


/* Canvas drawing variables */
void*pixels;
int pitch;
SDL_Color leftcolor = (SDL_Color){ .r=0, .g=0, .b=0, .a=255 };
SDL_Color rightcolor = (SDL_Color){ .r=0, .g=0, .b=0, .a=0 };


/* UI elements */
float toolsuiwidth = 96;
SDL_FRect toolsrect = { 8, 0, 0, 0 };
int currentool = 0;
SDL_FRect toolshoveredrect = { 0, 0, 48, 48 };
SDL_FRect toolselectedrect = { 0, 0, 48, 48 };
std::string toolnames[18] = {"Pen","Line","Eraser","Mirror","Dither","Lighten","Fill","Multi-Fill","","Rectangle","Circle","","Rectangle Select","Lasso Select","Magic Select","Grab","Gridlock","Pick Color"};
SDL_FRect cursizerectinborder;
SDL_FRect cursizerectborder;
SDL_FRect cursizerect;
float cursize = 1;
SDL_FRect cursizetextrect;
SDL_FRect selectedcolorect = { 0, 0, 36, 36 };
SDL_FRect selectedcolorealrect = { 0, 0, selectedcolorect.w-8, selectedcolorect.h-8 };
SDL_Vertex leftcoloralphapreview[] = {
    {{selectedcolorealrect.x,selectedcolorealrect.y}, {(float)leftcolor.r/255,(float)leftcolor.g/255,(float)leftcolor.b/255,(float)leftcolor.a/255}, {0,0}},
    {{selectedcolorealrect.x+selectedcolorealrect.w,selectedcolorealrect.y}, {(float)leftcolor.r/255,(float)leftcolor.g/255,(float)leftcolor.b/255,(float)leftcolor.a/255}, {0,0}},
    {{selectedcolorealrect.x,selectedcolorealrect.y+selectedcolorealrect.y}, {(float)leftcolor.r/255,(float)leftcolor.g/255,(float)leftcolor.b/255,(float)leftcolor.a/255}, {0,0}}
};
SDL_Vertex leftcolorpreview[] = {
    {{selectedcolorealrect.x+selectedcolorealrect.w,selectedcolorealrect.y}, {(float)leftcolor.r/255,(float)leftcolor.g/255,(float)leftcolor.b/255,1}, {0,0}},
    {{selectedcolorealrect.x+selectedcolorealrect.w,selectedcolorealrect.y+selectedcolorealrect.h}, {(float)leftcolor.r/255,(float)leftcolor.g/255,(float)leftcolor.b/255,1}, {0,0}},
    {{selectedcolorealrect.x,selectedcolorealrect.y+selectedcolorealrect.y}, {(float)leftcolor.r/255,(float)leftcolor.g/255,(float)leftcolor.b/255,1}, {0,0}}
};
SDL_Vertex rightcoloralphapreview[] = {
    {{selectedcolorealrect.x,selectedcolorealrect.y}, {(float)leftcolor.r/255,(float)rightcolor.g/255,(float)rightcolor.b/255,(float)rightcolor.a/255}, {0,0}},
    {{selectedcolorealrect.x+selectedcolorealrect.w,selectedcolorealrect.y}, {(float)rightcolor.r/255,(float)rightcolor.g/255,(float)rightcolor.b/255,(float)rightcolor.a/255}, {0,0}},
    {{selectedcolorealrect.x,selectedcolorealrect.y+selectedcolorealrect.y}, {(float)rightcolor.r/255,(float)rightcolor.g/255,(float)rightcolor.b/255,(float)rightcolor.a/255}, {0,0}}
};
SDL_Vertex rightcolorpreview[] = {
    {{selectedcolorealrect.x+selectedcolorealrect.w,selectedcolorealrect.y}, {(float)rightcolor.r/255,(float)rightcolor.g/255,(float)rightcolor.b/255,1}, {0,0}},
    {{selectedcolorealrect.x+selectedcolorealrect.w,selectedcolorealrect.y+selectedcolorealrect.h}, {(float)rightcolor.r/255,(float)rightcolor.g/255,(float)rightcolor.b/255,1}, {0,0}},
    {{selectedcolorealrect.x,selectedcolorealrect.y+selectedcolorealrect.y}, {(float)rightcolor.r/255,(float)rightcolor.g/255,(float)rightcolor.b/255,1}, {0,0}}
};
SDL_Vertex visualborders[] = {
    {{0,36}, {(float)29/255,(float)29/255,(float)29/255,1}, {0,0}},
    {{0,(float)windowsize.y}, {(float)29/255,(float)29/255,(float)29/255,1}, {0,0}},
    {{margin.a,36}, {(float)29/255,(float)29/255,(float)29/255,1}, {0,0}},
    {{margin.a,44}, {(float)29/255,(float)29/255,(float)29/255,1}, {0,0}},
    {{margin.a,(float)windowsize.y-8}, {(float)29/255,(float)29/255,(float)29/255,1}, {0,0}},
    {{margin.a,(float)windowsize.y}, {(float)29/255,(float)29/255,(float)29/255,1}, {0,0}},
    {{windowsize.x-margin.b,36}, {(float)29/255,(float)29/255,(float)29/255,1}, {0,0}},
    {{windowsize.x-margin.b,44}, {(float)29/255,(float)29/255,(float)29/255,1}, {0,0}},
    {{windowsize.x-margin.b,(float)windowsize.y-8}, {(float)29/255,(float)29/255,(float)29/255,1}, {0,0}},
    {{windowsize.x-margin.b,(float)windowsize.y}, {(float)29/255,(float)29/255,(float)29/255,1}, {0,0}},
    {{(float)windowsize.x,36}, {(float)29/255,(float)29/255,(float)29/255,1}, {0,0}},
    {{(float)windowsize.x,(float)windowsize.y}, {(float)29/255,(float)29/255,(float)29/255,1}, {0,0}}
};
int visualborderindicies[] = { 0, 1, 2, 2, 1, 5, 2, 3, 7, 7, 6, 2, 4, 5, 9, 9, 8, 4, 6, 9, 10, 10, 9, 11 };
// SDL_Vertex canvasborders[] = {
//     {{0,36}, {(float)160/255,(float)160/255,(float)160/255,1}, {0,0}},
// };


/* Limit function */
double limit(double value, std::optional<double> min = NULL, std::optional<double> max = NULL) { return ((min!=NULL)?((max!=NULL)?(value>max?max:(value<min?min:value)):(value<min?min:value)):(max!=NULL)?(value>max?max:value):value).value(); }


/* Contained function */
bool contained(fvec2 point, SDL_FRect container) { return ((container.w>0)?point.x>container.x:point.x<container.x) && ((container.h>0)?point.y>container.y:point.y<container.y) && ((container.w>0)?point.x<container.x+container.w:point.x>container.x+container.w) && ((container.h>0)?point.y<container.y+container.h:point.y>container.y+container.h); }


/* Max/Min functions */
double max(double a, double b) { return (a>b)?a:b; }
double min(double a, double b) { return (a<b)?a:b; }


/* Main! */
int main() {


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
    toolsrect = (SDL_FRect){ .x=toolsrect.x, .y=((float)windowsize.y/2)-(toolsrect.h/(toolsrect.h/toolsuiwidth)), .w=toolsuiwidth, .h=toolsuiwidth*(toolsrect.h/toolsrect.w) };
    toolshoveredrect = (SDL_FRect){ .x=0,.y=0,toolsrect.w/3,toolsrect.w/3 };
    toolselectedrect = (SDL_FRect){ .x=0,.y=0,toolsrect.w/3,toolsrect.w/3 };
    cursizerectborder = (SDL_FRect){ .x=toolsrect.x, .y=toolsrect.y, .w=toolsrect.w, .h=-toolsrect.w };
    cursizerectinborder = (SDL_FRect){ .x=toolsrect.x+4, .y=toolsrect.y-4, .w=toolsrect.w-8, .h=-toolsrect.w+8 };
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
    SDL_SetRenderTarget(renderer, NULL);
    SDL_SetTextureScaleMode(presprite, SDL_SCALEMODE_NEAREST);
    SDL_SetTextureScaleMode(sprite[0], SDL_SCALEMODE_NEAREST);
    SDL_SetTextureScaleMode(cursorture, SDL_SCALEMODE_NEAREST);
    SDL_GetTextureSize(presprite, &spriterect.w, &spriterect.h);
    spriterect = (SDL_FRect){ .x=0, .y=0, .w=spriterect.w, .h=spriterect.h };
    SDL_Surface * tempcursizetextrect = TTF_RenderText_Blended(font, "1x", (SDL_Color){ .r=255, .g=255, .b=255, .a=255 });
    SDL_Texture * cursizetextrecture = SDL_CreateTextureFromSurface(renderer, tempcursizetextrect);
    SDL_DestroySurface(tempcursizetextrect);
    SDL_GetTextureSize(cursizetextrecture, &cursizetextrect.w, &cursizetextrect.h);
    cursizetextrect = (SDL_FRect){ .x=cursizerectborder.x+((cursizerectborder.w-cursizetextrect.w)/2), .y=cursizerectborder.y+cursizerectborder.h-cursizetextrect.h, .w=cursizetextrect.w, .h=cursizetextrect.h };


    /* Setup variables that require data from textures */
    selectedcolorealrect = (SDL_FRect){ .x=8+((selectedcolorect.w-selectedcolorealrect.w)/2), .y=toolsrect.y+toolsrect.h+8+((selectedcolorect.h-selectedcolorealrect.h)/2), .w=selectedcolorealrect.w, .h=selectedcolorealrect.h };
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
    selectedcolorealrect = (SDL_FRect){ .x=toolsrect.w-selectedcolorect.w+8+((selectedcolorect.w-selectedcolorealrect.w)/2), .y=toolsrect.y+toolsrect.h+8+((selectedcolorect.h-selectedcolorealrect.h)/2), .w=selectedcolorealrect.w, .h=selectedcolorealrect.h };
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
                    precanvas = (SDL_FRect){ .x=margin.a, .y=44, .w=windowsize.x-margin.a-margin.b, .h=(float)windowsize.y-52 };
                    canvascenter = (fvec2){ .x=(margin.a/2)+((windowsize.x-margin.b)/2), .y=22+(((float)windowsize.y-8)/2) };
                    canvasize = (fvec2){ .x=resratio.x*((precanvas.w>precanvas.h)?precanvas.h:precanvas.w), .y=resratio.y*((precanvas.w>precanvas.h)?precanvas.h:precanvas.w) };
                    canvas = (SDL_FRect){ .x=canvas.x, .y=canvas.y, .w=canvas.w*(canvasize.x/oldcanvasize.x), .h=canvas.h*(canvasize.y/oldcanvasize.y) };
                    if (canvas.w<=precanvas.w) canvas.x=(precanvas.x+((precanvas.w-canvas.w)/2));
                    if (canvas.h<=precanvas.h) canvas.y=(precanvas.y+((precanvas.h-canvas.h)/2));

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


                    /* Reset color preview position */
                    selectedcolorealrect = (SDL_FRect){ .x=8+((selectedcolorect.w-selectedcolorealrect.w)/2), .y=toolsrect.y+toolsrect.h+8+((selectedcolorect.h-selectedcolorealrect.h)/2), .w=selectedcolorealrect.w, .h=selectedcolorealrect.h };
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
                    selectedcolorealrect = (SDL_FRect){ .x=toolsrect.w-selectedcolorect.w+8+((selectedcolorect.w-selectedcolorealrect.w)/2), .y=toolsrect.y+toolsrect.h+8+((selectedcolorect.h-selectedcolorealrect.h)/2), .w=selectedcolorealrect.w, .h=selectedcolorealrect.h };
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
                    visualborders[8].position.x = windowsize.x-margin.b;
                    visualborders[8].position.y = (float)windowsize.y-8;
                    visualborders[9].position.x = windowsize.x-margin.b;
                    visualborders[9].position.y = (float)windowsize.y;
                    visualborders[10].position.x = (float)windowsize.x;
                    visualborders[11].position.x = (float)windowsize.x;
                    visualborders[11].position.y = (float)windowsize.y;


                /* Zoom canvas */
                case SDL_EVENT_MOUSE_WHEEL:


                    /* Reset scroll */
                    scroll.x = e.wheel.x;
                    scroll.y = e.wheel.y;


                    /* Reset canvas */
                    if (keystates[SDL_SCANCODE_LCTRL]){
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
                    else{
                        if ((int)cursize!=(int)limit(cursize+(scroll.y/10),1,max(resolution.x,resolution.y))){
                            SDL_DestroyTexture(cursizetextrecture);
                            char tempchar[256];
                            snprintf(tempchar, sizeof(tempchar), "%d%s", (int)limit(cursize+(scroll.y/10),1,max(resolution.x,resolution.y)), "x");
                            tempcursizetextrect = TTF_RenderText_Blended(font, tempchar, (SDL_Color){ .r=255, .g=255, .b=255, .a=255 });
                            cursizetextrecture = SDL_CreateTextureFromSurface(renderer, tempcursizetextrect);
                            SDL_DestroySurface(tempcursizetextrect);
                            SDL_GetTextureSize(cursizetextrecture, &cursizetextrect.w, &cursizetextrect.h);
                            SDL_DestroySurface(tempcursizetextrect);
                            remove(tempchar);
                        }
                        cursize=limit(cursize+(scroll.y/10),1,max(resolution.x,resolution.y));


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


                /* Interact with UI */
                case SDL_EVENT_MOUSE_BUTTON_DOWN:
                    if (contained(mouse,toolsrect) && !(toolnames[((int)((mouse.x-toolsrect.x)/(toolsrect.w/3)))+((int)((mouse.y-toolsrect.y)/(toolsrect.w/3))*3)]=="")) currentool=((int)((mouse.x-toolsrect.x)/(toolsrect.w/3)))+((int)((mouse.y-toolsrect.y)/(toolsrect.w/3))*3);
                    lastmouse = mouse;
                    break;


                /* Update canvas */
                case SDL_EVENT_MOUSE_BUTTON_UP:
                    if ((mousebitmask & SDL_BUTTON_LMASK || mousebitmask & SDL_BUTTON_RMASK) && currentool == 1 && contained(lastmouse, canvas)){
                        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
                        SDL_SetRenderTarget(renderer, sprite[frame]);
                        if (mousebitmask & SDL_BUTTON_LMASK) SDL_SetRenderDrawColor(renderer, leftcolor.r, leftcolor.g, leftcolor.b, leftcolor.a);
                        else if (mousebitmask & SDL_BUTTON_RMASK) SDL_SetRenderDrawColor(renderer, rightcolor.r, rightcolor.g, rightcolor.b, rightcolor.a);
                        fvec2 roundlastmouse = (fvec2){ .x=(float)(int)(((lastmouse.x-((canvas.w/resolution.x)*((int)cursize-1)/2))-canvas.x)/(canvas.w/resolution.x)), .y=(float)(int)(((lastmouse.y-((canvas.h/resolution.y)*((int)cursize-1)/2))-canvas.y)/(canvas.h/resolution.y)) };
                        fvec2 roundmouse = (fvec2){ .x=(float)(int)(((mouse.x-((canvas.w/resolution.x)*((int)cursize-1)/2))-canvas.x)/(canvas.w/resolution.x)), .y=(float)(int)(((mouse.y-((canvas.h/resolution.y)*((int)cursize-1)/2))-canvas.y)/(canvas.h/resolution.y)) };
                        fvec2 fakeroundmouse = roundmouse;
                        if (keystates[SDL_SCANCODE_LSHIFT]) fakeroundmouse = (fvec2){ (float)(sin(round(atan2(roundmouse.x-roundlastmouse.x, roundmouse.y-roundlastmouse.y)/M_EIGHTHPI)*M_EIGHTHPI)*sqrt(((roundmouse.x-roundlastmouse.x)*(roundmouse.x-roundlastmouse.x))+((roundmouse.y-roundlastmouse.y)*(roundmouse.y-roundlastmouse.y))))+roundlastmouse.x, (float)(cos(round(atan2(roundmouse.x-roundlastmouse.x, roundmouse.y-roundlastmouse.y)/M_EIGHTHPI)*M_EIGHTHPI)*sqrt(((roundmouse.x-roundlastmouse.x)*(roundmouse.x-roundlastmouse.x))+((roundmouse.y-roundlastmouse.y)*(roundmouse.y-roundlastmouse.y))))+roundlastmouse.y };
                        for (int y = 0; y < (int)cursize; y++) {
                            for (int x = 0; x < (int)cursize; x++) {
                                SDL_RenderLine(renderer, roundlastmouse.x+x, roundlastmouse.y+y, fakeroundmouse.x+x, fakeroundmouse.y+y );
                            }
                        }
                        SDL_SetRenderTarget(renderer, NULL);
                        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
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
                    fvec2 roundlastmouse = (fvec2){ .x=(float)(int)(((lastmouse.x-((canvas.w/resolution.x)*((int)cursize-1)/2))-canvas.x)/(canvas.w/resolution.x)), .y=(float)(int)(((lastmouse.y-((canvas.h/resolution.y)*((int)cursize-1)/2))-canvas.y)/(canvas.h/resolution.y)) };
                    fvec2 roundmouse = (fvec2){ .x=(float)(int)(((mouse.x-((canvas.w/resolution.x)*((int)cursize-1)/2))-canvas.x)/(canvas.w/resolution.x)), .y=(float)(int)(((mouse.y-((canvas.h/resolution.y)*((int)cursize-1)/2))-canvas.y)/(canvas.h/resolution.y)) };
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


                /* Draw line from framelastmouse to mouse, mirrored if you have to */
                if (currentool == 3) {
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


            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
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
        if (contained(mouse, canvas) && !(mousebitmask & SDL_BUTTON_LMASK || mousebitmask & SDL_BUTTON_RMASK) && focus){
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
            SDL_SetRenderTarget(renderer, cursorture);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
            SDL_RenderClear(renderer);
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 51);
            cursorturect = (SDL_FRect){ .x=(float)(int)((((mouse.x-canvas.x)/canvas.w)*resolution.x)-((float)(int)(cursize-1)/2)), .y=(float)(int)((((mouse.y-canvas.y)/canvas.h)*resolution.y)-((float)(int)(cursize-1)/2)), .w=(float)(int)cursize, .h=(float)(int)cursize };
            SDL_RenderFillRect(renderer, &cursorturect);
            SDL_SetRenderTarget(renderer, NULL);
            SDL_RenderTexture(renderer, cursorture, NULL, &canvas);
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        }
        SDL_SetRenderDrawColor(renderer, 160, 160, 160, 255);
        SDL_RenderFillRect(renderer, &upbar);
        SDL_RenderFillRect(renderer, &downbar);
        SDL_RenderFillRect(renderer, &leftbar);
        SDL_RenderFillRect(renderer, &rightbar);


        /* Render margins */
        SDL_RenderGeometry(renderer, NULL, visualborders, 12, visualborderindicies, 24);


        /* Render borders */
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderFillRect(renderer, &nameborder);


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
        selectedcolorect = (SDL_FRect){ .x=8, .y=toolsrect.y+toolsrect.h+8, .w=selectedcolorect.w, .h=selectedcolorect.h };
        (contained(mouse,selectedcolorect))?SDL_SetRenderDrawColor(renderer, 136, 136, 136, 255):SDL_SetRenderDrawColor(renderer, 68, 68, 68, 255);
        SDL_RenderFillRect(renderer, &selectedcolorect);
        selectedcolorect = (SDL_FRect){ .x=toolsrect.w-selectedcolorect.w+8, .y=toolsrect.y+toolsrect.h+8, .w=selectedcolorect.w, .h=selectedcolorect.h };
        (contained(mouse,selectedcolorect))?SDL_SetRenderDrawColor(renderer, 136, 136, 136, 255):SDL_SetRenderDrawColor(renderer, 68, 68, 68, 255);
        SDL_RenderFillRect(renderer, &selectedcolorect);
        SDL_SetRenderDrawColor(renderer, leftcolor.r, leftcolor.g, leftcolor.b, leftcolor.a);
        selectedcolorealrect = (SDL_FRect){ .x=8+((selectedcolorect.w-selectedcolorealrect.w)/2), .y=toolsrect.y+toolsrect.h+8+((selectedcolorect.h-selectedcolorealrect.h)/2), .w=selectedcolorealrect.w, .h=selectedcolorealrect.h };
        SDL_RenderTexture(renderer, colorselector, NULL, &selectedcolorealrect);
        SDL_RenderGeometry(renderer, NULL, leftcoloralphapreview, 3, NULL, 0);
        SDL_RenderGeometry(renderer, NULL, leftcolorpreview, 3, NULL, 0);
        SDL_SetRenderDrawColor(renderer, rightcolor.r, rightcolor.g, rightcolor.b, rightcolor.a);
        selectedcolorealrect = (SDL_FRect){ .x=toolsrect.w-selectedcolorect.w+8+((selectedcolorect.w-selectedcolorealrect.w)/2), .y=toolsrect.y+toolsrect.h+8+((selectedcolorect.h-selectedcolorealrect.h)/2), .w=selectedcolorealrect.w, .h=selectedcolorealrect.h };
        SDL_RenderTexture(renderer, colorselector, NULL, &selectedcolorealrect);
        SDL_RenderGeometry(renderer, NULL, rightcoloralphapreview, 3, NULL, 0);
        SDL_RenderGeometry(renderer, NULL, rightcolorpreview, 3, NULL, 0);


        /* Render UI text */
        SDL_RenderTexture(renderer, cursizetextrecture, NULL, &cursizetextrect);


        /* Render title text */
        SDL_RenderTexture(renderer, title, NULL, &tirect);


        /* Push render content */
        SDL_RenderPresent(renderer);


        /* Update mouse variables */
        framelastmouse = mouse;


        /* Wait if unfocussed */
        if (!focus) SDL_Delay((Uint32)250);
        SDL_Delay((Uint32)2);
    }


    /* Exit properly */
    SDL_DestroyTexture(title);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0; }

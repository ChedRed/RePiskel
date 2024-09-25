#include "SDL3/SDL_blendmode.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_video.h"
#include <cstdlib>
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
struct fvec3 { float x; float y; float z; };
struct fvec4 { float w; float x; float y; float z; };
struct duo { float a; float b; };


/* Simple math things */
#define M_EIGHTHPI (((2*M_PI)/360)*22.5)


/* Display setup variables */
deltadata dtdata;
vec2 windowsize = { 1000, 650 };
SDL_Color gridmain = { .r=85, .g=85, .b=85, .a=255 };
SDL_Color gridalt = { .r=76, .g=76, .b=76, .a=255 };
SDL_FRect grid = { 0, 0, 8, 8 };


/* Input setup variables */
fvec2 mouse = { 0, 0 };
fvec2 scroll = { 0, 0 };
bool mousedowned = false;
fvec2 lastmouse = { 0, 0 };
fvec2 framelastmouse = { 0, 0 };
Uint32 mousebitmask;
const bool * keystates = SDL_GetKeyboardState(NULL);
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
SDL_Surface * prespritesurface;
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
SDL_BlendMode straightbrighten = SDL_ComposeCustomBlendMode(SDL_BLENDFACTOR_SRC_ALPHA, SDL_BLENDFACTOR_ONE, SDL_BLENDOPERATION_ADD, SDL_BLENDFACTOR_ZERO, SDL_BLENDFACTOR_ONE, SDL_BLENDOPERATION_REV_SUBTRACT);
SDL_BlendMode straightdarken = SDL_ComposeCustomBlendMode(SDL_BLENDFACTOR_SRC_ALPHA, SDL_BLENDFACTOR_ONE, SDL_BLENDOPERATION_REV_SUBTRACT, SDL_BLENDFACTOR_ZERO, SDL_BLENDFACTOR_ONE, SDL_BLENDOPERATION_REV_SUBTRACT);
SDL_BlendMode reverstraightbrighten = SDL_ComposeCustomBlendMode(SDL_BLENDFACTOR_ONE, SDL_BLENDFACTOR_SRC_ALPHA, SDL_BLENDOPERATION_ADD, SDL_BLENDFACTOR_ONE, SDL_BLENDFACTOR_ZERO, SDL_BLENDOPERATION_REV_SUBTRACT);
SDL_BlendMode reverstraightdarken = SDL_ComposeCustomBlendMode(SDL_BLENDFACTOR_ONE, SDL_BLENDFACTOR_SRC_ALPHA, SDL_BLENDOPERATION_REV_SUBTRACT, SDL_BLENDFACTOR_ONE, SDL_BLENDFACTOR_ZERO, SDL_BLENDOPERATION_REV_SUBTRACT);


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
SDL_FRect leftselectedcolorect = { 0, 0, 36, 36 };
SDL_FRect rightselectedcolorect = { 0, 0, 36, 36 };
SDL_FRect leftselectedcolorealrect = { 0, 0, leftselectedcolorect.w-8, leftselectedcolorect.h-8 };
SDL_FRect rightselectedcolorealrect = { 0, 0, rightselectedcolorect.w-8, rightselectedcolorect.h-8 };
SDL_FRect colorselectorui = { 8, 0, margin.a-16, 114 };
SDL_FRect colorselectelements[] = {
    { 0, 0, 20, 90 },
    { 0, 0, 90, 90 },
    { 0, 0, 20, 90 }
};
SDL_Vertex leftcoloralphapreview[] = {
    {{leftselectedcolorealrect.x, leftselectedcolorealrect.y}, {(float)leftcolor.r/255, (float)leftcolor.g/255, (float)leftcolor.b/255, (float)leftcolor.a/255}, {0, 0}},
    {{leftselectedcolorealrect.x+leftselectedcolorealrect.w, leftselectedcolorealrect.y}, {(float)leftcolor.r/255, (float)leftcolor.g/255, (float)leftcolor.b/255, (float)leftcolor.a/255}, {0, 0}},
    {{leftselectedcolorealrect.x, leftselectedcolorealrect.y+leftselectedcolorealrect.y}, {(float)leftcolor.r/255, (float)leftcolor.g/255, (float)leftcolor.b/255, (float)leftcolor.a/255}, {0, 0}}
};
SDL_Vertex leftcolorpreview[] = {
    {{leftselectedcolorealrect.x+leftselectedcolorealrect.w, leftselectedcolorealrect.y}, {(float)leftcolor.r/255, (float)leftcolor.g/255, (float)leftcolor.b/255, 1}, {0, 0}},
    {{leftselectedcolorealrect.x+leftselectedcolorealrect.w, leftselectedcolorealrect.y+leftselectedcolorealrect.h}, {(float)leftcolor.r/255, (float)leftcolor.g/255, (float)leftcolor.b/255, 1}, {0, 0}},
    {{leftselectedcolorealrect.x, leftselectedcolorealrect.y+leftselectedcolorealrect.y}, {(float)leftcolor.r/255, (float)leftcolor.g/255, (float)leftcolor.b/255, 1}, {0, 0}}
};
SDL_Vertex rightcoloralphapreview[] = {
    {{rightselectedcolorealrect.x, rightselectedcolorealrect.y}, {(float)leftcolor.r/255, (float)rightcolor.g/255, (float)rightcolor.b/255, (float)rightcolor.a/255}, {0, 0}},
    {{rightselectedcolorealrect.x+rightselectedcolorealrect.w, rightselectedcolorealrect.y}, {(float)rightcolor.r/255, (float)rightcolor.g/255, (float)rightcolor.b/255, (float)rightcolor.a/255}, {0, 0}},
    {{rightselectedcolorealrect.x, rightselectedcolorealrect.y+rightselectedcolorealrect.y}, {(float)rightcolor.r/255, (float)rightcolor.g/255, (float)rightcolor.b/255, (float)rightcolor.a/255}, {0, 0}}
};
SDL_Vertex rightcolorpreview[] = {
    {{rightselectedcolorealrect.x+rightselectedcolorealrect.w, rightselectedcolorealrect.y}, {(float)rightcolor.r/255, (float)rightcolor.g/255, (float)rightcolor.b/255, 1}, {0, 0}},
    {{rightselectedcolorealrect.x+rightselectedcolorealrect.w, rightselectedcolorealrect.y+leftselectedcolorealrect.h}, {(float)rightcolor.r/255, (float)rightcolor.g/255, (float)rightcolor.b/255, 1}, {0, 0}},
    {{rightselectedcolorealrect.x, rightselectedcolorealrect.y+rightselectedcolorealrect.y}, {(float)rightcolor.r/255, (float)rightcolor.g/255, (float)rightcolor.b/255, 1}, {0, 0}}
};
fvec4 HSVA = { 0, 1, 1, 1 };
bool colorselectorvisible = false;
bool leftcolorchanging = false;


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


/* Interpolation function */
double lerp(double a, double b, double c) { return a+((b-a)*c); }


/* Limit function */
double limit(double value, std::optional<double> min = std::nullopt, std::optional<double> max = std::nullopt) { return ((min.has_value())?((max.has_value())?(value>max?max:(value<min?min:value)):(value<min?min:value)):((max.has_value())?(value>max?max:value):value)).value(); }


/* Within limit function */
bool inlimit(double value, std::optional<double> min = std::nullopt, std::optional<double> max = std::nullopt) { return (min.has_value())?((max.has_value())?(value<max&&value>=min):(value>=min)):((max.has_value())?(value<max):true); }


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

        if (2*err>-distance.y) {
            err -= distance.y;
            start.x += mirror.x;
        }
        if (2*err<distance.x) {
            err += distance.x;
            start.y += mirror.y;
        }

        if (std::abs(start.x-end.x)<=1 && std::abs(start.y-end.y)<=1) break;
    }
}


/* Tertiary line function for lighten */
void lightenline(SDL_Renderer * renderer, vec2 start, vec2 end, bool darken, bool add) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 16);
    if (add) ((darken)?SDL_SetRenderDrawBlendMode(renderer, straightdarken):SDL_SetRenderDrawBlendMode(renderer, straightbrighten));
    else SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    vec2 distance = { abs(end.x-start.x), abs(end.y-start.y) };
    vec2 mirror = { (start.x<end.x)?1:-1, (start.y<end.y) ?1:-1 };
    int err = distance.x-distance.y;

    while (true) {
        SDL_RenderPoint(renderer, start.x, start.y);

        if (std::abs(start.x-end.x)<=1 && std::abs(start.y-end.y)<=1) break;

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


SDL_Color RGBfHSV(double h, double s, double v) {
    fvec3 recolor = { 0, 0, 0 };
    float c = v*s;
    float x = c*(1-std::abs(std::fmod(h/60, 2)-1));
    float m = v-c;
    if (inlimit(h,0,60)) { recolor.x = c; recolor.y = x; recolor.z = 0; }
    else if (inlimit(h,60,120)) { recolor.x = x; recolor.y = c; recolor.z = 0; }
    else if (inlimit(h,120,180)) { recolor.x = 0; recolor.y = c; recolor.z = x; }
    else if (inlimit(h,180,240)) { recolor.x = 0; recolor.y = x; recolor.z = c; }
    else if (inlimit(h,240,300)) { recolor.x = x; recolor.y = 0; recolor.z = c; }
    else { recolor.x = c; recolor.y = 0; recolor.z = x; }
    return (SDL_Color){ (Uint8)((recolor.x+m)*255), (Uint8)((recolor.y+m)*255), (Uint8)((recolor.z+m)*255), 255 };
}


fvec3 HSVfRGB(int r, int g, int b) {
    fvec3 rehsv = (fvec3){ (float)r/255, (float)g/255, (float)b/255 };
    fvec3 realhsv;
    float cmax = std::max(rehsv.x, std::max(rehsv.y, rehsv.z));
    float cmin = std::min(rehsv.x, std::min(rehsv.y, rehsv.z));
    float delta = cmax-cmin;
    realhsv.x = ((cmax==cmin)?0:fmod(((cmax==rehsv.x)?(60*((rehsv.y-rehsv.z)/delta)+360):((cmax==rehsv.y)?(60*((rehsv.z-rehsv.x)/delta)+120):(60*((rehsv.x-rehsv.y)/delta)+240))), 360));
    realhsv.y = (cmax==0)?0:(delta/cmax);
    realhsv.z = cmax;
    return realhsv;
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
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    snprintf(tempath, sizeof(tempath), "%s%s", SDL_GetBasePath(), "../Resources/tools.bmp");
    SDL_Surface * pretools = SDL_LoadBMP(tempath);
    SDL_Texture * tools = SDL_CreateTextureFromSurface(renderer, pretools);
    SDL_GetTextureSize(tools, &toolsrect.w, &toolsrect.h);
    toolsrect = (SDL_FRect){toolsrect.x, ((float)windowsize.y/2)-(toolsrect.h/(toolsrect.h/toolsuiwidth)), toolsuiwidth, toolsuiwidth*(toolsrect.h/toolsrect.w) };
    toolshoveredrect = (SDL_FRect){0, 0, toolsrect.w/3, toolsrect.w/3 };
    toolselectedrect = (SDL_FRect){0, 0, toolsrect.w/3, toolsrect.w/3 };
    cursizerectborder = (SDL_FRect){toolsrect.x, toolsrect.y, toolsrect.w, -toolsrect.w };
    cursizerectinborder = (SDL_FRect){toolsrect.x+4, toolsrect.y-4, toolsrect.w-8, -toolsrect.w+8 };
    SDL_Texture * toolsborder = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, toolsrect.w/3, toolsrect.w/3);
    SDL_FRect temprect = { 2, 2, (toolsrect.w/3)-4, (toolsrect.w/3)-4 };
    SDL_SetRenderTarget(renderer, toolsborder);
    SDL_SetRenderDrawColor(renderer, 255, 215, 0, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderFillRect(renderer, &temprect);
    SDL_Texture * colorselector = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, colorselectorui.w, colorselectorui.h);
    SDL_FRect colorselectemprect = { 4, 4, colorselectorui.w-8, colorselectorui.h-8 };
    SDL_SetRenderTarget(renderer, colorselector);
    SDL_SetRenderDrawColor(renderer, 136, 136, 136, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 43, 43, 43, 255);
    SDL_RenderFillRect(renderer, &colorselectemprect);
    SDL_SetRenderDrawColor(renderer, 32, 32, 32, 255);
    colorselectemprect = (SDL_FRect){ colorselectorui.w-33, 11, 22, colorselectorui.h-22 };
    SDL_RenderFillRect(renderer, &colorselectemprect);
    SDL_Color tempcolor = { 0, 0, 0, 0 };
    for (int y = 0; y < 90; y++) {
        tempcolor = RGBfHSV(lerp(0,360,(double)y/90), 1, 1);
        SDL_SetRenderDrawColor(renderer, tempcolor.r, tempcolor.g, tempcolor.b, tempcolor.a);
        for (int x = 0; x < 20; x++) {
            SDL_RenderPoint(renderer, colorselectorui.w-13-x, 12+y);
        }
    }
    SDL_SetRenderDrawColor(renderer, 32, 32, 32, 255);
    colorselectemprect = (SDL_FRect){ colorselectorui.w-131, 11, 92, 92 };
    SDL_RenderFillRect(renderer, &colorselectemprect);
    for (int y = 0; y < 90; y++){
        for (int x = 0; x < 90; x++){
            tempcolor=RGBfHSV(0, (double)x/90, 1-((double)y/90));
            SDL_SetRenderDrawColor(renderer, tempcolor.r, tempcolor.g, tempcolor.b, tempcolor.a);
            SDL_RenderPoint(renderer, colorselectorui.w-130+x, 12+y);
        }
    }
    SDL_SetRenderDrawColor(renderer, 32, 32, 32, 255);
    SDL_FRect colorselectrealrect = { colorselectorui.x+colorselectorui.w-158, colorselectorui.y+12, 20, 24 };
    colorselectemprect = (SDL_FRect){ colorselectorui.w-159, 11, 22, colorselectorui.h-22 };
    SDL_RenderFillRect(renderer, &colorselectemprect);
    colorselectemprect = (SDL_FRect){ colorselectorui.w-158, 12, 20, colorselectorui.h-24 };
    SDL_FRect colorselectalpharect = (SDL_FRect){ colorselectorui.w-158, 12, 20, colorselectorui.h-24 };
    SDL_Texture * colorselectalpha = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, colorselectalpharect.w, colorselectalpharect.h);
    SDL_SetRenderTarget(renderer, colorselectalpha);
    SDL_SetRenderDrawColor(renderer, gridmain.r, gridmain.g, gridmain.b, gridmain.a);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, gridalt.r, gridalt.g, gridalt.b, gridalt.a);
    for (int y = 0; y < (int)(colorselectemprect.h/8)+2; y++) {
        grid.y = (y*8)+((int)colorselectemprect.y%8)-4;
        for (int x = 0; x < (int)(colorselectemprect.w/8); x++) {
            grid.x = (x*16)-((y%2)*8)+((int)colorselectemprect.x%8);
            SDL_RenderFillRect(renderer, &grid); }}
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    for (int y = 0; y < 90; y++) {
        SDL_SetRenderDrawColor(renderer, gridmain.r, gridmain.g, gridmain.b, (Uint8)lerp(255, 0, (double)y/90));
        for (int x = 0; x < 21; x++) {
            SDL_RenderPoint(renderer, x, y); }}
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    SDL_SetRenderTarget(renderer, colorselector);
    SDL_RenderTexture(renderer, colorselectalpha, NULL, &colorselectalpharect);
    SDL_Texture * colorselectoruitems[] = {
        SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 24, 10),
        SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 12, 12),
        SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 24, 10)
    };
    SDL_FRect colorselectoruitemsrects[] = {
        { 0, 0, 24, 10 },
        { 1, 1, 22, 8 },
        { 3, 3, 18, 4 },
        { 0, 0, 12, 12 },
        { 1, 1, 10, 10 },
        { 3, 3, 6, 6 },
        { 0, 0, 24, 10 },
    };
    SDL_SetRenderTarget(renderer, colorselectoruitems[0]);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 64);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &colorselectoruitemsrects[1]);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderFillRect(renderer, &colorselectoruitemsrects[2]);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 127);
    SDL_RenderRect(renderer, &colorselectoruitemsrects[2]);
    SDL_SetRenderTarget(renderer, colorselectoruitems[1]);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 64);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &colorselectoruitemsrects[4]);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderFillRect(renderer, &colorselectoruitemsrects[5]);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 127);
    SDL_RenderRect(renderer, &colorselectoruitemsrects[5]);
    SDL_SetRenderTarget(renderer, colorselectoruitems[2]);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 64);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &colorselectoruitemsrects[1]);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderFillRect(renderer, &colorselectoruitemsrects[2]);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 127);
    SDL_RenderRect(renderer, &colorselectoruitemsrects[2]);
    SDL_Texture * leftcolorselector = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, leftselectedcolorealrect.w, leftselectedcolorealrect.h);
    SDL_Texture * rightcolorselector = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, rightselectedcolorealrect.w, rightselectedcolorealrect.h);
    sprite.push_back(SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, resolution.x, resolution.y));
    presprite = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, resolution.x, resolution.y);
    cursorture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, resolution.x, resolution.y);
    undotextures[frame].push_back(SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, resolution.x, resolution.y));
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_SetRenderTarget(renderer, sprite[frame]);
    SDL_RenderClear(renderer);
    SDL_SetRenderTarget(renderer, presprite);
    SDL_RenderClear(renderer);
    SDL_SetRenderTarget(renderer, undotextures[frame][0]);
    SDL_RenderClear(renderer);
    SDL_SetRenderTarget(renderer, NULL);
    SDL_SetTextureScaleMode(toolsborder, SDL_SCALEMODE_NEAREST);
    SDL_SetTextureScaleMode(presprite, SDL_SCALEMODE_NEAREST);
    SDL_SetTextureScaleMode(sprite[0], SDL_SCALEMODE_NEAREST);
    SDL_SetTextureScaleMode(cursorture, SDL_SCALEMODE_NEAREST);
    SDL_SetTextureScaleMode(leftcolorselector, SDL_SCALEMODE_NEAREST);
    SDL_SetTextureScaleMode(rightcolorselector, SDL_SCALEMODE_NEAREST);
    SDL_GetTextureSize(presprite, &spriterect.w, &spriterect.h);
    spriterect = (SDL_FRect){0, 0, spriterect.w, spriterect.h };
    SDL_Surface * tempcursizetextrect = TTF_RenderText_Blended(font, "1x", (SDL_Color){ .r=255, .g=255, .b=255, .a=255 });
    SDL_Texture * cursizetextrecture = SDL_CreateTextureFromSurface(renderer, tempcursizetextrect);
    SDL_DestroySurface(tempcursizetextrect);
    SDL_GetTextureSize(cursizetextrecture, &cursizetextrect.w, &cursizetextrect.h);
    cursizetextrect = (SDL_FRect){cursizerectborder.x+((cursizerectborder.w-cursizetextrect.w)/2), cursizerectborder.y+cursizerectborder.h-cursizetextrect.h, cursizetextrect.w, cursizetextrect.h };
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);


    /* Setup variables that require data from textures */
    leftselectedcolorealrect = (SDL_FRect){ 8+((leftselectedcolorect.w-leftselectedcolorealrect.w)/2), toolsrect.y+toolsrect.h+8+((leftselectedcolorect.h-leftselectedcolorealrect.h)/2), leftselectedcolorealrect.w, leftselectedcolorealrect.h };
    rightselectedcolorealrect = (SDL_FRect){ toolsrect.w-rightselectedcolorealrect.w+((rightselectedcolorect.w-rightselectedcolorealrect.w)/2), toolsrect.y+toolsrect.h+8+((rightselectedcolorect.h-rightselectedcolorealrect.h)/2), rightselectedcolorealrect.w, rightselectedcolorealrect.h };
    leftcoloralphapreview[0].position.x=leftselectedcolorealrect.x;
    leftcoloralphapreview[0].position.y=leftselectedcolorealrect.y;
    leftcoloralphapreview[1].position.x=leftselectedcolorealrect.x+leftselectedcolorealrect.w;
    leftcoloralphapreview[1].position.y=leftselectedcolorealrect.y;
    leftcoloralphapreview[2].position.x=leftselectedcolorealrect.x;
    leftcoloralphapreview[2].position.y=leftselectedcolorealrect.y+leftselectedcolorealrect.h;
    leftcolorpreview[0].position.x=leftselectedcolorealrect.x+leftselectedcolorealrect.w;
    leftcolorpreview[0].position.y=leftselectedcolorealrect.y;
    leftcolorpreview[1].position.x=leftselectedcolorealrect.x+leftselectedcolorealrect.w;
    leftcolorpreview[1].position.y=leftselectedcolorealrect.y+leftselectedcolorealrect.h;
    leftcolorpreview[2].position.x=leftselectedcolorealrect.x;
    leftcolorpreview[2].position.y=leftselectedcolorealrect.y+leftselectedcolorealrect.h;
    rightcoloralphapreview[0].position.x=rightselectedcolorealrect.x;
    rightcoloralphapreview[0].position.y=rightselectedcolorealrect.y;
    rightcoloralphapreview[1].position.x=rightselectedcolorealrect.x+rightselectedcolorealrect.w;
    rightcoloralphapreview[1].position.y=rightselectedcolorealrect.y;
    rightcoloralphapreview[2].position.x=rightselectedcolorealrect.x;
    rightcoloralphapreview[2].position.y=rightselectedcolorealrect.y+rightselectedcolorealrect.h;
    rightcolorpreview[0].position.x=rightselectedcolorealrect.x+rightselectedcolorealrect.w;
    rightcolorpreview[0].position.y=rightselectedcolorealrect.y;
    rightcolorpreview[1].position.x=rightselectedcolorealrect.x+rightselectedcolorealrect.w;
    rightcolorpreview[1].position.y=rightselectedcolorealrect.y+rightselectedcolorealrect.h;
    rightcolorpreview[2].position.x=rightselectedcolorealrect.x;
    rightcolorpreview[2].position.y=rightselectedcolorealrect.y+rightselectedcolorealrect.h;
    leftselectedcolorect = (SDL_FRect){8, toolsrect.y+toolsrect.h+8, leftselectedcolorect.w, leftselectedcolorect.h };
    rightselectedcolorect = (SDL_FRect){toolsrect.w-rightselectedcolorect.w+8, toolsrect.y+toolsrect.h+8, rightselectedcolorect.w, rightselectedcolorect.h };
    colorselectorui.y = leftselectedcolorealrect.y+leftselectedcolorealrect.h+12;
    colorselectelements[0] = (SDL_FRect){ colorselectorui.x+colorselectorui.w-33, colorselectorui.y+12, colorselectelements[0].w, colorselectelements[0].h };
    colorselectelements[1] = (SDL_FRect){ colorselectorui.x+colorselectorui.w-130, colorselectorui.y+12, colorselectelements[1].w, colorselectelements[1].h };
    colorselectelements[2] = (SDL_FRect){ colorselectorui.x+colorselectorui.w-160, colorselectorui.y+12, colorselectelements[2].w, colorselectelements[2].h };
    cursizerect = (SDL_FRect){(float)limit((cursizerectinborder.x+(cursizerectinborder.w/2)-((canvas.w/resolution.x)/2*(int)cursize)), cursizerectinborder.x, 100), (float)limit(cursizerectinborder.y-(cursizerectinborder.w/2)+((canvas.w/resolution.y)/2*(int)cursize), std::nullopt, cursizerectinborder.y), (float)limit(canvas.w/resolution.x*(int)cursize, std::nullopt, cursizerectinborder.w), -(float)limit(canvas.h/resolution.y*(int)cursize, std::nullopt, -cursizerectinborder.h) };
    SDL_SetRenderTarget(renderer, leftcolorselector);
    SDL_SetRenderDrawColor(renderer, gridmain.r, gridmain.g, gridmain.b, gridmain.a);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, gridalt.r, gridalt.g, gridalt.b, gridalt.a);
    for (int y = 0; y < (int)(leftselectedcolorect.h/8)+1; y++) {
        grid.y = (y*8)-((int)leftselectedcolorealrect.y%8);
        for (int x = 0; x < (int)(leftselectedcolorect.w/8); x++) {
            grid.x = (x*16)-((y%2)*8)-((int)leftselectedcolorealrect.x%8)-4;
            SDL_RenderFillRect(renderer, &grid); }}
    SDL_SetRenderTarget(renderer, rightcolorselector);
    SDL_SetRenderDrawColor(renderer, gridmain.r, gridmain.g, gridmain.b, gridmain.a);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, gridalt.r, gridalt.g, gridalt.b, gridalt.a);
    for (int y = 0; y < (int)(leftselectedcolorect.h/8)+1; y++) {
        grid.y = (y*8)-((int)leftselectedcolorealrect.y%8);
        for (int x = 0; x < (int)(leftselectedcolorect.w/8); x++) {
            grid.x = (x*16)-((y%2)*8)-((int)leftselectedcolorealrect.x%8)-4;
            SDL_RenderFillRect(renderer, &grid); }}
    SDL_SetRenderTarget(renderer, NULL);


    /* Remove unecessary data */
    SDL_DestroySurface(pretools);


    /* Main loop */
    while (loop) {


        /* Get mouse pos and get FPS */
        dtdata.then = dtdata.now;
        dtdata.now = SDL_GetPerformanceCounter();
        dtdata.deltime = (dtdata.now - dtdata.then) / (double)SDL_GetPerformanceFrequency();
        dtdata.fps = (1/dtdata.deltime);


        /* Clear renderer and draw background grid */
        SDL_SetRenderDrawColor(renderer, gridmain.r, gridmain.g, gridmain.b, gridmain.a);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, gridalt.r, gridalt.g, gridalt.b, gridalt.a);
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
        while (SDL_PollEvent(&e)){
            switch (e.type) {
                case SDL_EVENT_QUIT:
                    loop = false;
                    break;


                /* Resize window */
                case SDL_EVENT_WINDOW_RESIZED:
                    SDL_GetWindowSize(window, &windowsize.x, &windowsize.y);
                    SDL_SetWindowSize(window, std::round(windowsize.x), std::round(windowsize.y));
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
                    leftselectedcolorealrect = (SDL_FRect){ 8+((leftselectedcolorect.w-leftselectedcolorealrect.w)/2), toolsrect.y+toolsrect.h+8+((leftselectedcolorect.h-leftselectedcolorealrect.h)/2), leftselectedcolorealrect.w, leftselectedcolorealrect.h };
                    rightselectedcolorealrect = (SDL_FRect){ toolsrect.w-rightselectedcolorealrect.w+((rightselectedcolorect.w-rightselectedcolorealrect.w)/2), toolsrect.y+toolsrect.h+8+((rightselectedcolorect.h-rightselectedcolorealrect.h)/2), rightselectedcolorealrect.w, rightselectedcolorealrect.h };
                    leftcoloralphapreview[0].position.x=leftselectedcolorealrect.x;
                    leftcoloralphapreview[0].position.y=leftselectedcolorealrect.y;
                    leftcoloralphapreview[1].position.x=leftselectedcolorealrect.x+leftselectedcolorealrect.w;
                    leftcoloralphapreview[1].position.y=leftselectedcolorealrect.y;
                    leftcoloralphapreview[2].position.x=leftselectedcolorealrect.x;
                    leftcoloralphapreview[2].position.y=leftselectedcolorealrect.y+leftselectedcolorealrect.h;
                    leftcolorpreview[0].position.x=leftselectedcolorealrect.x+leftselectedcolorealrect.w;
                    leftcolorpreview[0].position.y=leftselectedcolorealrect.y;
                    leftcolorpreview[1].position.x=leftselectedcolorealrect.x+leftselectedcolorealrect.w;
                    leftcolorpreview[1].position.y=leftselectedcolorealrect.y+leftselectedcolorealrect.h;
                    leftcolorpreview[2].position.x=leftselectedcolorealrect.x;
                    leftcolorpreview[2].position.y=leftselectedcolorealrect.y+leftselectedcolorealrect.h;
                    rightcoloralphapreview[0].position.x=rightselectedcolorealrect.x;
                    rightcoloralphapreview[0].position.y=rightselectedcolorealrect.y;
                    rightcoloralphapreview[1].position.x=rightselectedcolorealrect.x+rightselectedcolorealrect.w;
                    rightcoloralphapreview[1].position.y=rightselectedcolorealrect.y;
                    rightcoloralphapreview[2].position.x=rightselectedcolorealrect.x;
                    rightcoloralphapreview[2].position.y=rightselectedcolorealrect.y+rightselectedcolorealrect.h;
                    rightcolorpreview[0].position.x=rightselectedcolorealrect.x+rightselectedcolorealrect.w;
                    rightcolorpreview[0].position.y=rightselectedcolorealrect.y;
                    rightcolorpreview[1].position.x=rightselectedcolorealrect.x+rightselectedcolorealrect.w;
                    rightcolorpreview[1].position.y=rightselectedcolorealrect.y+rightselectedcolorealrect.h;
                    rightcolorpreview[2].position.x=rightselectedcolorealrect.x;
                    rightcolorpreview[2].position.y=rightselectedcolorealrect.y+rightselectedcolorealrect.h;


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


                    /* Reset UI */
                    cursizerect = (SDL_FRect){(float)limit((cursizerectinborder.x+(cursizerectinborder.w/2)-((canvas.w/resolution.x)/2*(int)cursize)), cursizerectinborder.x, 100), (float)limit(cursizerectinborder.y-(cursizerectinborder.w/2)+((canvas.w/resolution.y)/2*(int)cursize), std::nullopt, cursizerectinborder.y), (float)limit(canvas.w/resolution.x*(int)cursize, std::nullopt, cursizerectinborder.w), -(float)limit(canvas.h/resolution.y*(int)cursize, std::nullopt, -cursizerectinborder.h) };
                    colorselectorui.y = leftselectedcolorealrect.y+leftselectedcolorealrect.h+12;
                    colorselectrealrect = { colorselectorui.x+colorselectorui.w-158, colorselectorui.y+12, 20, 24 };
                    colorselectelements[0] = (SDL_FRect){ colorselectorui.x+colorselectorui.w-34, colorselectorui.y+12, colorselectelements[0].w, colorselectelements[0].h };
                    colorselectelements[1] = (SDL_FRect){ colorselectorui.x+colorselectorui.w-132, colorselectorui.y+12, colorselectelements[1].w, colorselectelements[1].h };
                    colorselectelements[2] = (SDL_FRect){ colorselectorui.x+colorselectorui.w-164, colorselectorui.y+12, colorselectelements[2].w, colorselectelements[2].h };
                    leftselectedcolorect = (SDL_FRect){8, toolsrect.y+toolsrect.h+8, leftselectedcolorect.w, leftselectedcolorect.h };
                    rightselectedcolorect = (SDL_FRect){toolsrect.w-rightselectedcolorect.w+8, toolsrect.y+toolsrect.h+8, rightselectedcolorect.w, rightselectedcolorect.h };
                    SDL_SetRenderTarget(renderer, leftcolorselector);
                    SDL_SetRenderDrawColor(renderer, gridmain.r, gridmain.g, gridmain.b, gridmain.a);
                    SDL_RenderClear(renderer);
                    SDL_SetRenderDrawColor(renderer, gridalt.r, gridalt.g, gridalt.b, gridalt.a);
                    for (int y = 0; y < (int)(leftselectedcolorect.h/8)+1; y++) {
                        grid.y = (y*8)-((int)leftselectedcolorealrect.y%8);
                        for (int x = 0; x < (int)(leftselectedcolorect.w/8); x++) {
                            grid.x = (x*16)-((y%2)*8)-((int)leftselectedcolorealrect.x%8)+4;
                            SDL_RenderFillRect(renderer, &grid); }}
                    SDL_SetRenderTarget(renderer, rightcolorselector);
                    SDL_SetRenderDrawColor(renderer, gridmain.r, gridmain.g, gridmain.b, gridmain.a);
                    SDL_RenderClear(renderer);
                    SDL_SetRenderDrawColor(renderer, gridalt.r, gridalt.g, gridalt.b, gridalt.a);
                    for (int y = 0; y < (int)(leftselectedcolorect.h/8)+1; y++) {
                        grid.y = (y*8)-((int)leftselectedcolorealrect.y%8);
                        for (int x = 0; x < (int)(leftselectedcolorect.w/8); x++) {
                            grid.x = (x*16)-((y%2)*8)-((int)leftselectedcolorealrect.x%8)+4;
                            SDL_RenderFillRect(renderer, &grid); }}
                    colorselectemprect = (SDL_FRect){ colorselectorui.w-158, 12, 20, colorselectorui.h-24 };
                    colorselectalpharect = (SDL_FRect){ colorselectorui.w-158, 12, 20, colorselectorui.h-24 };
                    SDL_SetRenderTarget(renderer, colorselectalpha);
                    SDL_SetRenderDrawColor(renderer, gridmain.r, gridmain.g, gridmain.b, gridmain.a);
                    SDL_RenderClear(renderer);
                    SDL_SetRenderDrawColor(renderer, gridalt.r, gridalt.g, gridalt.b, gridalt.a);
                    for (int y = 0; y < (int)(colorselectemprect.h/8)+2; y++) {
                        grid.y = (y*8)-((int)colorselectemprect.y%8);
                        for (int x = 0; x < (int)(colorselectemprect.w/8); x++) {
                            grid.x = (x*16)-((y%2)*8)-((int)colorselectemprect.x%8);
                            SDL_RenderFillRect(renderer, &grid); }}
                    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
                    for (int y = 0; y < 90; y++) {
                        SDL_SetRenderDrawColor(renderer, gridmain.r, gridmain.g, gridmain.b, (Uint8)lerp(255, 0, (double)y/90));
                        for (int x = 0; x < 21; x++) {
                            SDL_RenderPoint(renderer, x, y); }}
                    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
                    SDL_SetRenderTarget(renderer, colorselector);
                    colorselectalpharect = (SDL_FRect){ (float)(int)(colorselectalpharect.x), (float)(int)(colorselectalpharect.y), colorselectalpharect.w, colorselectalpharect.h };
                    SDL_RenderTexture(renderer, colorselectalpha, NULL, &colorselectalpharect);
                    SDL_SetRenderTarget(renderer, NULL);
                    break;


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
                    cursizerect = (SDL_FRect){(float)limit((cursizerectinborder.x+(cursizerectinborder.w/2)-((canvas.w/resolution.x)/2*(int)cursize)), cursizerectinborder.x, 100), (float)limit(cursizerectinborder.y-(cursizerectinborder.w/2)+((canvas.w/resolution.y)/2*(int)cursize), std::nullopt, cursizerectinborder.y), (float)limit(canvas.w/resolution.x*(int)cursize, std::nullopt, cursizerectinborder.w), -(float)limit(canvas.h/resolution.y*(int)cursize, std::nullopt, -cursizerectinborder.h) };
                    break;




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
                    colorselectorvisible = (e.button.button == SDL_BUTTON_LMASK && (contained(mouse, leftselectedcolorect) || contained(mouse, rightselectedcolorect) || (contained(mouse, colorselectorui) && colorselectorvisible)));
                    if (colorselectorvisible && !contained(mouse, colorselectorui) && e.button.button == SDL_BUTTON_LMASK) {
                        leftcolorchanging = contained(mouse, leftselectedcolorect);
                        fvec3 tempvecolor = (leftcolorchanging)?HSVfRGB(leftcolor.r, leftcolor.g, leftcolor.b):HSVfRGB(rightcolor.r, rightcolor.g, rightcolor.b);
                        HSVA = (fvec4){ tempvecolor.x, tempvecolor.y, tempvecolor.z, ((leftcolorchanging)?(float)leftcolor.a/255:(float)rightcolor.a/255) };
                        SDL_SetRenderTarget(renderer, colorselector);
                        for (int y = 0; y < 90; y++){
                            for (int x = 0; x < 90; x++){
                                tempcolor = RGBfHSV(HSVA.w, (double)x/90, 1-((double)y/90));
                                SDL_SetRenderDrawColor(renderer, tempcolor.r, tempcolor.g, tempcolor.b, tempcolor.a);
                                SDL_RenderPoint(renderer, colorselectorui.w-130+x, 12+y);
                            }
                        }
                        SDL_SetRenderTarget(renderer, NULL);
                    }
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
                            SDL_SetTextureBlendMode(presprite, (mousebitmask & SDL_BUTTON_RMASK)?straightdarken:straightbrighten);
                            SDL_RenderTexture(renderer, presprite, NULL, &spriterect);
                            SDL_SetTextureBlendMode(presprite, SDL_BLENDMODE_BLEND);
                            SDL_SetRenderTarget(renderer, NULL);
                            skiprespriterender = true;
                        }
                    }
                    if (contained(lastmouse, canvas)) undupdatequeued = true;
                    break;


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
                    break;
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
                                lightenline(renderer, (vec2){ (int)(((framelastmouse.x-((canvas.w/resolution.x)*((int)cursize-1)/2))-canvas.x)/(canvas.w/resolution.x))+x, (int)(((framelastmouse.y-((canvas.h/resolution.y)*((int)cursize-1)/2))-canvas.y)/(canvas.h/resolution.y))+y }, (vec2){ (int)(((mouse.x-((canvas.w/resolution.x)*((int)cursize-1)/2))-canvas.x)/(canvas.w/resolution.x))+x, (int)(((mouse.y-((canvas.h/resolution.y)*((int)cursize-1)/2))-canvas.y)/(canvas.h/resolution.y))+y }, (mousebitmask & SDL_BUTTON_RMASK), false);
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
                                lightenline(renderer, (vec2){ (int)(((framelastmouse.x-((canvas.w/resolution.x)*((int)cursize-1)/2))-canvas.x)/(canvas.w/resolution.x))+x, (int)(((framelastmouse.y-((canvas.h/resolution.y)*((int)cursize-1)/2))-canvas.y)/(canvas.h/resolution.y))+y }, (vec2){ (int)(((mouse.x-((canvas.w/resolution.x)*((int)cursize-1)/2))-canvas.x)/(canvas.w/resolution.x))+x, (int)(((mouse.y-((canvas.h/resolution.y)*((int)cursize-1)/2))-canvas.y)/(canvas.h/resolution.y))+y }, (mousebitmask & SDL_BUTTON_RMASK), true);
                            }
                        }
                        SDL_SetRenderTarget(renderer, NULL);
                    }
                }


                else if (currentool == 17) {
                    if (contained(mouse, canvas)) {
                        (mousebitmask & SDL_BUTTON_LMASK)?SDL_ReadSurfacePixel(prespritesurface, (mouse.x-canvas.x)/(canvas.w/resolution.x), (mouse.y-canvas.y)/(canvas.h/resolution.y), &leftcolor.r, &leftcolor.g, &leftcolor.b, &leftcolor.a):SDL_ReadSurfacePixel(prespritesurface, (mouse.x-canvas.x)/(canvas.w/resolution.x), (mouse.y-canvas.y)/(canvas.h/resolution.y), &rightcolor.r, &rightcolor.g, &rightcolor.b, &rightcolor.a);
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
            SDL_SetRenderTarget(renderer, sprite[frame]);
            prespritesurface = SDL_RenderReadPixels(renderer, NULL);
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
            else {
                if (currentool == 5) SDL_SetTextureBlendMode(presprite, (mousebitmask & SDL_BUTTON_RMASK)?straightdarken:straightbrighten);
                SDL_RenderTexture(renderer, presprite, NULL, &canvas);
                if (currentool == 5) SDL_SetTextureBlendMode(presprite, SDL_BLENDMODE_BLEND);
            }
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
        SDL_RenderFillRect(renderer, &cursizerect);
        ((contained(mouse, leftselectedcolorect) && !(mousebitmask & SDL_BUTTON_LMASK)) || (leftcolorchanging && colorselectorvisible))?SDL_SetRenderDrawColor(renderer, 136, 136, 136, 255):SDL_SetRenderDrawColor(renderer, 68, 68, 68, 255);
        SDL_RenderFillRect(renderer, &leftselectedcolorect);
        ((contained(mouse, rightselectedcolorect) && !(mousebitmask & SDL_BUTTON_LMASK)) || (!leftcolorchanging && colorselectorvisible))?SDL_SetRenderDrawColor(renderer, 136, 136, 136, 255):SDL_SetRenderDrawColor(renderer, 68, 68, 68, 255);
        SDL_RenderFillRect(renderer, &rightselectedcolorect);
        SDL_SetRenderDrawColor(renderer, leftcolor.r, leftcolor.g, leftcolor.b, leftcolor.a);
        if (colorselectorvisible) {
            colorselectorui = (SDL_FRect){ std::round(colorselectorui.x), std::round(colorselectorui.y), colorselectorui.w, colorselectorui.h };
            SDL_RenderTexture(renderer, colorselector, NULL, &colorselectorui);
            if (mousebitmask & SDL_BUTTON_LMASK) {
                if (contained(lastmouse, colorselectelements[0])) {
                    HSVA.w = limit((mouse.y-colorselectelements[0].y)*(360/colorselectelements[0].h), 0, 360);
                    SDL_SetRenderTarget(renderer, colorselector);
                    for (int y = 0; y < 90; y++){
                        for (int x = 0; x < 90; x++){
                            tempcolor=RGBfHSV(HSVA.w, (double)x/90, 1-((double)y/90));
                            SDL_SetRenderDrawColor(renderer, tempcolor.r, tempcolor.g, tempcolor.b, tempcolor.a);
                            SDL_RenderPoint(renderer, colorselectorui.w-130+x, 12+y);
                        }
                    }
                    SDL_SetRenderTarget(renderer, NULL);
                }
                else if (contained(lastmouse, colorselectelements[1])) {
                    HSVA.x = limit((mouse.x-colorselectelements[1].x)/colorselectelements[1].w, 0, 1);
                    HSVA.y = limit(1-(mouse.y-colorselectelements[1].y)/colorselectelements[1].h, 0, 1);
                }
                else if (contained(lastmouse, colorselectelements[2])) {
                    HSVA.z = limit(1-((mouse.y-colorselectelements[0].y)/colorselectelements[0].h), 0, 1);
                }
                tempcolor = RGBfHSV(HSVA.w, HSVA.x, HSVA.y);
                if (leftcolorchanging) leftcolor = (SDL_Color){ tempcolor.r, tempcolor.g, tempcolor.b, (Uint8)(HSVA.z*255) };
                else rightcolor = (SDL_Color){ tempcolor.r, tempcolor.g, tempcolor.b, (Uint8)(HSVA.z*255) };
                leftcoloralphapreview[0].color = (SDL_FColor){ (float)leftcolor.r/255, (float)leftcolor.g/255, (float)leftcolor.b/255, (float)leftcolor.a/255 };
                leftcoloralphapreview[1].color = (SDL_FColor){ (float)leftcolor.r/255, (float)leftcolor.g/255, (float)leftcolor.b/255, (float)leftcolor.a/255 };
                leftcoloralphapreview[2].color = (SDL_FColor){ (float)leftcolor.r/255, (float)leftcolor.g/255, (float)leftcolor.b/255, (float)leftcolor.a/255 };
                leftcolorpreview[0].color = (SDL_FColor){ (float)leftcolor.r/255, (float)leftcolor.g/255, (float)leftcolor.b/255, 1 };
                leftcolorpreview[1].color = (SDL_FColor){ (float)leftcolor.r/255, (float)leftcolor.g/255, (float)leftcolor.b/255, 1 };
                leftcolorpreview[2].color = (SDL_FColor){ (float)leftcolor.r/255, (float)leftcolor.g/255, (float)leftcolor.b/255, 1 };
                rightcoloralphapreview[0].color = (SDL_FColor){ (float)rightcolor.r/255, (float)rightcolor.g/255, (float)rightcolor.b/255, (float)rightcolor.a/255 };
                rightcoloralphapreview[1].color = (SDL_FColor){ (float)rightcolor.r/255, (float)rightcolor.g/255, (float)rightcolor.b/255, (float)rightcolor.a/255 };
                rightcoloralphapreview[2].color = (SDL_FColor){ (float)rightcolor.r/255, (float)rightcolor.g/255, (float)rightcolor.b/255, (float)rightcolor.a/255 };
                rightcolorpreview[0].color = (SDL_FColor){ (float)rightcolor.r/255, (float)rightcolor.g/255, (float)rightcolor.b/255, 1 };
                rightcolorpreview[1].color = (SDL_FColor){ (float)rightcolor.r/255, (float)rightcolor.g/255, (float)rightcolor.b/255, 1 };
                rightcolorpreview[2].color = (SDL_FColor){ (float)rightcolor.r/255, (float)rightcolor.g/255, (float)rightcolor.b/255, 1 };
            }
            colorselectoruitemsrects[0] = (SDL_FRect){ colorselectorui.x+colorselectorui.w-34, (float)(int)(colorselectorui.y+7+((HSVA.w/360)*90)), colorselectoruitemsrects[0].w, colorselectoruitemsrects[0].h };
            SDL_RenderTexture(renderer, colorselectoruitems[0], NULL, &colorselectoruitemsrects[0]);
            colorselectoruitemsrects[3] = (SDL_FRect){ (float)(int)(colorselectorui.x+colorselectorui.w-136+(HSVA.x*90)), (float)(int)(colorselectorui.y+6+((1-HSVA.y)*90)), colorselectoruitemsrects[3].w, colorselectoruitemsrects[3].h };
            SDL_RenderTexture(renderer, colorselectoruitems[1], NULL, &colorselectoruitemsrects[3]);
            colorselectoruitemsrects[6] = (SDL_FRect){ colorselectorui.x+colorselectorui.w-160, (float)(int)(colorselectorui.y+7+((1-HSVA.z)*90)), colorselectoruitemsrects[0].w, colorselectoruitemsrects[0].h };
            SDL_RenderTexture(renderer, colorselectoruitems[2], NULL, &colorselectoruitemsrects[6]);
        }
        SDL_RenderTexture(renderer, leftcolorselector, NULL, &leftselectedcolorealrect);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_RenderGeometry(renderer, NULL, leftcoloralphapreview, 3, NULL, 0);
        SDL_RenderGeometry(renderer, NULL, leftcolorpreview, 3, NULL, 0);
        SDL_SetRenderDrawColor(renderer, rightcolor.r, rightcolor.g, rightcolor.b, rightcolor.a);
        SDL_RenderTexture(renderer, rightcolorselector, NULL, &rightselectedcolorealrect);
        SDL_RenderGeometry(renderer, NULL, rightcoloralphapreview, 3, NULL, 0);
        SDL_RenderGeometry(renderer, NULL, rightcolorpreview, 3, NULL, 0);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);


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

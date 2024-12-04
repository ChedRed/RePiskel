#define _USE_MATH_DEFINES
#include "TextHelp.hpp"


/*
Get os
    0: ¯\_(ツ)_/¯
    1: Windows
    2: MacOS
    3: Linux
*/
int os = 0;


/* OS-dependent stuff */
std::string rpath;


/* SDL setup variables */
SDL_Event e;
bool loop = true;
bool focus = true;


/* New 'data types' */
struct vec2 { int x; int y; };
struct fvec3 { float x; float y; float z; };
struct fvec4 { float w; float x; float y; float z; };
struct duo { float a; float b; };


/* Simple math things */
#define M_EIGHTHPI (((2*M_PI)/360)*22.5)


/* Display setup variables */
float deltime;
float then;
vec2 windowsize = { 1000, 650 };
SDL_Color gridmain = { .r=85, .g=85, .b=85, .a=255 };
SDL_Color gridalt = { .r=76, .g=76, .b=76, .a=255 };
SDL_FRect grid = { 0, 0, 8, 8 };


/* Input setup variables */
Vector2 mouse = { 0, 0 };
Vector2 scroll = { 0, 0 };
bool mousedowned = false;
Vector2 lastmouse = { 0, 0 };
Vector2 framelastmouse = { 0, 0 };
Uint32 mousebitmask;
const bool * keystates = SDL_GetKeyboardState(NULL);
bool oldshift = false;
bool oldmousedown = false;
bool oldleftmousedown = false;


/* UI */
duo margin = { 240, 260 };
SDL_FRect nameborder = { 0, 0, (float)windowsize.x, 36 };


/* Canvas */
bool edited = false;
Vector2 resolution = { 16, 16 };
Vector2 drawresolution = { 0, 0 };
Vector2 resratio = Vector2((resolution.x<resolution.y)?(float)resolution.x/resolution.y:1, (resolution.x>resolution.y)?(float)resolution.y/resolution.x:1);
Vector2 canvascenter = Vector2((margin.a/2)+((windowsize.x-margin.b)/2), (22)+(((float)windowsize.y-8)/2));
SDL_FRect precanvas = { margin.a, 44, windowsize.x-margin.a-margin.b, (float)windowsize.y-52 };
Vector2 canvasize = Vector2(resratio.x*((resolution.x<resolution.y)?precanvas.h:precanvas.w), resratio.y*((resolution.x>resolution.y)?precanvas.h:precanvas.w));
Vector2 oldcanvasize = canvasize;
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
SDL_BlendMode reverstraightbrighten = SDL_ComposeCustomBlendMode(SDL_BLENDFACTOR_ONE, SDL_BLENDFACTOR_SRC_ALPHA, SDL_BLENDOPERATION_ADD, SDL_BLENDFACTOR_ONE, SDL_BLENDFACTOR_ZERO, SDL_BLENDOPERATION_REV_SUBTRACT);
SDL_BlendMode straightdarken = SDL_ComposeCustomBlendMode(SDL_BLENDFACTOR_SRC_ALPHA, SDL_BLENDFACTOR_ONE, SDL_BLENDOPERATION_REV_SUBTRACT, SDL_BLENDFACTOR_ZERO, SDL_BLENDFACTOR_ONE, SDL_BLENDOPERATION_REV_SUBTRACT);
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
std::string toolnames[18] = {"Pen", "Line", "Eraser", "Mirror", "Dither", "Lighten", "Fill", "Multi-Fill", "", "Rectangle", "Circle", "Free Shape", "Rectangle Select", "Free Select", "Magic Select", "Grab", "Pick Color", ""};
SDL_FRect cursizerectinborder;
SDL_FRect cursizerectborder;
SDL_FRect cursizerect;
int cursize = 1;
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
void lightenline(SDL_Renderer * renderer, vec2 start, vec2 end, bool darken, bool safe) { // Fix brightening invisible pixels (preferably with GetSurfacePixel)
    SDL_Color thispixel;
    int addvalue = 0;
    vec2 distance = { abs(end.x-start.x), abs(end.y-start.y) };
    vec2 mirror = { (start.x<end.x)?1:-1, (start.y<end.y) ?1:-1 };
    int err = distance.x-distance.y;

    while (true) {
        SDL_ReadSurfacePixel(prespritesurface, start.x, start.y, &thispixel.r, &thispixel.g, &thispixel.b, &thispixel.a);
        addvalue = (thispixel.a==0&&safe)?0:((darken)?-16:16);
        SDL_SetRenderDrawColor(renderer, limit(thispixel.r + addvalue, 0, 255), limit(thispixel.g + addvalue, 0, 255), limit(thispixel.b + addvalue, 0, 255), thispixel.a);
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
int main(int argc, char* argv[]) {
    /* Set os variable */
    #ifdef _WIN32
    os = 1;
    #elifdef __APPLE__
    os = 2;
    #elifdef __linux__
    os = 3;
    #endif


    if (os == 1) rpath = "./";
    elif (os == 2) rpath = "../Resources/";


    undotextures.resize(1);
    /* Initialize SDL, create window and renderer */
    std::cout << "Initializing SDL3" << std::endl;
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window * window = SDL_CreateWindow("RePiskel", windowsize.x, windowsize.y, SDL_WINDOW_RESIZABLE | SDL_WINDOW_MOUSE_CAPTURE);
    SDL_Renderer * renderer = SDL_CreateRenderer(window, NULL);
    SDL_SetWindowMinimumSize(window, 960, 540);
    SDL_SetRenderVSync(renderer, 1);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_StartTextInput(window);
    std::cout << "Success! Initializing loop" << std::endl;


    /* Initialize SDL_ttf, create font object */
    TTF_Init();
    TTF_Font * font = TTF_OpenFont((SDL_GetBasePath()+rpath+"Font.ttf").c_str(), 26);
    // int kerning;
    // TTF_Font * TTfont = TTF_OpenFont((SDL_GetBasePath()+rpath+"Font.ttf").c_str(), 260);
    // TTF_GetGlyphKerning(TTfont, 107, 101, &kerning);
    // SDL_SetWindowTitle(window, std::to_string(kerning).c_str());
    // SDL_Surface * tempsurface = TTF_RenderText_Blended(font, "New Piskel", 10, {255, 255, 255, 255});
    // SDL_Texture * tempthisthat = SDL_CreateTextureFromSurface(renderer, tempsurface);
    // SDL_DestroySurface(tempsurface);
    // SDL_FRect tempthisthatrect = {0, 0, 0, 0};
    // SDL_GetTextureSize(tempthisthat, &tempthisthatrect.w, &tempthisthatrect.h);
    // tempthisthatrect = {((float)windowsize.x/2)-(tempthisthatrect.w/2), 18-(tempthisthatrect.h/2), tempthisthatrect.w, tempthisthatrect.h};


    /* Init text assistant :) */
    TextCharacters Characters = {renderer, font, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890,.~!@#$%^&*()_+-=:;\"'? "};


    /* Create text objects */
    TextObject Title = {"New Piskel", Center, Vector2(windowsize.x/2, 18), true};
    TextObject CurSizeText = {"1x", Center, Vector2(cursizerectborder.x/2, (cursizerectborder.y+cursizerectborder.h)), false};


    /* Load textures */
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    SDL_Surface * pretools = SDL_LoadBMP((SDL_GetBasePath()+rpath+"tools.bmp").c_str());
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
    CurSizeText.Position = Vector2(cursizerectborder.x+(cursizerectborder.w/2), cursizerectborder.y+cursizerectborder.h-(Characters.GetMaxHeight(CurSizeText.Text)/2));
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
    cursizerect = (SDL_FRect){(float)limit((cursizerectinborder.x+(cursizerectinborder.w/2)-((canvas.w/resolution.x)/2*cursize)), cursizerectinborder.x, 100), (float)limit(cursizerectinborder.y-(cursizerectinborder.w/2)+((canvas.w/resolution.y)/2*cursize), std::nullopt, cursizerectinborder.y), (float)limit(canvas.w/resolution.x*cursize, std::nullopt, cursizerectinborder.w), -(float)limit(canvas.h/resolution.y*cursize, std::nullopt, -cursizerectinborder.h) };
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
        deltime = (SDL_GetPerformanceCounter() - then) / (double)SDL_GetPerformanceFrequency();
        then = SDL_GetPerformanceCounter();


        /* Clear renderer and draw background grid */
        SDL_SetRenderDrawColor(renderer, gridmain.r, gridmain.g, gridmain.b, gridmain.a);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, gridalt.r, gridalt.g, gridalt.b, gridalt.a);
        for (int y = 0; y < (int)(windowsize.y/8)+1; y++) {
            grid.y = (y*8);
            for (int x = 0; x < (int)(windowsize.x/8); x++) {
                grid.x = (x*16)-((y%2)*8);
                SDL_RenderFillRect(renderer, &grid); }}


        /* Update mouse and input text */
        mousebitmask = SDL_GetMouseState(&mouse.x, &mouse.y);
        scroll.x = 0;
        scroll.y = 0;
        std::string Input = "";


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
                    Title.Position = Vector2(windowsize.x/2, Title.Position.y);


                    /* Reset canvas */
                    precanvas = (SDL_FRect){margin.a, 44, windowsize.x-margin.a-margin.b, (float)windowsize.y-52 };
                    canvascenter = Vector2((margin.a/2)+((windowsize.x-margin.b)/2), 22+(((float)windowsize.y-8)/2));
                    canvasize = Vector2(resratio.x*((precanvas.w>precanvas.h)?precanvas.h:precanvas.w), resratio.y*((precanvas.w>precanvas.h)?precanvas.h:precanvas.w));
                    canvas = (SDL_FRect){canvas.x, canvas.y, canvas.w*(canvasize.x/oldcanvasize.x), canvas.h*(canvasize.y/oldcanvasize.y) };
                    if (canvas.w<=precanvas.w) canvas.x=(precanvas.x+((precanvas.w-canvas.w)/2));
                    if (canvas.h<=precanvas.h) canvas.y=(precanvas.y+((precanvas.h-canvas.h)/2));
                    oldcanvasize=canvasize;


                    /* Reset UI */
                    toolsrect.y=((float)windowsize.y/2)-toolsrect.w;
                    cursizerectborder.y=toolsrect.y;
                    cursizerectinborder.y=toolsrect.y-4;


                    /* Reset pen size text */
                    CurSizeText.Position = Vector2(cursizerectborder.x+(cursizerectborder.w/2), cursizerectborder.y+cursizerectborder.h-10);


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
                    cursizerect = {(float)limit((cursizerectinborder.x+(cursizerectinborder.w/2)-((canvas.w/resolution.x)/2*cursize)), cursizerectinborder.x, 100), (float)limit(cursizerectinborder.y-(cursizerectinborder.w/2)+((canvas.w/resolution.y)/2*cursize), std::nullopt, cursizerectinborder.y), (float)limit(canvas.w/resolution.x*cursize, std::nullopt, cursizerectinborder.w), -(float)limit(canvas.h/resolution.y*cursize, std::nullopt, -cursizerectinborder.h) };
                    colorselectorui.y = leftselectedcolorealrect.y+leftselectedcolorealrect.h+12;
                    colorselectrealrect = { colorselectorui.x+colorselectorui.w-158, colorselectorui.y+12, 20, 24 };
                    colorselectelements[0] = { colorselectorui.x+colorselectorui.w-34, colorselectorui.y+12, colorselectelements[0].w, colorselectelements[0].h };
                    colorselectelements[1] = { colorselectorui.x+colorselectorui.w-132, colorselectorui.y+12, colorselectelements[1].w, colorselectelements[1].h };
                    colorselectelements[2] = { colorselectorui.x+colorselectorui.w-164, colorselectorui.y+12, colorselectelements[2].w, colorselectelements[2].h };
                    leftselectedcolorect = {8, toolsrect.y+toolsrect.h+8, leftselectedcolorect.w, leftselectedcolorect.h };
                    rightselectedcolorect = {toolsrect.w-rightselectedcolorect.w+8, toolsrect.y+toolsrect.h+8, rightselectedcolorect.w, rightselectedcolorect.h };
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
                        if (cursize!=(int)limit(cursize+scroll.y, 1, std::max(resolution.x, resolution.y))){
                            CurSizeText.Text = std::to_string((int)limit(cursize+scroll.y, 1, std::max(resolution.x, resolution.y))) + "x";
                        }
                        cursize = limit(cursize+scroll.y, 1, std::max(resolution.x, resolution.y));
                    }
                    cursizerect = (SDL_FRect){(float)limit((cursizerectinborder.x+(cursizerectinborder.w/2)-((canvas.w/resolution.x)/2*cursize)), cursizerectinborder.x, 100), (float)limit(cursizerectinborder.y-(cursizerectinborder.w/2)+((canvas.w/resolution.y)/2*cursize), std::nullopt, cursizerectinborder.y), (float)limit(canvas.w/resolution.x*cursize, std::nullopt, cursizerectinborder.w), -(float)limit(canvas.h/resolution.y*cursize, std::nullopt, -cursizerectinborder.h) };
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
                        Vector2 roundlastmouse = Vector2((float)(int)(((lastmouse.x-((canvas.w/resolution.x)*(cursize-1)/2))-canvas.x)/(canvas.w/resolution.x)), (float)(int)(((lastmouse.y-((canvas.h/resolution.y)*(cursize-1)/2))-canvas.y)/(canvas.h/resolution.y)));
                        Vector2 roundmouse = Vector2((float)(int)(((mouse.x-((canvas.w/resolution.x)*(cursize-1)/2))-canvas.x)/(canvas.w/resolution.x)), (float)(int)(((mouse.y-((canvas.h/resolution.y)*(cursize-1)/2))-canvas.y)/(canvas.h/resolution.y)));
                        Vector2 fakeroundmouse = roundmouse;
                        if (keystates[SDL_SCANCODE_LSHIFT]) fakeroundmouse = Vector2((float)(sin(round(atan2(roundmouse.x-roundlastmouse.x, roundmouse.y-roundlastmouse.y)/M_EIGHTHPI)*M_EIGHTHPI)*sqrt(((roundmouse.x-roundlastmouse.x)*(roundmouse.x-roundlastmouse.x))+((roundmouse.y-roundlastmouse.y)*(roundmouse.y-roundlastmouse.y))))+roundlastmouse.x, (float)(cos(round(atan2(roundmouse.x-roundlastmouse.x, roundmouse.y-roundlastmouse.y)/M_EIGHTHPI)*M_EIGHTHPI)*sqrt(((roundmouse.x-roundlastmouse.x)*(roundmouse.x-roundlastmouse.x))+((roundmouse.y-roundlastmouse.y)*(roundmouse.y-roundlastmouse.y))))+roundlastmouse.y);
                        for (int y = 0; y < cursize; y++) {
                            for (int x = 0; x < cursize; x++) {
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
                    if (keystates[SDL_MODKEY]) {
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


                    /* Delete */
                    if (e.key.key == SDLK_DELETE || e.key.key == SDLK_BACKSPACE){
                        Input = "/D";
                    }
                    elif (e.key.key == SDLK_LEFT){
                        Input = "/L";
                    }
                    elif (e.key.key == SDLK_RIGHT){
                        Input = "/R";
                    }
                    break;




                case SDL_EVENT_TEXT_INPUT:
                    Input = e.text.text;
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
                    for (int y = 0; y < cursize; y++) {
                        for (int x = 0; x < cursize; x++) {
                            SDL_RenderLine(renderer, (int)(((framelastmouse.x-((canvas.w/resolution.x)*(cursize-1)/2))-canvas.x)/(canvas.w/resolution.x))+x, (int)(((framelastmouse.y-((canvas.h/resolution.y)*(cursize-1)/2))-canvas.y)/(canvas.h/resolution.y))+y, (int)(((mouse.x-((canvas.w/resolution.x)*(cursize-1)/2))-canvas.x)/(canvas.w/resolution.x))+x, (int)(((mouse.y-((canvas.h/resolution.y)*(cursize-1)/2))-canvas.y)/(canvas.h/resolution.y))+y);
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
                    Vector2 roundlastmouse = Vector2((float)(int)(((lastmouse.x-((canvas.w/resolution.x)*(cursize-1)/2))-canvas.x)/(canvas.w/resolution.x)), (float)(int)(((lastmouse.y-((canvas.h/resolution.y)*(cursize-1)/2))-canvas.y)/(canvas.h/resolution.y)));
                        Vector2 roundmouse = Vector2((float)(int)(((mouse.x-((canvas.w/resolution.x)*(cursize-1)/2))-canvas.x)/(canvas.w/resolution.x)), (float)(int)(((mouse.y-((canvas.h/resolution.y)*(cursize-1)/2))-canvas.y)/(canvas.h/resolution.y)));
                    Vector2 fakeroundmouse = roundmouse;
                    if (keystates[SDL_SCANCODE_LSHIFT]) fakeroundmouse = Vector2((float)(sin(round(atan2(roundmouse.x-roundlastmouse.x, roundmouse.y-roundlastmouse.y)/M_EIGHTHPI)*M_EIGHTHPI)*sqrt(((roundmouse.x-roundlastmouse.x)*(roundmouse.x-roundlastmouse.x))+((roundmouse.y-roundlastmouse.y)*(roundmouse.y-roundlastmouse.y))))+roundlastmouse.x, (float)(cos(round(atan2(roundmouse.x-roundlastmouse.x, roundmouse.y-roundlastmouse.y)/M_EIGHTHPI)*M_EIGHTHPI)*sqrt(((roundmouse.x-roundlastmouse.x)*(roundmouse.x-roundlastmouse.x))+((roundmouse.y-roundlastmouse.y)*(roundmouse.y-roundlastmouse.y))))+roundlastmouse.y);
                    for (int y = 0; y < cursize; y++) {
                        for (int x = 0; x < cursize; x++) {
                            SDL_RenderLine(renderer, roundlastmouse.x+x, roundlastmouse.y+y, fakeroundmouse.x+x, fakeroundmouse.y+y );
                        }
                    }
                    SDL_SetRenderTarget(renderer, NULL);
                }


                /* Erase from framelastmouse to mouse */
                else if (currentool == 2) {
                    SDL_SetRenderTarget(renderer, sprite[frame]);
                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
                    for (int y = 0; y < cursize; y++) {
                        for (int x = 0; x < cursize; x++) {
                            SDL_RenderLine(renderer, (int)(((mouse.x-((canvas.w/resolution.x)*(cursize-1)/2))-canvas.x)/(canvas.w/resolution.x))+x, (int)(((mouse.y-((canvas.h/resolution.y)*(cursize-1)/2))-canvas.y)/(canvas.h/resolution.y))+y, (int)((mouse.x-canvas.x)/(canvas.w/resolution.x)), (int)((mouse.y-canvas.y)/(canvas.h/resolution.y)));
                        }
                    }
                    SDL_SetRenderTarget(renderer, NULL);
                }


                /* Draw line from framelastmouse to mouse mirrored */
                else if (currentool == 3) {
                    SDL_SetRenderTarget(renderer, sprite[frame]);
                    (mousebitmask & SDL_BUTTON_LMASK)?SDL_SetRenderDrawColor(renderer, leftcolor.r, leftcolor.g, leftcolor.b, leftcolor.a):SDL_SetRenderDrawColor(renderer, rightcolor.r, rightcolor.g, rightcolor.b, rightcolor.a);
                    for (int y = 0; y < cursize; y++) {
                        for (int x = 0; x < cursize; x++) {
                            SDL_RenderLine(renderer, (int)(((framelastmouse.x-((canvas.w/resolution.x)*(cursize-1)/2))-canvas.x)/(canvas.w/resolution.x))+x, (int)(((framelastmouse.y-((canvas.h/resolution.y)*(cursize-1)/2))-canvas.y)/(canvas.h/resolution.y))+y, (int)(((mouse.x-((canvas.w/resolution.x)*(cursize-1)/2))-canvas.x)/(canvas.w/resolution.x))+x, (int)(((mouse.y-((canvas.h/resolution.y)*(cursize-1)/2))-canvas.y)/(canvas.h/resolution.y))+y);
                        }
                    }
                    if (keystates[SDL_SCANCODE_LSHIFT]) {
                        for (int y = 0; y < cursize; y++) {
                            for (int x = 0; x < cursize; x++) {
                                SDL_RenderLine(renderer, (int)(((framelastmouse.x-((canvas.w/resolution.x)*(cursize-1)/2))-canvas.x)/(canvas.w/resolution.x))+x, -(int)(((framelastmouse.y-((canvas.h/resolution.y)*(cursize-1)/2))-canvas.y)/(canvas.h/resolution.y))+y+resolution.y-cursize, (int)(((mouse.x-((canvas.w/resolution.x)*(cursize-1)/2))-canvas.x)/(canvas.w/resolution.x))+x, -(int)(((mouse.y-((canvas.h/resolution.y)*(cursize-1)/2))-canvas.y)/(canvas.h/resolution.y))+y+resolution.y-cursize);
                            }
                        }
                        for (int y = 0; y < cursize; y++) {
                            for (int x = 0; x < cursize; x++) {
                                SDL_RenderLine(renderer, -(int)(((framelastmouse.x-((canvas.w/resolution.x)*(cursize-1)/2))-canvas.x)/(canvas.w/resolution.x))+x+resolution.x-cursize, -(int)(((framelastmouse.y-((canvas.h/resolution.y)*(cursize-1)/2))-canvas.y)/(canvas.h/resolution.y))+y+resolution.y-cursize, -(int)(((mouse.x-((canvas.w/resolution.x)*(cursize-1)/2))-canvas.x)/(canvas.w/resolution.x))+x+resolution.x-cursize, -(int)(((mouse.y-((canvas.h/resolution.y)*(cursize-1)/2))-canvas.y)/(canvas.h/resolution.y))+y+resolution.y-cursize);
                            }
                        }
                        for (int y = 0; y < cursize; y++) {
                            for (int x = 0; x < cursize; x++) {
                                SDL_RenderLine(renderer, -(int)(((framelastmouse.x-((canvas.w/resolution.x)*(cursize-1)/2))-canvas.x)/(canvas.w/resolution.x))+x+resolution.x-cursize, (int)(((framelastmouse.y-((canvas.h/resolution.y)*(cursize-1)/2))-canvas.y)/(canvas.h/resolution.y))+y, -(int)(((mouse.x-((canvas.w/resolution.x)*(cursize-1)/2))-canvas.x)/(canvas.w/resolution.x))+x+resolution.x-cursize, (int)(((mouse.y-((canvas.h/resolution.y)*(cursize-1)/2))-canvas.y)/(canvas.h/resolution.y))+y);
                            }
                        }
                    }
                    else if (keystates[SDL_SCANCODE_LCTRL]) {
                        for (int y = 0; y < cursize; y++) {
                            for (int x = 0; x < cursize; x++) {
                                SDL_RenderLine(renderer, (int)(((framelastmouse.x-((canvas.w/resolution.x)*(cursize-1)/2))-canvas.x)/(canvas.w/resolution.x))+x, -(int)(((framelastmouse.y-((canvas.h/resolution.y)*(cursize-1)/2))-canvas.y)/(canvas.h/resolution.y))+y+resolution.y-cursize, (int)(((mouse.x-((canvas.w/resolution.x)*(cursize-1)/2))-canvas.x)/(canvas.w/resolution.x))+x, -(int)(((mouse.y-((canvas.h/resolution.y)*(cursize-1)/2))-canvas.y)/(canvas.h/resolution.y))+y+resolution.y-cursize);
                            }
                        }
                    }
                    else {
                        for (int y = 0; y < cursize; y++) {
                            for (int x = 0; x < cursize; x++) {
                                SDL_RenderLine(renderer, -(int)(((framelastmouse.x-((canvas.w/resolution.x)*(cursize-1)/2))-canvas.x)/(canvas.w/resolution.x))+x+resolution.x-cursize, (int)(((framelastmouse.y-((canvas.h/resolution.y)*(cursize-1)/2))-canvas.y)/(canvas.h/resolution.y))+y, -(int)(((mouse.x-((canvas.w/resolution.x)*(cursize-1)/2))-canvas.x)/(canvas.w/resolution.x))+x+resolution.x-cursize, (int)(((mouse.y-((canvas.h/resolution.y)*(cursize-1)/2))-canvas.y)/(canvas.h/resolution.y))+y);
                            }
                        }
                    }
                    SDL_SetRenderTarget(renderer, NULL);
                }


                /* Draw dithered line from framelastmouse to mouse */
                else if (currentool == 4) {
                    SDL_SetRenderTarget(renderer, sprite[frame]);
                    for (int y = 0; y < cursize; y++) {
                        for (int x = 0; x < cursize; x++) {
                            ditherline(renderer, leftcolor, rightcolor, (vec2){(int)(((framelastmouse.x-((canvas.w/resolution.x)*(cursize-1)/2))-canvas.x)/(canvas.w/resolution.x))+x, (int)(((framelastmouse.y-((canvas.h/resolution.y)*(cursize-1)/2))-canvas.y)/(canvas.h/resolution.y))+y}, (vec2){(int)(((mouse.x-((canvas.w/resolution.x)*(cursize-1)/2))-canvas.x)/(canvas.w/resolution.x))+x, (int)(((mouse.y-((canvas.h/resolution.y)*(cursize-1)/2))-canvas.y)/(canvas.h/resolution.y))+y}, (mousebitmask & SDL_BUTTON_LMASK));
                        }
                    }
                    SDL_SetRenderTarget(renderer, NULL);
                }


                /* Increase or decrease a pixel's brightness */
                else if (currentool == 5) {
                        SDL_SetRenderTarget(renderer, sprite[frame]);
                        for (int y = 0; y < cursize; y++) {
                            for (int x = 0; x < cursize; x++) {
                                lightenline(renderer, (vec2){ (int)(((framelastmouse.x-((canvas.w/resolution.x)*(cursize-1)/2))-canvas.x)/(canvas.w/resolution.x))+x, (int)(((framelastmouse.y-((canvas.h/resolution.y)*(cursize-1)/2))-canvas.y)/(canvas.h/resolution.y))+y }, (vec2){ (int)(((mouse.x-((canvas.w/resolution.x)*(cursize-1)/2))-canvas.x)/(canvas.w/resolution.x))+x, (int)(((mouse.y-((canvas.h/resolution.y)*(cursize-1)/2))-canvas.y)/(canvas.h/resolution.y))+y }, (mousebitmask & SDL_BUTTON_RMASK), !keystates[SDL_SCANCODE_LSHIFT]);
                            }
                        }
                        SDL_SetRenderTarget(renderer, NULL);
                }


                /* Pick color */
                else if (currentool == 16) {
                    if (contained(mouse, canvas)) {
                        (mousebitmask & SDL_BUTTON_LMASK)?SDL_ReadSurfacePixel(prespritesurface, (mouse.x-canvas.x)/(canvas.w/resolution.x), (mouse.y-canvas.y)/(canvas.h/resolution.y), &leftcolor.r, &leftcolor.g, &leftcolor.b, &leftcolor.a):SDL_ReadSurfacePixel(prespritesurface, (mouse.x-canvas.x)/(canvas.w/resolution.x), (mouse.y-canvas.y)/(canvas.h/resolution.y), &rightcolor.r, &rightcolor.g, &rightcolor.b, &rightcolor.a);
                        if (mousebitmask & SDL_BUTTON_LMASK){
                            leftcoloralphapreview[0].color = (SDL_FColor){ (float)leftcolor.r/255, (float)leftcolor.g/255, (float)leftcolor.b/255, (float)leftcolor.a/255 };
                            leftcoloralphapreview[1].color = (SDL_FColor){ (float)leftcolor.r/255, (float)leftcolor.g/255, (float)leftcolor.b/255, (float)leftcolor.a/255 };
                            leftcoloralphapreview[2].color = (SDL_FColor){ (float)leftcolor.r/255, (float)leftcolor.g/255, (float)leftcolor.b/255, (float)leftcolor.a/255 };
                            leftcolorpreview[0].color = (SDL_FColor){ (float)leftcolor.r/255, (float)leftcolor.g/255, (float)leftcolor.b/255, 1 };
                            leftcolorpreview[1].color = (SDL_FColor){ (float)leftcolor.r/255, (float)leftcolor.g/255, (float)leftcolor.b/255, 1 };
                            leftcolorpreview[2].color = (SDL_FColor){ (float)leftcolor.r/255, (float)leftcolor.g/255, (float)leftcolor.b/255, 1 };
                        }
                        else{
                            rightcoloralphapreview[0].color = (SDL_FColor){ (float)rightcolor.r/255, (float)rightcolor.g/255, (float)rightcolor.b/255, (float)rightcolor.a/255 };
                            rightcoloralphapreview[1].color = (SDL_FColor){ (float)rightcolor.r/255, (float)rightcolor.g/255, (float)rightcolor.b/255, (float)rightcolor.a/255 };
                            rightcoloralphapreview[2].color = (SDL_FColor){ (float)rightcolor.r/255, (float)rightcolor.g/255, (float)rightcolor.b/255, (float)rightcolor.a/255 };
                            rightcolorpreview[0].color = (SDL_FColor){ (float)rightcolor.r/255, (float)rightcolor.g/255, (float)rightcolor.b/255, 1 };
                            rightcolorpreview[1].color = (SDL_FColor){ (float)rightcolor.r/255, (float)rightcolor.g/255, (float)rightcolor.b/255, 1 };
                            rightcolorpreview[2].color = (SDL_FColor){ (float)rightcolor.r/255, (float)rightcolor.g/255, (float)rightcolor.b/255, 1 };
                        }
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
            cursorturect = (SDL_FRect){(float)(int)((((mouse.x-canvas.x)/canvas.w)*resolution.x)-((float)(int)(cursize-1)/2)), (float)(int)((((mouse.y-canvas.y)/canvas.h)*resolution.y)-((float)(int)(cursize-1)/2)), (float)cursize, (float)cursize };
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
        CurSizeText.Render(renderer, Characters);
        Title.Render(renderer, Characters, deltime, mouse, mousebitmask & SDL_BUTTON_LMASK, oldmousedown, Input);


        /* Push render content */
        SDL_RenderPresent(renderer);


        /* Update input variables */
        framelastmouse = mouse;
        oldshift = keystates[SDL_SCANCODE_LSHIFT];
        oldmousedown = (mousebitmask & SDL_BUTTON_LMASK || mousebitmask & SDL_BUTTON_RMASK);
        oldleftmousedown = mousebitmask & SDL_BUTTON_LMASK;


        /* Wait if unfocussed */
        if (!focus) SDL_Delay(250);
    }


    /* Exit properly */
    SDL_StopTextInput(window);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0; }


#ifdef _WIN32
/* Windows window subsystem :( */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow){
    return main(__argc, __argv);
}
#endif

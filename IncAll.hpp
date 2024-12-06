#pragma once
#include <cstdlib>
#include <iostream>
#define _USE_MATH_DEFINES
#include <cmath>
#include <string>
#include <vector>
#include <optional>
#ifdef _WIN32
#include <Windows.h>
#endif
#include <algorithm>
#define SDL_MAIN_HANDLED
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "MoreMaths.hpp"
#include "Vector2.hpp"
#include "TextHelp.hpp"

#undef min
#undef max
#define elif else if
#ifdef _WIN32
#define SDL_MODKEY SDL_SCANCODE_LCTRL
#elifdef __APPLE__
#define SDL_MODKEY SDL_SCANCODE_LGUI
#endif

#include <iostream>

#include <SDL3/SDL.h>

int main() {
    std::cout << "Initializing SDL3" << std::endl;
    SDL_Init(SDL_INIT_EVENTS);

    return 0;
}

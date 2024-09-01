Build command (macos): cmake ../.. && make
To build:
- make a build directory in nogit
- have the SDL3 and freetype dylibs in RePiskel/../../Libraries/Cached (it's weird, but it works)
- have the SDL3 headers located in /opt/homebrew/include (I wish homebrew allowed you to install sdl3)

(CMake is nightmarish to work with, but it works when it does ig ¯\_(ツ)_/¯)

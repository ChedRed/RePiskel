cmake_minimum_required(VERSION 3.10)

project(RePiskel)

add_executable(repiskel main.cpp)

target_link_libraries(repiskel PRIVATE ../../Libraries/Cached/libSDL3.0.dylib)
target_link_libraries(repiskel PRIVATE ../../Libraries/Cached/libfreetype.6.dylib)
target_include_directories(repiskel PRIVATE /opt/homebrew/include)

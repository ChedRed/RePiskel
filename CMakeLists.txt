# Basic project requirements
cmake_minimum_required(VERSION 3.10)
project(RePiskel)


# Set CMake variables
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


# Main executable
add_executable(repiskel main.cpp)


# Set macos executable rpath and result path
set_target_properties(repiskel PROPERTIES INSTALL_RPATH "@executable_path/../Resources" BUILD_WITH_INSTALL_RPATH TRUE)
set_target_properties(repiskel PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/nogit/RePiskel.app/Contents/MacOS)


# Link libraries and include directories
target_link_libraries(repiskel PRIVATE ${CMAKE_SOURCE_DIR}/../Cached/libSDL3.0.dylib)
target_link_libraries(repiskel PRIVATE ${CMAKE_SOURCE_DIR}/../Cached/libSDL3_ttf.0.0.0.dylib)
# target_link_libraries(repiskel PRIVATE ${CMAKE_SOURCE_DIR}/../Cached/libgtk-4.1.dylib)
target_include_directories(repiskel PRIVATE /opt/homebrew/include)


# Make directories for macos app
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/nogit/RePiskel.app/Contents/MacOS)
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/nogit/RePiskel.app/Contents/Resources)


# Add basic files to macos app
file(COPY ${CMAKE_SOURCE_DIR}/buildfiles/info.plist DESTINATION ${CMAKE_SOURCE_DIR}/nogit/RePiskel.app/Contents)
file(COPY ${CMAKE_SOURCE_DIR}/buildfiles/icon.icns DESTINATION ${CMAKE_SOURCE_DIR}/nogit/RePiskel.app/Contents/Resources)
file(COPY ${CMAKE_SOURCE_DIR}/buildfiles/FreeSans.ttf DESTINATION ${CMAKE_SOURCE_DIR}/nogit/RePiskel.app/Contents/Resources)
file(COPY ${CMAKE_SOURCE_DIR}/buildfiles/tools.bmp DESTINATION ${CMAKE_SOURCE_DIR}/nogit/RePiskel.app/Contents/Resources)
file(COPY ${CMAKE_SOURCE_DIR}/buildfiles/border.bmp DESTINATION ${CMAKE_SOURCE_DIR}/nogit/RePiskel.app/Contents/Resources)
file(COPY ${CMAKE_SOURCE_DIR}/buildfiles/colorselector.bmp DESTINATION ${CMAKE_SOURCE_DIR}/nogit/RePiskel.app/Contents/Resources)


# Add resources to macos app
file(COPY ${CMAKE_SOURCE_DIR}/../Cached/libSDL3.0.dylib DESTINATION ${CMAKE_SOURCE_DIR}/nogit/RePiskel.app/contents/Resources)
configure_file(${CMAKE_SOURCE_DIR}/../Cached/libSDL3_ttf.0.0.0.dylib ${CMAKE_SOURCE_DIR}/nogit/RePiskel.app/contents/Resources/libSDL3_ttf.0.dylib COPYONLY)
# configure_file(${CMAKE_SOURCE_DIR}/../Cached/libgtk-4.1.dylib ${CMAKE_SOURCE_DIR}/nogit/RePiskel.app/contents/Resources/libgtk-4.1.dylib COPYONLY)

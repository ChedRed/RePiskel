# Basic project requirements
cmake_minimum_required(VERSION 3.30)
if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(CMAKE_OSX_SYSROOT macosx)
endif()
project(RePiskel)


# Set CMake variables
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# Main executable
string(APPEND CMAKE_CXX_FLAGS " -Wno-narrowing")
if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    add_executable(RePiskel MACOSX_BUNDLE src/main.cpp buildfiles/icon.icns)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    add_executable(RePiskel src/main.cpp buildfiles/wicon.rc) # WIN32
else(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    add_executable(RePiskel src/main.cpp)
endif()


if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    # Set macos executable rpath and result path
    set_target_properties(RePiskel PROPERTIES INSTALL_RPATH "@executable_path/../Resources" BUILD_WITH_INSTALL_RPATH TRUE)
    set_target_properties(RePiskel PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)


    # Link libraries and include directories
    find_package(SDL3 REQUIRED CONFIG COMPONENTS SDL3-shared)
    find_package(SDL3_ttf REQUIRED CONFIG COMPONENTS SDL3_ttf-shared)
    target_link_libraries(RePiskel PRIVATE SDL3::SDL3)
    target_link_libraries(RePiskel PRIVATE SDL3_ttf::SDL3_ttf)

    # Make directories for macos app
    file(MAKE_DIRECTORY "${CMAKE_SOURCE_DIR}/build/RePiskel.app/Contents/MacOS")
    file(COPY "${CMAKE_SOURCE_DIR}/buildfiles/Resources" DESTINATION "${CMAKE_SOURCE_DIR}/build/RePiskel.app/Contents")


    # Add basic files to macos app
    file(COPY ${CMAKE_SOURCE_DIR}/buildfiles/info.plist DESTINATION "${CMAKE_SOURCE_DIR}/build/RePiskel.app/Contents")
    file(COPY ${CMAKE_SOURCE_DIR}/buildfiles/icon.icns DESTINATION "${CMAKE_SOURCE_DIR}/build/RePiskel.app/Contents/Resources")


    # Add resources to macos app
    add_custom_command(TARGET CanvasView POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:SDL3::SDL3-shared> $<TARGET_FILE_DIR:CanvasView>/../Resources)
    add_custom_command(TARGET CanvasView POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:SDL3_ttf::SDL3_ttf-shared> $<TARGET_FILE_DIR:CanvasView>/../Resources)
    add_custom_command(TARGET CanvasView POST_BUILD COMMAND ${CMAKE_COMMAND} -E rename $<TARGET_FILE_DIR:CanvasView>/../Resources/libSDL3_ttf.0.0.0.dylib $<TARGET_FILE_DIR:CanvasView>/../Resources/libSDL3_ttf.0.dylib)


elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    # Set linux result path
    set_target_properties(RePiskel PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/RePiskel)
    set_target_properties(RePiskel PROPERTIES INSTALL_RPATH "$ORIGIN/Resources" BUILD_WITH_INSTALL_RPATH TRUE)
    find_package(SDL3 REQUIRED CONFIG COMPONENTS SDL3-shared)
    find_package(SDL3_ttf REQUIRED CONFIG COMPONENTS SDL3_ttf-shared)
    target_link_libraries(RePiskel PRIVATE SDL3::SDL3)
    target_link_libraries(RePiskel PRIVATE SDL3_ttf::SDL3_ttf)

    file(MAKE_DIRECTORY "${CMAKE_SOURCE_DIR}/build/RePiskel")
    file(COPY "${CMAKE_SOURCE_DIR}/buildfiles/Resources" DESTINATION "${CMAKE_SOURCE_DIR}/build/RePiskel")

    add_custom_command(TARGET RePiskel POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:SDL3::SDL3-shared> $<TARGET_FILE_DIR:RePiskel>/Resources)
    add_custom_command(TARGET RePiskel POST_BUILD COMMAND ${CMAKE_COMMAND} -E rename $<TARGET_FILE_DIR:RePiskel>/Resources/libSDL3.so.0.2.5 $<TARGET_FILE_DIR:RePiskel>/Resources/libSDL3.so.0)
    add_custom_command(TARGET RePiskel POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:SDL3_ttf::SDL3_ttf-shared> $<TARGET_FILE_DIR:RePiskel>/Resources)
    add_custom_command(TARGET RePiskel POST_BUILD COMMAND ${CMAKE_COMMAND} -E rename $<TARGET_FILE_DIR:RePiskel>/Resources/libSDL3_ttf.so.0.1.2 $<TARGET_FILE_DIR:RePiskel>/Resources/libSDL3_ttf.so.0)



else(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    # Set windows result path
    set_target_properties(RePiskel PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build/RePiskel")


    # Link libraries and include directories
    target_link_libraries(RePiskel PRIVATE C:/usr/local/lib/SDL3.lib)
    target_link_libraries(RePiskel PRIVATE C:/usr/local/lib/SDL3_ttf.lib)
    target_include_directories(RePiskel PRIVATE C:/usr/local/include)


    # Add basic files to windows app
    file(COPY ${CMAKE_SOURCE_DIR}/buildfiles/Resources DESTINATION "${CMAKE_SOURCE_DIR}/build/RePiskel")


    # Add resources to windows app
    file(COPY C:/usr/local/lib/SDL3.dll DESTINATION "${CMAKE_SOURCE_DIR}/build/RePiskel")
    file(COPY C:/usr/local/lib/SDL3_ttf.dll DESTINATION "${CMAKE_SOURCE_DIR}/build/RePiskel")
endif()

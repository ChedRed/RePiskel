#include <string>
#include <vector>
#include <SDL3/SDL.h>
#define elif else if

inline std::string SDL_GetBasePathNOS(){
    std::string data = SDL_GetBasePath();
    #ifdef _WIN32
    std::string returnv;
    for (int i = 0; i < data.length(); i++){
        if (data[i] == '\\'){
            returnv += "/";
        }
        else{
            returnv += data[i];
        }
    }
    return returnv;
    #else
    return data;
    #endif
}

inline std::string ParsePath(std::string path) {
    std::string returnv;


    for (int i = 0; i < path.length(); i++) {
        bool Append = true;
        if (path[i] == '\\') {
            returnv += "/";
            Append = false;
        } elif (path[i] == '~') {
            returnv += SDL_GetBasePathNOS();
            Append = false;
        } elif (i > 0) {
            if (std::string(path[i-1], path[i]) != "\\\\") {
                Append = false;
            }
        }
        returnv += path[i];
    }

    return returnv;
}


inline std::vector<std::string> SplitString(std::string str, char delimiter) {
    std::vector<std::string> returnv;
    std::string part;
    for (int i = 0; i < str.length(); i++) {
        if (str[i] == delimiter) {
            returnv.push_back(part);
            part.clear();
        } else {
            part += str[i];
        }
    }
    return returnv;
}

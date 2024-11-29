#pragma once
#include "IncAll.h"
#include "SDL3/SDL_render.h"
#include "Vector2.hpp"
#include <string>
#include <vector>

typedef int Alignment;
#define Left -1
#define Center 0
#define Right 1


// Character cache
class TextCharacters{
public:
TextCharacters(SDL_Renderer * renderer = nullptr, TTF_Font * font = nullptr, std::string characters = "");
SDL_Renderer * GetRenderer();
SDL_Texture * GetCharacter(std::string character);
float GetTotalLength(std::string characters);
private:
TTF_Font * Font;
std::string Charin;
SDL_Renderer * Renderer;
std::vector<SDL_Texture *> Characters;
};


inline TextCharacters::TextCharacters(SDL_Renderer * renderer, TTF_Font * font, std::string characters){
    SDL_Surface * tempsurf;
    Font = font;
    Renderer = renderer;
    Charin = characters;

    for (int i = 0; i < characters.length(); i++){
        tempsurf = TTF_RenderText_Blended(Font, (std::string() + characters[i]).c_str(), 1, {255, 255, 255, 255});
        Characters.push_back(SDL_CreateTextureFromSurface(Renderer, tempsurf));
        SDL_DestroySurface(tempsurf);
    }
}


inline SDL_Texture * TextCharacters::GetCharacter(std::string character){
    return Characters[Charin.find(character)];
}


inline float TextCharacters::GetTotalLength(std::string characters){
    float value = 0;
    float addvalue = 0;
    for (int i = 0; i < characters.length(); i++){
        SDL_GetTextureSize(Characters[Charin.find(characters[i])], &addvalue, nullptr);
    }
    value += addvalue;
    return value;
}


inline SDL_Renderer * TextCharacters::GetRenderer(){
    return Renderer;
}


// Actual object to render text
class TextObject{
public:
TextObject(const char * text, Alignment align, Vector2 position);
void Render(SDL_Renderer * renderer, TextCharacters Characters);
void Destroy();
std::string Text;
Alignment Align;
Vector2 Position;
};


inline TextObject::TextObject(const char * text, Alignment align, Vector2 position){
    Text = text;
    Align = align;
    Position = position;
}


inline void TextObject::Render(SDL_Renderer * renderer, TextCharacters Characters){
    float offset = Position.x;
    SDL_FRect charect = {Position.x, Position.y, 0, 0};
    SDL_Texture * curchar;
    if (Align != Left){
        offset -= Characters.GetTotalLength(Text) * ((float)(Align+1)/2);
    }
    for (int i = 0; i < Text.length(); i++){
        curchar = Characters.GetCharacter(std::string() + Text[i]);
        SDL_GetTextureSize(curchar, &charect.w, &charect.h);
        charect.x = offset;
        offset += charect.w;
        SDL_RenderTexture(renderer, curchar, NULL, &charect);
    }
}

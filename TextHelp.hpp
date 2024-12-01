#pragma once
#include "MoreMaths.hpp"
#include "SDL3/SDL_blendmode.h"
#include "SDL3/SDL_keyboard.h"
#include "SDL3/SDL_render.h"
#include "Vector2.hpp"
#include <cmath>
#include <string>
#include <vector>

typedef int Alignment;
#define Left -1
#define Center 0
#define Right 1


// Character cache
class TextCharacters{
public:
TextCharacters(SDL_Renderer * renderer, TTF_Font * font, std::string characters);
SDL_Renderer * GetRenderer();
SDL_Texture * GetCharacter(std::string character);
float GetTotalLength(std::string characters);
float GetMaxHeight(std::string characters);
TTF_Font * GetFont();
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
        value += addvalue;
    }
    return value;
}


inline float TextCharacters::GetMaxHeight(std::string characters){
    float value = 0;
    float highest = 0;
    for (int i = 0; i < characters.length(); i++){
        SDL_GetTextureSize(Characters[Charin.find(characters[i])], nullptr, &value);
        highest = (value>highest)?value:highest;
    }
    return highest;
}


inline SDL_Renderer * TextCharacters::GetRenderer(){
    return Renderer;
}


inline TTF_Font * TextCharacters::GetFont(){
    return Font;
}


// Actual object to render text
class TextObject{
public:
TextObject(const char * text, Alignment align, Vector2 position, bool editable);
void Render(SDL_Renderer * renderer, TextCharacters Characters, float DeltaTime = 0, Vector2 CursorPosition = {0, 0}, bool LeftMouseState = false, bool OldLeftMouseState = false, std::string InputChars = "");
void Destroy();
bool Editable;
std::string Text;
Alignment Align;
Vector2 Position;
private:
bool Editing;
bool LastEditing;
int FirstCursorPos;
int SecondCursorPos;
Vector2 InitialCursorPos;
float FirstCursorWindowPos;
float SecondCursorWindowPos;
float Incrementime;
float Incrementpart;
Vector2 SelectionRange;
};


inline TextObject::TextObject(const char * text, Alignment align, Vector2 position, bool editable){
    Text = text;
    Align = align;
    Position = position;
    Editable = editable;
}


inline void TextObject::Render(SDL_Renderer * renderer, TextCharacters Characters, float DeltaTime, Vector2 CursorPosition, bool LeftMouseState, bool OldLeftMouseState, std::string InputChars){
    Incrementime += DeltaTime;
    std::modf(Incrementime, &Incrementpart);
    SDL_FRect charect = {Position.x, Position.y - (Characters.GetMaxHeight(Text)/2), 0, 0};
    SDL_Texture * curchar;


    if (Align != Left){
        charect.x -= Characters.GetTotalLength(Text) * (((float)(Align+1))/2);
    }
    InitialCursorPos = (LeftMouseState && !OldLeftMouseState)?CursorPosition:InitialCursorPos;
    Editing = contained(InitialCursorPos, {charect.x-8, charect.y, Characters.GetTotalLength(Text)+16, Characters.GetMaxHeight(Text)}) && Editable;
    for (int i = 0; i < Text.length(); i++){
        curchar = Characters.GetCharacter(std::string() + Text[i]);
        SDL_GetTextureSize(curchar, &charect.w, &charect.h);
        if (LeftMouseState){
            if (contained(CursorPosition, charect)){
                if (OldLeftMouseState){
                    SecondCursorPos = i+(CursorPosition.x-charect.x > charect.x+charect.w-CursorPosition.x);
                }
                else{
                    FirstCursorPos = i+(CursorPosition.x-charect.x > charect.x+charect.w-CursorPosition.x);
                }
            }
            elif ((CursorPosition.x < charect.x || CursorPosition.y < charect.y) && OldLeftMouseState){
                if (i == 0) SecondCursorPos = 0;
            }
            elif ((CursorPosition.x > charect.x + charect.w || CursorPosition.y > charect.y + charect.h) && OldLeftMouseState){
                if (i == Text.length()-1) SecondCursorPos = i+1;
            }
        }
        if (Editing){
            if (FirstCursorPos == i){
                FirstCursorWindowPos = charect.x;
            }
            elif (FirstCursorPos == i+1){
                FirstCursorWindowPos = charect.x+charect.w;
            }
            if (SecondCursorPos == i){
                SecondCursorWindowPos = charect.x;
            }
            elif (SecondCursorPos == i+1){;
                SecondCursorWindowPos = charect.x+charect.w;
            }
        }
        charect.x += charect.w;
    }



    if (Editing && LastEditing){
        SDL_SetRenderDrawColor(renderer, 20, 20, 50, 255);
        SelectionRange = Vector2(FirstCursorPos, SecondCursorPos);
        SDL_FRect SelectionBox = {FirstCursorWindowPos, charect.y, SecondCursorWindowPos-FirstCursorWindowPos, Characters.GetMaxHeight(Text)};
        SDL_RenderFillRect(renderer, &SelectionBox);
        SDL_SetRenderDrawColorFloat(renderer, 1, 1, 1, Incrementpart);
        SDL_RenderLine(renderer, SecondCursorWindowPos, charect.y, SecondCursorWindowPos, Characters.GetMaxHeight(Text)+1);
    }
    charect.x = Position.x - Characters.GetTotalLength(Text) * (((float)(Align+1))/2);
    for (int i = 0; i < Text.length(); i++){
        curchar = Characters.GetCharacter(std::string() + Text[i]);
        SDL_GetTextureSize(curchar, &charect.w, &charect.h);
        SDL_RenderTexture(renderer, curchar, NULL, &charect);
        charect.x += charect.w;
    }
    LastEditing = Editing;
}

#pragma once
#include "MoreMaths.hpp"
#include "SDL3/SDL_blendmode.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_video.h"
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


    if (InputChars == ""){
        InitialCursorPos = (LeftMouseState && !OldLeftMouseState)?CursorPosition:InitialCursorPos;
        Editing = contained(InitialCursorPos, {charect.x-8, charect.y, Characters.GetTotalLength(Text)+16, Characters.GetMaxHeight(Text)}) && Editable;
        for (int i = 0; i < Text.length(); i++){
            curchar = Characters.GetCharacter(std::string() + Text[i]);
            SDL_GetTextureSize(curchar, &charect.w, &charect.h);
            if (LeftMouseState){
                if (contained(CursorPosition, charect)){
                    if (!OldLeftMouseState){
                        FirstCursorPos = i+(CursorPosition.x-charect.x > charect.x+charect.w-CursorPosition.x);
                    }
                        SecondCursorPos = i+(CursorPosition.x-charect.x > charect.x+charect.w-CursorPosition.x);
                }
                elif (CursorPosition.x < charect.x || CursorPosition.y < charect.y){
                    if (i == 0){
                        if (!OldLeftMouseState) FirstCursorPos = 0;
                        SecondCursorPos = 0;
                    }
                }
                elif ((CursorPosition.x > charect.x + charect.w || CursorPosition.y > charect.y + charect.h)){
                    if (i == Text.length()-1){
                        if (!OldLeftMouseState) FirstCursorPos = i+1;
                        SecondCursorPos = i+1;
                    }
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
                elif (SecondCursorPos == i+1){
                    SecondCursorWindowPos = charect.x+charect.w;
                }
            }
            charect.x += charect.w - ((std::string() + Text[i]=="k" && std::string() + Text[i+1]=="e")?1:0);
        }



        if (Editing && LastEditing){
            SelectionRange = Vector2(FirstCursorPos, SecondCursorPos);
            SDL_SetRenderDrawColor(renderer, 20, 20, 50, 255);
            SDL_FRect SelectionBox = {FirstCursorWindowPos, charect.y+2, SecondCursorWindowPos-FirstCursorWindowPos, Characters.GetMaxHeight(Text)-4};
            SDL_RenderFillRect(renderer, &SelectionBox);
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColorFloat(renderer, 1, 1, 1, Incrementpart);
            SDL_RenderLine(renderer, SecondCursorWindowPos, charect.y+2, SecondCursorWindowPos, Characters.GetMaxHeight(Text)-1);
        }
        charect.x = Position.x - Characters.GetTotalLength(Text) * (((float)(Align+1))/2);
    }
    else{
        if (Editing){
            std::string NewText = "";
            int LowerPos = (FirstCursorPos<SecondCursorPos)?FirstCursorPos:SecondCursorPos;
            int HigherPos = (FirstCursorPos>SecondCursorPos)?FirstCursorPos:SecondCursorPos;
            int NewPos = 0;
            for (int i = 0; i < Text.length(); i++){
                if (i < LowerPos || i >= HigherPos){
                    NewText += Text[i];
                    if (i+1 == LowerPos && i+1 == HigherPos){
                        NewText += InputChars;
                        NewPos = i+2;
                    }
                }
                elif (i == LowerPos){
                    NewText += InputChars;
                    NewPos = i+1;
                }
            }
            FirstCursorPos = NewPos;
            SecondCursorPos = NewPos;
            Text = NewText;
            Editing = false;
        }
        if (Align != Left){
            charect.x = Position.x - (Characters.GetTotalLength(Text) * (((float)(Align+1))/2));
        }
    }
    for (int i = 0; i < Text.length(); i++){
        curchar = Characters.GetCharacter(std::string() + Text[i]);
        SDL_GetTextureSize(curchar, &charect.w, &charect.h);
        SDL_RenderTexture(renderer, curchar, NULL, &charect);
        charect.x += charect.w - ((std::string() + Text[i]=="k" && std::string() + Text[i+1]=="e")?1:0);
    }
    LastEditing = Editing;
}

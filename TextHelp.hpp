#pragma once
#include "MoreMaths.hpp"
#include "SDL3/SDL_blendmode.h"
#include "SDL3/SDL_render.h"
#include "Vector2.hpp"

typedef int Alignment;
#define Left 0
#define Center 1
#define Right 2


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
void Render(SDL_Renderer * renderer, TextCharacters Characters);
void ShiftCursor(bool Shift, bool MoveLeft);
void Edit(std::string InputChars);
void Edit();
void Destroy();
bool CheckSelected(Vector2 CursorPosition, TextCharacters Characters);
bool Editable;
bool Selected;
std::string Text;
Alignment Align;
Vector2 Position;
private:
int Cursor;
int Selection;
};


inline TextObject::TextObject(const char * text, Alignment align, Vector2 position, bool editable){
    Text = text;
    Align = align;
    Position = position;
    Editable = editable;
    Cursor = -1;
    Selection = 0;
}


inline bool TextObject::CheckSelected(Vector2 CursorPosition, TextCharacters Characters){
    if (contained(CursorPosition, {Position.x - (Characters.GetTotalLength(Text)*((float)Align/2))-8, Position.y - (Characters.GetMaxHeight(Text)/2), Characters.GetTotalLength(Text)+16, Characters.GetMaxHeight(Text)})){
        Selected = true;
        float distance = Characters.GetTotalLength(Text);
        for (int i = 0; i < Text.length()+1; i++){
            if (CursorPosition.x - (Position.x - (Characters.GetTotalLength(Text)*((float)Align/2)) + Characters.GetTotalLength(slice(Text, 0, i))) > 0){
                distance = abs(CursorPosition.x - (Position.x - (Characters.GetTotalLength(Text)*((float)Align/2)) + Characters.GetTotalLength(slice(Text, 0, i))));
            }
            else{
                Cursor = i-(distance < abs(CursorPosition.x - (Position.x - (Characters.GetTotalLength(Text)*((float)Align/2)) + Characters.GetTotalLength(slice(Text, 0, i)))));
                Selection = 0;
                break;
            }
            if (i == Text.length()-1) Cursor = i+1;
        }
        return true;
    }
    Selected = false;
    return false;
}


inline void ContSelecText(Vector2 CursorPosition){

}


inline void TextObject::ShiftCursor(bool Shift, bool MoveLeft){
    if (Selected){
        if (Shift){
            Selection -= (MoveLeft-0.5)*2;
            Selection = limit(Selection, -Cursor, Text.length()-Cursor);
        }
        else{
            Cursor -= (MoveLeft-0.5)*2 - Selection;
            Selection = 0;
        }
    }
    Cursor = limit(Cursor, 0, Text.length());
}


inline void TextObject::Edit(std::string InputChars){

}


inline void TextObject::Edit(){

}


inline void TextObject::Render(SDL_Renderer * renderer, TextCharacters Characters){
    SDL_Texture * curchar;
    SDL_FRect charect;
    charect.x = Position.x - (Characters.GetTotalLength(Text)*((float)Align/2));
    charect.y = Position.y - Characters.GetMaxHeight(Text)/2;
    if (Selected){
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        if (Selection != 0){
            SDL_SetRenderDrawColor(renderer, 50, 50, 128, 128);
            SDL_FRect SelecRect = {charect.x+Characters.GetTotalLength(slice(Text, 0, Cursor + Selection)), charect.y, ((Selection>0)?-1:1)*Characters.GetTotalLength(slice(Text, ((Selection<0)?Cursor+Selection:Cursor), ((Selection>0)?Cursor+Selection:Cursor))), Characters.GetMaxHeight(Text)};
            SDL_RenderFillRect(renderer, &SelecRect);
        }

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderLine(renderer, charect.x+Characters.GetTotalLength(slice(Text, 0, Cursor + Selection)), charect.y, charect.x+Characters.GetTotalLength(slice(Text, 0, Cursor + Selection)), charect.y+Characters.GetMaxHeight(Text)-1);
    }
    else {
        Cursor = -1;
        Selection = 0;
    }
    for (int i = 0; i < Text.length(); i++){
        curchar = Characters.GetCharacter(std::string() + Text[i]);
        SDL_GetTextureSize(curchar, &charect.w, &charect.h);
        SDL_RenderTexture(renderer, curchar, NULL, &charect);
        charect.x += charect.w - ((std::string() + Text[i]=="k" && std::string() + Text[i+1]=="e")?1:0);
    }
}

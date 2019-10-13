#ifndef TEXT_H
#define TEXT_H
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL.h>
typedef enum
{
    TEXT_ANCHOR_TOP 	= 1<<0,
    TEXT_ANCHOR_BOTTOM 	= 1<<1,
    TEXT_ANCHOR_LEFT 	= 1<<2,
    TEXT_ANCHOR_RIGHT 	= 1<<3,
    TEXT_ANCHOR_V_CENTER = 1<<4,
    TEXT_ANCHOR_H_CENTER = 1<<5
} TextAnchor;

SDL_Texture**
TextRenderer_CreateCharset(SDL_Renderer* renderer, TTF_Font* font, SDL_Color* color);

void
TextRenderer_DestroyCharset(SDL_Texture** charset);

void
TextRenderer_SetCharset(SDL_Texture** chaset);

void
TextRenderer_Draw(SDL_Renderer* renderer, const char* text, int x, int y, int anchor);

#endif // TEXT_H

#include "textrenderer.h"
SDL_Texture** _Charset = NULL;
SDL_Texture **TextRenderer_CreateCharset(SDL_Renderer *renderer, TTF_Font *font, SDL_Color* color)
{
    SDL_Texture** charset = (SDL_Texture**) SDL_malloc(sizeof (SDL_Texture*) * 127);
    if(!charset)
    {
        SDL_SetError("Out of memory");
        return NULL;
    }
    SDL_memset(charset, 0, sizeof (SDL_Texture*));
    char str[2];
    SDL_Surface* surface = NULL;
    SDL_bool error = SDL_FALSE;
    for(char c = 1; (c < 127) && (!error); c++)
    {
        str[0] = c;
        str[1] = '\0';
        surface = TTF_RenderText_Solid(font, str, *color);
        if(!surface)
        {
            error = SDL_TRUE;
        }
        charset[(int) c] = SDL_CreateTextureFromSurface(renderer, surface);
        if(!charset[(int) c])
        {
            error = SDL_TRUE;
        }
    }
    if(error)
    {
        TextRenderer_DestroyCharset(charset);
        return NULL;
    }
    return charset;
}

void TextRenderer_DestroyCharset(SDL_Texture **charset)
{
    if(!charset)
        return;
    for(int i = 1; i < 127; i++)
    {
        if(charset[i])
            SDL_DestroyTexture(charset[i]);
    }
    SDL_free(charset);
}

void TextRenderer_SetCharset(SDL_Texture **chaset)
{
    _Charset = chaset;

}

void TextRenderer_Draw(SDL_Renderer *renderer, const char *text, int x, int y, int anchor)
{
    int len = strlen(text);
    int charw;
    int charh;
    SDL_Rect dstrect;
    int textWidth = 0;
    int textHeight;
    for(int i = 0; i < len; i++)
    {
        SDL_QueryTexture(_Charset[(int) text[i]], NULL, NULL, &charw, &textHeight);
        textWidth+=charw;
    }

    if((anchor & TEXT_ANCHOR_TOP) && (anchor & TEXT_ANCHOR_LEFT))
    {

        for(int i = 0; i < len; i++)
        {
            SDL_QueryTexture(_Charset[(int) text[i]], NULL, NULL, &charw, &charh);
            dstrect.w = charw;
            dstrect.h = charh;
            dstrect.x = x;
            dstrect.y = y;
            x += charw;
            SDL_RenderCopy(renderer, _Charset[(int) text[i]], NULL, &dstrect);
        }
    }
    else if((anchor & TEXT_ANCHOR_TOP) && (anchor & TEXT_ANCHOR_RIGHT))
    {
        for(int i = 0; i < len; i++)
        {
            SDL_QueryTexture(_Charset[(int) text[i]], NULL, NULL, &charw, &charh);
            dstrect.w = charw;
            dstrect.h = charh;
            dstrect.x = x - textWidth;
            dstrect.y = y;
            x += charw;
            SDL_RenderCopy(renderer, _Charset[(int) text[i]], NULL, &dstrect);
        }

    }
    else if((anchor & TEXT_ANCHOR_TOP) && (anchor & TEXT_ANCHOR_H_CENTER))
    {
        for(int i = 0; i < len; i++)
        {
            SDL_QueryTexture(_Charset[(int) text[i]], NULL, NULL, &charw, &charh);
            dstrect.w = charw;
            dstrect.h = charh;
            dstrect.x = x - textWidth / 2;
            dstrect.y = y;
            x += charw;
            SDL_RenderCopy(renderer, _Charset[(int) text[i]], NULL, &dstrect);
        }
    }
    else if((anchor & TEXT_ANCHOR_BOTTOM) && (anchor & TEXT_ANCHOR_LEFT))
    {

        for(int i = 0; i < len; i++)
        {
            SDL_QueryTexture(_Charset[(int) text[i]], NULL, NULL, &charw, &charh);
            dstrect.w = charw;
            dstrect.h = charh;
            dstrect.x = x;
            dstrect.y = y - textHeight;
            x += charw;
            SDL_RenderCopy(renderer, _Charset[(int) text[i]], NULL, &dstrect);
        }
    }
    else if((anchor & TEXT_ANCHOR_BOTTOM) && (anchor & TEXT_ANCHOR_RIGHT))
    {
        for(int i = 0; i < len; i++)
        {
            SDL_QueryTexture(_Charset[(int) text[i]], NULL, NULL, &charw, &charh);
            dstrect.w = charw;
            dstrect.h = charh;
            dstrect.x = x - textWidth;
            dstrect.y = y - textHeight;
            x += charw;
            SDL_RenderCopy(renderer, _Charset[(int) text[i]], NULL, &dstrect);
        }

    }
    else if((anchor & TEXT_ANCHOR_BOTTOM) && (anchor & TEXT_ANCHOR_H_CENTER))
    {
        for(int i = 0; i < len; i++)
        {
            SDL_QueryTexture(_Charset[(int) text[i]], NULL, NULL, &charw, &charh);
            dstrect.w = charw;
            dstrect.h = charh;
            dstrect.x = x - textWidth / 2;
            dstrect.y = y - textHeight;
            x += charw;
            SDL_RenderCopy(renderer, _Charset[(int) text[i]], NULL, &dstrect);
        }
    }
    else if((anchor & TEXT_ANCHOR_V_CENTER) && (anchor & TEXT_ANCHOR_LEFT))
    {

        for(int i = 0; i < len; i++)
        {
            SDL_QueryTexture(_Charset[(int) text[i]], NULL, NULL, &charw, &charh);
            dstrect.w = charw;
            dstrect.h = charh;
            dstrect.x = x;
            dstrect.y = y - textHeight / 2;
            x += charw;
            SDL_RenderCopy(renderer, _Charset[(int) text[i]], NULL, &dstrect);
        }
    }
    else if((anchor & TEXT_ANCHOR_V_CENTER) && (anchor & TEXT_ANCHOR_RIGHT))
    {
        for(int i = 0; i < len; i++)
        {
            SDL_QueryTexture(_Charset[(int) text[i]], NULL, NULL, &charw, &charh);
            dstrect.w = charw;
            dstrect.h = charh;
            dstrect.x = x - textWidth;
            dstrect.y = y - textHeight / 2;
            x += charw;
            SDL_RenderCopy(renderer, _Charset[(int) text[i]], NULL, &dstrect);
        }

    }
    else if((anchor & TEXT_ANCHOR_V_CENTER) && (anchor & TEXT_ANCHOR_H_CENTER))
    {
        for(int i = 0; i < len; i++)
        {
            SDL_QueryTexture(_Charset[(int) text[i]], NULL, NULL, &charw, &charh);
            dstrect.w = charw;
            dstrect.h = charh;
            dstrect.x = x - textWidth / 2;
            dstrect.y = y - textHeight / 2;
            x += charw;
            SDL_RenderCopy(renderer, _Charset[(int) text[i]], NULL, &dstrect);
        }
    }

}

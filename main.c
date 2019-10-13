
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdlib.h>
#include "textrenderer.h"
#define SNAKE_DELAY 		80
#define WIN_WIDTH 			800
#define WIN_HEIGHT 			800
#define GAME_BOARD_WIDTH 	10
#define GAME_ROARD_HEIGHT 	10
#define BLOCK_SIZE			WIN_WIDTH / GAME_BOARD_WIDTH
#define SPRITE_SIZE 		64
#define FOOD_SPRITE_ROW 	3
#define FOOD_SPRITE_COL		0
#define GAME_OVER_SCREEN_TIME 3000
#define SPRITE_SHEET_WIDTH 5
#define FONT_SIZE 30
enum Diretion {
    DIRECTION_LEFT, DIRECTION_RIGHT, DIRECTION_UP, DIRECTION_DOWN
} static g_snake_direction[100], g_input_direction;
static const int DX[4] = {-1, +1, 0, 0};
static const int DY[4] = {0, 0,-1, +1};
static const double ROTATION[4] = {180.0, 0.0, 270.0, 90.0};
typedef enum {
    SPRITE_HEAD, SPRITE_SATRAIGHT, SPRITE_CURVED,SPRITE_TAIL, SPRITE_FOOD
} Sprite;

typedef enum {
    SCREEN_TITLE, SCREEN_IN_GAME, SCREEN_GAME_OVER
}  Screen;

static const int SPRITE_MAPING[] = {4, 1, 0, 14, 15};
static SDL_bool g_is_game_over = SDL_FALSE;
static unsigned int g_snake_last_update_tick;
static int g_food_x;
static int g_food_y;
static int g_snake_x[100];
static int g_snake_y[100];
static int g_snake_length = 3;
static SDL_Window* g_window = NULL;
static SDL_Renderer* g_renderer = NULL;
static SDL_bool g_is_game_running = SDL_TRUE;
static SDL_Texture* g_sprite_sheet = NULL;
static unsigned int g_game_over_screen_last_update_tick;
static SDL_Texture* g_game_over_message = NULL;
static SDL_Texture* g_digits[10];
static SDL_Texture* g_background = NULL;
static SDL_Texture* g_dever_logo;
static SDL_Texture** g_game_over_charset;
static TTF_Font* g_font;
static int g_score = 0;
static int g_scr_state;
static Screen g_screen;

SDL_bool init(void);
void loop(void);
void render(void);
void quit(void);
void draw_in_game_screen(void);
void draw_game_over_screen(void);
void update_in_game_screen(void);
void reset_game(void);
void reset_food_position(void);
void reset_snake(void);

void draw_sprite_old(int row, int col, int x, int y);

void draw_sprite(Sprite sprite, SDL_Rect* dstRect);

void draw_sprite_ex(Sprite sprite, SDL_Rect* dstRect, double angle, SDL_RendererFlip flip);

void set_score(int score);

void add_score(int score);

void change_screen(Screen scr);

void draw_title_screen(void);


int main(int argc, char** argv)
{

    if (init())
    {
        reset_game();
        loop();
    }
    quit();
    return 0;
}
void reset_snake() {
    g_snake_x[0] = 2; g_snake_y[0] = 0;
    g_snake_x[1] = 1; g_snake_y[1] = 0;
    g_snake_x[2] = 0; g_snake_y[2] = 0;
    g_snake_length = 3;
    g_snake_direction[0] = g_snake_direction[1] = g_snake_direction[2] = DIRECTION_RIGHT;
    g_input_direction = DIRECTION_RIGHT;
    g_snake_last_update_tick = SDL_GetTicks();
}
SDL_bool init()
{

    //use SDL_log instead of SDL_LogError function
    if (SDL_Init(SDL_INIT_EVERYTHING) == -1)
    {
        SDL_Log( "Could not init sdl: %s", SDL_GetError());
        return SDL_FALSE;
    }

    int flags = IMG_INIT_PNG | IMG_INIT_JPG;
    if(IMG_Init(flags) != flags)
    {
        SDL_Log( "Could not init sdl_image: %s", IMG_GetError());
        return SDL_FALSE;
    }

    if(TTF_Init() == -1)
    {
        SDL_Log("Could not init sdl_ttf: %s", TTF_GetError());
        return SDL_FALSE;
    }



    g_window = SDL_CreateWindow("Snake", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIN_WIDTH, WIN_HEIGHT, SDL_WINDOW_SHOWN);

    if (!g_window)
    {
        SDL_Log( "Could not create window: %s", SDL_GetError());
        return SDL_FALSE;
    }

    g_renderer = SDL_CreateRenderer(g_window, -1, SDL_RENDERER_ACCELERATED);

    if (!g_renderer)
    {
        SDL_Log( "Could not create renderer: %s", SDL_GetError());
        return SDL_FALSE;

    }

    SDL_Surface* surface = IMG_Load("./snake-graphics.png");

    if(!surface)
    {
        SDL_Log("Could not load sprite sheet: %s", SDL_GetError());
        return SDL_FALSE;
    }

    g_sprite_sheet = SDL_CreateTextureFromSurface(g_renderer, surface);

    if(!g_sprite_sheet)
    {
        SDL_Log("Could not create texture: %s", SDL_GetError());
        return SDL_FALSE;
    }

    SDL_FreeSurface(surface);

    g_font = TTF_OpenFont("./font.ttf", FONT_SIZE);
    if(!g_font)
    {
        SDL_Log("Could not open font: %s", TTF_GetError());
        return SDL_FALSE;

    }

    SDL_Color white_color = {.r = 255, .g = 255, .b = 255, .a = 255};
    SDL_Color red_color = {.r = 255, .g = 0, .b = 0, .a = 255};

    for(int i = 0; i < 10; i++)
    {
        char buff[] = "";

        surface = TTF_RenderText_Solid(g_font, buff, white_color);
        g_digits[i] = SDL_CreateTextureFromSurface(g_renderer, surface);
        SDL_FreeSurface(surface);
    }

    surface = TTF_RenderText_Solid(g_font, "GAME OVER", red_color);
    g_game_over_message = SDL_CreateTextureFromSurface(g_renderer, surface);
    SDL_FreeSurface(surface);


    surface = IMG_Load("./background.png");

    if(!surface)
    {
        SDL_Log("Could not load background: %s", SDL_GetError());
        return SDL_FALSE;
    }

    g_background = SDL_CreateTextureFromSurface(g_renderer, surface);

    TTF_Font* font = TTF_OpenFont("./font.ttf", 15);

    SDL_Color c = {255, 255, 255, 255};

    g_game_over_charset = TextRenderer_CreateCharset(g_renderer, font, &c);

    surface = IMG_Load("./dever_logo.png");

    if(!surface)
    {
        SDL_Log("%s", SDL_GetError());
        return SDL_FALSE;
    }

    g_dever_logo = SDL_CreateTextureFromSurface(g_renderer, surface);

    SDL_FreeSurface(surface);
    change_screen(SCREEN_IN_GAME);

    srand(SDL_GetTicks());

    return SDL_TRUE;
}

void reset_food_position() {
    SDL_bool valid;
    do{
        valid = SDL_TRUE;
        g_food_x = rand() % GAME_BOARD_WIDTH;
        g_food_y = rand() % GAME_ROARD_HEIGHT;
        for(int i = 0; i < g_snake_length; ++i)
        {
            if((g_food_x == g_snake_x[i]) && (g_food_y == g_snake_y[i]))
            {
                valid = SDL_FALSE;
                break;
            }
        }
    }while(!valid);
}
void quit()
{
    for(int i = 0; i < 10; i++)
    {
        SDL_DestroyTexture(g_digits[i]);
    }
    TextRenderer_DestroyCharset(g_game_over_charset);
    SDL_DestroyTexture(g_game_over_message);
    TTF_CloseFont(g_font);
    SDL_DestroyTexture(g_sprite_sheet);
    SDL_DestroyRenderer(g_renderer);
    SDL_DestroyWindow(g_window);
    SDL_Quit();
    IMG_Quit();
    TTF_Quit();
}

void render()
{
    switch (g_screen) {
    case SCREEN_TITLE:
        break;
    case SCREEN_IN_GAME:
        draw_in_game_screen();
        break;
    case SCREEN_GAME_OVER:
        draw_game_over_screen();
        break;
    }
}

void update_in_game_screen() {
    //change the snake's moving direction
    if(g_scr_state == 0)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if(event.type == SDL_QUIT)
                g_is_game_running = SDL_FALSE;
            else if(event.type == SDL_KEYDOWN)
                g_scr_state = 1;
        }
    }
    else if(g_scr_state == 1)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if(event.type == SDL_QUIT)
                g_is_game_running = SDL_FALSE;
        }
        const uint8_t* keystates = SDL_GetKeyboardState(NULL); //get keyboard states
        if(keystates[SDL_SCANCODE_LEFT] && g_snake_direction[0] != DIRECTION_RIGHT)
        {
            g_input_direction = DIRECTION_LEFT;
        } else if(keystates[SDL_SCANCODE_RIGHT] && g_snake_direction[0] != DIRECTION_LEFT)
        {
            g_input_direction = DIRECTION_RIGHT;
        } else if(keystates[SDL_SCANCODE_UP] && g_snake_direction[0] != DIRECTION_DOWN)
        {
            g_input_direction = DIRECTION_UP;
        } else if(keystates[SDL_SCANCODE_DOWN] && g_snake_direction[0] != DIRECTION_UP)
        {
            g_input_direction = DIRECTION_DOWN;
        }


        if (SDL_GetTicks() - g_snake_last_update_tick > SNAKE_DELAY) {
            //move snake's body
            for (int i = g_snake_length - 1; i > 0; i--) {
                g_snake_x[i] = g_snake_x[i - 1];
                g_snake_y[i] = g_snake_y[i - 1];
                g_snake_direction[i] = g_snake_direction[i - 1];
            }
            //move snake's head
            g_snake_direction[0] = g_input_direction;
            g_snake_x[0] += DX[g_snake_direction[0]];
            g_snake_y[0] += DY[g_snake_direction[0]];

            g_snake_direction[g_snake_length - 1] = g_snake_direction[g_snake_length - 2];

            //check is snake out of game board
            if (g_snake_x[0] >= GAME_BOARD_WIDTH)
                g_snake_x[0] = 0;
            if (g_snake_y[0] >= GAME_ROARD_HEIGHT)
                g_snake_y[0] = 0;
            if (g_snake_x[0] < 0)
                g_snake_x[0] = GAME_BOARD_WIDTH - 1;
            if (g_snake_y[0] < 0)
                g_snake_y[0] = GAME_ROARD_HEIGHT - 1;
            //is snake bite itself
            if (g_snake_length > 4)
                for (int i = 4; i < g_snake_length; i++)
                {
                    if (g_snake_x[0] == g_snake_x[i] && g_snake_y[0] == g_snake_y[i])
                    {
                        //activate game over screen
                        change_screen(SCREEN_GAME_OVER);
                    }
                }
            //check collision between foof and snake head(index of snake's head is zero)

            if (g_food_x == g_snake_x[0] && g_food_y == g_snake_y[0])
            {

                reset_food_position();
                // add new element at the end of  the snake
                g_snake_length++;
                g_snake_x[g_snake_length - 1] = g_snake_x[g_snake_length - 2] - DX[g_snake_direction[g_snake_length - 2]];
                g_snake_y[g_snake_length - 1] = g_snake_y[g_snake_length - 2] - DY[g_snake_direction[g_snake_length - 2]];
                g_snake_direction[g_snake_length - 1] = g_snake_direction[g_snake_length - 2];
                g_score++;
            }
            g_snake_last_update_tick = SDL_GetTicks();
    }
    }
}
void loop()
{
    Uint32 start_ticks;
    Uint32 used_ticks;
    Uint32 ticks_per_frame = 1000/60;
    while (g_is_game_running)
    {
        start_ticks = SDL_GetTicks();
        render();
        SDL_RenderPresent(g_renderer);
        used_ticks = SDL_GetTicks() - start_ticks;

        if (used_ticks < ticks_per_frame)
        {
            SDL_Delay(ticks_per_frame - used_ticks);
        }

    }
}


void reset_game()
{
    g_score = 0;
    reset_snake();
    reset_food_position();
}

void draw_in_game_screen(void)
{
    update_in_game_screen();


    //clear screen
    SDL_SetRenderDrawColor(g_renderer, 0, 0, 0, 0);
    SDL_RenderClear(g_renderer);

    static SDL_Rect rect;

    rect.x = rect.y = 0;
    rect.w = WIN_WIDTH;
    rect.h = WIN_HEIGHT;

    SDL_RenderCopy(g_renderer, g_background, NULL, &rect);

    //draw snake's body
    rect.w = rect.h = BLOCK_SIZE;
    rect.x = g_snake_x[0] * BLOCK_SIZE;
    rect.y = g_snake_y[0] * BLOCK_SIZE;
    draw_sprite_ex(SPRITE_HEAD, &rect, ROTATION[g_snake_direction[0]],SDL_FLIP_NONE);

    for (int i = 1; i < g_snake_length - 1; i++) {
        rect.x = g_snake_x[i] * BLOCK_SIZE;
        rect.y = g_snake_y[i] * BLOCK_SIZE;

        enum Diretion prev_dir = g_snake_direction[i - 1];
        enum Diretion curr_dir = g_snake_direction[i];
        if(curr_dir != prev_dir)
        {

            if((curr_dir == DIRECTION_LEFT && prev_dir == DIRECTION_DOWN) || (curr_dir == DIRECTION_UP && prev_dir == DIRECTION_RIGHT))
            {
                draw_sprite_ex(SPRITE_CURVED, &rect, 0.0, SDL_FLIP_NONE);
            }
            else if((curr_dir == DIRECTION_RIGHT && prev_dir == DIRECTION_DOWN) || (curr_dir == DIRECTION_UP && prev_dir == DIRECTION_LEFT))
            {
                draw_sprite_ex(SPRITE_CURVED, &rect, 0.0, SDL_FLIP_HORIZONTAL);
            }
            else if((curr_dir == DIRECTION_LEFT && prev_dir == DIRECTION_UP) || (curr_dir == DIRECTION_DOWN && prev_dir == DIRECTION_RIGHT))
            {
                draw_sprite_ex(SPRITE_CURVED, &rect, 0.0, SDL_FLIP_VERTICAL);
            }
            //r->up && down->l
            else
            {
                draw_sprite_ex(SPRITE_CURVED, &rect, 180.0, SDL_FLIP_NONE);
            }

        }
        else
        {
            draw_sprite_ex(SPRITE_SATRAIGHT, &rect, ROTATION[curr_dir], SDL_FLIP_NONE);
        }
    }
    rect.x = g_snake_x[g_snake_length - 1] * BLOCK_SIZE ;
    rect.y = g_snake_y[g_snake_length - 1] * BLOCK_SIZE ;
    draw_sprite_ex(SPRITE_TAIL, &rect, ROTATION[g_snake_direction[g_snake_length - 1]], SDL_FLIP_NONE);
    //old draw food code
    /*
    rect.w = rect.h = BLOCK_SIZE;
    rect.x = g_food_x * BLOCK_SIZE;
    rect.y = g_food_y * BLOCK_SIZE;
    SDL_SetRenderDrawColor(g_renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(g_renderer, &rect);
    */

    //new draw food code
    rect.x = g_food_x * BLOCK_SIZE ;
    rect.y = g_food_y * BLOCK_SIZE ;
    draw_sprite_ex(SPRITE_FOOD, &rect, 0.0, SDL_FLIP_NONE);

    //draw score
    char scores[10] = "";
    sprintf(scores, "%d", g_score);
    TextRenderer_SetCharset(g_game_over_charset);
    TextRenderer_Draw(g_renderer, scores, 0, 0, TEXT_ANCHOR_TOP | TEXT_ANCHOR_LEFT);


}

void draw_game_over_screen()
{
        SDL_SetRenderDrawColor(g_renderer, 0, 0, 0, 100);
        SDL_RenderClear(g_renderer);
        static SDL_Rect dstrect;
        int w,h;
        SDL_QueryTexture(g_game_over_message, NULL, NULL, &w, &h);
        dstrect.w = w;
        dstrect.h = h;
        dstrect.x = WIN_WIDTH / 2 - w /2;
        dstrect.y = WIN_HEIGHT / 2 - h /2;
        SDL_RenderCopy(g_renderer, g_game_over_message, NULL, &dstrect);

        TextRenderer_SetCharset(g_game_over_charset);

        char buff[100];
        sprintf(buff, "your score: %d", g_score);

        TextRenderer_Draw(g_renderer, buff, WIN_WIDTH / 2, WIN_HEIGHT / 2 + 50, TEXT_ANCHOR_TOP | TEXT_ANCHOR_H_CENTER);

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if(event.type == SDL_QUIT)
                g_is_game_running = SDL_FALSE;
            else if(event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_RETURN)
            {
                change_screen(SCREEN_IN_GAME);
            }
        }

}

void draw_sprite_ex(Sprite sprite, SDL_Rect* dstrect, double angle, SDL_RendererFlip flip)
{
    static SDL_Rect srcrect;
    int index = SPRITE_MAPING[sprite];
    int row = index / SPRITE_SHEET_WIDTH;
    int col = index % SPRITE_SHEET_WIDTH;
    srcrect.w = SPRITE_SIZE;
    srcrect.h = SPRITE_SIZE;
    srcrect.y = row * SPRITE_SIZE;
    srcrect.x = col * SPRITE_SIZE;
    SDL_RenderCopyEx(g_renderer, g_sprite_sheet, &srcrect, dstrect, angle, NULL, flip);
}

void change_screen(Screen scr)
{
    g_screen = scr;
    switch (scr) {
    case SCREEN_TITLE:
        break;
    case SCREEN_IN_GAME:
        reset_game();
        g_scr_state = 0;
        break;
    case SCREEN_GAME_OVER:
        g_scr_state = 0;
        break;
    }
}

void draw_title_screen()
{

}


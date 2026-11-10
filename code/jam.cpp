
#include "raylib.h"

#define GLOBAL        static
#define INTERNAL      static
#define LOCAL_PERSIST static

#define MIN(A, B) ((A) < (B) ? (A) : (B))

#define TARGET_WIDTH  640
#define TARGET_HEIGHT 340

struct Window {
    int screen_width;
    int screen_height;
};

struct Game_State {
    Window window;
    Rectangle border;
};

GLOBAL Game_State g_game_state;

INTERNAL void SetWindowDimensions(int *width, int *height) {
    *width  = GetScreenWidth();
    *height = GetScreenHeight();
}

INTERNAL void AspectRatioToFit(Game_State *game_state) {
    Window *window = &game_state->window;
    Rectangle *result = &game_state->border;  
    int optimal_width = (int)(window->screen_height * ((float)TARGET_WIDTH / (float)TARGET_HEIGHT)); 
    int optimal_height = (int)(window->screen_width *((float)TARGET_HEIGHT / (float)TARGET_WIDTH));

    if (optimal_width > window->screen_width) {
        result->width  = window->screen_width;
        result->height = optimal_height;
        result->x = 0;
        int empty_height = window->screen_height - optimal_height;
        result->y = empty_height*0.5f;
    } else {
        result->width  = optimal_width;
        result->height = window->screen_height;
        int empty_width = window->screen_width - optimal_width;
        result->x = empty_width*0.5f;
        result->y = 0;
    }
}

INTERNAL void GameStateInit(Game_State *game_state) {
    SetWindowDimensions(&game_state->window.screen_width, &game_state->window.screen_height); 
    game_state->border.width  = game_state->window.screen_width;
    game_state->border.height = game_state->window.screen_height;
}

INTERNAL void GameUpdate(Game_State *game_state) {
    if (IsWindowResized()) {
        SetWindowDimensions(&game_state->window.screen_width, &game_state->window.screen_height); 
        AspectRatioToFit(game_state);
    }
}

INTERNAL void GameRender(Game_State *game_state) {
    BeginDrawing();
    ClearBackground(BLACK);

    float thickness = 50.0f;
    DrawRectangleLinesEx(game_state->border, thickness, BROWN);

    EndDrawing();
}


int main() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(640, 360, "Eternal Jam");
    SetTargetFPS(60);

    GameStateInit(&g_game_state);

    while(!WindowShouldClose()) {
        GameUpdate(&g_game_state);
        GameRender(&g_game_state);
    }

    CloseWindow();
}

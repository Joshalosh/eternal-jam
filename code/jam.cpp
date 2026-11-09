
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
    int render_width;
    int render_height;
};

struct Game_State {
    Window window;
};

GLOBAL Game_State g_game_state;

INTERNAL void SetWindowDimensions(int *width, int *height) {
    *width  = GetScreenWidth();
    *height = GetScreenHeight();
}

INTERNAL void AspectRatioToFit(Game_State *game_state) {
    Window *window = &game_state->window;
    float scale_width  = window->screen_height * ((float)TARGET_WIDTH  / (float)TARGET_HEIGHT);
    float scale_height = window->screen_width  * ((float)TARGET_HEIGHT / (float)TARGET_WIDTH);


    window->render_width = (int)(scale_width);
    window->render_height = (int)(scale_height);
}

INTERNAL void GameStateInit(Game_State *game_state) {
    SetWindowDimensions(&game_state->window.screen_width, &game_state->window.screen_height); 
    game_state->window.render_width = game_state->window.screen_width;
    game_state->window.render_height = game_state->window.screen_height;
}

INTERNAL void GameUpdate(Game_State *game_state) {
    if (IsWindowResized()) {
        SetWindowDimensions(&game_state->window.screen_width, &game_state->window.screen_height); 
        AspectRatioToFit(game_state);
    }
}

INTERNAL void GameRender(Game_State *game_state) {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    Rectangle border = {0.0f, 0.0f, (float)game_state->window.render_width, 
                                    (float)game_state->window.render_height};
    float thickness = 50.0f;
    DrawRectangleLinesEx(border, thickness, BROWN);

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

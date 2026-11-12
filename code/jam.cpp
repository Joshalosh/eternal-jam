
#include "raylib.h"
#include "staples.h"

#define CANVAS_WIDTH  640
#define CANVAS_HEIGHT 360

struct Window {
    S32 screen_width;
    S32 screen_height;
};

struct Game_State {
    Window window;
    RenderTexture2D canvas;
};

GLOBAL Game_State g_game_state;

INTERNAL void SetWindowDimensions(S32 *width, S32 *height) {
    *width  = GetScreenWidth();
    *height = GetScreenHeight();
}

INTERNAL Rectangle AspectRatioFittedRectangle(S32 canvas_width, S32 canvas_height, 
                                              S32 window_width, S32 window_height) {
    Rectangle result {};

    if ((canvas_width  > 0) &&
        (canvas_height > 0) &&
        (window_width  > 0) &&
        (window_height > 0)) {

        R32 optimal_width   = ((R32)window_height * ((R32)canvas_width / (R32)canvas_height)); 
        R32 optimal_height  = ((R32)window_width  * ((R32)canvas_height / (R32)canvas_width));

        if (optimal_width > (R32)window_width) {
            result.width  = (R32)window_width;
            result.height = optimal_height;
            result.x = 0;
            R32 empty_height = (R32)window_height - optimal_height;
            result.y = empty_height*0.5f;
        } else {
            result.width  = (R32)optimal_width;
            result.height = (R32)window_height;
            R32 empty_width = (R32)window_width - optimal_width;
            result.x = empty_width*0.5f;
            result.y = 0;
        }
    }

    return result;
}

INTERNAL void GameStateInit(Game_State *game_state) {
    SetWindowDimensions(&game_state->window.screen_width, &game_state->window.screen_height); 

    game_state->canvas = LoadRenderTexture(CANVAS_WIDTH, CANVAS_HEIGHT);
    SetTextureFilter(game_state->canvas.texture, TEXTURE_FILTER_POINT);
}

INTERNAL void GameUpdate(Game_State *game_state) {
    if (IsWindowResized()) {
        SetWindowDimensions(&game_state->window.screen_width, &game_state->window.screen_height); 
    }
}

INTERNAL void GameRender(Game_State *game_state) {
    BeginTextureMode(game_state->canvas);
    ClearBackground(RAYWHITE);

    R32 thickness = 50.0f;
    Rectangle border = {0, 0, CANVAS_WIDTH, CANVAS_HEIGHT};
    DrawRectangleLinesEx(border, thickness, BROWN);
    EndTextureMode();


    BeginDrawing();
    ClearBackground(BLACK);
    Rectangle source_rect = {0, 0, (R32)CANVAS_WIDTH, -(R32)CANVAS_HEIGHT};
    Rectangle dest_rect   = AspectRatioFittedRectangle(CANVAS_WIDTH, CANVAS_HEIGHT, 
                                                       game_state->window.screen_width, 
                                                       game_state->window.screen_height);
    DrawTexturePro(game_state->canvas.texture, source_rect, dest_rect, Vector2{0, 0}, 0, WHITE);

    EndDrawing();
}


int main() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(CANVAS_WIDTH, CANVAS_HEIGHT, "Eternal Jam");
    SetTargetFPS(60);

    GameStateInit(&g_game_state);

    while(!WindowShouldClose()) {
        GameUpdate(&g_game_state);
        GameRender(&g_game_state);
    }

    CloseWindow();
}

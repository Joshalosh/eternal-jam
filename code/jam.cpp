
#include "raylib.h"

#define GLOBAL        static
#define INTERNAL      static
#define LOCAL_PERSIST static

#define MIN(A, B) ((A) < (B) ? (A) : (B))

#define CANVAS_WIDTH  640
#define CANVAS_HEIGHT 360

struct Window {
    int screen_width;
    int screen_height;
};

struct Game_State {
    Window window;
    RenderTexture2D canvas;
};

GLOBAL Game_State g_game_state;

INTERNAL void SetWindowDimensions(int *width, int *height) {
    *width  = GetScreenWidth();
    *height = GetScreenHeight();
}

INTERNAL Rectangle MaximumSizeAspectRatioRectangle(int canvas_width, int canvas_height, int window_width, int window_height) {
    Rectangle result {};

    if ((canvas_width  > 0) &&
        (canvas_height > 0) &&
        (window_width  > 0) &&
        (window_height > 0)) {

        float optimal_width   = ((float)window_height * ((float)CANVAS_WIDTH / (float)CANVAS_HEIGHT)); 
        float optimal_height  = ((float)window_width * ((float)CANVAS_HEIGHT / (float)CANVAS_WIDTH));

        if (optimal_width > window_width) {
            result.width  = window_width;
            result.height = optimal_height;
            result.x = 0;
            float empty_height = window_height - optimal_height;
            result.y = empty_height*0.5f;
        } else {
            result.width  = optimal_width;
            result.height = window_height;
            float empty_width = window_width - optimal_width;
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

    float thickness = 50.0f;
    Rectangle border = {0, 0, CANVAS_WIDTH, CANVAS_HEIGHT};
    DrawRectangleLinesEx(border, thickness, BROWN);
    EndTextureMode();


    BeginDrawing();
    ClearBackground(BLACK);
    Rectangle source_rect = {0, 0, CANVAS_WIDTH, CANVAS_HEIGHT};
    Rectangle dest_rect   = MaximumSizeAspectRatioRectangle(CANVAS_WIDTH, CANVAS_HEIGHT, 
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

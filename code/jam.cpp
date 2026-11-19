
#include "raylib.h"
#include "staples.h"

#define CANVAS_WIDTH  640
#define CANVAS_HEIGHT 360

struct Window {
    S32       screen_width;
    S32       screen_height;
    Rectangle render_rect;
};

struct Clickable {
    S32   width;
    S32   height;
    Color col;
    Color pressed_col;
    B32   is_hovered;
};

enum EntityType {
    EntityNone,
    EntityButton,
    EntityTalker,
};

struct Entity {
    Texture2D  image;
    Vector2    origin;
    Vector2    position;
    R32        scale;
    Clickable  button;
    EntityType Type;
};

struct Game_State {
    Window          window;
    RenderTexture2D canvas;
    Vector2         mouse_pos;

    Entity          com;
    Entity          clickable;
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

        R32 optimal_width   = ((R32)window_height * ((R32)canvas_width  / (R32)canvas_height)); 
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
    // Window Init
    SetWindowDimensions(&game_state->window.screen_width, &game_state->window.screen_height); 
    game_state->window.render_rect = AspectRatioFittedRectangle(CANVAS_WIDTH, CANVAS_HEIGHT,
                                                                game_state->window.screen_width,
                                                                game_state->window.screen_height);

    // Canvas Init
    game_state->canvas = LoadRenderTexture(CANVAS_WIDTH, CANVAS_HEIGHT);
    SetTextureFilter(game_state->canvas.texture, TEXTURE_FILTER_POINT);

    // entity Init
    game_state->com.image    = LoadTexture("../assets/sprites/com_1.png");
    game_state->com.scale    = 2.0f;
    game_state->com.origin   = {game_state->com.image.width*game_state->com.scale*0.5f,
                                game_state->com.image.height*game_state->com.scale*0.5f};
    game_state->com.position = {0.5f*CANVAS_WIDTH   - game_state->com.origin.x, 
                                0.5f*CANVAS_HEIGHT  - game_state->com.origin.y};

    game_state->clickable.position = {game_state->com.position.x, game_state->com.position.y - 10.0f};
    game_state->clickable.button.width = 60;
    game_state->clickable.button.height = 20;
    game_state->clickable.button.col = GREEN;
    game_state->clickable.button.pressed_col = BLUE;
}

INTERNAL void GameUpdate(Game_State *game_state) {
    if (IsWindowResized()) {
        SetWindowDimensions(&game_state->window.screen_width, &game_state->window.screen_height); 
        game_state->window.render_rect = AspectRatioFittedRectangle(CANVAS_WIDTH, CANVAS_HEIGHT,
                                                                    game_state->window.screen_width,
                                                                    game_state->window.screen_height);
    }

    game_state->mouse_pos = GetMousePosition();
    Vector2 mouse_relative_to_canvas = {game_state->mouse_pos.x - game_state->window.render_rect.x,
                                        game_state->mouse_pos.y - game_state->window.render_rect.y};
    game_state->mouse_pos.x = mouse_relative_to_canvas.x * 
                              (CANVAS_WIDTH  / game_state->window.render_rect.width);
    game_state->mouse_pos.y = mouse_relative_to_canvas.y * 
                              (CANVAS_HEIGHT / game_state->window.render_rect.height);

    Vector2 mouse_pos = game_state->mouse_pos;
    Entity clickable = game_state->clickable;
    clickable->button.is_hovered = (mouse_pos.x >= clickable.position.x && 
                                    mouse_pos.x < clickable.position.x + clickable.button.width) &&
                                   (mouse_pos.y >= clickable.position.y &&
                                    mouse_pos.y < clickable.position.y + clickable.button.height);
}

INTERNAL void GameRender(Game_State *game_state) {
    // Draw to texture
    BeginTextureMode(game_state->canvas);
    ClearBackground(RAYWHITE);

    R32 thickness = 50.0f;
    Rectangle border = {0, 0, CANVAS_WIDTH, CANVAS_HEIGHT};
    DrawRectangleLinesEx(border, thickness, BROWN);
    DrawTextureEx(game_state->com.image, game_state->com.position, 0, game_state->com.scale, WHITE);

    Rectangle button = {game_state->clickable.position.x, game_state->clickable.position.y,
                        (float)game_state->clickable.button.width, (float)game_state->clickable.button.height};
    Color col = game_state->clickable.button.is_hovered ? game_state->clickable.button.pressed_col : game_state->clickable.button.col;
    DrawRectangleRec(button, col);
    DrawCircleV(game_state->mouse_pos, 4.0f, RED);

    EndTextureMode();


    // Draw texture to screen
    BeginDrawing();
    ClearBackground(BLACK);
    Rectangle source_rect = {0, 0, (R32)CANVAS_WIDTH, -(R32)CANVAS_HEIGHT};
    DrawTexturePro(game_state->canvas.texture, source_rect, game_state->window.render_rect, Vector2{0, 0}, 0, WHITE);

    Vector2 mouse_pos = GetMousePosition();

    DrawText(TextFormat("%f, %f", mouse_pos.x, mouse_pos.y), 100, 100, 20, BLACK);
    DrawText(TextFormat("%f, %f", game_state->mouse_pos.x, game_state->mouse_pos.y), 200, 200, 20, GREEN);

    EndDrawing();
}


int main() {
    B32 directory_changed = ChangeDirectory(GetApplicationDirectory());
    ASSERT(!directory_changed);
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

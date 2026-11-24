
#include "raylib.h"
#include "staples.h"

#define BASE_WIDTH  640
#define BASE_HEIGHT 360

struct Window {
    S32  width;
    S32  height;
};

struct Canvas {
    RenderTexture2D render_tex;
    Rectangle bounds;
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
    Window  window;
    Canvas  canvas;
    Vector2 mouse_pos;

    Entity  com;
    Entity  clickable;
};


GLOBAL Game_State g_game_state;

#if 0
INTERNAL void SetWindowDimensions(S32 *width, S32 *height) {
    *width  = GetScreenWidth();
    *height = GetScreenHeight();
}
#endif

#if 0
INTERNAL void SetWindowDimensions(Window *window) {
    window->width  = GetScreenWidth();
    window->height = GetScreenHeight();
}
#endif

#if 1 
INTERNAL Window SetWindowDimensions(S32 width, S32 height) {
    ASSERT(width > 0 && height > 0);
    Window result;
    result.width  = width;
    result.height = height;
    return result;
}
#endif


INTERNAL Rectangle AspectRatioFittedRectangle(S32 base_width, S32 base_height, 
                                              S32 window_width, S32 window_height) {
    Rectangle result {};

    if ((base_width    > 0) &&
        (base_height   > 0) &&
        (window_width  > 0) &&
        (window_height > 0)) {

        R32 optimal_width   = ((R32)window_height * ((R32)base_width  / (R32)base_height)); 
        R32 optimal_height  = ((R32)window_width  * ((R32)base_height / (R32)base_width));

        if (optimal_width > (R32)window_width) {
            result.width     = (R32)window_width;
            result.height    = optimal_height;
            R32 empty_height = (R32)window_height - optimal_height;
            result.x         = 0;
            result.y         = empty_height*0.5f;
        } else {
            result.width     = (R32)optimal_width;
            result.height    = (R32)window_height;
            R32 empty_width  = (R32)window_width - optimal_width;
            result.x         = empty_width*0.5f;
            result.y         = 0;
        }
    }

    return result;
}

INTERNAL void GameStateInit(Game_State *game_state) {
    // Window Init

    S32 window_width   = GetScreenWidth();
    S32 window_height  = GetScreenHeight();
    game_state->window = SetWindowDimensions(window_width, window_height); 

    // Canvas Init
    game_state->canvas.render_tex = LoadRenderTexture(BASE_WIDTH, BASE_HEIGHT);
    SetTextureFilter(game_state->canvas.render_tex.texture, TEXTURE_FILTER_POINT);
    game_state->canvas.bounds = AspectRatioFittedRectangle(BASE_WIDTH, BASE_HEIGHT,
                                                           window_width, window_height);

    // Entity Init
    game_state->com.image    = LoadTexture("../assets/sprites/com_1.png");
    game_state->com.scale    = 2.0f;
    game_state->com.origin   = {game_state->com.image.width*game_state->com.scale*0.5f,
                                game_state->com.image.height*game_state->com.scale*0.5f};
    game_state->com.position = {0.5f*BASE_WIDTH  - game_state->com.origin.x, 
                                0.5f*BASE_HEIGHT - game_state->com.origin.y};

    game_state->clickable.position           = {game_state->com.position.x, game_state->com.position.y - 10.0f};
    game_state->clickable.button.width       = 60;
    game_state->clickable.button.height      = 20;
    game_state->clickable.button.col         = GREEN;
    game_state->clickable.button.pressed_col = BLUE;
}

INTERNAL void GameUpdate(Game_State *game_state) {
    if (IsWindowResized()) {
        S32 width  = GetScreenWidth();
        S32 height = GetScreenHeight();
        if (width > 0 && height > 0){
            game_state->window = SetWindowDimensions(width, height); 
            game_state->canvas.bounds = AspectRatioFittedRectangle(BASE_WIDTH, BASE_HEIGHT,
                                                                   game_state->window.width,
                                                                   game_state->window.height);
        }
    }

    // Translating mouse coordinates from screen space to game space
    Vector2 mouse_pos = GetMousePosition();
    Rectangle canvas = game_state->canvas.bounds;
    ASSERT(canvas.width > 0 && canvas.height > 0);
    mouse_pos.x = (mouse_pos.x - canvas.x) * ((R32)BASE_WIDTH  / (R32)canvas.width);
    mouse_pos.y = (mouse_pos.y - canvas.y) * ((R32)BASE_HEIGHT / (R32)canvas.height);
    game_state->mouse_pos = mouse_pos;

    // Button check
    Vector2 button_min = game_state->clickable.position;
    Vector2 button_max = {button_min.x + (R32)game_state->clickable.button.width, 
                          button_min.y + (R32)game_state->clickable.button.height};
    B32 hover_check = (mouse_pos.x >= button_min.x && mouse_pos.x < button_max.x) &&
                      (mouse_pos.y >= button_min.y && mouse_pos.y < button_max.y);
    game_state->clickable.button.is_hovered = hover_check;

    Color button_col = BLUE;
    S32 mouse_left = 0;
    S32 x_offset = 0;
    S32 y_offset = 0;
    if (hover_check && IsMouseButtonDown(mouse_left)) {
        button_col = RED;
        x_offset = GetRandomValue(-5, 5);
        y_offset = GetRandomValue(-5, 5);
        Vector2 pos_offset = {x_offset, y_offset};
    }
    game_state->clickable.button.pressed_col = button_col;
    game_state->com.position.x += (R32)x_offset;
    game_state->com.position.y += (R32)y_offset;
}

INTERNAL void GameRender(Game_State *game_state) {
    // Draw to texture
    BeginTextureMode(game_state->canvas.render_tex);
    ClearBackground(RAYWHITE);

    R32 thickness = 50.0f;
    Rectangle border = {0, 0, BASE_WIDTH, BASE_HEIGHT};
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
    Rectangle source_rect = {0, 0, (R32)BASE_WIDTH, -(R32)BASE_HEIGHT};
    DrawTexturePro(game_state->canvas.render_tex.texture, source_rect, game_state->canvas.bounds, Vector2{0, 0}, 0, WHITE);

    EndDrawing();
}


int main() {
    B32 directory_changed = ChangeDirectory(GetApplicationDirectory());
    ASSERT(!directory_changed);
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(BASE_WIDTH, BASE_HEIGHT, "Eternal Jam");
    SetTargetFPS(60);

    GameStateInit(&g_game_state);

    while(!WindowShouldClose()) {
        GameUpdate(&g_game_state);
        GameRender(&g_game_state);
    }

    CloseWindow();
}

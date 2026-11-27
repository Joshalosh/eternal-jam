
#include "raylib.h"
#include "staples.h"
#include "raymath.h"

#define BASE_WIDTH       640
#define BASE_HEIGHT      360
#define BORDER_THICKNESS 25.0f
#define CANVAS_SPLIT     (R32)BASE_WIDTH * (1.0f/3.0f)

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
    Vector2    position;
    R32        scale;
    Clickable  button;
    EntityType Type;
    B32        is_shaking;
};

struct Game_State {
    Window  window;
    Canvas  canvas;
    Vector2 mouse_pos;
    R64     world_time;

    Entity  com;
    Entity  jar;
    Entity  clickable;
};


GLOBAL Game_State g_game_state;

INTERNAL Window SetWindowDimensions(S32 width, S32 height) {
    ASSERT(width > 0 && height > 0);
    Window result;
    result.width  = width;
    result.height = height;
    return result;
}

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
    game_state->world_time = 100;

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
    game_state->com.position = {0.5f*(CANVAS_SPLIT + BORDER_THICKNESS), 0.33333f*BASE_HEIGHT};

    game_state->jar.image    = LoadTexture("../assets/sprites/jar.png");
    game_state->jar.scale    = 2.0f;
    R32 half_right_panel_width = ((R32)BASE_WIDTH - (R32)CANVAS_SPLIT) * 0.5f;
    game_state->jar.position = {CANVAS_SPLIT + half_right_panel_width, 0.5f*BASE_HEIGHT};
    //game_state->jar.position = {0.5f*BASE_WIDTH, 0.5f*BASE_HEIGHT};

    // Button Init
    {
    S32 width     = 64;
    S32 height    = 16;
    game_state->clickable.button.width       = width;
    game_state->clickable.button.height      = height;

    game_state->clickable.position           = {0.5f*(CANVAS_SPLIT + BORDER_THICKNESS), (BASE_HEIGHT * (2.0f/3.0f))};
    game_state->clickable.button.col         = GREEN;
    game_state->clickable.button.pressed_col = BLUE;
    }
}

INTERNAL void GameUpdate(Game_State *game_state, R32 delta_time) {
    game_state->world_time += (R64)delta_time;
    if (IsWindowResized()) {
        S32 width  = GetScreenWidth();
        S32 height = GetScreenHeight();
        if (width > 0 && height > 0){
            game_state->window        = SetWindowDimensions(width, height); 
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
    R32 button_width  = (R32)game_state->clickable.button.width;
    R32 button_height = (R32)game_state->clickable.button.height;
    Vector2 button_min = {game_state->clickable.position.x - (button_width*0.5f), game_state->clickable.position.y - (button_height*0.5f)};
    Vector2 button_max = {button_min.x + button_width, button_min.y + button_height};
    B32 hover_check = (mouse_pos.x >= button_min.x && mouse_pos.x < button_max.x) &&
                      (mouse_pos.y >= button_min.y && mouse_pos.y < button_max.y);
    game_state->clickable.button.is_hovered = hover_check;

    Color button_col = BLUE;
    S32 mouse_left = 0;
    B32 is_com_shaking = false;
    if (hover_check && IsMouseButtonDown(mouse_left)) {
        button_col = RED;
        is_com_shaking = true;
    }
    game_state->com.is_shaking               = is_com_shaking;
    game_state->clickable.button.pressed_col = button_col;
}

INTERNAL void DrawCentredRect(Rectangle rect, Color col) {
    Vector2 position = {rect.x - rect.width*0.5f, rect.y - rect.height*0.5f};
    Vector2 size     = {rect.width, rect.height};
    DrawRectangleV(position, size, col);
}

INTERNAL void DrawCentredTexture(Texture2D texture, Vector2 centre_pos, R32 scale, Color col) {
    centre_pos.x -= texture.width  * scale * 0.5f; 
    centre_pos.y -= texture.height * scale * 0.5f; 
    DrawTextureEx(texture, centre_pos, 0, scale, col);
}

INTERNAL void GameRender(Game_State *game_state) {
    // Draw to canvas
    BeginTextureMode(game_state->canvas.render_tex);
    ClearBackground(RAYWHITE);

    R32 a_third = 1.0f / 3.0f;
    Rectangle vertical_bar = {BASE_WIDTH*a_third, 0, BORDER_THICKNESS, BASE_HEIGHT};
    Rectangle border = {0, 0, BASE_WIDTH, BASE_HEIGHT};
    DrawRectangleLinesEx(vertical_bar, BORDER_THICKNESS, BROWN);
    DrawRectangleLinesEx(border, BORDER_THICKNESS, BROWN);

    Vector2 com_draw_pos = game_state->com.position;
    if (game_state->com.is_shaking) {
        R32 t = (R32)game_state->world_time;
        R32 x_speed   = 73.0f;
        R32 y_speed   = 59.0f;
        R32 amplitude = 4.0f;

        com_draw_pos.x += sinf(t*x_speed)*amplitude;
        com_draw_pos.y += sinf(t*y_speed)*amplitude;
    }
    DrawCentredTexture(game_state->com.image, com_draw_pos, game_state->com.scale, WHITE);

    Vector2 jar_draw_pos = game_state->jar.position;
    DrawCentredTexture(game_state->jar.image, jar_draw_pos, game_state->jar.scale, WHITE);
    DrawCircleV(jar_draw_pos, 2.0f, RED);

    Vector2 button_draw_pos  = game_state->clickable.position;
    Rectangle button = {button_draw_pos.x, button_draw_pos.y,
                        (float)game_state->clickable.button.width, (float)game_state->clickable.button.height};
    Color col = game_state->clickable.button.is_hovered ? game_state->clickable.button.pressed_col : game_state->clickable.button.col;
    DrawCentredRect(button, col);
    DrawCircleV(button_draw_pos, 2.0f, RED);
    DrawCircleV(game_state->com.position, 2.0f, RED);
    DrawCircleV(game_state->mouse_pos, 4.0f, RED);

    EndTextureMode();

    // Draw texture/canvas to screen
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
        R32 delta_time = GetFrameTime();
        if (delta_time > 0.05f) {
            delta_time = 0.05f;
        }
        GameUpdate(&g_game_state, delta_time);
        GameRender(&g_game_state);
    }

    CloseWindow();
}

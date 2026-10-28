
#include "raylib.h"

int main() {
    int screen_width  = 640;
    int screen_height = 360;

    InitWindow(screen_width, screen_height, "Eternal Jam");

    SetTargetFPS(60);

    while(!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("Brand New Window", 190, 200, 20, LIGHTGRAY);
        EndDrawing();
    }

    CloseWindow();
}

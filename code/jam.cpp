
#include "raylib.h"

int NullTerminatedTextLength(char *text) {
    // TODO: Assert the initial pointer is not null.

    char *scan = text;
    while(*scan) {
        scan++;
    }
    int result = (int)(scan - text);
    return result;
}

int main() {
    int screen_width  = 640;
    int screen_height = 360;

    InitWindow(screen_width, screen_height, "Eternal Jam");

    SetTargetFPS(60);


    while(!WindowShouldClose()) {
        Rectangle border = {0.0f, 0.0f, (float)screen_width, (float)screen_height};
        float thickness = 50.0f;
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawRectangleLinesEx(border, thickness, BROWN);

        char *text = "Brand New Window";
        int text_size = NullTerminatedTextLength(text);
        int draw_point = screen_width*0.5 - text_size*0.5;
        DrawText(text, draw_point, screen_height*0.5, 20, LIGHTGRAY);
        EndDrawing();
    }

    CloseWindow();
}

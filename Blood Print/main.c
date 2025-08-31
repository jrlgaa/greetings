#include "raylib.h"
#include <stdio.h>
#include <direct.h>   // for _getcwd

typedef enum {
    STATE_LOADING,
    STATE_MENU,
    STATE_PLAY,
    STATE_EXIT
} GameState;

int main(void)
{
    InitWindow(800, 600, "Bloodprint: The Final Puzzle");
    SetTargetFPS(60);

    // Print working directory (debug)
    char cwd[512];
    if (_getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("Current working directory: %s\n", cwd);
    }

    // Load font
    Font font = LoadFont("src/assets/fonts/arial.ttf");
    if (font.texture.id == 0) {
        font = GetFontDefault();
        printf("Failed to load font, using default.\n");
    }

    // Load background image for loading screen
    Texture2D loadingBackground = LoadTexture("src/assets/images/my_image.png");
    if (loadingBackground.id == 0) {
        printf("Failed to load background, using plain color.\n");
    }

    Color white = RAYWHITE;

    // Loading screen variables
    int dots = 0;
    double lastTime = GetTime();
    float progress = 0.0f;

    // Game state
    GameState state = STATE_LOADING;

    while (!WindowShouldClose())
    {
        // === LOADING STATE ===
        if (state == STATE_LOADING)
        {
            // Animate dots
            if (GetTime() - lastTime > 0.5)
            {
                dots = (dots + 1) % 6;
                lastTime = GetTime();
            }

            // Simulate loading
            progress += 0.005f;
            if (progress >= 1.0f)
            {
                progress = 1.0f;
                state = STATE_MENU; // Done loading
            }

            BeginDrawing();
                ClearBackground(BLACK);

                // Draw scaled background if available
                if (loadingBackground.id != 0)
                {
                    DrawTexturePro(
                        loadingBackground,
                        (Rectangle){0, 0, (float)loadingBackground.width, (float)loadingBackground.height},
                        (Rectangle){0, 0, (float)GetScreenWidth(), (float)GetScreenHeight()},
                        (Vector2){0,0},
                        0.0f,
                        WHITE
                    );
                }

                // Title text
                const char *title = "Bloodprint: The Final Puzzle";
                int titleWidth = MeasureTextEx(font, title, 48, 2).x;
                DrawTextEx(font, title,
                           (Vector2){(GetScreenWidth() - titleWidth) / 2, 200},
                           48, 2, white);

                // Loading text
                char loadingText[32];
                sprintf(loadingText, "LOADING%.*s", dots, ".....");
                int loadWidth = MeasureTextEx(font, loadingText, 32, 2).x;
                DrawTextEx(font, loadingText,
                           (Vector2){(GetScreenWidth() - loadWidth) / 2, 300},
                           32, 2, white);

                // Progress bar
                int barWidth = 400;
                int barHeight = 30;
                int barX = (GetScreenWidth() - barWidth) / 2;
                int barY = 400;
                DrawRectangle(barX, barY, barWidth, barHeight, DARKGRAY);
                DrawRectangle(barX, barY, (int)(barWidth * progress), barHeight, SKYBLUE);
                DrawRectangleLines(barX, barY, barWidth, barHeight, WHITE);
            EndDrawing();
        }

        // === MENU STATE ===
        else if (state == STATE_MENU)
        {
            Vector2 mouse = GetMousePosition();

            Rectangle playBtn = {(GetScreenWidth() - 200) / 2, 280, 200, 50};
            Rectangle exitBtn = {(GetScreenWidth() - 200) / 2, 360, 200, 50};

            bool playHover = CheckCollisionPointRec(mouse, playBtn);
            bool exitHover = CheckCollisionPointRec(mouse, exitBtn);

            if (playHover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) state = STATE_PLAY;
            if (exitHover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) state = STATE_EXIT;

            BeginDrawing();
                ClearBackground((Color){10, 10, 30, 255});

                const char *menuTitle = "SELECT AN OPTION";
                int menuTitleWidth = MeasureTextEx(font, menuTitle, 36, 2).x;
                DrawTextEx(font, menuTitle,
                           (Vector2){(GetScreenWidth() - menuTitleWidth) / 2, 150},
                           36, 2, white);

                // PLAY button
                DrawRectangleRec(playBtn, playHover ? DARKBLUE : DARKGRAY);
                const char *playText = "PLAY";
                int playWidth = MeasureTextEx(font, playText, 28, 2).x;
                DrawTextEx(font, playText,
                           (Vector2){playBtn.x + (200 - playWidth) / 2, playBtn.y + 10},
                           28, 2, white);

                // EXIT button
                DrawRectangleRec(exitBtn, exitHover ? DARKBLUE : DARKGRAY);
                const char *exitText = "EXIT";
                int exitWidth = MeasureTextEx(font, exitText, 28, 2).x;
                DrawTextEx(font, exitText,
                           (Vector2){exitBtn.x + (200 - exitWidth) / 2, exitBtn.y + 10},
                           28, 2, white);
            EndDrawing();
        }

        // === PLAY STATE ===
        else if (state == STATE_PLAY)
        {
            BeginDrawing();
                ClearBackground(BLACK);
                DrawText("GAME STARTED! (Press ESC to quit)", 200, 300, 20, GREEN);
            EndDrawing();
        }

        // === EXIT STATE ===
        else if (state == STATE_EXIT)
        {
            break; // Exit loop
        }
    }

    // Cleanup
    UnloadFont(font);
    if (loadingBackground.id != 0) UnloadTexture(loadingBackground);
    CloseWindow();

    return 0;
}

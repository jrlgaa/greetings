#include "raylib.h"
#include <stdio.h>
#include <direct.h>
#include <stdbool.h>

typedef enum {
    STATE_LOADING,
    STATE_MENU,
    STATE_PLAY,
    STATE_SETTINGS,
    STATE_EXIT
} GameState;

typedef struct {
    Rectangle rect;
    const char *text;
    bool hover;
} Button;

#define SCREEN_WIDTH 1080
#define SCREEN_HEIGHT 680

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Bloodprint: The Final Puzzle");
    SetTargetFPS(60);
    InitAudioDevice();

   
    char cwd[512];
    if (_getcwd(cwd, sizeof(cwd)) != NULL)
        printf("Current working directory: %s\n", cwd);

    // font
    Font font = LoadFont("src/assets/fonts/PixelNES.otf");
    if (font.texture.id == 0) {
        font = GetFontDefault();
        printf("Failed to load font, using default.\n");
    }

    // textures
    Texture2D loadingBackground = LoadTexture("src/assets/images/loading.png");
    Texture2D menuBackground = LoadTexture("src/assets/images/menu_bg.png");
    if (loadingBackground.id == 0) printf("Failed to load loading background.\n");
    if (menuBackground.id == 0) printf("Failed to load menu background.\n");

    //music
    Music loadingBGM = LoadMusicStream("src/assets/audio/loading_bgm.mp3");
    Music menuBGM = LoadMusicStream("src/assets/audio/menu_bgm.mp3");
    if (menuBGM.stream.buffer != NULL) menuBGM.looping = true;

    GameState state = STATE_LOADING;

    // Loading screen
    int dots = 0;
    double lastDotTime = GetTime();
    float progress = 0.0f;
    bool loadingMusicStarted = false;

    // Menu buttons
    Button menuButtons[3];
    menuButtons[0] = (Button){ { (SCREEN_WIDTH-200)/2, 280, 200, 50 }, "PLAY", false };
    menuButtons[1] = (Button){ { (SCREEN_WIDTH-200)/2, 360, 200, 50 }, "SETTINGS", false };
    menuButtons[2] = (Button){ { (SCREEN_WIDTH-200)/2, 440, 200, 50 }, "EXIT", false };

    // Settings
    float masterVolume = 1.0f; // 0.0 - 1.0
    bool menuMusicStartedInSettings = false;

    while (!WindowShouldClose())
    {
        // music
        if ((state == STATE_MENU || state == STATE_PLAY || state == STATE_SETTINGS) && menuBGM.stream.buffer != NULL)
            UpdateMusicStream(menuBGM);
        if (state == STATE_LOADING && loadingBGM.stream.buffer != NULL)
            UpdateMusicStream(loadingBGM);

        if (state == STATE_LOADING)
        {
            if (!loadingMusicStarted && loadingBGM.stream.buffer != NULL) {
                PlayMusicStream(loadingBGM);
                SetMusicVolume(loadingBGM, masterVolume);
                loadingMusicStarted = true;
            }

            if (GetTime() - lastDotTime > 0.5) {
                dots = (dots + 1) % 6;
                lastDotTime = GetTime();
            }

            if (progress < 1.0f) progress += 0.01f;

            BeginDrawing();
                ClearBackground(BLACK);

                if (loadingBackground.id != 0)
                    DrawTexturePro(loadingBackground,
                                   (Rectangle){0,0,(float)loadingBackground.width,(float)loadingBackground.height},
                                   (Rectangle){0,0,(float)SCREEN_WIDTH,(float)SCREEN_HEIGHT},
                                   (Vector2){0,0}, 0.0f, WHITE);

                const char *title = "Bloodprint: The Final Puzzle";
                int titleWidth = MeasureTextEx(font, title, 48, 2).x;
                DrawTextEx(font, title, (Vector2){(SCREEN_WIDTH-titleWidth)/2, 280}, 48, 2, RAYWHITE);

                char loadingText[64];
                if (progress < 1.0f)
                    sprintf(loadingText, "LOADING ASSETS%.*s", dots, ".....");
                else
                    sprintf(loadingText, "CLICK TO ENTER THE GAME");

                int textWidth = MeasureTextEx(font, loadingText, 20, 2).x;
                DrawTextEx(font, loadingText, (Vector2){(SCREEN_WIDTH-textWidth)/2, 450}, 20, 2, RAYWHITE);

                if (progress < 1.0f)
                {
                    int barWidth = 400, barHeight = 10;
                    int barX = (SCREEN_WIDTH-barWidth)/2, barY = 480;
                    DrawRectangle(barX, barY, barWidth, barHeight, DARKGRAY);
                    DrawRectangle(barX, barY, (int)(barWidth*progress), barHeight, SKYBLUE);
                    DrawRectangleLines(barX, barY, barWidth, barHeight, WHITE);
                }
            EndDrawing();

            if (progress >= 1.0f && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                state = STATE_MENU;
                if (loadingBGM.stream.buffer != NULL) StopMusicStream(loadingBGM);
                if (menuBGM.stream.buffer != NULL) {
                    PlayMusicStream(menuBGM);
                    SetMusicVolume(menuBGM, masterVolume);
                }
            }
        }

        else if (state == STATE_MENU)
        {
            Vector2 mouse = GetMousePosition();
            for (int i=0; i<3; i++)
                menuButtons[i].hover = CheckCollisionPointRec(mouse, menuButtons[i].rect);

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                if (menuButtons[0].hover) state = STATE_PLAY;
                if (menuButtons[1].hover) state = STATE_SETTINGS;
                if (menuButtons[2].hover) state = STATE_EXIT;
            }

            BeginDrawing();
                // menu background
                if (menuBackground.id != 0)
                    DrawTexturePro(menuBackground, (Rectangle){0,0,(float)menuBackground.width,(float)menuBackground.height},
                                   (Rectangle){0,0,(float)SCREEN_WIDTH,(float)SCREEN_HEIGHT}, (Vector2){0,0}, 0.0f, WHITE);
                else ClearBackground((Color){10,10,30,255});

                const char *menuTitle = "SELECT AN OPTION";
                int titleWidth = MeasureTextEx(font, menuTitle, 36, 2).x;
                DrawTextEx(font, menuTitle, (Vector2){(SCREEN_WIDTH-titleWidth)/2, 150}, 36, 2, RAYWHITE);

                for (int i = 0; i < 3; i++)
                {
                    int textWidth = MeasureTextEx(font, menuButtons[i].text, 28, 2).x;
                    DrawTextEx(font, menuButtons[i].text, 
                        (Vector2){
                            menuButtons[i].rect.x + (menuButtons[i].rect.width - textWidth)/2,
                            menuButtons[i].rect.y + 10
                        }, 
                        28, 2, menuButtons[i].hover ? SKYBLUE : RAYWHITE);
                }
            EndDrawing();
        }

        else if (state == STATE_SETTINGS)
        {
            Vector2 mouse = GetMousePosition();

            if (!menuMusicStartedInSettings && menuBGM.stream.buffer != NULL) {
                PlayMusicStream(menuBGM);
                SetMusicVolume(menuBGM, masterVolume);
                menuMusicStartedInSettings = true;
            }

            // Buttons
            Rectangle plusBtn = { (SCREEN_WIDTH-50)/2 + 80, 300, 50, 50 };
            Rectangle minusBtn = { (SCREEN_WIDTH-50)/2 - 80, 300, 50, 50 };
            bool plusHover = CheckCollisionPointRec(mouse, plusBtn);
            bool minusHover = CheckCollisionPointRec(mouse, minusBtn);

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                if (plusHover && masterVolume < 1.0f) masterVolume += 0.05f;
                if (minusHover && masterVolume > 0.0f) masterVolume -= 0.05f;
                if (masterVolume > 1.0f) masterVolume = 1.0f;
                if (masterVolume < 0.0f) masterVolume = 0.0f;
                if (menuBGM.stream.buffer != NULL) SetMusicVolume(menuBGM, masterVolume);
                if (loadingBGM.stream.buffer != NULL) SetMusicVolume(loadingBGM, masterVolume);
            }

            // Back button
            Rectangle backBtn = { (SCREEN_WIDTH-200)/2, 400, 200, 50 };
            bool backHover = CheckCollisionPointRec(mouse, backBtn);
            if (backHover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                state = STATE_MENU;
                menuMusicStartedInSettings = false;
            }

            // Draw Settings with menu background
            BeginDrawing();
                if (menuBackground.id != 0)
                    DrawTexturePro(menuBackground,
                                   (Rectangle){0,0,(float)menuBackground.width,(float)menuBackground.height},
                                   (Rectangle){0,0,(float)SCREEN_WIDTH,(float)SCREEN_HEIGHT},
                                   (Vector2){0,0}, 0.0f, WHITE);
                else ClearBackground((Color){30,30,50,255});

                DrawText("SETTINGS", 440, 150, 36, RAYWHITE);
                DrawText("Volume", 510, 250, 20, RAYWHITE);

                char volText[16];
                sprintf(volText, "%d", (int)(masterVolume*100));
                int volWidth = MeasureText(volText, 36);
                DrawText(volText, (SCREEN_WIDTH-volWidth)/2, 300, 36, SKYBLUE);

                DrawRectangleRec(plusBtn, plusHover ? DARKBLUE : DARKGRAY);
                DrawText("+", plusBtn.x + 15, plusBtn.y + 5, 36, RAYWHITE);

                DrawRectangleRec(minusBtn, minusHover ? DARKBLUE : DARKGRAY);
                DrawText("-", minusBtn.x + 15, minusBtn.y + 5, 36, RAYWHITE);

                DrawRectangleRec(backBtn, backHover ? DARKBLUE : DARKGRAY);
                int textWidth = MeasureText("BACK", 28);
                DrawText("BACK", backBtn.x + (backBtn.width - textWidth)/2, backBtn.y + 10, 28, RAYWHITE);
            EndDrawing();
        }

        else if (state == STATE_PLAY)
        {
            if (IsKeyPressed(KEY_ESCAPE)) state = STATE_MENU;

            BeginDrawing();
                ClearBackground(BLACK);
                DrawText("COMING SOON...", 120, 300, 100, GREEN);
            EndDrawing();
        }


        else if (state == STATE_EXIT) break;
    }

    UnloadFont(font);
    if (loadingBackground.id != 0) UnloadTexture(loadingBackground);
    if (menuBackground.id != 0) UnloadTexture(menuBackground);
    if (loadingBGM.stream.buffer != NULL) UnloadMusicStream(loadingBGM);
    if (menuBGM.stream.buffer != NULL) UnloadMusicStream(menuBGM);
    CloseAudioDevice();
    CloseWindow();

    return 0;
}

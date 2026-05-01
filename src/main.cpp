#include "raylib.h"
#include "Game.h"

int main()
{
    const int screenWidth = 500;
    const int screenHeight = 660;

    InitWindow(screenWidth, screenHeight, "Tetris");
    SetTargetFPS(60);

    Game game;

    while (!WindowShouldClose())
    {
        if (IsKeyPressed(KEY_R) && game.getState() == GameState::GameOver)
            game.reset();

        game.update();

        BeginDrawing();
        game.draw();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}

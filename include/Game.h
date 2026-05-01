#pragma once

#include "raylib.h"
#include "Tetromino.h"
#include "GameGrid.h"
#include "InputHandler.h"
#include "Renderer.h"
#include <random>

enum class GameState {
    Running,
    Paused,
    GameOver
};

class Game {
public:
    Game();

    void update();
    void draw();
    void reset();

    int getScore() const { return score; }
    int getHighScore() const { return highScore; }
    int getLevel() const { return level; }
    int getLines() const { return linesCleared; }
    GameState getState() const { return state; }
    const GameGrid& getGrid() const { return grid; }
    const Tetromino& getCurrentPiece() const { return currentPiece; }
    int getCurrentX() const { return currentX; }
    int getCurrentY() const { return currentY; }
    const Tetromino& getNextPiece() const { return nextPiece; }

private:
    static constexpr int SPAWN_X = 3;
    static constexpr int SPAWN_Y = -1;

    static constexpr float INITIAL_DROP_INTERVAL = 1.0f;
    static constexpr float FAST_DROP_INTERVAL = 0.05f;

    GameState state;
    GameGrid grid;

    Tetromino currentPiece;
    int currentX, currentY;

    Tetromino nextPiece;

    int score;
    int highScore;
    int level;
    int linesCleared;

    float dropTimer;
    float dropInterval;
    bool softDropping;

    Renderer renderer;
    InputHandler input;
    std::mt19937 rng;

    Tetromino randomPiece();
    void spawnPiece();
    void lockPiece();
    void movePiece(int dx, int dy);
    void rotatePiece();
    void hardDrop();
    void executeCommand(GameCommand cmd);
    void updateDrop();
    bool checkCollision(const Tetromino& piece, int x, int y) const;
    void addScore(int cleared);
    void updateLevel();
};

#pragma once

#include "raylib.h"
#include "Tetromino.h"
#include "GameGrid.h"
#include "ParticleSystem.h"
#include <vector>

class Game;

class Renderer {
public:
    static constexpr int SCREEN_WIDTH  = 500;
    static constexpr int SCREEN_HEIGHT = 660;
    static constexpr int GRID_OFFSET_X = 30;
    static constexpr int GRID_OFFSET_Y = 30;
    static constexpr int PANEL_X       = GRID_OFFSET_X + GameGrid::COLS * GameGrid::CELL_SIZE + 15;

    Renderer();

    void draw(const Game& game);

    void triggerPieceLock(const Tetromino& piece, int pieceX, int pieceY);
    void triggerLineClear(const std::vector<int>& rows);
    void triggerGameOver();
    void resetEffects();

private:
    static constexpr float CELL_ROUNDNESS   = 0.15f;
    static constexpr float PANEL_ROUNDNESS  = 0.1f;
    static constexpr int   SHADOW_OFFSET    = 3;
    static constexpr float FLASH_DURATION   = 0.45f;
    static constexpr float GAME_OVER_ANIM_DURATION = 1.0f;

    ParticleSystem particles;

    std::vector<int> flashRows;
    float flashTimer;

    float gameOverTimer;
    bool  gameOverAnimating;

    void drawBackground() const;
    void drawGridBorder() const;
    void drawGridCells(const GameGrid& grid) const;
    void drawCellBlock(int x, int y, int s, Color color) const;
    void drawCellShadow(int x, int y, int s) const;
    void drawCurrentPiece(const Game& game) const;
    void drawGhostPiece(const Game& game) const;
    void drawPreviewPanel(const Tetromino& nextPiece) const;
    void drawInfoPanel(int score, int highScore, int level, int lines) const;
    void drawControls() const;
    void drawLineClearFlash() const;
    void drawGameOverAnimation() const;
    void drawPauseOverlay() const;
    void drawGameOverOverlay(int score, int highScore) const;

    int calculateGhostY(const Tetromino& piece, int pieceX, int pieceY,
                        const GameGrid& grid) const;
};

#pragma once

#include "raylib.h"
#include "Tetromino.h"
#include <array>
#include <vector>

class GameGrid {
public:
    static constexpr int COLS = 10;
    static constexpr int ROWS = 20;
    static constexpr int CELL_SIZE = 30;

    GameGrid();

    bool isValidMove(const Tetromino& piece, int posX, int posY) const;
    void mergePiece(const Tetromino& piece, int posX, int posY);
    int clearLines(std::vector<int>* outClearedRows = nullptr);
    void draw(int offsetX, int offsetY) const;

    const auto& getGrid() const { return grid; }

private:
    std::array<std::array<Color, COLS>, ROWS> grid;

    bool isRowFull(int row) const;
    void removeRow(int row);
};

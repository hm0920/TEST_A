#include "GameGrid.h"

GameGrid::GameGrid()
{
    for (int r = 0; r < ROWS; ++r)
        for (int c = 0; c < COLS; ++c)
            grid[r][c] = BLANK;
}

bool GameGrid::isValidMove(const Tetromino& piece, int posX, int posY) const
{
    const auto& shape = piece.getShape();

    for (int r = 0; r < 4; ++r)
    {
        for (int c = 0; c < 4; ++c)
        {
            if (shape[r][c] == 0)
                continue;

            int gridCol = posX + c;
            int gridRow = posY + r;

            if (gridCol < 0 || gridCol >= COLS)
                return false;

            if (gridRow >= ROWS)
                return false;

            if (gridRow < 0)
                continue;

            if (grid[gridRow][gridCol].a != 0)
                return false;
        }
    }

    return true;
}

void GameGrid::mergePiece(const Tetromino& piece, int posX, int posY)
{
    const auto& shape = piece.getShape();
    Color color = piece.getColor();

    for (int r = 0; r < 4; ++r)
    {
        for (int c = 0; c < 4; ++c)
        {
            if (shape[r][c] == 0)
                continue;

            int gridCol = posX + c;
            int gridRow = posY + r;

            if (gridRow < 0 || gridRow >= ROWS)
                continue;
            if (gridCol < 0 || gridCol >= COLS)
                continue;

            grid[gridRow][gridCol] = color;
        }
    }
}

int GameGrid::clearLines(std::vector<int>* outClearedRows)
{
    int linesCleared = 0;

    for (int r = ROWS - 1; r >= 0; --r)
    {
        while (isRowFull(r))
        {
            if (outClearedRows)
                outClearedRows->push_back(r);
            removeRow(r);
            ++linesCleared;
        }
    }

    return linesCleared;
}

void GameGrid::draw(int offsetX, int offsetY) const
{
    for (int r = 0; r < ROWS; ++r)
    {
        for (int c = 0; c < COLS; ++c)
        {
            int x = offsetX + c * CELL_SIZE;
            int y = offsetY + r * CELL_SIZE;

            Color cellColor = grid[r][c];

            if (cellColor.a != 0)
            {
                DrawRectangle(x, y, CELL_SIZE, CELL_SIZE, cellColor);
                DrawRectangleLines(x, y, CELL_SIZE, CELL_SIZE, ColorAlpha(WHITE, 0.3f));
            }
            else
            {
                DrawRectangleLines(x, y, CELL_SIZE, CELL_SIZE, ColorAlpha(WHITE, 0.1f));
            }
        }
    }
}

bool GameGrid::isRowFull(int row) const
{
    for (int c = 0; c < COLS; ++c)
    {
        if (grid[row][c].a == 0)
            return false;
    }
    return true;
}

void GameGrid::removeRow(int row)
{
    for (int r = row; r > 0; --r)
        for (int c = 0; c < COLS; ++c)
            grid[r][c] = grid[r - 1][c];

    for (int c = 0; c < COLS; ++c)
        grid[0][c] = BLANK;
}

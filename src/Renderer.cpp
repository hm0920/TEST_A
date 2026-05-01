#include "Renderer.h"
#include "Game.h"
#include <algorithm>

static constexpr Color BG_COLOR        { 20, 20, 30, 255 };
static constexpr Color GRID_BG_COLOR   { 10, 10, 18, 255 };
static constexpr Color PANEL_BG_COLOR  { 16, 16, 24, 255 };
static constexpr Color BORDER_COLOR    { 60, 60, 80, 255 };
static constexpr Color GRID_LINE_COLOR { 34, 34, 50, 255 };
static constexpr Color SHADOW_COLOR    { 0, 0, 0, 80 };

Renderer::Renderer()
    : flashTimer(0.0f)
    , gameOverTimer(0.0f)
    , gameOverAnimating(false)
{
}

void Renderer::triggerPieceLock(const Tetromino& piece, int pieceX, int pieceY)
{
    const auto& shape = piece.getShape();
    Color color = piece.getColor();

    for (int r = 0; r < 4; ++r)
        for (int c = 0; c < 4; ++c)
            if (shape[r][c] != 0)
                particles.emit(pieceX + c, pieceY + r, color,
                               GRID_OFFSET_X, GRID_OFFSET_Y, GameGrid::CELL_SIZE);
}

void Renderer::triggerLineClear(const std::vector<int>& rows)
{
    flashRows = rows;
    flashTimer = FLASH_DURATION;
}

void Renderer::triggerGameOver()
{
    gameOverAnimating = true;
    gameOverTimer = GAME_OVER_ANIM_DURATION;
}

void Renderer::resetEffects()
{
    particles.clear();
    flashRows.clear();
    flashTimer = 0.0f;
    gameOverAnimating = false;
    gameOverTimer = 0.0f;
}

void Renderer::draw(const Game& game)
{
    float dt = GetFrameTime();

    particles.update();

    if (flashTimer > 0.0f)
        flashTimer -= dt;

    if (gameOverAnimating)
    {
        gameOverTimer -= dt;
        if (gameOverTimer <= 0.0f)
        {
            gameOverTimer = 0.0f;
            gameOverAnimating = false;
        }
    }

    ClearBackground(BG_COLOR);

    drawBackground();
    drawGridBorder();
    drawGridCells(game.getGrid());

    drawGhostPiece(game);
    drawCurrentPiece(game);

    drawLineClearFlash();
    particles.draw();

    if (gameOverAnimating)
        drawGameOverAnimation();

    drawPreviewPanel(game.getNextPiece());
    drawInfoPanel(game.getScore(), game.getHighScore(),
                  game.getLevel(), game.getLines());
    drawControls();

    GameState s = game.getState();
    if (s == GameState::Paused)
        drawPauseOverlay();
    else if (s == GameState::GameOver && !gameOverAnimating)
        drawGameOverOverlay(game.getScore(), game.getHighScore());
}

void Renderer::drawBackground() const
{
    DrawRectangleRounded(
        { static_cast<float>(GRID_OFFSET_X),
          static_cast<float>(GRID_OFFSET_Y),
          static_cast<float>(GameGrid::COLS * GameGrid::CELL_SIZE),
          static_cast<float>(GameGrid::ROWS * GameGrid::CELL_SIZE) },
        0.02f, 8, GRID_BG_COLOR);
}

void Renderer::drawGridBorder() const
{
    int x0 = GRID_OFFSET_X - 2;
    int y0 = GRID_OFFSET_Y - 2;
    int w  = GameGrid::COLS * GameGrid::CELL_SIZE + 4;
    int h  = GameGrid::ROWS * GameGrid::CELL_SIZE + 4;

    DrawRectangleRounded(
        { static_cast<float>(x0), static_cast<float>(y0),
          static_cast<float>(w), static_cast<float>(h) },
        0.02f, 8, SHADOW_COLOR);

    DrawRectangleRoundedLines(
        { static_cast<float>(x0), static_cast<float>(y0),
          static_cast<float>(w), static_cast<float>(h) },
        0.02f, 8, 1.5f, BORDER_COLOR);
}

void Renderer::drawCellBlock(int x, int y, int s, Color color) const
{
    Rectangle rect = { static_cast<float>(x), static_cast<float>(y),
                       static_cast<float>(s), static_cast<float>(s) };

    DrawRectangleRounded(rect, CELL_ROUNDNESS, 8, color);

    Color highlight = ColorAlpha(WHITE, 0.18f);
    DrawRectangleRoundedLines(rect, CELL_ROUNDNESS, 8, 1.5f,
                              ColorAlpha(highlight, 0.7f));

    Rectangle inner = { static_cast<float>(x + 1), static_cast<float>(y + 1),
                        static_cast<float>(s - 2), static_cast<float>(s - 2) };
    DrawRectangleRoundedLines(inner, CELL_ROUNDNESS * 0.7f, 8, 1.0f,
                              ColorAlpha(WHITE, 0.10f));
}

void Renderer::drawCellShadow(int x, int y, int s) const
{
    Rectangle rect = { static_cast<float>(x + SHADOW_OFFSET),
                       static_cast<float>(y + SHADOW_OFFSET),
                       static_cast<float>(s), static_cast<float>(s) };
    DrawRectangleRounded(rect, CELL_ROUNDNESS, 8, SHADOW_COLOR);
}

void Renderer::drawGridCells(const GameGrid& grid) const
{
    for (int r = 0; r < GameGrid::ROWS; ++r)
    {
        for (int c = 0; c < GameGrid::COLS; ++c)
        {
            int x = GRID_OFFSET_X + c * GameGrid::CELL_SIZE;
            int y = GRID_OFFSET_Y + r * GameGrid::CELL_SIZE;
            int s = GameGrid::CELL_SIZE;

            Color cell = grid.getGrid()[r][c];

            if (cell.a != 0)
            {
                drawCellBlock(x, y, s, cell);
            }
            else
            {
                DrawRectangleLines(x, y, s, s, GRID_LINE_COLOR);
            }
        }
    }
}

void Renderer::drawCurrentPiece(const Game& game) const
{
    GameState s = game.getState();
    if (s != GameState::Running && s != GameState::Paused)
        return;

    const auto& shape = game.getCurrentPiece().getShape();
    Color color = game.getCurrentPiece().getColor();
    int pieceX = game.getCurrentX();
    int pieceY = game.getCurrentY();
    int cellS = GameGrid::CELL_SIZE;

    for (int r = 0; r < 4; ++r)
    {
        for (int c = 0; c < 4; ++c)
        {
            if (shape[r][c] == 0)
                continue;

            int drawRow = pieceY + r;
            if (drawRow < 0)
                continue;

            int x = GRID_OFFSET_X + (pieceX + c) * cellS;
            int y = GRID_OFFSET_Y + drawRow * cellS;

            drawCellShadow(x, y, cellS);
        }
    }

    for (int r = 0; r < 4; ++r)
    {
        for (int c = 0; c < 4; ++c)
        {
            if (shape[r][c] == 0)
                continue;

            int drawRow = pieceY + r;
            if (drawRow < 0)
                continue;

            int x = GRID_OFFSET_X + (pieceX + c) * cellS;
            int y = GRID_OFFSET_Y + drawRow * cellS;

            drawCellBlock(x, y, cellS, color);
        }
    }
}

void Renderer::drawGhostPiece(const Game& game) const
{
    if (game.getState() != GameState::Running)
        return;

    int ghostY = calculateGhostY(game.getCurrentPiece(),
                                 game.getCurrentX(), game.getCurrentY(),
                                 game.getGrid());
    int pieceY = game.getCurrentY();
    if (ghostY == pieceY)
        return;

    const auto& shape = game.getCurrentPiece().getShape();
    Color baseColor = game.getCurrentPiece().getColor();
    Color ghostColor = ColorAlpha(baseColor, 0.2f);
    int pieceX = game.getCurrentX();
    int s = GameGrid::CELL_SIZE;

    for (int r = 0; r < 4; ++r)
    {
        for (int c = 0; c < 4; ++c)
        {
            if (shape[r][c] == 0)
                continue;

            int drawRow = ghostY + r;
            if (drawRow < 0)
                continue;

            int x = GRID_OFFSET_X + (pieceX + c) * s;
            int y = GRID_OFFSET_Y + drawRow * s;

            Rectangle rect = { static_cast<float>(x), static_cast<float>(y),
                               static_cast<float>(s), static_cast<float>(s) };
            DrawRectangleRounded(rect, CELL_ROUNDNESS, 8, ghostColor);
            DrawRectangleRoundedLines(rect, CELL_ROUNDNESS, 8, 1.5f,
                                      ColorAlpha(baseColor, 0.4f));
        }
    }
}

int Renderer::calculateGhostY(const Tetromino& piece, int pieceX, int pieceY,
                              const GameGrid& grid) const
{
    int ghostY = pieceY;
    while (grid.isValidMove(piece, pieceX, ghostY + 1))
        ++ghostY;
    return ghostY;
}

void Renderer::drawLineClearFlash() const
{
    if (flashTimer <= 0.0f)
        return;

    float progress = flashTimer / FLASH_DURATION;
    float alpha = (progress < 0.5f) ? progress * 2.0f : (1.0f - progress) * 2.0f;

    Color flashColor = ColorAlpha(WHITE, alpha * 0.8f);
    int s = GameGrid::CELL_SIZE;

    for (int row : flashRows)
    {
        int y = GRID_OFFSET_Y + row * s;

        Rectangle rect = { static_cast<float>(GRID_OFFSET_X),
                           static_cast<float>(y),
                           static_cast<float>(GameGrid::COLS * s),
                           static_cast<float>(s) };
        DrawRectangleRounded(rect, CELL_ROUNDNESS * 0.5f, 8, flashColor);
    }
}

void Renderer::drawGameOverAnimation() const
{
    float progress = 1.0f - (gameOverTimer / GAME_OVER_ANIM_DURATION);
    int rowsToFill = static_cast<int>(GameGrid::ROWS * progress);
    int s = GameGrid::CELL_SIZE;

    for (int i = 0; i < rowsToFill; ++i)
    {
        int row = GameGrid::ROWS - 1 - i;
        int y = GRID_OFFSET_Y + row * s;

        for (int c = 0; c < GameGrid::COLS; ++c)
        {
            int x = GRID_OFFSET_X + c * s;
            Color dark = { 30, 30, 30, 200 };
            Rectangle rect = { static_cast<float>(x), static_cast<float>(y),
                               static_cast<float>(s), static_cast<float>(s) };
            DrawRectangleRounded(rect, CELL_ROUNDNESS, 8, dark);
            DrawRectangleRoundedLines(rect, CELL_ROUNDNESS, 8, 1.0f,
                                      ColorAlpha(WHITE, 0.15f));
        }
    }
}

void Renderer::drawPreviewPanel(const Tetromino& nextPiece) const
{
    int panelW = 130;
    int panelH = 110;

    Rectangle panel = { static_cast<float>(PANEL_X),
                        static_cast<float>(GRID_OFFSET_Y),
                        static_cast<float>(panelW),
                        static_cast<float>(panelH) };
    DrawRectangleRounded(panel, PANEL_ROUNDNESS, 8, PANEL_BG_COLOR);
    DrawRectangleRoundedLines(panel, PANEL_ROUNDNESS, 8, 1.5f, BORDER_COLOR);

    DrawText("NEXT", PANEL_X + 8, GRID_OFFSET_Y + 8, 16, ColorAlpha(WHITE, 0.7f));

    const auto& shape = nextPiece.getShape();
    Color color = nextPiece.getColor();
    int s = 20;

    int offsetX = PANEL_X + 15;
    int offsetY = GRID_OFFSET_Y + 32;

    for (int r = 0; r < 4; ++r)
    {
        for (int c = 0; c < 4; ++c)
        {
            if (shape[r][c] == 0)
                continue;

            int x = offsetX + c * s;
            int y = offsetY + r * s;

            Rectangle rect = { static_cast<float>(x), static_cast<float>(y),
                               static_cast<float>(s), static_cast<float>(s) };
            DrawRectangleRounded(rect, CELL_ROUNDNESS * 0.8f, 6, color);
            DrawRectangleRoundedLines(rect, CELL_ROUNDNESS * 0.8f, 6, 1.0f,
                                      ColorAlpha(BLACK, 0.4f));
        }
    }
}

void Renderer::drawInfoPanel(int score, int highScore, int level, int lines) const
{
    int panelW = 130;
    int panelH = 180;
    int panelY = GRID_OFFSET_Y + 125;

    Rectangle panel = { static_cast<float>(PANEL_X),
                        static_cast<float>(panelY),
                        static_cast<float>(panelW),
                        static_cast<float>(panelH) };
    DrawRectangleRounded(panel, PANEL_ROUNDNESS, 8, PANEL_BG_COLOR);
    DrawRectangleRoundedLines(panel, PANEL_ROUNDNESS, 8, 1.5f, BORDER_COLOR);

    int y = panelY + 10;
    int labelColor = ColorAlpha(WHITE, 0.6f);
    int valueColor = WHITE;

    DrawText("SCORE", PANEL_X + 8, y, 14, labelColor);
    DrawText(TextFormat("%d", score), PANEL_X + 8, y + 18, 20, valueColor);

    DrawText("HIGH SCORE", PANEL_X + 8, y + 48, 14, labelColor);
    DrawText(TextFormat("%d", highScore), PANEL_X + 8, y + 66, 20, GOLD);

    DrawText("LEVEL", PANEL_X + 8, y + 96, 14, labelColor);
    DrawText(TextFormat("%d", level), PANEL_X + 8, y + 114, 20, valueColor);

    DrawText("LINES", PANEL_X + 8, y + 144, 14, labelColor);
    DrawText(TextFormat("%d", lines), PANEL_X + 8, y + 162, 20, valueColor);
}

void Renderer::drawControls() const
{
    int panelY = GRID_OFFSET_Y + 320;
    int panelW = 130;
    int panelH = 160;

    Rectangle panel = { static_cast<float>(PANEL_X),
                        static_cast<float>(panelY),
                        static_cast<float>(panelW),
                        static_cast<float>(panelH) };
    DrawRectangleRounded(panel, PANEL_ROUNDNESS, 8, PANEL_BG_COLOR);
    DrawRectangleRoundedLines(panel, PANEL_ROUNDNESS, 8, 1.5f, BORDER_COLOR);

    int y = panelY + 8;
    int titleClr = ColorAlpha(WHITE, 0.6f);
    int keyClr   = ColorAlpha(WHITE, 0.85f);
    int descClr  = ColorAlpha(WHITE, 0.45f);

    DrawText("CONTROLS", PANEL_X + 8, y, 14, titleClr);
    y += 22;

    DrawText("\x81\x9F \x80\x9F", PANEL_X + 12, y, 14, keyClr);
    DrawText("Move", PANEL_X + 60, y, 12, descClr);
    y += 18;

    DrawText("\x82\x9F", PANEL_X + 12, y, 14, keyClr);
    DrawText("Rotate", PANEL_X + 60, y, 12, descClr);
    y += 18;

    DrawText("\x83\x9F", PANEL_X + 12, y, 14, keyClr);
    DrawText("Soft Drop", PANEL_X + 60, y, 12, descClr);
    y += 18;

    DrawText("SPACE", PANEL_X + 12, y, 14, keyClr);
    DrawText("Hard Drop", PANEL_X + 60, y, 12, descClr);
    y += 18;

    DrawText("P", PANEL_X + 12, y, 14, keyClr);
    DrawText("Pause", PANEL_X + 60, y, 12, descClr);
}

void Renderer::drawPauseOverlay() const
{
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, ColorAlpha(BLACK, 0.65f));

    const char* text = "PAUSED";
    int textW = MeasureText(text, 40);
    DrawText(text, (SCREEN_WIDTH - textW) / 2, 290, 40, WHITE);

    const char* hint = "Press P to resume";
    int hintW = MeasureText(hint, 18);
    DrawText(hint, (SCREEN_WIDTH - hintW) / 2, 340, 18, ColorAlpha(WHITE, 0.6f));
}

void Renderer::drawGameOverOverlay(int score, int highScore) const
{
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, ColorAlpha(BLACK, 0.7f));

    const char* title = "GAME OVER";
    int titleW = MeasureText(title, 38);
    DrawText(title, (SCREEN_WIDTH - titleW) / 2, 240, 38, WHITE);

    const char* scoreText = TextFormat("Score: %d", score);
    int scoreW = MeasureText(scoreText, 22);
    DrawText(scoreText, (SCREEN_WIDTH - scoreW) / 2, 295, 22, WHITE);

    const char* highText = TextFormat("High Score: %d", highScore);
    int highW = MeasureText(highText, 18);
    DrawText(highText, (SCREEN_WIDTH - highW) / 2, 330, 18, GOLD);

    const char* hint = "Press R to restart";
    int hintW = MeasureText(hint, 18);
    DrawText(hint, (SCREEN_WIDTH - hintW) / 2, 375, 18, ColorAlpha(WHITE, 0.6f));
}

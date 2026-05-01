#include "Game.h"
#include <algorithm>

Game::Game()
    : currentPiece(TetrominoType::I)
    , nextPiece(TetrominoType::I)
    , highScore(0)
    , rng(std::random_device{}())
{
    reset();
}

void Game::reset()
{
    renderer.resetEffects();

    state = GameState::Running;
    grid = GameGrid();
    score = 0;
    level = 1;
    linesCleared = 0;
    dropTimer = 0.0f;
    dropInterval = INITIAL_DROP_INTERVAL;
    softDropping = false;

    currentPiece = randomPiece();
    nextPiece = randomPiece();
    spawnPiece();
}

Tetromino Game::randomPiece()
{
    static const TetrominoType types[] = {
        TetrominoType::I, TetrominoType::O, TetrominoType::T,
        TetrominoType::L, TetrominoType::J, TetrominoType::S,
        TetrominoType::Z
    };

    std::uniform_int_distribution<int> dist(0, 6);
    return Tetromino(types[dist(rng)]);
}

void Game::spawnPiece()
{
    currentX = SPAWN_X;
    currentY = SPAWN_Y;

    if (!checkCollision(currentPiece, currentX, currentY))
    {
        state = GameState::GameOver;
        renderer.triggerGameOver();
    }
}

void Game::update()
{
    if (state == GameState::Paused)
    {
        if (IsKeyPressed(KEY_P))
            state = GameState::Running;
        return;
    }

    if (state != GameState::Running)
        return;

    input.update();
    softDropping = false;

    for (GameCommand cmd : input.pollCommands())
        executeCommand(cmd);

    updateDrop();
}

void Game::executeCommand(GameCommand cmd)
{
    switch (cmd)
    {
    case GameCommand::MoveLeft:
        movePiece(-1, 0);
        break;

    case GameCommand::MoveRight:
        movePiece(1, 0);
        break;

    case GameCommand::SoftDrop:
        softDropping = true;
        if (checkCollision(currentPiece, currentX, currentY + 1))
            lockPiece();
        else
            currentY += 1;
        break;

    case GameCommand::Rotate:
        rotatePiece();
        break;

    case GameCommand::HardDrop:
        hardDrop();
        break;

    case GameCommand::Pause:
        state = GameState::Paused;
        break;

    case GameCommand::None:
    default:
        break;
    }
}

void Game::updateDrop()
{
    float interval = softDropping ? FAST_DROP_INTERVAL : dropInterval;

    dropTimer += GetFrameTime();

    if (dropTimer >= interval)
    {
        dropTimer = 0.0f;

        if (checkCollision(currentPiece, currentX, currentY + 1))
            lockPiece();
        else
            currentY += 1;
    }
}

bool Game::checkCollision(const Tetromino& piece, int x, int y) const
{
    return !grid.isValidMove(piece, x, y);
}

void Game::movePiece(int dx, int dy)
{
    if (checkCollision(currentPiece, currentX + dx, currentY + dy))
        return;

    currentX += dx;
    currentY += dy;
}

void Game::rotatePiece()
{
    Tetromino rotated = currentPiece;
    rotated.rotate();

    if (!checkCollision(rotated, currentX, currentY))
    {
        currentPiece = rotated;
        return;
    }

    if (!checkCollision(rotated, currentX - 1, currentY))
    {
        currentX -= 1;
        currentPiece = rotated;
        return;
    }

    if (!checkCollision(rotated, currentX + 1, currentY))
    {
        currentX += 1;
        currentPiece = rotated;
        return;
    }

    if (currentPiece.getType() == TetrominoType::I)
    {
        if (!checkCollision(rotated, currentX - 2, currentY))
        {
            currentX -= 2;
            currentPiece = rotated;
            return;
        }
        if (!checkCollision(rotated, currentX + 2, currentY))
        {
            currentX += 2;
            currentPiece = rotated;
            return;
        }
    }
}

void Game::hardDrop()
{
    int dropY = currentY;
    while (!checkCollision(currentPiece, currentX, dropY + 1))
        ++dropY;

    currentY = dropY;
    lockPiece();
}

void Game::lockPiece()
{
    renderer.triggerPieceLock(currentPiece, currentX, currentY);
    grid.mergePiece(currentPiece, currentX, currentY);

    std::vector<int> clearedRows;
    int cleared = grid.clearLines(&clearedRows);
    if (cleared > 0)
    {
        renderer.triggerLineClear(clearedRows);
        linesCleared += cleared;
        addScore(cleared);
        updateLevel();
    }

    currentPiece = nextPiece;
    nextPiece = randomPiece();
    spawnPiece();

    if (state == GameState::GameOver)
        return;

    dropTimer = 0.0f;
}

void Game::addScore(int cleared)
{
    static const int points[] = { 0, 100, 300, 500, 800 };
    score += points[cleared] * level;
    if (score > highScore)
        highScore = score;
}

void Game::updateLevel()
{
    int newLevel = linesCleared / 10 + 1;
    if (newLevel > level)
    {
        level = newLevel;
        dropInterval = std::max(0.1f, INITIAL_DROP_INTERVAL - (level - 1) * 0.1f);
    }
}

void Game::draw()
{
    renderer.draw(*this);
}

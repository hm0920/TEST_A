#include "InputHandler.h"

InputHandler::InputHandler()
{
    leftState    = { false, 0.0f, false };
    rightState   = { false, 0.0f, false };
    softDropState = { false, 0.0f, false };
}

void InputHandler::updateDASKey(DASKeyState& state, int key, bool alternativeKey)
{
    bool pressed = IsKeyDown(key) || (alternativeKey && IsKeyDown(KEY_S));

    if (pressed)
    {
        if (!state.held)
        {
            state.held = true;
            state.holdTime = 0.0f;
            state.dasTriggered = false;
        }
        else
        {
            state.holdTime += GetFrameTime();
        }
    }
    else
    {
        state.held = false;
        state.holdTime = 0.0f;
        state.dasTriggered = false;
    }
}

bool InputHandler::shouldFireDAS(DASKeyState& state, float arrInterval) const
{
    if (!state.held)
        return false;

    if (!state.dasTriggered)
    {
        if (state.holdTime >= DAS_DELAY)
        {
            state.dasTriggered = true;
            state.holdTime = 0.0f;
            return true;
        }
        return false;
    }

    if (state.holdTime >= arrInterval)
    {
        state.holdTime = 0.0f;
        return true;
    }

    return false;
}

void InputHandler::update()
{
    updateDASKey(leftState, KEY_LEFT, false);
    updateDASKey(rightState, KEY_RIGHT, false);
    updateDASKey(softDropState, KEY_DOWN, true);
}

std::vector<GameCommand> InputHandler::pollCommands() const
{
    std::vector<GameCommand> commands;

    bool leftJustPressed = IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A);
    bool rightJustPressed = IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D);

    if (leftJustPressed)
        commands.push_back(GameCommand::MoveLeft);

    if (rightJustPressed)
        commands.push_back(GameCommand::MoveRight);

    DASKeyState leftCopy = leftState;
    DASKeyState rightCopy = rightState;
    DASKeyState softCopy = softDropState;

    if (shouldFireDAS(leftCopy, ARR_INTERVAL))
        commands.push_back(GameCommand::MoveLeft);

    if (shouldFireDAS(rightCopy, ARR_INTERVAL))
        commands.push_back(GameCommand::MoveRight);

    if (shouldFireDAS(softCopy, SOFT_DROP_ARR))
        commands.push_back(GameCommand::SoftDrop);

    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W))
        commands.push_back(GameCommand::Rotate);

    if (IsKeyPressed(KEY_SPACE))
        commands.push_back(GameCommand::HardDrop);

    if (IsKeyPressed(KEY_P))
        commands.push_back(GameCommand::Pause);

    return commands;
}

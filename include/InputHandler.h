#pragma once

#include "raylib.h"
#include <vector>

enum class GameCommand {
    None,
    MoveLeft,
    MoveRight,
    SoftDrop,
    Rotate,
    HardDrop,
    Pause
};

struct DASKeyState {
    bool held;
    float holdTime;
    bool dasTriggered;
};

class InputHandler {
public:
    static constexpr float DAS_DELAY       = 0.17f;
    static constexpr float ARR_INTERVAL    = 0.05f;
    static constexpr float SOFT_DROP_ARR   = 0.03f;

    InputHandler();

    void update();
    std::vector<GameCommand> pollCommands() const;

private:
    DASKeyState leftState;
    DASKeyState rightState;
    DASKeyState softDropState;

    void updateDASKey(DASKeyState& state, int key, bool alternativeKey);
    bool shouldFireDAS(DASKeyState& state, float arrInterval) const;
};

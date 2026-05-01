#pragma once

#include "raylib.h"
#include <array>

enum class TetrominoType {
    I, O, T, L, J, S, Z
};

class Tetromino {
public:
    using Shape = std::array<std::array<int, 4>, 4>;

    explicit Tetromino(TetrominoType type);

    void rotate();
    const Shape& getShape() const;
    Color getColor() const;
    TetrominoType getType() const;

private:
    TetrominoType type;
    Shape shape;
    Color color;

    static Shape createShape(TetrominoType type);
    static Color getColorForType(TetrominoType type);
};

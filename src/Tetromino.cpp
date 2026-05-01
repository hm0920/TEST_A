#include "Tetromino.h"

Tetromino::Tetromino(TetrominoType t)
    : type(t), shape(createShape(t)), color(getColorForType(t))
{
}

void Tetromino::rotate()
{
    Shape rotated{};

    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            rotated[j][3 - i] = shape[i][j];

    shape = rotated;
}

const Tetromino::Shape& Tetromino::getShape() const
{
    return shape;
}

Color Tetromino::getColor() const
{
    return color;
}

TetrominoType Tetromino::getType() const
{
    return type;
}

Tetromino::Shape Tetromino::createShape(TetrominoType type)
{
    Shape s{};

    switch (type)
    {
    case TetrominoType::I:
        s = {{
            {0, 0, 0, 0},
            {1, 1, 1, 1},
            {0, 0, 0, 0},
            {0, 0, 0, 0}
        }};
        break;

    case TetrominoType::O:
        s = {{
            {1, 1, 0, 0},
            {1, 1, 0, 0},
            {0, 0, 0, 0},
            {0, 0, 0, 0}
        }};
        break;

    case TetrominoType::T:
        s = {{
            {0, 1, 0, 0},
            {1, 1, 1, 0},
            {0, 0, 0, 0},
            {0, 0, 0, 0}
        }};
        break;

    case TetrominoType::L:
        s = {{
            {1, 0, 0, 0},
            {1, 1, 1, 0},
            {0, 0, 0, 0},
            {0, 0, 0, 0}
        }};
        break;

    case TetrominoType::J:
        s = {{
            {0, 0, 1, 0},
            {1, 1, 1, 0},
            {0, 0, 0, 0},
            {0, 0, 0, 0}
        }};
        break;

    case TetrominoType::S:
        s = {{
            {0, 1, 1, 0},
            {1, 1, 0, 0},
            {0, 0, 0, 0},
            {0, 0, 0, 0}
        }};
        break;

    case TetrominoType::Z:
        s = {{
            {1, 1, 0, 0},
            {0, 1, 1, 0},
            {0, 0, 0, 0},
            {0, 0, 0, 0}
        }};
        break;
    }

    return s;
}

Color Tetromino::getColorForType(TetrominoType type)
{
    switch (type)
    {
    case TetrominoType::I: return SKYBLUE;
    case TetrominoType::O: return YELLOW;
    case TetrominoType::T: return PURPLE;
    case TetrominoType::L: return ORANGE;
    case TetrominoType::J: return BLUE;
    case TetrominoType::S: return GREEN;
    case TetrominoType::Z: return RED;
    default:              return WHITE;
    }
}

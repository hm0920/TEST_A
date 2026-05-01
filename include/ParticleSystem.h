#pragma once

#include "raylib.h"
#include <vector>

struct Particle {
    Vector2 position;
    Vector2 velocity;
    float lifetime;
    float maxLifetime;
    Color color;
    float size;
};

class ParticleSystem {
public:
    void emit(int gridCol, int gridRow, Color color,
              int gridOffsetX, int gridOffsetY, int cellSize);
    void update();
    void draw() const;
    bool isEmpty() const;
    void clear();

private:
    std::vector<Particle> particles;
};

#include "ParticleSystem.h"
#include <algorithm>
#include <cmath>
#include <cstdlib>

static float randomFloat(float min, float max)
{
    return min + static_cast<float>(rand()) / RAND_MAX * (max - min);
}

void ParticleSystem::emit(int gridCol, int gridRow, Color color,
                          int gridOffsetX, int gridOffsetY, int cellSize)
{
    float cx = gridOffsetX + gridCol * cellSize + cellSize / 2.0f;
    float cy = gridOffsetY + gridRow * cellSize + cellSize / 2.0f;

    int count = 6;
    for (int i = 0; i < count; ++i)
    {
        float angle = randomFloat(0.0f, 2.0f * PI);
        float speed = randomFloat(40.0f, 120.0f);

        Particle p;
        p.position = { cx, cy };
        p.velocity = { cosf(angle) * speed, sinf(angle) * speed - 60.0f };
        p.lifetime = randomFloat(0.3f, 0.7f);
        p.maxLifetime = p.lifetime;
        p.color = color;
        p.size = randomFloat(2.0f, 5.0f);
        particles.push_back(p);
    }
}

void ParticleSystem::update()
{
    float dt = GetFrameTime();

    for (auto& p : particles)
    {
        p.lifetime -= dt;
        p.position.x += p.velocity.x * dt;
        p.position.y += p.velocity.y * dt;
        p.velocity.y += 200.0f * dt;
    }

    particles.erase(
        std::remove_if(particles.begin(), particles.end(),
                       [](const Particle& p) { return p.lifetime <= 0.0f; }),
        particles.end());
}

void ParticleSystem::draw() const
{
    for (const auto& p : particles)
    {
        float alpha = p.lifetime / p.maxLifetime;
        Color c = ColorAlpha(p.color, alpha);
        float s = p.size * alpha;
        DrawRectangle(static_cast<int>(p.position.x - s / 2),
                      static_cast<int>(p.position.y - s / 2),
                      static_cast<int>(s), static_cast<int>(s), c);
    }
}

bool ParticleSystem::isEmpty() const
{
    return particles.empty();
}

void ParticleSystem::clear()
{
    particles.clear();
}

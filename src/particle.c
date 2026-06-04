#include "particle.h"
#include "raylib.h"
#include <math.h>

// Simple LCG for deterministic "random" in C without stdlib rand side-effects
static unsigned int seed = 12345;
static float frand(void) {
    seed = seed * 1664525u + 1013904223u;
    return (float)(seed >> 16) / 65535.0f;
}

void particle_emit(Particle particles[], Vector2 pos, Color color,
                   int count, float speed, float life) {
    int spawned = 0;
    for (int i = 0; i < MAX_PARTICLES && spawned < count; i++) {
        if (particles[i].active) continue;
        float angle = frand() * 6.2832f;
        float spd   = speed * (0.3f + frand() * 0.7f);
        particles[i].pos     = pos;
        particles[i].vel.x   = cosf(angle) * spd;
        particles[i].vel.y   = sinf(angle) * spd;
        particles[i].color   = color;
        particles[i].life    = life * (0.5f + frand() * 0.5f);
        particles[i].maxLife = particles[i].life;
        particles[i].size    = 2.0f + frand() * 4.0f;
        particles[i].active  = true;
        spawned++;
    }
}

void particle_update_all(Particle particles[], float dt) {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (!particles[i].active) continue;
        particles[i].pos.x += particles[i].vel.x * dt;
        particles[i].pos.y += particles[i].vel.y * dt;
        particles[i].vel.x *= (1.0f - 2.0f * dt); // drag
        particles[i].vel.y *= (1.0f - 2.0f * dt);
        particles[i].life  -= dt;
        if (particles[i].life <= 0) particles[i].active = false;
    }
}

void particle_draw_all(const Particle particles[]) {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (!particles[i].active) continue;
        float alpha = particles[i].life / particles[i].maxLife;
        Color c = particles[i].color;
        c.a = (unsigned char)(alpha * 255);
        float sz = particles[i].size * alpha;
        DrawCircleV(particles[i].pos, sz, c);
    }
}

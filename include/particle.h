#ifndef PARTICLE_H
#define PARTICLE_H

#include "game.h"

void particle_emit(Particle particles[], Vector2 pos, Color color,
                   int count, float speed, float life);
void particle_update_all(Particle particles[], float dt);
void particle_draw_all(const Particle particles[]);

#endif

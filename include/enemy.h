#ifndef ENEMY_H
#define ENEMY_H

#include "game.h"

void enemy_spawn_wave(Enemy enemies[], int wave, float speedMultiplier);
void enemy_update_all(Enemy enemies[], Bullet bullets[], Particle particles[],
                      const Player *player, float dt, int wave, int *enemiesLeft);
void enemy_draw_all(const Enemy enemies[]);
int  enemy_count_active(const Enemy enemies[]);

#endif

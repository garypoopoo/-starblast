#ifndef BULLET_H
#define BULLET_H

#include "game.h"

void bullet_fire(Bullet bullets[], Vector2 pos, Vector2 vel, bool fromPlayer);
void bullet_update_all(Bullet bullets[], float dt);
void bullet_draw_all(const Bullet bullets[]);

#endif

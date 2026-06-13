#ifndef POWERUP_H
#define POWERUP_H

#include "game.h"

void powerup_spawn(PowerUp powerups[], Vector2 pos);
void powerup_update_all(PowerUp powerups[], float dt);
void powerup_draw_all(const PowerUp powerups[]);
void powerup_check_collect(PowerUp powerups[], Player *player);

#endif

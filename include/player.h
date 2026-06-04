#ifndef PLAYER_H
#define PLAYER_H

#include "game.h"

void player_init(Player *p);
void player_update(Player *p, Bullet bullets[], float dt);
void player_draw(const Player *p);
void player_take_damage(Player *p, float dmg);

#endif

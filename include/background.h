#ifndef BACKGROUND_H
#define BACKGROUND_H

#include "game.h"

void background_init(Star stars[]);
void background_update(Star stars[], float dt);
void background_draw(const Star stars[]);

#endif

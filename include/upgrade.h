#ifndef UPGRADE_H
#define UPGRADE_H

#include "game.h"

void upgrade_generate(UpgradeOption options[3]);
void upgrade_apply(Game *g, int type);

#endif

#ifndef UI_H
#define UI_H

#include "game.h"

void ui_draw_hud(const Game *g);
void ui_draw_menu(void);
void ui_draw_pause(void);
void ui_draw_gameover(int score, int highScore, bool isNewBest);
void ui_draw_win(int score, int wave);
void ui_draw_upgrade(const UpgradeOption options[3]);

#endif

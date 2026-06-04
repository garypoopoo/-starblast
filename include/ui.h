#ifndef UI_H
#define UI_H

#include "game.h"

void ui_draw_hud(const Game *g);
void ui_draw_menu(void);
void ui_draw_pause(void);
void ui_draw_gameover(int score);
void ui_draw_win(int score, int wave);

#endif

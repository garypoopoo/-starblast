#include "ui.h"
#include "raylib.h"
#include <stdio.h>
#include <math.h>    // sinf() used in ui_draw_menu pulsing animation

#define COL_ACCENT  (Color){80, 220, 255, 255}
#define COL_RED     (Color){255, 70, 70, 255}
#define COL_GOLD    (Color){255, 210, 50, 255}
#define COL_DARK    (Color){10, 10, 28, 220}

// ─── HUD ─────────────────────────────────────────────────────────────────────
void ui_draw_hud(const Game *g) {
    char buf[64];

    // HP bar background
    DrawRectangle(10, 10, 202, 18, COL_DARK);
    float hpRatio = g->player.hp / g->player.maxHp;
    Color hpColor = hpRatio > 0.5f ? (Color){60,240,80,255}
                  : hpRatio > 0.25f? (Color){240,200,40,255}
                  : COL_RED;
    DrawRectangle(11, 11, (int)(200*hpRatio), 16, hpColor);
    DrawRectangleLines(10, 10, 202, 18, COL_ACCENT);
    DrawText("HP", 14, 13, 12, WHITE);

    // Lives
    for (int i = 0; i < g->player.lives; i++) {
        float lx = 12.0f + i * 22.0f;
        Vector2 v1 = {lx+6,  34};
        Vector2 v2 = {lx,    50};
        Vector2 v3 = {lx+12, 50};
        DrawTriangle(v1, v2, v3, (Color){60,200,255,200});
    }

    // Bombs (orange circles with "B" label, below lives)
    for (int i = 0; i < g->player.bombs; i++) {
        int bx = 18 + i * 24;
        int by = 66;
        DrawCircle(bx, by, 9, (Color){255, 160, 40, 220});
        DrawCircle(bx, by, 5, (Color){255, 200, 80, 180});
        DrawText("B", bx - 3, by - 5, 10, WHITE);
    }

    // Score
    snprintf(buf, sizeof(buf), "SCORE %07d", g->player.score);
    DrawText(buf, SCREEN_W/2 - MeasureText(buf,20)/2, 10, 20, COL_GOLD);

    // Combo (only when > 1, flashing yellow)
    if (g->player.combo > 1) {
        float flash = 0.5f + 0.5f * sinf((float)GetTime() * 8.0f);
        Color comboCol = {255, 255, 50, (unsigned char)(flash * 255)};
        snprintf(buf, sizeof(buf), "x%d COMBO!", g->player.combo);
        int tw = MeasureText(buf, 24);
        DrawText(buf, SCREEN_W/2 - tw/2, 38, 24, comboCol);
    }

    // Wave
    snprintf(buf, sizeof(buf), "WAVE %d", g->wave);
    DrawText(buf, SCREEN_W - 110, 10, 20, COL_ACCENT);

    // Weapon level indicator
    snprintf(buf, sizeof(buf), "WPN Lv.%d", g->player.weaponLevel);
    DrawText(buf, SCREEN_W - 110, 34, 16, (Color){60, 255, 140, 255});

    // Wave clear banner
    if (g->waveClear) {
        const char *msg = "WAVE CLEAR!";
        int tw = MeasureText(msg, 36);
        DrawRectangle(SCREEN_W/2-tw/2-12, SCREEN_H/2-30, tw+24, 50,
                      (Color){0,0,0,160});
        DrawText(msg, SCREEN_W/2-tw/2, SCREEN_H/2-22, 36, COL_GOLD);
    } else if (g->wave % 5 == 0) {
        float alpha = 0.5f + 0.5f * sinf((float)GetTime() * 5.0f);
        Color warnCol = {255, 50, 50, (unsigned char)(alpha * 255)};
        const char *msg = "⚠ BOSS WAVE ⚠";
        int tw = MeasureText(msg, 32);
        DrawText(msg, SCREEN_W/2 - tw/2, 80, 32, warnCol);
    }
}

// ─── Helper: centered text button appearance ──────────────────────────────────
static void draw_label(const char *text, int y, int size, Color col) {
    int tw = MeasureText(text, size);
    DrawText(text, SCREEN_W/2 - tw/2, y, size, col);
}

// ─── MENU ────────────────────────────────────────────────────────────────────
void ui_draw_menu(void) {
    // Title glow
    const char *t1 = "STAR";
    const char *t2 = "BLAST";
    int tw1 = MeasureText(t1, 80), tw2 = MeasureText(t2, 80);
    int totalW = tw1 + tw2 + 4;
    int startX = SCREEN_W/2 - totalW/2;

    // Glow behind title
    for (int r = 30; r >= 0; r -= 5) {
        DrawText(t1, startX,      120, 80, (Color){40,160,255,(unsigned char)(r*4)});
        DrawText(t2, startX+tw1+4,120, 80, (Color){255,80,80,(unsigned char)(r*4)});
    }
    DrawText(t1, startX,       120, 80, (Color){100,220,255,255});
    DrawText(t2, startX+tw1+4, 120, 80, (Color){255,100,100,255});

    draw_label("A top-down space shooter",      230, 20, (Color){160,200,220,255});
    draw_label("─────────────────────────────", 260, 18, (Color){60,100,120,200});

    draw_label("WASD / Arrow Keys  →  Move",    300, 18, WHITE);
    draw_label("SPACE / Z          →  Shoot",   325, 18, WHITE);
    draw_label("X                  →  Bomb",     350, 18, (Color){255, 180, 60, 255});
    draw_label("P / ESC            →  Pause",   375, 18, WHITE);

    draw_label("─────────────────────────────", 405, 18, (Color){60,100,120,200});

    // Pulsing start prompt
    float alpha = 0.5f + 0.5f * sinf((float)GetTime() * 3.0f);
    draw_label("PRESS ENTER TO START", 445, 28,
               (Color){80,220,255,(unsigned char)(alpha*255)});

    draw_label("v2.0  –  Made with raylib", SCREEN_H - 30, 16,
               (Color){80,100,120,255});
}

// ─── PAUSE ───────────────────────────────────────────────────────────────────
void ui_draw_pause(void) {
    DrawRectangle(0, 0, SCREEN_W, SCREEN_H, (Color){0,0,0,140});
    draw_label("PAUSED",         SCREEN_H/2 - 60, 50, COL_ACCENT);
    draw_label("Press P to resume",  SCREEN_H/2 + 10, 22, WHITE);
    draw_label("Press ESC for menu", SCREEN_H/2 + 40, 22, (Color){180,180,180,255});
}

// ─── GAME OVER ────────────────────────────────────────────────────────────────
void ui_draw_gameover(int score, int highScore, bool isNewBest) {
    char buf[64];
    DrawRectangle(0, 0, SCREEN_W, SCREEN_H, (Color){0,0,0,180});
    draw_label("GAME OVER",         SCREEN_H/2 - 90, 60, COL_RED);
    snprintf(buf, sizeof(buf), "FINAL SCORE: %d", score);
    draw_label(buf,                 SCREEN_H/2 - 10, 28, COL_GOLD);
    
    if (isNewBest) {
        draw_label("★ NEW BEST! ★",   SCREEN_H/2 + 25, 24, (Color){255, 255, 50, 255});
    } else {
        snprintf(buf, sizeof(buf), "BEST: %d", highScore);
        draw_label(buf,                 SCREEN_H/2 + 25, 24, (Color){180, 180, 180, 255});
    }

    draw_label("Press ENTER to retry", SCREEN_H/2 + 70, 22, WHITE);
    draw_label("Press ESC for menu",   SCREEN_H/2 + 100, 22, (Color){180,180,180,255});
}

// ─── WIN ─────────────────────────────────────────────────────────────────────
void ui_draw_win(int score, int wave) {
    char buf[64];
    DrawRectangle(0, 0, SCREEN_W, SCREEN_H, (Color){0,0,0,160});
    draw_label("YOU WIN!",          SCREEN_H/2 - 110, 60, COL_GOLD);
    snprintf(buf, sizeof(buf), "Cleared %d waves!", wave);
    draw_label(buf,                 SCREEN_H/2 - 30,  28, COL_ACCENT);
    snprintf(buf, sizeof(buf), "FINAL SCORE: %d", score);
    draw_label(buf,                 SCREEN_H/2 + 10,  28, COL_GOLD);
    draw_label("Press ENTER to play again", SCREEN_H/2 + 60, 22, WHITE);
    draw_label("Press ESC for menu",         SCREEN_H/2 + 90, 22, (Color){180,180,180,255});
}

// ─── UPGRADE ─────────────────────────────────────────────────────────────────
void ui_draw_upgrade(const UpgradeOption options[3]) {
    DrawRectangle(0, 0, SCREEN_W, SCREEN_H, (Color){0,0,0,200});
    draw_label("CHOOSE UPGRADE", 100, 50, COL_ACCENT);

    int boxW = 240;
    int boxH = 300;
    int gap = 30;
    int startX = SCREEN_W/2 - (boxW*3 + gap*2)/2;
    int startY = 220;

    for (int i = 0; i < 3; i++) {
        int x = startX + i * (boxW + gap);
        
        // Background
        DrawRectangle(x, startY, boxW, boxH, (Color){20, 30, 50, 255});
        DrawRectangleLines(x, startY, boxW, boxH, (Color){80, 120, 180, 255});
        
        // Key prompt
        char keyBuf[16];
        snprintf(keyBuf, sizeof(keyBuf), "PRESS %d", i+1);
        DrawRectangle(x, startY - 30, boxW, 30, (Color){40, 60, 100, 255});
        int tw = MeasureText(keyBuf, 20);
        DrawText(keyBuf, x + boxW/2 - tw/2, startY - 25, 20, WHITE);
        
        // Label
        int lw = MeasureText(options[i].label, 24);
        DrawText(options[i].label, x + boxW/2 - lw/2, startY + 40, 24, COL_GOLD);
        
        // Description
        int dw = MeasureText(options[i].desc, 18);
        DrawText(options[i].desc, x + boxW/2 - dw/2, startY + 140, 18, WHITE);
    }
}

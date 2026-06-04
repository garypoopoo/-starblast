#include "game.h"
#include "player.h"
#include "enemy.h"
#include "bullet.h"
#include "particle.h"
#include "background.h"
#include "collision.h"
#include "ui.h"
#include "raylib.h"
#include <string.h>

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

#define MAX_WAVES        5
#define WAVE_CLEAR_DELAY 2.5f

static Game g;

static void game_reset(Game *g) {
    memset(g, 0, sizeof(Game));
    g->state = STATE_PLAYING;
    g->wave  = 1;
    player_init(&g->player);
    background_init(g->stars);
    enemy_spawn_wave(g->enemies, g->wave);
    g->enemiesLeft = enemy_count_active(g->enemies);
}

static void update_draw_frame(void) {
    float dt = GetFrameTime();
    g.dt = dt;

    switch (g.state) {

    case STATE_MENU:
        background_update(g.stars, dt);
        if (IsKeyPressed(KEY_ENTER)) game_reset(&g);
        break;

    case STATE_PLAYING: {
        background_update(g.stars, dt);
        if (IsKeyPressed(KEY_P) || IsKeyPressed(KEY_ESCAPE))
            g.state = STATE_PAUSED;

        if (!g.waveClear) {
            player_update(&g.player, g.bullets, dt);
            enemy_update_all(g.enemies, g.bullets, g.particles, &g.player, dt);
            bullet_update_all(g.bullets, dt);
            particle_update_all(g.particles, dt);
            collision_check(&g);

            if (g.player.hp <= 0) {
                g.player.lives--;
                if (g.player.lives <= 0) {
                    g.state = STATE_GAMEOVER;
                } else {
                    g.player.hp    = g.player.maxHp;
                    g.player.pos.x = SCREEN_W / 2.0f;
                    g.player.pos.y = SCREEN_H - 80.0f;
                    g.player.invincible = true;
                    g.player.invTimer   = 2.5f;
                    memset(g.bullets, 0, sizeof(g.bullets)); // Fix #5: clear lingering bullets on respawn
                }
            }

            if (enemy_count_active(g.enemies) == 0) {
                g.waveClear = true;
                g.waveTimer = WAVE_CLEAR_DELAY;
            }
        } else {
            g.waveTimer -= dt;
            particle_update_all(g.particles, dt);
            bullet_update_all(g.bullets, dt);

            if (g.waveTimer <= 0.0f) {
                g.wave++;
                if (g.wave > MAX_WAVES) {
                    g.state = STATE_WIN;
                } else {
                    g.waveClear = false;
                    memset(g.bullets, 0, sizeof(g.bullets));
                    enemy_spawn_wave(g.enemies, g.wave);
                    g.enemiesLeft = enemy_count_active(g.enemies);
                }
            }
        }
        break;
    }

    case STATE_PAUSED:
        if (IsKeyPressed(KEY_P)) g.state = STATE_PLAYING;
        if (IsKeyPressed(KEY_ESCAPE)) g.state = STATE_MENU;
        break;

    case STATE_GAMEOVER:
        background_update(g.stars, dt);
        particle_update_all(g.particles, dt);
        if (IsKeyPressed(KEY_ENTER)) game_reset(&g);
        if (IsKeyPressed(KEY_ESCAPE)) { memset(&g,0,sizeof(g)); g.state=STATE_MENU; background_init(g.stars); }
        break;

    case STATE_WIN:
        background_update(g.stars, dt);
        if (IsKeyPressed(KEY_ENTER)) game_reset(&g);
        if (IsKeyPressed(KEY_ESCAPE)) { memset(&g,0,sizeof(g)); g.state=STATE_MENU; background_init(g.stars); }
        break;
    }

    BeginDrawing();
    background_draw(g.stars);
    switch (g.state) {
    case STATE_MENU:
        ui_draw_menu();
        break;
    case STATE_PLAYING:
    case STATE_PAUSED:
        particle_draw_all(g.particles);
        bullet_draw_all(g.bullets);
        enemy_draw_all(g.enemies);
        player_draw(&g.player);
        ui_draw_hud(&g);
        if (g.state == STATE_PAUSED) ui_draw_pause();
        break;
    case STATE_GAMEOVER:
        particle_draw_all(g.particles);
        ui_draw_gameover(g.player.score);
        break;
    case STATE_WIN:
        ui_draw_win(g.player.score, g.wave - 1);
        break;
    }
    EndDrawing();
}

int main(void) {
    InitWindow(SCREEN_W, SCREEN_H, TITLE);
    SetTargetFPS(TARGET_FPS);
    SetExitKey(0);

    memset(&g, 0, sizeof(g));
    g.state = STATE_MENU;
    background_init(g.stars);

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(update_draw_frame, 0, 1);
#else
    while (!WindowShouldClose())
        update_draw_frame();
    CloseWindow();
#endif
    return 0;
}

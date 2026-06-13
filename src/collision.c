#include "collision.h"
#include "particle.h"
#include "player.h"    // Fix: was missing – needed for player_take_damage()
#include "powerup.h"
#include "raylib.h"
#include <math.h>

static float dist2(Vector2 a, Vector2 b) {
    float dx = a.x - b.x, dy = a.y - b.y;
    return dx*dx + dy*dy;
}

// Fix #1 #2: centralise death-particle logic so every kill path triggers it
static void enemy_kill(Game *g, int e) {
    Color c = (g->enemies[e].type == ENEMY_BOSS) ? (Color){255,200, 50,255}
            : (g->enemies[e].type == ENEMY_TANK) ? (Color){255,100, 50,255}
            :                                      (Color){100,220,255,255};
    particle_emit(g->particles, g->enemies[e].pos, c,                       20, 200.0f, 0.8f);
    particle_emit(g->particles, g->enemies[e].pos, (Color){255,200,100,255},10, 120.0f, 1.2f);

    // 20% chance to spawn power-up at death position
    powerup_spawn(g->powerups, g->enemies[e].pos);

    g->enemies[e].active = false;   // deactivate immediately (not deferred to enemy_update_all)
    if (g->enemiesLeft > 0) g->enemiesLeft--;  // Fix #3 #9: keep counter in sync
}

void collision_check(Game *g) {
    // ── Bomb: instant screen-clear ────────────────────────────────────────────
    if (g->bombActive) {
        for (int e = 0; e < MAX_ENEMIES; e++) {
            if (!g->enemies[e].active) continue;
            g->enemies[e].hp -= 999.0f;
            if (g->enemies[e].hp <= 0) {
                // Score for bomb kills (no combo)
                int pts = (g->enemies[e].type == ENEMY_BOSS)     ? 500
                        : (g->enemies[e].type == ENEMY_TANK)     ? 150
                        : (g->enemies[e].type == ENEMY_SHIELDER) ? 120
                        : (g->enemies[e].type == ENEMY_SNIPER)   ? 100
                        : (g->enemies[e].type == ENEMY_KAMIKAZE) ?  80
                        : (g->enemies[e].type == ENEMY_ZIGZAG)   ?  60
                        : 50;
                g->player.score += pts;
                enemy_kill(g, e);
            }
        }
        // Clear all enemy bullets
        for (int b = 0; b < MAX_BULLETS; b++) {
            if (g->bullets[b].active && !g->bullets[b].fromPlayer)
                g->bullets[b].active = false;
        }
        // Bomb flash particles at screen center
        Vector2 center = { SCREEN_W / 2.0f, SCREEN_H / 2.0f };
        particle_emit(g->particles, center, (Color){255,255,200,255}, 40, 400.0f, 0.6f);
        g->bombActive = false;
    }

    // ── Player bullets vs enemies ─────────────────────────────────────────────
    for (int b = 0; b < MAX_BULLETS; b++) {
        if (!g->bullets[b].active || !g->bullets[b].fromPlayer) continue;

        for (int e = 0; e < MAX_ENEMIES; e++) {
            if (!g->enemies[e].active) continue;

            float r = (g->enemies[e].type == ENEMY_BOSS) ? 28.0f
                    : (g->enemies[e].type == ENEMY_TANK) ? 20.0f : 16.0f;

            if (dist2(g->bullets[b].pos, g->enemies[e].pos) < r*r) {
                if (g->enemies[e].type == ENEMY_SHIELDER && g->enemies[e].shieldActive) {
                    if (g->bullets[b].vel.y > 0) {
                        g->bullets[b].active = false;
                        continue;
                    }
                    g->enemies[e].shieldActive = false;
                }

                g->bullets[b].active = false;

                float dmg = (g->enemies[e].type == ENEMY_BOSS) ? 10.0f : 20.0f;
                g->enemies[e].hp -= dmg;

                // Small hit spark
                particle_emit(g->particles, g->bullets[b].pos,
                               (Color){255,255,100,255}, 6, 150.0f, 0.3f);

                if (g->enemies[e].hp <= 0) {
                    int pts = (g->enemies[e].type == ENEMY_BOSS)     ? 500
                            : (g->enemies[e].type == ENEMY_TANK)     ? 150
                            : (g->enemies[e].type == ENEMY_SHIELDER) ? 120
                            : (g->enemies[e].type == ENEMY_SNIPER)   ? 100
                            : (g->enemies[e].type == ENEMY_KAMIKAZE) ?  80
                            : (g->enemies[e].type == ENEMY_ZIGZAG)   ?  60
                            : 50;
                    // Combo multiplier
                    int multiplier = g->player.combo > 1 ? g->player.combo : 1;
                    g->player.score += pts * multiplier;
                    g->player.combo++;
                    g->player.comboTimer = g->player.comboWindow;
                    enemy_kill(g, e); // Fix #1 #2: immediate death + particles
                }
                break;
            }
        }
    }

    // ── Enemy bullets vs player ───────────────────────────────────────────────
    for (int b = 0; b < MAX_BULLETS; b++) {
        if (!g->bullets[b].active || g->bullets[b].fromPlayer) continue;

        if (dist2(g->bullets[b].pos, g->player.pos) < 18.0f*18.0f) {
            g->bullets[b].active = false;
            player_take_damage(&g->player, 20.0f);
            particle_emit(g->particles, g->player.pos,
                          (Color){80,160,255,255}, 10, 180.0f, 0.5f);
            g->hitFlashTimer = 0.1f;
        }
    }

    // ── Enemies collide with player ───────────────────────────────────────────
    for (int e = 0; e < MAX_ENEMIES; e++) {
        if (!g->enemies[e].active) continue;
        if (dist2(g->enemies[e].pos, g->player.pos) < 30.0f*30.0f) {
            enemy_kill(g, e);              // Fix #2: was only hp=0, no particles
            float contactDmg = (g->enemies[e].type == ENEMY_KAMIKAZE) ? 80.0f : 40.0f;
            player_take_damage(&g->player, contactDmg);
            g->hitFlashTimer = 0.1f;
        }
    }
}

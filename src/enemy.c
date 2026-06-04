#include "enemy.h"
#include "bullet.h"
#include "particle.h"
#include "raylib.h"
#include <math.h>

// Simple LCG
static unsigned int eseed = 98765;
static float efrand(void) {
    eseed = eseed * 1664525u + 1013904223u;
    return (float)(eseed >> 16) / 65535.0f;
}

// ─── Spawn a formation for the given wave ────────────────────────────────────
void enemy_spawn_wave(Enemy enemies[], int wave) {
    // Clear old enemies
    for (int i = 0; i < MAX_ENEMIES; i++) enemies[i].active = false;

    int count   = 5 + (wave - 1) * 3;
    if (count > MAX_ENEMIES - 1) count = MAX_ENEMIES - 1;

    int cols    = 8;
    float startX= 80.0f;
    float gapX  = (SCREEN_W - 160.0f) / (float)(cols - 1);

    for (int i = 0; i < count; i++) {
        int col = i % cols;
        int row = i / cols;

        EnemyType type;
        if (wave >= 3 && i == 0) type = ENEMY_BOSS;  // one boss per wave ≥ 3
        else if (efrand() < 0.25f + wave * 0.05f) type = ENEMY_TANK;
        else type = ENEMY_SCOUT;

        enemies[i].pos.x       = startX + col * gapX;
        enemies[i].pos.y       = 60.0f + row * 70.0f;
        enemies[i].vel.x       = (efrand() > 0.5f ? 1.0f : -1.0f) * (40.0f + wave * 10.0f);
        enemies[i].vel.y       = 0.0f;
        enemies[i].type        = type;
        enemies[i].active      = true;
        enemies[i].shootTimer  = efrand() * 2.0f;

        switch (type) {
            case ENEMY_SCOUT:
                enemies[i].hp = enemies[i].maxHp = 20.0f + wave * 5;
                enemies[i].shootCooldown = 2.0f - wave * 0.1f;
                if (enemies[i].shootCooldown < 0.8f) enemies[i].shootCooldown = 0.8f;
                break;
            case ENEMY_TANK:
                enemies[i].hp = enemies[i].maxHp = 60.0f + wave * 10;
                enemies[i].shootCooldown = 3.0f;
                break;
            case ENEMY_BOSS:
                enemies[i].hp = enemies[i].maxHp = 200.0f + wave * 30;
                enemies[i].shootCooldown = 0.6f;
                enemies[i].vel.x = 80.0f;
                enemies[i].pos.x = SCREEN_W / 2.0f;
                enemies[i].pos.y = 80.0f;
                break;
        }
    }
}

// ─── Update all enemies ───────────────────────────────────────────────────────
void enemy_update_all(Enemy enemies[], Bullet bullets[], Particle particles[],
                      const Player *player, float dt) {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!enemies[i].active) continue;

        Enemy *e = &enemies[i];

        // Movement
        e->pos.x += e->vel.x * dt;
        if (e->type == ENEMY_BOSS) {
            // Boss drifts toward player slowly
            float dx = player->pos.x - e->pos.x;
            e->pos.y += 20.0f * dt;
            e->vel.x += dx * 0.5f * dt;
        }

        // Bounce off walls
        if (e->pos.x < 30)          { e->pos.x = 30;          e->vel.x = fabsf(e->vel.x); }
        if (e->pos.x > SCREEN_W-30) { e->pos.x = SCREEN_W-30; e->vel.x = -fabsf(e->vel.x); }

        // If enemy reaches bottom, they pass through (counted as missed)
        if (e->pos.y > SCREEN_H + 40) e->active = false;

        // Shooting
        e->shootTimer -= dt;
        if (e->shootTimer <= 0.0f) {
            Vector2 bpos = { e->pos.x, e->pos.y + 20 };

            if (e->type == ENEMY_BOSS) {
                // Boss fires 3-spread
                for (int s = -1; s <= 1; s++) {
                    Vector2 vel = { s * 80.0f, 320.0f };
                    bullet_fire(bullets, bpos, vel, false);
                }
            } else {
                // Aim loosely at player
                float dx = player->pos.x - bpos.x;
                float dy = player->pos.y - bpos.y;
                float dist = sqrtf(dx*dx + dy*dy);
                if (dist < 1) dist = 1;
                float spd = 280.0f;
                Vector2 vel = { dx/dist * spd, dy/dist * spd };
                bullet_fire(bullets, bpos, vel, false);
            }

            e->shootTimer = e->shootCooldown * (0.8f + efrand() * 0.4f);
        }

        // Death check
        if (e->hp <= 0) {
            e->active = false;
            Color c = (e->type == ENEMY_BOSS) ? (Color){255,200,50,255}
                    : (e->type == ENEMY_TANK) ? (Color){255,100,50,255}
                    : (Color){100,220,255,255};
            particle_emit(particles, e->pos, c, 20, 200.0f, 0.8f);
            particle_emit(particles, e->pos, (Color){255,200,100,255}, 10, 120.0f, 1.2f);
        }
    }
}

// ─── Draw all enemies ─────────────────────────────────────────────────────────
static void draw_healthbar(Vector2 pos, float hp, float maxHp, float w) {
    float ratio = hp / maxHp;
    DrawRectangle((int)(pos.x - w/2), (int)(pos.y - 28), (int)w, 5,
                  (Color){60,60,60,200});
    DrawRectangle((int)(pos.x - w/2), (int)(pos.y - 28), (int)(w*ratio), 5,
                  (Color){80,240,80,255});
}

void enemy_draw_all(const Enemy enemies[]) {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!enemies[i].active) continue;

        float x = enemies[i].pos.x;
        float y = enemies[i].pos.y;

        switch (enemies[i].type) {
            case ENEMY_SCOUT: {
                // Small red triangle (pointing down)
                Vector2 v1 = { x,      y + 16 };
                Vector2 v2 = { x - 14, y - 10 };
                Vector2 v3 = { x + 14, y - 10 };
                DrawTriangle(v1, v2, v3, (Color){220, 60, 80, 255});
                DrawCircle((int)x, (int)(y+2), 5, (Color){255, 120, 140, 255});
                draw_healthbar(enemies[i].pos, enemies[i].hp, enemies[i].maxHp, 30);
                break;
            }
            case ENEMY_TANK: {
                // Wider hexagon-ish shape
                DrawRectangle((int)(x-18), (int)(y-14), 36, 28, (Color){160, 80, 220, 255});
                DrawRectangle((int)(x-12), (int)(y-20), 24, 8,  (Color){200,120,255,255});
                DrawCircle((int)x, (int)y, 8, (Color){220,160,255,255});
                draw_healthbar(enemies[i].pos, enemies[i].hp, enemies[i].maxHp, 40);
                break;
            }
            case ENEMY_BOSS: {
                // Larger imposing ship
                DrawRectangle((int)(x-30), (int)(y-20), 60, 40, (Color){200,150,20,255});
                DrawRectangle((int)(x-50), (int)(y- 4), 100,12, (Color){240,200,40,255});
                DrawCircle((int)x, (int)y, 16, (Color){255,230,80,255});
                DrawCircle((int)x, (int)y,  8, (Color){255,255,180,255});
                // Pulsing glow
                float pulse = (float)GetTime() * 3.0f;
                int glow = (int)(40 + 30*sinf(pulse));
                DrawCircle((int)x, (int)y, 22, (Color){255,200,0,(unsigned char)glow});
                draw_healthbar(enemies[i].pos, enemies[i].hp, enemies[i].maxHp, 80);
                break;
            }
        }
    }
}

int enemy_count_active(const Enemy enemies[]) {
    int n = 0;
    for (int i = 0; i < MAX_ENEMIES; i++)
        if (enemies[i].active) n++;
    return n;
}

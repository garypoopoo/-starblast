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
void enemy_spawn_wave(Enemy enemies[], int wave, float speedMultiplier) {
    eseed = (unsigned int)GetTime() ^ (unsigned int)wave;
    // Clear old enemies
    for (int i = 0; i < MAX_ENEMIES; i++) enemies[i].active = false;

    int count   = 5 + wave * 2;
    if (count > MAX_ENEMIES - 1) count = MAX_ENEMIES - 1;

    int cols    = 8;
    float startX= 80.0f;
    float gapX  = (SCREEN_W - 160.0f) / (float)(cols - 1);

    if (wave % 5 == 0) {
        count = wave / 5;
        if (count > 4) count = 4;
        if (count < 1) count = 1;

        for (int i = 0; i < count; i++) {
            enemies[i].pos.x       = SCREEN_W / (float)(count + 1) * (i + 1);
            enemies[i].pos.y       = 80.0f + i * 60.0f;
            enemies[i].vel.x       = 80.0f * speedMultiplier * (i % 2 == 0 ? 1 : -1);
            enemies[i].vel.y       = 0.0f;
            enemies[i].type        = ENEMY_BOSS;
            enemies[i].active      = true;
            enemies[i].shootTimer  = efrand() * 2.0f;
            enemies[i].spinTimer   = 1.0f;
            enemies[i].chargeTimer = 0.0f;
            enemies[i].zigPhase    = 0.0f;
            enemies[i].canDive     = false;
            enemies[i].diving      = false;
            enemies[i].shieldActive = false;
            enemies[i].launched    = false;
            enemies[i].hp = enemies[i].maxHp = 200.0f + wave * 30;
            enemies[i].shootCooldown = 0.6f;
        }
    } else {
        for (int i = 0; i < count; i++) {
            int col = i % cols;
            int row = i / cols;

            EnemyType pool[7];
            int poolSize = 0;
            pool[poolSize++] = ENEMY_SCOUT;
            pool[poolSize++] = ENEMY_SCOUT;
            if (wave >= 2) pool[poolSize++] = ENEMY_TANK;
            if (wave >= 3) pool[poolSize++] = ENEMY_SNIPER;
            if (wave >= 4) pool[poolSize++] = ENEMY_ZIGZAG;
            if (wave >= 5) pool[poolSize++] = ENEMY_KAMIKAZE;
            if (wave >= 6) pool[poolSize++] = ENEMY_SHIELDER;
            EnemyType type = pool[(int)(efrand() * poolSize)];

            enemies[i].pos.x       = startX + col * gapX;
            enemies[i].pos.y       = 60.0f + row * 70.0f;
            enemies[i].vel.x       = (efrand() > 0.5f ? 1.0f : -1.0f) * (40.0f + wave * 10.0f) * speedMultiplier;
            enemies[i].vel.y       = 0.0f;
            enemies[i].type        = type;
            enemies[i].active      = true;
            enemies[i].shootTimer  = efrand() * 2.0f;
            enemies[i].spinTimer   = 1.0f;
            enemies[i].chargeTimer = 0.0f;
            enemies[i].zigPhase    = 0.0f;
            enemies[i].canDive     = false;
            enemies[i].diving      = false;
            enemies[i].shieldActive = false;
            enemies[i].launched    = false;

            switch (type) {
                case ENEMY_SCOUT:
                    enemies[i].hp = enemies[i].maxHp = 20.0f + wave * 5;
                    enemies[i].shootCooldown = 2.0f - wave * 0.1f;
                    if (enemies[i].shootCooldown < 0.8f) enemies[i].shootCooldown = 0.8f;
                    if (efrand() < 0.25f) enemies[i].canDive = true;
                    break;
                case ENEMY_TANK:
                    enemies[i].hp = enemies[i].maxHp = 60.0f + wave * 10;
                    enemies[i].shootCooldown = 3.0f;
                    break;
                case ENEMY_BOSS:
                    enemies[i].hp = enemies[i].maxHp = 200.0f + wave * 30;
                    enemies[i].shootCooldown = 0.6f;
                    enemies[i].vel.x = 80.0f * speedMultiplier;
                    enemies[i].pos.x = SCREEN_W / 2.0f;
                    enemies[i].pos.y = 80.0f;
                    enemies[i].spinTimer   = 1.0f;
                    break;
                case ENEMY_SNIPER:
                    enemies[i].hp = enemies[i].maxHp = 30.0f + wave * 8;
                    enemies[i].shootCooldown = 0.3f;
                    enemies[i].chargeTimer = 2.0f;
                    enemies[i].vel.x = (efrand() > 0.5f ? 1.0f : -1.0f) * 20.0f;
                    break;
                case ENEMY_ZIGZAG:
                    enemies[i].hp = enemies[i].maxHp = 25.0f + wave * 4;
                    enemies[i].shootCooldown = 999.0f;
                    enemies[i].vel.x = (efrand() > 0.5f ? 1.0f : -1.0f) * 160.0f * speedMultiplier;
                    enemies[i].vel.y = 60.0f * speedMultiplier;
                    enemies[i].zigPhase = efrand() * 6.28f;
                    break;
                case ENEMY_SHIELDER:
                    enemies[i].hp = enemies[i].maxHp = 80.0f + wave * 12;
                    enemies[i].shootCooldown = 2.5f;
                    enemies[i].shieldActive = true;
                    enemies[i].vel.x = (efrand() > 0.5f ? 1.0f : -1.0f) * 50.0f * speedMultiplier;
                    break;
                case ENEMY_KAMIKAZE:
                    enemies[i].hp = enemies[i].maxHp = 15.0f + wave * 3;
                    enemies[i].shootCooldown = 999.0f;
                    enemies[i].vel.x = 0.0f;
                    enemies[i].vel.y = 0.0f;
                    enemies[i].launched = false;
                    break;
            }
        }
    }
}

// ─── Update all enemies ───────────────────────────────────────────────────────
void enemy_update_all(Enemy enemies[], Bullet bullets[], Particle particles[],
                      const Player *player, float dt, int wave, int *enemiesLeft) {
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

        if (e->type == ENEMY_SNIPER) {
            e->chargeTimer -= dt;
            if (e->chargeTimer <= 0.0f && e->shootTimer <= 0.0f) {
                float dx = player->pos.x - e->pos.x;
                float dy = player->pos.y - e->pos.y;
                float dist = sqrtf(dx*dx + dy*dy);
                if (dist < 1) dist = 1;
                Vector2 vel = { dx/dist * 500.0f, dy/dist * 500.0f };
                bullet_fire(bullets, (Vector2){e->pos.x, e->pos.y + 10}, vel, false);
                e->shootTimer = e->shootCooldown;
                e->chargeTimer = 1.8f;
                e->vel.x = (efrand() > 0.5f ? 1.0f : -1.0f) * 20.0f;
            }
        }

        if (e->type == ENEMY_ZIGZAG) {
            e->zigPhase += dt * 3.0f;
            e->pos.x += cosf(e->zigPhase) * 150.0f * dt;
            e->pos.y += e->vel.y * dt;
        }

        if (e->type == ENEMY_KAMIKAZE && !e->launched && e->pos.y > SCREEN_H * 0.25f) {
            e->launched = true;
            float dx = player->pos.x - e->pos.x;
            float dy = player->pos.y - e->pos.y;
            float dist = sqrtf(dx*dx + dy*dy);
            if (dist < 1) dist = 1;
            float speedMultiplier = powf(1.08f, (float)(wave - 1));
            float spd = 380.0f * speedMultiplier;
            e->vel.x = dx/dist * spd;
            e->vel.y = dy/dist * spd;
        }

        // If enemy reaches bottom, they pass through (counted as missed)
        if (e->pos.y > SCREEN_H + 40) {
            e->active = false;
            if (enemiesLeft && *enemiesLeft > 0) (*enemiesLeft)--;
        }

        // Scout diving logic
        if (e->type == ENEMY_SCOUT && e->canDive && !e->diving && e->pos.y > SCREEN_H * 0.5f) {
            e->diving = true;
            e->vel.y = 250.0f;
            e->vel.x *= 0.5f;
        }

        // Shooting
        e->shootTimer -= dt;
        if (e->shootTimer <= 0.0f &&
            e->type != ENEMY_SNIPER &&
            e->type != ENEMY_ZIGZAG &&
            e->type != ENEMY_KAMIKAZE) {
            Vector2 bpos = { e->pos.x, e->pos.y + 20 };

            if (e->type == ENEMY_BOSS) {
                if (e->hp / e->maxHp < 0.5f) {
                    // Phase 2: 5-spread
                    for (int s = -2; s <= 2; s++) {
                        Vector2 vel = { s * 80.0f, 320.0f };
                        bullet_fire(bullets, bpos, vel, false);
                    }
                    e->shootTimer = e->shootCooldown * 0.6f * (0.8f + efrand() * 0.4f);
                } else {
                    // Boss fires 3-spread
                    for (int s = -1; s <= 1; s++) {
                        Vector2 vel = { s * 80.0f, 320.0f };
                        bullet_fire(bullets, bpos, vel, false);
                    }
                    e->shootTimer = e->shootCooldown * (0.8f + efrand() * 0.4f);
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
                e->shootTimer = e->shootCooldown * (0.8f + efrand() * 0.4f);
            }
        }

        // ── Boss rotating bullet ring (wave >= 6) ────────────────────────────
        if (e->type == ENEMY_BOSS && wave >= 6) {
            e->spinTimer -= dt;
            if (e->spinTimer <= 0.0f) {
                float spinSpd = 200.0f;
                for (int d = 0; d < 8; d++) {
                    float angle = (float)d * (6.2832f / 8.0f);
                    Vector2 vel = { cosf(angle) * spinSpd, sinf(angle) * spinSpd };
                    bullet_fire(bullets, e->pos, vel, false);
                }
                e->spinTimer = 0.4f;
            }
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
                bool phase2 = (enemies[i].hp / enemies[i].maxHp) < 0.5f;
                Color bodyCol = phase2 ? (Color){220,60,30,255} : (Color){200,150,20,255};
                Color detailCol = phase2 ? (Color){255,100,50,255} : (Color){240,200,40,255};

                // Larger imposing ship
                DrawRectangle((int)(x-30), (int)(y-20), 60, 40, bodyCol);
                DrawRectangle((int)(x-50), (int)(y- 4), 100,12, detailCol);
                DrawCircle((int)x, (int)y, 16, (Color){255,230,80,255});
                DrawCircle((int)x, (int)y,  8, (Color){255,255,180,255});

                // Pulsing glow
                float pulse = (float)GetTime() * 3.0f;
                int glow = (int)(40 + 30*sinf(pulse));
                if (phase2) {
                    DrawCircle((int)x, (int)y, 22, (Color){255,50,0,(unsigned char)glow});
                } else {
                    DrawCircle((int)x, (int)y, 22, (Color){255,200,0,(unsigned char)glow});
                }
                draw_healthbar(enemies[i].pos, enemies[i].hp, enemies[i].maxHp, 80);
                break;
            }
            case ENEMY_SNIPER: {
                const Enemy *e = &enemies[i];
                bool charging = (e->chargeTimer > 0.5f);
                Color col = charging ? (Color){40,100,220,255} : (Color){20,60,180,255};
                Vector2 pts[4] = {
                    {x, y-20}, {x+8, y}, {x, y+20}, {x-8, y}
                };
                DrawTriangle(pts[0], pts[1], pts[2], col);
                DrawTriangle(pts[0], pts[3], pts[2], col);
                if (charging) {
                    DrawCircleLines((int)x, (int)y, 6+(int)(e->chargeTimer*4), (Color){200,200,255,120});
                }
                draw_healthbar(e->pos, e->hp, e->maxHp, 30);
                break;
            }
            case ENEMY_ZIGZAG: {
                const Enemy *e = &enemies[i];
                Color col = {240, 140, 20, 255};
                DrawTriangle((Vector2){x,y+12}, (Vector2){x-18,y-10}, (Vector2){x,y-2}, col);
                DrawTriangle((Vector2){x,y+12}, (Vector2){x+18,y-10}, (Vector2){x,y-2}, col);
                DrawCircle((int)x, (int)y, 4, (Color){255,220,100,255});
                draw_healthbar(e->pos, e->hp, e->maxHp, 30);
                break;
            }
            case ENEMY_SHIELDER: {
                const Enemy *e = &enemies[i];
                DrawRectangle((int)(x-16), (int)(y-14), 32, 28, (Color){40,180,80,255});
                DrawCircle((int)x, (int)y, 8, (Color){120,255,160,255});
                if (e->shieldActive) {
                    DrawCircleSector((Vector2){x,y}, 24, 200, 340, 12, (Color){100,220,255,160});
                }
                draw_healthbar(e->pos, e->hp, e->maxHp, 40);
                break;
            }
            case ENEMY_KAMIKAZE: {
                const Enemy *e = &enemies[i];
                Color col = e->launched ? (Color){255,50,50,255} : (Color){180,30,30,255};
                Vector2 v1 = {x, y+18}, v2 = {x-10, y-14}, v3 = {x+10, y-14};
                DrawTriangle(v1, v2, v3, col);
                if (e->launched) {
                    DrawCircle((int)(x - e->vel.x*0.03f), (int)(y - e->vel.y*0.03f), 5, (Color){255,100,50,120});
                    DrawCircle((int)(x - e->vel.x*0.06f), (int)(y - e->vel.y*0.06f), 3, (Color){255,80,30,60});
                }
                draw_healthbar(e->pos, e->hp, e->maxHp, 24);
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

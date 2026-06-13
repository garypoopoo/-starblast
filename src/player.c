#include "player.h"
#include "bullet.h"
#include "raylib.h"
#include <math.h>

#define PLAYER_SPEED        300.0f
#define PLAYER_SHOOT_CD     0.18f
#define PLAYER_HP           100.0f
#define PLAYER_INVINCIBLE_T 0.8f
#define BULLET_SPEED        520.0f

void player_init(Player *p) {
    p->pos.x        = SCREEN_W / 2.0f;
    p->pos.y        = SCREEN_H - 80.0f;
    p->hp           = PLAYER_HP;
    p->maxHp        = PLAYER_HP;
    p->speed        = PLAYER_SPEED;
    p->shootTimer   = 0.0f;
    p->shootCooldown= PLAYER_SHOOT_CD;
    p->score        = 0;
    p->lives        = 3;
    p->invincible   = false;
    p->invTimer     = 0.0f;
    p->weaponLevel  = 1;
    p->combo        = 0;
    p->comboTimer   = 0.0f;
    p->bombs        = 3;
    p->shield       = false;
    p->speedBoostTimer = 0.0f;
    p->permSpeedBonus  = 0.0f;
    p->comboWindow     = 3.0f;
}

void player_update(Game *g, float dt) {
    Player *p = &g->player;
    Bullet *bullets = g->bullets;

    p->speed = (p->speedBoostTimer > 0.0f) ? (450.0f + p->permSpeedBonus) : (300.0f + p->permSpeedBonus);
    if (p->speedBoostTimer > 0.0f) {
        p->speedBoostTimer -= dt;
    }

    // ── Movement ──────────────────────────────────────────────────────────────
    if (IsKeyDown(KEY_LEFT)  || IsKeyDown(KEY_A))
        p->pos.x -= p->speed * dt;
    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D))
        p->pos.x += p->speed * dt;
    if (IsKeyDown(KEY_UP)    || IsKeyDown(KEY_W))
        p->pos.y -= p->speed * dt;
    if (IsKeyDown(KEY_DOWN)  || IsKeyDown(KEY_S))
        p->pos.y += p->speed * dt;

    // Clamp to screen
    if (p->pos.x < 20)          p->pos.x = 20;
    if (p->pos.x > SCREEN_W-20) p->pos.x = SCREEN_W-20;
    if (p->pos.y < 20)          p->pos.y = 20;
    if (p->pos.y > SCREEN_H-20) p->pos.y = SCREEN_H-20;

    // ── Shooting (weapon-level dependent) ─────────────────────────────────────
    p->shootTimer -= dt;
    if ((IsKeyDown(KEY_SPACE) || IsKeyDown(KEY_Z)) && p->shootTimer <= 0.0f) {
        Vector2 pos = p->pos;

        if (p->weaponLevel == 1) {
            // Single shot
            Vector2 vel = { 0, -BULLET_SPEED };
            bullet_fire(bullets, pos, vel, true);
        } else if (p->weaponLevel == 2) {
            // Double shot: left and right offset
            Vector2 posL = { pos.x - 10, pos.y };
            Vector2 posR = { pos.x + 10, pos.y };
            Vector2 vel  = { 0, -BULLET_SPEED };
            bullet_fire(bullets, posL, vel, true);
            bullet_fire(bullets, posR, vel, true);
        } else {
            // Level 3: center + left/right + two angled shots
            Vector2 velC = { 0, -BULLET_SPEED };
            bullet_fire(bullets, pos, velC, true);

            Vector2 posL = { pos.x - 10, pos.y };
            Vector2 posR = { pos.x + 10, pos.y };
            Vector2 velS = { 0, -BULLET_SPEED };
            bullet_fire(bullets, posL, velS, true);
            bullet_fire(bullets, posR, velS, true);

            Vector2 velAL = { -80.0f, -BULLET_SPEED };
            Vector2 velAR = {  80.0f, -BULLET_SPEED };
            bullet_fire(bullets, pos, velAL, true);
            bullet_fire(bullets, pos, velAR, true);
        }

        p->shootTimer = p->shootCooldown;
    }

    // ── Bomb (KEY_X) ──────────────────────────────────────────────────────────
    if (IsKeyPressed(KEY_X) && p->bombs > 0) {
        g->bombActive = true;
        p->bombs--;
    }

    // ── Invincibility timer ───────────────────────────────────────────────────
    if (p->invincible) {
        p->invTimer -= dt;
        if (p->invTimer <= 0.0f)
            p->invincible = false;
    }
}

void player_draw(const Player *p) {
    // Flash during invincibility
    if (p->invincible && (int)(p->invTimer * 10) % 2 == 0) return;

    float x = p->pos.x, y = p->pos.y;

    // Engine glow
    DrawCircle((int)x, (int)(y + 14), 8, (Color){80, 160, 255, 120});

    // Main body (triangle ship)
    Vector2 v1 = { x,       y - 22 };
    Vector2 v2 = { x - 16,  y + 16 };
    Vector2 v3 = { x + 16,  y + 16 };
    DrawTriangle(v1, v2, v3, (Color){60, 200, 255, 255});

    // Cockpit
    DrawCircle((int)x, (int)(y - 4), 5, (Color){200, 240, 255, 255});

    // Wing accents
    Vector2 wa1 = { x - 16, y + 16 };
    Vector2 wa2 = { x - 26, y + 10 };
    Vector2 wa3 = { x - 8,  y + 6  };
    DrawTriangle(wa1, wa2, wa3, (Color){30, 140, 220, 255});

    Vector2 wb1 = { x + 16, y + 16 };
    Vector2 wb2 = { x + 8,  y + 6  };
    Vector2 wb3 = { x + 26, y + 10 };
    DrawTriangle(wb1, wb2, wb3, (Color){30, 140, 220, 255});

    if (p->shield) {
        DrawCircle((int)x, (int)y, 24, (Color){80, 160, 255, 80});
        DrawCircleLines((int)x, (int)y, 24, (Color){100, 200, 255, 180});
    }
}

void player_take_damage(Player *p, float dmg) {
    if (p->invincible) return;
    if (p->shield) {
        p->shield = false;
        p->invincible = true;
        p->invTimer   = PLAYER_INVINCIBLE_T;
        return;
    }
    p->hp -= dmg;
    p->invincible = true;
    p->invTimer   = PLAYER_INVINCIBLE_T;
}

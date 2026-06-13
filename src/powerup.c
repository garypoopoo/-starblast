#include "powerup.h"
#include "raylib.h"
#include <math.h>

// Simple LCG (same pattern as other modules)
static unsigned int pseed = 77777;
static float pfrand(void) {
    pseed = pseed * 1664525u + 1013904223u;
    return (float)(pseed >> 16) / 65535.0f;
}

#define POWERUP_SPEED     100.0f
#define POWERUP_COLLECT_R  24.0f

void powerup_spawn(PowerUp powerups[], Vector2 pos) {
    pseed = (unsigned int)GetTime() ^ (unsigned int)(pos.x * 1000.0f);
    // 20% chance to spawn
    if (pfrand() > 0.20f) return;

    for (int i = 0; i < MAX_POWERUPS; i++) {
        if (!powerups[i].active) {
            powerups[i].pos    = pos;
            powerups[i].vel_y  = POWERUP_SPEED;
            powerups[i].active = true;
            
            float r = pfrand();
            if (r < 0.30f) powerups[i].type = 0;
            else if (r < 0.55f) powerups[i].type = 1;
            else if (r < 0.75f) powerups[i].type = 2;
            else if (r < 0.90f) powerups[i].type = 3;
            else powerups[i].type = 4;
            
            return;
        }
    }
}

void powerup_update_all(PowerUp powerups[], float dt) {
    for (int i = 0; i < MAX_POWERUPS; i++) {
        if (!powerups[i].active) continue;
        powerups[i].pos.y += powerups[i].vel_y * dt;
        // Off-screen removal
        if (powerups[i].pos.y > SCREEN_H + 20)
            powerups[i].active = false;
    }
}

void powerup_draw_all(const PowerUp powerups[]) {
    for (int i = 0; i < MAX_POWERUPS; i++) {
        if (!powerups[i].active) continue;

        float x = powerups[i].pos.x;
        float y = powerups[i].pos.y;
        float t = (float)GetTime();

        // 0=Weapon(W), 1=HP(H), 2=Shield(S), 3=Bomb(B), 4=Speed(V)
        Color glowCol = WHITE, tri1Col = WHITE, tri2Col = WHITE;
        const char *label = "?";
        switch (powerups[i].type) {
            case 0: // Weapon - Green
                glowCol = (Color){80, 255, 120, 60};
                tri1Col = (Color){60, 255, 140, 230};
                tri2Col = (Color){40, 220, 120, 230};
                label = "W";
                break;
            case 1: // HP - Light Green
                glowCol = (Color){100, 255, 100, 60};
                tri1Col = (Color){80, 255, 80, 230};
                tri2Col = (Color){60, 220, 60, 230};
                label = "H";
                break;
            case 2: // Shield - Blue
                glowCol = (Color){80, 120, 255, 60};
                tri1Col = (Color){60, 140, 255, 230};
                tri2Col = (Color){40, 120, 220, 230};
                label = "S";
                break;
            case 3: // Bomb - Orange
                glowCol = (Color){255, 160, 80, 60};
                tri1Col = (Color){255, 180, 60, 230};
                tri2Col = (Color){220, 140, 40, 230};
                label = "B";
                break;
            case 4: // Speed - Yellow
                glowCol = (Color){255, 255, 80, 60};
                tri1Col = (Color){255, 255, 60, 230};
                tri2Col = (Color){220, 220, 40, 230};
                label = "V";
                break;
        }

        // Outer glow (pulsing)
        float pulse = 0.6f + 0.4f * sinf(t * 4.0f);
        DrawCircle((int)x, (int)y, 16 * pulse, glowCol);

        // Rotating diamond shape
        float angle = t * 3.0f;
        float cs = cosf(angle), sn = sinf(angle);
        float sz = 10.0f;
        Vector2 v1 = { x + cs * sz,       y + sn * sz       };
        Vector2 v2 = { x + (-sn) * sz,    y + cs * sz        };
        Vector2 v3 = { x + (-cs) * sz,    y + (-sn) * sz     };
        Vector2 v4 = { x + sn * sz,       y + (-cs) * sz     };

        DrawTriangle(v1, v2, v3, tri1Col);
        DrawTriangle(v1, v3, v4, tri2Col);

        // Label
        DrawText(label, (int)x - 4, (int)y - 5, 12, WHITE);
    }
}

void powerup_check_collect(PowerUp powerups[], Player *player) {
    for (int i = 0; i < MAX_POWERUPS; i++) {
        if (!powerups[i].active) continue;

        float dx = powerups[i].pos.x - player->pos.x;
        float dy = powerups[i].pos.y - player->pos.y;
        if (dx*dx + dy*dy < POWERUP_COLLECT_R * POWERUP_COLLECT_R) {
            powerups[i].active = false;
            switch (powerups[i].type) {
                case 0:
                    if (player->weaponLevel < 3) player->weaponLevel++;
                    break;
                case 1:
                    player->hp += 30.0f;
                    if (player->hp > player->maxHp) player->hp = player->maxHp;
                    break;
                case 2:
                    player->shield = true;
                    break;
                case 3:
                    player->bombs++;
                    break;
                case 4:
                    player->speedBoostTimer = 10.0f;
                    break;
            }
        }
    }
}

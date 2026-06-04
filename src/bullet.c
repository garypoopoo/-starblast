#include "bullet.h"
#include "raylib.h"

void bullet_fire(Bullet bullets[], Vector2 pos, Vector2 vel, bool fromPlayer) {
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (!bullets[i].active) {
            bullets[i].pos        = pos;
            bullets[i].vel        = vel;
            bullets[i].active     = true;
            bullets[i].fromPlayer = fromPlayer;
            return;
        }
    }
}

void bullet_update_all(Bullet bullets[], float dt) {
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (!bullets[i].active) continue;
        bullets[i].pos.x += bullets[i].vel.x * dt;
        bullets[i].pos.y += bullets[i].vel.y * dt;

        // Deactivate if off-screen
        if (bullets[i].pos.x < -10 || bullets[i].pos.x > SCREEN_W + 10 ||
            bullets[i].pos.y < -10 || bullets[i].pos.y > SCREEN_H + 10) {
            bullets[i].active = false;
        }
    }
}

void bullet_draw_all(const Bullet bullets[]) {
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (!bullets[i].active) continue;
        if (bullets[i].fromPlayer) {
            // Player bullet: bright cyan bolt
            DrawRectangle((int)bullets[i].pos.x - 2,
                          (int)bullets[i].pos.y - 10, 4, 20,
                          (Color){120, 255, 255, 255});
            DrawRectangle((int)bullets[i].pos.x - 1,
                          (int)bullets[i].pos.y - 12, 2, 4,
                          WHITE);
        } else {
            // Enemy bullet: red/orange bolt
            DrawRectangle((int)bullets[i].pos.x - 2,
                          (int)bullets[i].pos.y - 8, 4, 16,
                          (Color){255, 80, 40, 255});
            DrawCircle((int)bullets[i].pos.x,
                       (int)bullets[i].pos.y, 3,
                       (Color){255, 160, 60, 200});
        }
    }
}

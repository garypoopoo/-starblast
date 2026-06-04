#include "background.h"
#include "raylib.h"

static unsigned int bseed = 55555;
static float bfrand(void) {
    bseed = bseed * 1664525u + 1013904223u;
    return (float)(bseed >> 16) / 65535.0f;
}

void background_init(Star stars[]) {
    for (int i = 0; i < MAX_STARS; i++) {
        stars[i].pos.x     = bfrand() * SCREEN_W;
        stars[i].pos.y     = bfrand() * SCREEN_H;
        stars[i].speed     = 20.0f + bfrand() * 80.0f;
        stars[i].brightness= 0.3f  + bfrand() * 0.7f;
        stars[i].size      = 1.0f  + bfrand() * 2.5f;
    }
}

void background_update(Star stars[], float dt) {
    for (int i = 0; i < MAX_STARS; i++) {
        stars[i].pos.y += stars[i].speed * dt;
        if (stars[i].pos.y > SCREEN_H + 5) {
            stars[i].pos.y = -5;
            stars[i].pos.x = bfrand() * SCREEN_W;
        }
    }
}

void background_draw(const Star stars[]) {
    ClearBackground((Color){5, 5, 18, 255});
    for (int i = 0; i < MAX_STARS; i++) {
        unsigned char b = (unsigned char)(stars[i].brightness * 220);
        unsigned char blue = ((int)b + 30 > 255) ? 255 : (unsigned char)((int)b + 30); // Fix #8: clamp to avoid overflow
        DrawCircleV(stars[i].pos, stars[i].size, (Color){b, b, blue, 255});
    }
}

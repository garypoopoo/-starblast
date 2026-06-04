#ifndef GAME_H
#define GAME_H

#include "raylib.h"
#include <stdbool.h>

// ─── Window ───────────────────────────────────────────────────────────────────
#define SCREEN_W   900
#define SCREEN_H   700
#define TITLE      "STARBLAST"
#define TARGET_FPS  60

// ─── Game limits ──────────────────────────────────────────────────────────────
#define MAX_BULLETS   64
#define MAX_ENEMIES   32
#define MAX_PARTICLES 256
#define MAX_STARS     200

// ─── Game states ──────────────────────────────────────────────────────────────
typedef enum {
    STATE_MENU = 0,
    STATE_PLAYING,
    STATE_PAUSED,
    STATE_GAMEOVER,
    STATE_WIN
} GameState;

// ─── Bullet ───────────────────────────────────────────────────────────────────
typedef struct {
    Vector2 pos;
    Vector2 vel;
    bool    active;
    bool    fromPlayer;   // true = player bullet, false = enemy bullet
} Bullet;

// ─── Enemy ────────────────────────────────────────────────────────────────────
typedef enum { ENEMY_SCOUT, ENEMY_TANK, ENEMY_BOSS } EnemyType;

typedef struct {
    Vector2    pos;
    Vector2    vel;
    float      hp;
    float      maxHp;
    float      shootTimer;
    float      shootCooldown;
    bool       active;
    EnemyType  type;
} Enemy;

// ─── Particle ─────────────────────────────────────────────────────────────────
typedef struct {
    Vector2 pos;
    Vector2 vel;
    Color   color;
    float   life;     // remaining seconds
    float   maxLife;
    float   size;
    bool    active;
} Particle;

// ─── Player ───────────────────────────────────────────────────────────────────
typedef struct {
    Vector2 pos;
    float   hp;
    float   maxHp;
    float   speed;
    float   shootTimer;
    float   shootCooldown;
    int     score;
    int     lives;
    bool    invincible;   // brief mercy frames after hit
    float   invTimer;
} Player;

// ─── Background star ─────────────────────────────────────────────────────────
typedef struct {
    Vector2 pos;
    float   speed;
    float   brightness;
    float   size;
} Star;

// ─── Full game context ────────────────────────────────────────────────────────
typedef struct {
    GameState state;
    Player    player;
    Bullet    bullets[MAX_BULLETS];
    Enemy     enemies[MAX_ENEMIES];
    Particle  particles[MAX_PARTICLES];
    Star      stars[MAX_STARS];
    int       wave;
    int       enemiesLeft;
    float     waveTimer;       // delay before next wave spawns
    bool      waveClear;
    float     dt;
} Game;

#endif // GAME_H

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
#define MAX_BULLETS   128
#define MAX_ENEMIES   32
#define MAX_PARTICLES 256
#define MAX_STARS     200
#define MAX_POWERUPS  8

// ─── Game states ──────────────────────────────────────────────────────────────
typedef enum {
    STATE_MENU = 0,
    STATE_PLAYING,
    STATE_PAUSED,
    STATE_GAMEOVER,
    STATE_UPGRADE,
    STATE_WIN
} GameState;

// ─── Upgrade ──────────────────────────────────────────────────────────────────
typedef struct {
    int type;    // 0=MaxHP+50, 1=武器升級, 2=炸彈+2, 3=射速+15%, 4=永久速+20, 5=Combo時間+1s
    const char *label;
    const char *desc;
} UpgradeOption;

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
    float      spinTimer;      // boss rotating-bullet cooldown (wave >= 6)
    bool       active;
    bool       canDive;
    bool       diving;
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

// ─── PowerUp ──────────────────────────────────────────────────────────────────
typedef struct {
    Vector2 pos;
    float   vel_y;
    bool    active;
    int     type;     // 0=Weapon, 1=HP, 2=Shield, 3=Bomb, 4=Speed
} PowerUp;

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
    int     weaponLevel;  // 1=single, 2=double, 3=triple-spread
    int     combo;        // kill streak count
    float   comboTimer;   // 3s countdown, resets on kill
    int     bombs;        // screen-clear bombs (initial: 3)
    bool    shield;       // absorbs one hit
    float   speedBoostTimer;
    float   permSpeedBonus;
    float   comboWindow;
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
    PowerUp   powerups[MAX_POWERUPS];
    int       wave;
    int       enemiesLeft;
    float     waveTimer;       // delay before next wave spawns
    bool      waveClear;
    float     dt;
    bool      bombActive;      // one-frame screen-clear flag
    float     speedMultiplier; // cumulative enemy speed multiplier per wave
    float     hitFlashTimer;
    UpgradeOption upgradeOptions[3];
    int       upgradeChoice;
    int       highScore;
    bool      isNewBest;
} Game;

#endif // GAME_H

#include "upgrade.h"
#include "raylib.h"

static UpgradeOption ALL_UPGRADES[6] = {
    { 0, "MAX HP UP", "Max HP +50 & Heal" },
    { 1, "WEAPON UP", "Weapon Lv Up (Max 3)" },
    { 2, "BOMBS +2", "+2 Screen Bombs" },
    { 3, "FIRE RATE", "+15% Fire Rate" },
    { 4, "SPEED UP", "+20 Ship Speed" },
    { 5, "COMBO TIME", "+1s Combo Time" }
};

void upgrade_generate(UpgradeOption options[3]) {
    int choices[6] = {0, 1, 2, 3, 4, 5};
    int n = 6;
    // Simple shuffle logic using GetRandomValue
    for (int i = 0; i < 3; i++) {
        int idx = GetRandomValue(0, n - 1);
        options[i] = ALL_UPGRADES[choices[idx]];
        choices[idx] = choices[n - 1];
        n--;
    }
}

void upgrade_apply(Game *g, int type) {
    switch (type) {
        case 0:
            g->player.maxHp += 50.0f;
            g->player.hp = g->player.maxHp;
            break;
        case 1:
            if (g->player.weaponLevel < 3) g->player.weaponLevel++;
            break;
        case 2:
            g->player.bombs += 2;
            break;
        case 3:
            g->player.shootCooldown *= 0.85f;
            if (g->player.shootCooldown < 0.08f) g->player.shootCooldown = 0.08f;
            break;
        case 4:
            g->player.permSpeedBonus += 20.0f;
            if (g->player.permSpeedBonus > 200.0f) g->player.permSpeedBonus = 200.0f;
            break;
        case 5:
            g->player.comboWindow += 1.0f;
            break;
    }
}

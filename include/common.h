#ifndef COMMON_H
#define COMMON_H

struct Attributes  // factors that scales the entity's stats
{
    int vigor        = 0; // increases health points & mana points
    int strength     = 0; // increases physical damage
    int endurance    = 0; // increases damage reduction
    int intelligence = 0; // increases magical damage & mana points
    int dexterity    = 0; // increases dodge chance & critical chance
};

enum class Slot {
    MainHand,
    OffHand,
    Helmet,
    Chestplate,
    Boots,
    COUNT
};

#endif
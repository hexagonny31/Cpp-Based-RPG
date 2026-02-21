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
//  for equipment vector
enum class Slot {
    MainHand,
    OffHand,
    Helmet,
    Chestplate,
    Boots,
    COUNT
};
constexpr size_t to_index(Slot s) noexcept {
    return static_cast<size_t>(s);
}
//  for item properties.
//  identifier for equipment.
enum class EquipType {
    None,
    Weapon,
    Helmet,
    Chestplate,
    Boots,
};
//  identifier for weapons equip requirement.
enum class WeaponRequire {
    None,
    OneHand,
    TwoHand
};
//  identifier for armor type
enum class ArmorType {
    None,
    Light,
    Medium,
    Heavy,
    FatHeavy
};

#endif
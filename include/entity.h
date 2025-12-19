#ifndef ENTITYOBJECT_H
#define ENTITYOBJECT_H

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <algorithm>

struct Attributes {
    int vigor        = 0; // increases health points & mana points.
    int strength     = 0; // increases physical damage.
    int endurance    = 0; // increases damage reduction.
    int intelligence = 0; // increases magical damage & mana points.
    int dexterity    = 0; // increases dodge chance & critical chance.
};

struct Stats  // shared between player & monster
{
    static constexpr double max_bonus = 50.0;

    double current_health = 100.0;

    const Attributes& attr;

    explicit Stats(const Attributes& a) : attr(a) {}

    double currentHealth() const { return current_health; }
    double totalHealth()   const {
        const double default_health = 100.0;
        double bonus_health = (max_bonus * attr.vigor) / (100.0 * (max_bonus + attr.vigor));
        return default_health + bonus_health;
    }

    //  Place-holder items.
    double tempWeaponDamage = 20.0;
    double tempHelm         = 0.005;
    double tempChest        = 0.015;
    double tempBoots        = 0.001;

    //  Place-holder boots dodge chance bonus.
    double tempEquipBonus = 0.07;

    double damage() const
    {
        double baseDamage = (max_bonus * attr.strength) / (100.0 * (max_bonus + attr.strength));
        return tempWeaponDamage * (1 + baseDamage);
    }

    double physicalResist() const
    {
        double baseResistance = (max_bonus * attr.endurance) / (100.0 * (max_bonus + attr.endurance));
        return tempHelm + tempChest + tempBoots + baseResistance;
    }

    double dodgeChance() const
    {
        double baseDodgeChance = (max_bonus * attr.dexterity) / (100.0 * (max_bonus + attr.dexterity));
        return std::min(1.0, 0.15 + tempEquipBonus + baseDodgeChance);
    }

    bool isAlive() const { return current_health > 0.0; }
    void updateHealth()  { current_health = totalHealth(); }
    bool didDodge() const {
        double total = dodgeChance();
        double random = static_cast<double>(rand())/RAND_MAX;
        return random < total;
    }   
};

struct Item {
    std::string name;
    std::string id;

    // attribute modifiers.
    int vigor_mod        = 0;
    int strength_mod     = 0;
    int endurance_mod    = 0;
    int intelligence_mod = 0;
    int dexterity_mod    = 0;

    // stat modifiers.
    double increase_HP  = 0.0;  // adds health points
    double health_bonus = 0.0;  // extra health points
    double damage_bonus = 0.0;  // extra weapon damage
    double resist_bonus = 0.0;  // extra armor/resist
    double dodge_bonus  = 0.0;  // extra dodge

    bool equipped = false;
    bool equippable;
    // other stuff maybe in the future like rarity bonuses and shit like that.
};

enum class Slot {
    MainHand,
    OffHand,
    Helmet,
    Chestplate,
    Boots,
    COUNT
};

struct Player
{
    std::string name;
    int         allocation_pts = 0;
    Attributes  attributes;
    Stats       stats;

    std::vector<Item*> inventory;
    std::vector<Item*> equipment;

    Player() : stats(attributes), equipment(static_cast<size_t>(Slot::COUNT), nullptr) {}

    std::string getName()                   const { return name; }
    int         getAllocation()             const { return allocation_pts; }
    Item*       getEquipment    (Slot slot) const { return equipment[static_cast<size_t>(slot)]; }
    std::string getEquipmentName(Slot slot) const {
        Item* item = getEquipment(slot);
        return item ? item->name : "Empty";
    }

    void setName      (const std::string& newName)   { name = newName; }
    void setAllocation(int newAllocation)            { allocation_pts = newAllocation; }
};

#endif
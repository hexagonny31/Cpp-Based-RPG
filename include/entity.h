#ifndef ENTITYOBJECT_H
#define ENTITYOBJECT_H

#include "item_database.hpp"

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <algorithm>

struct Attributes  // factors that scales the entity's stats
{
    int vigor        = 0; // increases health points & mana points
    int strength     = 0; // increases physical damage
    int endurance    = 0; // increases damage reduction
    int intelligence = 0; // increases magical damage & mana points
    int dexterity    = 0; // increases dodge chance & critical chance
};

struct Stats  // shared between player & monster
{
    static constexpr double max_bonus = 250.0;

    double current_health = 100.0;  // fallback value. just incase if it doesn't update initially.
    double current_mana   = 100.0;

    const Attributes& attr;
    const Player* owner = nullptr;

    explicit Stats(const Attributes& a, const Player* p = nullptr) : attr(a), owner(p) {}

    // health/mana manipulators n' shit
    double getCurrentHealth() const { return current_health; }
    double getTotalHealth  () const {
        const double default_health = 100.0;
        double total_vigor = attr.vigor;
        double bonus_health = 0.0;
        if(owner) {
            for(const Item* item : owner->equipment) {
                if(item && item->equipped) {
                    bonus_health += item->health_bonus;
                    total_vigor += item->attribute.vigor;
                }
            }
        }
        bonus_health = bonus_health + (10.0 * total_vigor);
        return default_health + bonus_health;
    }

    double getCurrentMana() const { return current_mana; }
    double getTotalMana  () const { 
        const double default_mana = 100.0;
        double bonus_mana = 15.0 * attr.intelligence;
        if(owner) {
            for(const Item* item : owner->equipment) {
                if(item && item->equipped) bonus_mana += item->attribute.intelligence;
            }
        }
        return default_mana + bonus_mana;
    }

    void setCurrentHealth(const double new_health) { current_health = new_health; }
    void setCurrentMana  (const double new_mana)   { current_mana   = new_mana; }

    //  Place-holder items.
    double tempWeaponDamage = 20.0;
    double tempHelm         = 0.005;
    double tempChest        = 0.015;
    double tempBoots        = 0.001;

    //  Place-holder boots dodge chance bonus.
    double tempEquipBonus = 0.07;

    double getDamage() const {
        double base_damage = 10.0;
        int total_strength = attr.strength;
        if(owner) {
            for(const Item* item : owner->equipment) {
                if(item && item->equipped) total_strength += item->attribute.strength;
            }
            Item* weapon = owner->getEquipment(Slot::MainHand);
            if(weapon && weapon->equipped) {
                base_damage += weapon->base_damage;
            }
        }
        double strength_scaling = (max_bonus * total_strength) / (100.0 * (max_bonus + total_strength));
        double total_weapon_damage = base_damage;
        return total_weapon_damage * (1.0 + strength_scaling);
    }

    double getPhysicalResist() const {
        int total_endurance = attr.endurance;
        double total_resist_bonus = 0.0;
        if(owner) {
            for(const Item* item : owner->equipment) {
                if(item && item->equipped) {
                    total_endurance += item->attribute.endurance;  // from any equipment
                    total_resist_bonus += item->resist_bonus;      // from armor and shields
                }
            }
        }
        double base_resist = (max_bonus * total_endurance) / (100.0 * (max_bonus + total_endurance));
        return total_resist_bonus + base_resist;
    }

    double getDodgeChance() const {
        int total_dexterity = attr.dexterity;
        double total_dodge_bonus = 0.0;
        if(owner) {
            for(const Item* item : owner->equipment) {
                if(item && item->equipped) {
                    total_dexterity += item->attribute.dexterity;  // from any equipment
                    total_dodge_bonus += item->dodge_bonus;        // from armor and shields
                }
            }
        }
        double base_dodge = (max_bonus * attr.dexterity) / (100.0 * (max_bonus + attr.dexterity));
        return std::min(1.0, 0.15 + total_dodge_bonus + base_dodge);
    }

    bool isAlive     () const { return current_health > 0.0; }
    void updateHealth()       { current_health = getTotalHealth(); }
    bool didDodge    () const {
        double total = getDodgeChance();
        double random = static_cast<double>(rand())/RAND_MAX;
        return random < total;
    }   
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
    Attributes  attribute;
    Stats       stats;

    std::vector<Item*> inventory;
    std::vector<Item*> equipment;

    Player() : stats(attribute, this), equipment(static_cast<size_t>(Slot::COUNT), nullptr) {}

    std::string getName         ()          const { return name; }
    int         getAllocationPts()          const { return allocation_pts; }
    Item*       getEquipment    (Slot slot) const { return equipment[static_cast<size_t>(slot)]; }
    std::string getEquipmentName(Slot slot) const {
        Item* item = getEquipment(slot);
        return item ? item->name : "Empty";
    }

    void setName       (const std::string& newName) { name = newName; }
    void setAllocation (int newAllocation)          { allocation_pts = newAllocation; }
    bool addToInventory(const std::string& id) {
        auto init = ItemDatabase::instance().find(id);
        if(!init) return false;

        Item* new_item = new Item(init.value());
        inventory.push_back(new_item);
        return true;
    }
    void equipItem(Item* item, Slot slot) {
        if(!item || !item->equippable) return;
        Item* current = getEquipment(slot);
        if(current) current->equipped = false;

        equipment[static_cast<size_t>(slot)] = item;
        item->equipped = true;
    }
    void unequipItem(Slot slot) {
        Item* current = getEquipment(slot);
        if(!current) return;
        current->equipped = false;
        equipment[static_cast<size_t>(slot)] = nullptr;
    }
};

#endif
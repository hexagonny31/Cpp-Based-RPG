#ifndef ENTITYOBJECT_H
#define ENTITYOBJECT_H

#include "item_database.hpp"

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <algorithm>

struct Entity {
    std::string name;
    Attributes attribute;
    std::vector<Item*> equipment;

    Entity() : equipment(to_index(Slot::COUNT), nullptr) {}

    static constexpr double max_bonus = 250.0;
    double current_health = 100.0;  // fallback value. just incase if it doesn't update initially.
    double current_mana   = 100.0;

    std::string        getName         ()   const { return name; }
    Attributes         getAttributes   ()   const { return attribute; }
    std::vector<Item*> getEquipment    ()   const { return equipment; }
    int         getVigor        ()          const { return attribute.vigor; }
    int         getStrength     ()          const { return attribute.strength; }
    int         getEndurance    ()          const { return attribute.endurance; }
    int         getIntelligence ()          const { return attribute.intelligence; }
    int         getDexterity    ()          const { return attribute.dexterity; }
    Item*       getEquipment    (Slot slot) const { return equipment[to_index(slot)]; }
    std::string getEquipmentName(Slot slot) const {
        Item* item = getEquipment(slot);
        return item ? item->name : "Empty";
    }

    // health/mana manipulators n' shit
    double getCurrentHealth() const { return current_health; }
    double getTotalHealth  () const {
        const double default_health = 100.0;
        double total_vigor = attribute.vigor;
        double bonus_health = 0.0;
        
        for(const Item* item : equipment) {
            if(item && item->equipped) {
                bonus_health += item->health_bonus;
                total_vigor += item->attribute.vigor;
            }
        }
        
        bonus_health = bonus_health + (10.0 * total_vigor);
        return default_health + bonus_health;
    }
    double getCurrentMana() const { return current_mana; }
    double getTotalMana  () const { 
        const double default_mana = 100.0;
        double bonus_mana = 15.0 * attribute.intelligence;
        
        for(const Item* item : equipment) {
            if(item && item->equipped) bonus_mana += item->attribute.intelligence;
        }
        
        return default_mana + bonus_mana;
    }
    // actual stats n' shit.
    double getDamage() const {
        double base_damage = 10.0;
        int total_strength = attribute.strength;
        
        for(const Item* item : equipment) {
            if(item && item->equipped) total_strength += item->attribute.strength;
        }
        Item* weapon = getEquipment(Slot::MainHand);
        if(weapon && weapon->equipped) {
            base_damage += weapon->base_damage;
        }
        
        double strength_scaling = (max_bonus * total_strength) / (100.0 * (max_bonus + total_strength));
        double total_weapon_damage = base_damage;
        return total_weapon_damage * (1.0 + strength_scaling);
    }
    double getPhysicalResist() const {
        int total_endurance = attribute.endurance;
        double total_resist_bonus = 0.0;
        
        for(const Item* item : equipment) {
            if(item && item->equipped) {
                total_endurance += item->attribute.endurance;  // from any equipment
                total_resist_bonus += item->resist_bonus;      // from armor and shields
            }
        }
        
        double base_resist = (max_bonus * total_endurance) / (100.0 * (max_bonus + total_endurance));
        return total_resist_bonus + base_resist;
    }
    double getDodgeChance() const {
        int total_dexterity = attribute.dexterity;
        double total_dodge_bonus = 0.0;
        
        for(const Item* item : equipment) {
            if(item && item->equipped) {
                total_dexterity += item->attribute.dexterity;  // from any equipment
                total_dodge_bonus += item->dodge_bonus;        // from armor and shields
            }
        }
    
        double base_dodge = (max_bonus * attribute.dexterity) / (100.0 * (max_bonus + attribute.dexterity));
        return std::min(1.0, 0.15 + total_dodge_bonus + base_dodge);
    }

    void setName         (const std::string& newName) { name = newName; }
    void setCurrentHealth(const double new_health)    { current_health = new_health; }
    void setCurrentMana  (const double new_mana)      { current_mana   = new_mana; }

    bool isAlive     () const { return current_health > 0.0; }
    void updateHealth()       { current_health = getTotalHealth(); }
    void updateMana  ()       { current_mana   = getTotalMana(); }
    bool didDodge    () const {
        double total = getDodgeChance();
        double random = static_cast<double>(rand())/RAND_MAX;
        return random < total;
    }
};

struct Player : public Entity
{
    int allocation_pts = 0;
    std::vector<Item> inventory;

    Player() {}
    
    int         getAllocationPts()             const { return allocation_pts; }
    Item        getItem         (size_t slot)  const { return inventory[slot]; }
    std::string getItemName     (size_t slot)  const {
        if(slot < 0 || slot >= inventory.size())
            return "Empty";
        return inventory[slot].name;
    }

    void setAllocation(int newAllocation) { allocation_pts = newAllocation; }
    bool setAttributes ();

    bool addToInventory(const std::string& id) {
        auto init = ItemDatabase::instance().find(id);
        if(!init) return false;
        inventory.push_back(init.value());
        return true;
    }
    void equipItem(Item* item, Slot slot) {
        if(!item || item->property.equip_type == EquipType::None) return;
        Item* current = getEquipment(slot);
        if(current) current->equipped = false;
        equipment[to_index(slot)] = item;
        item->equipped = true;
    }
    void unequipItem(Slot slot) {
        Item* current = getEquipment(slot);
        if(!current) return;
        current->equipped = false;
        equipment[to_index(slot)] = nullptr;
    }
};

struct Monster : public Entity {
    int lvl     = 1;
    int xp_reward = 0;

    Monster() {}

    static Monster create(const std::string& new_name, int new_lvl = 1) {
        Monster m;
        m.name = new_name;
        m.lvl = new_lvl;

        int base_vigor = 15, base_str = 12, base_end = 14, base_int = 8, base_dex = 16;
        m.attribute.vigor        = base_vigor + (new_lvl * 3);
        m.attribute.strength     = base_str   + (new_lvl * 2);
        m.attribute.endurance    = base_end   + (new_lvl * 2);
        m.attribute.intelligence = base_int   + new_lvl;
        m.attribute.dexterity    = base_dex   + (new_lvl * 2);
        
        m.xp_reward = 30 + (new_lvl * 20);
        m.updateHealth();
        return m; 
    }
};

#endif
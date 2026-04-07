#ifndef ENTITYOBJECT_H
#define ENTITYOBJECT_H

#include "item_database.hpp"

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <algorithm>
#include <utility>

struct Entity {
protected:
    std::string name;
    Attributes attribute;
    std::vector<Item*> equipment;
    double dmg = 1.0;

    Entity() : equipment(to_index(Slot::COUNT), nullptr) {}

    static constexpr double max_bonus = 250.0;
    double df_hp = 100.0;  // fallback value. just incase if it doesn't update initially.
    double df_mp = 100.0;
    double curr_hp;
    double curr_mp;

public:
    std::string        getName      ()      const { return name; }
    Attributes         getAttributes()      const { return attribute; }
    std::vector<Item*> getEquipment ()      const { return equipment; }
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
    double getCurrentHealth() const { return curr_hp; }
    double getTotalHealth(const bool ignore_equipment) const {
        double total_vigor = attribute.vigor;
        double bonus_hp = 0.0;
        
        if(!ignore_equipment) {
            for(const Item* item : equipment) {
                if(item && item->equipped) {
                    bonus_hp += item->health_bonus;
                    total_vigor += item->attribute.vigor;
                }
            }
            return df_hp + bonus_hp + (10.0 * total_vigor);
        } else {
            return (10.0 * total_vigor);
        }
    }
    double getCurrentMana() const { return curr_mp; }
    double getTotalMana  (const bool ignore_equipment) const { 
        double bonus_mp = 15.0 * attribute.intelligence;
        
        if(!ignore_equipment) {
            for(const Item* item : equipment) {
                if(item && item->equipped) bonus_mp += item->attribute.intelligence;
            }
            return df_mp + bonus_mp;
        } else {
            return bonus_mp;
        }
    }
    // actual stats n' shit.
    double getDamage(const bool ignore_equipment) const {
        double base_dmg = dmg;
        int total_str = attribute.strength;
        
        if(!ignore_equipment) {
            for(const Item* item : equipment) {
                if(item && item->equipped) total_str += item->attribute.strength;
            }
            Item* weapon = getEquipment(Slot::MainHand);
            if(weapon && weapon->equipped) {
                base_dmg += weapon->base_damage;
            }
            double total_weapon_dmg = base_dmg;
            return total_weapon_dmg * (1.0 + ((max_bonus * total_str) / (100.0 * (max_bonus + total_str))));
        } else {
            return (max_bonus * total_str) / (100.0 * (max_bonus + total_str));
        }
    }
    double getPhysicalResist(const bool ignore_equipment) const {
        int total_end = attribute.endurance;
        double total_resist_bonus = 0.0;
  
        if(!ignore_equipment) {
            for(const Item* item : equipment) {
                if(item && item->equipped) {
                    total_end += item->attribute.endurance;    // from any equipment
                    total_resist_bonus += item->resist_bonus;  // from armor and shields
                }
            }
            return total_resist_bonus + ((max_bonus * total_end) / (100.0 * (max_bonus + total_end)));
        } else {
            return (max_bonus * total_end) / (100.0 * (max_bonus + total_end));
        }
    }
    double getDodgeChance(const bool ignore_equipment) const {
        int total_dex = attribute.dexterity;
        double total_dodge_bonus = 0.0;
        
        if(!ignore_equipment) {
            for(const Item* item : equipment) {
                if(item && item->equipped) {
                    total_dex += item->attribute.dexterity;  // from any equipment
                    total_dodge_bonus += item->dodge_bonus;  // from armor and shields
                }
            }
        }

        double base_dodge = (max_bonus * total_dex) / (100.0 * (max_bonus + total_dex));
        return (std::min)(1.0, (ignore_equipment ? 0 : 0.05) + total_dodge_bonus + base_dodge);
    }

    void setName         (const std::string& newName) { name = newName; }
    void setCurrentHealth(const double new_hp)        { curr_hp = new_hp; }
    void setCurrentMana  (const double new_mp)        { curr_mp = new_mp; }
    void setAttributes   (const Attributes new_attr)  { attribute = new_attr; }
    void setVigor        (const int new_alloc)        { attribute.vigor = new_alloc; }
    void setStrength     (const int new_alloc)        { attribute.strength = new_alloc; }
    void setEndurance    (const int new_alloc)        { attribute.endurance = new_alloc; }
    void setIntelligence (const int new_alloc)        { attribute.intelligence = new_alloc; }
    void setDexterity    (const int new_alloc)        { attribute.dexterity = new_alloc; }

    bool isAlive     () const { return curr_hp > 0.0; }
    void updateHealth()       { curr_hp = getTotalHealth(false); }
    void updateMana  ()       { curr_mp = getTotalMana(false); }
    bool didDodge    () const {
        double total = getDodgeChance(false);
        double random = static_cast<double>(rand())/RAND_MAX;
        return random < total;
    }
};

struct Player : public Entity {
private:
    int allocation_pts = 0;
    std::vector<Item> inventory;

public:
    Player() = default;
    Player(const Player&) = delete;
    Player& operator=(const Player&) = delete;

    Player(Player&&) = default;
    Player& operator=(Player&&) = default;

    int         getAllocationPts()             const { return allocation_pts; }
    Item        getItem         (size_t slot)  const { return inventory[slot]; }
    std::string getItemName     (size_t slot)  const {
        if(slot < 0 || slot >= inventory.size())
            return "Empty";
        return inventory[slot].name;
    }
    std::vector<Item>&       getInventory()       { return inventory; }
    const std::vector<Item>& getInventory() const { return inventory; }

    void setAllocation(int newAllocation) { allocation_pts = newAllocation; }
    bool setAttribute ();

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
private:
    std::string id = "";
    int lvl        = 1;
    int xp_reward  = 0;
    std::pair<int,int> gold_reward = {0,0};
    LootTable loot;

public:
    Monster() : loot({{"Nothing", 1.0}}) {}

    std::string        getID()        const { return id; }
    int                getLvl()       const { return lvl; }
    int                getXP()        const { return xp_reward; }
    std::pair<int,int> getGold()      const { return gold_reward; }
    LootTable          getLootTable() const { return loot; }

    void setID       (const std::string new_id)                 { id = new_id; }
    void setLvl      (const int new_lvl)                        { lvl = new_lvl; }
    void setXP       (const int new_xp_reward)                  { xp_reward = new_xp_reward; }
    void setGold     (const std::pair<int,int> new_gold_reward) { gold_reward = std::move(new_gold_reward); }
    void setLootTable(const LootTable new_loot)                 { loot = std::move(new_loot); }
};

#endif
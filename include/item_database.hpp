#pragma once

#include "hutils.h"
#include "common.h"
#include "json.hpp"

#include <string>
#include <fstream>
#include <optional>
#include <unordered_map>

using json = nlohmann::json;

//  descriptions that defines an equipment 
struct Properties {
    std::string weapon = "";
    std::string damage = "";
    std::string armor  = "";
    std::string ammo   = "";
};

struct Item {
    std::string name;
    std::string id;
    std::string equip_type = "";

    // attribute modifiers
    Attributes attribute;

    // stat modifiers
    double increase_HP  = 0.0;  // adds health points
    double base_damage  = 0.0;  // flat damage of a weapon
    double health_bonus = 0.0;  // extra health points
    double damage_bonus = 0.0;  // extra weapon damage
    double resist_bonus = 0.0;  // extra armor/resist
    double dodge_bonus  = 0.0;  // extra dodge

    bool equipped = false;
    bool equippable;
    // other stuff maybe in the future like rarity bonuses and shit like that.
};

struct ItemDatabase {
private:
    ItemDatabase() = default;
    static std::unordered_map<std::string, Item> itemDatabase;
public:
    static ItemDatabase& instance() {
        static ItemDatabase db;
        return db;
    }

    bool load(const std::string& FILE_NAME = "json/items.json") {
        std::ifstream file(FILE_NAME);
        if(!file.is_open()) return false;

        json j;
        try {
            file >> j;
        } catch(const json::parse_error&) {
            return false;
        }
        if(!j.is_array()) return false;

        itemDatabase.clear();
        for(const auto& entry : j) {
            if(!entry.contains("name") || !entry.contains("id") || !entry.contains("equippable")) continue;

            Item item;
            item.id         = entry["id"].get<std::string>();
            item.name       = entry["name"].get<std::string>();
            item.equippable = entry["equippable"].get<bool>();

            Attributes a;
            a.vigor        = entry.value("vigor", 0);
            a.strength     = entry.value("strength", 0);
            a.endurance    = entry.value("endurance", 0);
            a.intelligence = entry.value("intelligence", 0);
            a.dexterity    = entry.value("dexterity", 0);
            item.attribute = a;
            
            item.increase_HP  = entry.value("increase_HP", 0);
            item.base_damage  = entry.value("base_damage", 0);
            item.health_bonus = entry.value("health_bonus", 0);
            item.damage_bonus = entry.value("damage_bonus", 0);
            item.resist_bonus = entry.value("resist_bonus", 0);
            item.dodge_bonus  = entry.value("dodge_bonus", 0);

            itemDatabase[item.id] = item;
        }
        return true;
    }
    std::optional<Item> find(const std::string& id) const {
        auto cartesian = itemDatabase.find(id);
        if(cartesian != itemDatabase.end()) return cartesian->second;
        return std::nullopt;
    }
};
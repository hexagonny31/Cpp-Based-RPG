#pragma once

#include "hutils.h"
#include "common.h"
#include "json.hpp"

#include <string>
#include <fstream>
#include <optional>
#include <unordered_map>

using nj = nlohmann::json;

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

    // attribute modifiers
    Attributes attribute{};

    // stat modifiers
    double increase_HP  = 0.0;  // adds health points by a percentage
    double increase_DMG = 0.0;  // adds flat damage by a percentage
    double base_damage  = 0.0;  // flat damage of a weapon
    double health_bonus = 0.0;  // extra health points
    double damage_bonus = 0.0;  // extra weapon damage
    double resist_bonus = 0.0;  // extra armor/resist
    double dodge_bonus  = 0.0;  // extra dodge

    bool equipped   = false;
    EquipType equip_type = EquipType::None;
    // other stuff maybe in the future like rarity bonuses and shit like that.
};

struct ItemDatabase {
private:
    ItemDatabase() = default;
    static std::unordered_map<std::string, Item> itemDatabase;
    EquipType findSlot(const std::string &s) {
        if(s == "weapon")     return EquipType::Weapon;
        if(s == "helmet")     return EquipType::Helmet;
        if(s == "chestplate") return EquipType::Chestplate;
        if(s == "boots")      return EquipType::Boots;
        return EquipType::None;
    }
public:
    static ItemDatabase& instance() {
        static ItemDatabase db;
        return db;
    }

    bool load(const std::string& FILE_NAME = "json/items.json") {
        std::ifstream file(FILE_NAME);
        if(!file.is_open()) return false;

        nj json;
        try {
            file >> json;
        } catch(const nj::parse_error&) {
            return false;
        }
        if(!json.is_array()) return false;

        itemDatabase.clear();
        for(const auto& e : json) {
            if(!e.contains("name") || !e.contains("id")) continue;

            Item item;
            item.id          = e["id"].get<std::string>();
            item.name        = e["name"].get<std::string>();
            std::string t    = e.value("equip_type", "none");
            item.equip_type  = findSlot(t);

            if(e.contains("attribute") && e["attribute"].is_object()) {
                const auto& a = e["attribute"];
                item.attribute.vigor        = a.value("vigor", 0);
                item.attribute.strength     = a.value("strength", 0);
                item.attribute.endurance    = a.value("endurance", 0);
                item.attribute.intelligence = a.value("intelligence", 0);
                item.attribute.dexterity    = a.value("dexterity", 0);
            }
            
            item.increase_HP  = e.value("increase_HP", 0);
            item.increase_DMG = e.value("increase_DMG", 0);
            item.base_damage  = e.value("base_damage", 0);
            item.health_bonus = e.value("health_bonus", 0);
            item.damage_bonus = e.value("damage_bonus", 0);
            item.resist_bonus = e.value("resist_bonus", 0);
            item.dodge_bonus  = e.value("dodge_bonus", 0);

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
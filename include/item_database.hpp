#pragma once

#include "hutils.h"
#include "common.h"
#include "json.hpp"

#include <string>
#include <fstream>
#include <optional>
#include <random>
#include <unordered_map>
#include <utility>

using nj = nlohmann::json;

// Properties struct representing the equip type and requirements of an item.
struct Properties {
    EquipType     equip_type     = EquipType::None;      // Defines the type of equipment (e.g., weapon, armor).
    WeaponRequire weapon_require = WeaponRequire::None;  // For weapons, indicates if it's one-handed or two-handed.
    ArmorType     armor_type     = ArmorType::None;      // For armor, indicates if it's light, medium, heavy, etc.
};

// Item struct representing equippable items in the game.
struct Item {
    std::string name;
    std::string id;

    Attributes attribute{};
    Properties property{};

    double increase_HP  = 0.0;  // Adds health points by a percentage.
    double increase_DMG = 0.0;  // Adds flat damage by a percentage.
    double base_damage  = 0.0;  // Flat damage of a weapon.
    double health_bonus = 0.0;  // Extra health points.
    double damage_bonus = 0.0;  // Extra weapon damage.
    double resist_bonus = 0.0;  // Extra armor/resist.
    double dodge_bonus  = 0.0;  // Extra dodge chance.

    bool equipped   = false;
    // other stuff maybe in the future like rarity bonuses and shit like that.
};

/** 
 * @brief Weighted random loot table using std::discrete_distribution.
 *
 *  This class represents a weighted random selection table, usually
 *  Loot drops from monsters, chests, or any other entities.
 *
 *  Items are identified only by IDs (which should match keys in ItemDatabase).
 *  Each entry has an associated POSITIVE weight that determines its relative probability.
 */
struct LootTable {
private:
    std::vector<std::string> item_ids;
    std::vector<double> weights;
    std::discrete_distribution<size_t> dist;
    std::mt19937 gen;
public:
    /**
     * @brief Constructs a loot table from item ID -> weight pairs.
     * @param list Vector of {item_id, weight} pairs
     * 
     * Only entries with weight > 0.0 are kept. (Please do not try)
     * If the resulting list is empty, dist ramins default-constructed
     * and dropItem() will cause undefined behavior.
     */
    LootTable(std::vector<std::pair<std::string, double>> list)
        : item_ids(), weights(), gen(std::random_device{}())
    {
        item_ids.reserve(list.size());
        weights.reserve(list.size());
        
        for(const auto& [item, weight] : list) {
            if(weight <= 0) continue;
            item_ids.push_back(item);
            weights.push_back(weight);
        }
        if(!weights.empty())
            dist = std::discrete_distribution<size_t>(weights.begin(), weights.end());
    };

    /**
     * @brief Randomly selects an item ID based on the weights.
     * @return A randomly selected item ID from the loot table.
     * @warning Undefined behavior if the loot table is empty (no items with weight > 0).
     */
    std::string dropItem() { return item_ids[dist(gen)]; }

    /**
     * @brief Drops multiple items based on the loot table.
     * @param x The number of items to drop.
     * @return A vector of randomly selected item IDs from the loot table.
     * @warning Undefined behavior if the loot table is empty (no items with weight > 0).
     */
    std::vector<std::string> dropItem(const size_t x) {
        std::vector<std::string> it;
        for(size_t i = 0; i < x; ++i) it.push_back(dropItem());
        return it;
    }
};

/**
 * @brief Singleton class for managing the item database.
 * 
 * This class loads item data from a JSON file and provides access to item information.
 * The item database is stored as an unordered_map for efficient retrieval by item ID.
 * 
 * The load() function reads the JSON file and populates the item database,
 * while the find() function allows retrieval of items by their ID.
 */
struct ItemDatabase {
private:
    ItemDatabase() = default;
    /**
     * @brief The actual storage: ID -> full Item mapping
     * Uses unordered_map for fast lookup by string key
     */
    static std::unordered_map<std::string, Item> itemDatabase;
public:
    /**
     * @brief Returns the single global instance (Meyers singleton).
     * @return Reference to the ItemDatabase.
     */
    static ItemDatabase& instance() {
        static ItemDatabase db;
        return db;
    }

    /**
     * @brief Loads items from a JSON file into the item database.
     * @param FILE_NAME The path to the JSON file containing item data.
     * @return true if loading was successful, false otherwise.
     */
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

            if(e.contains("attribute") && e["attribute"].is_object()) {
                const auto& a = e["attribute"];
                item.attribute.vigor        = a.value("vigor", 0);
                item.attribute.strength     = a.value("strength", 0);
                item.attribute.endurance    = a.value("endurance", 0);
                item.attribute.intelligence = a.value("intelligence", 0);
                item.attribute.dexterity    = a.value("dexterity", 0);
            }
            if(e.contains("properties") && e["properties"].is_object()) {
                const auto& p  = e["properties"];
                item.property.armor_type     = (ArmorType)p.value("armor_type", 0);
                item.property.equip_type     = (EquipType)p.value("equip_type", 0);
                item.property.weapon_require = (WeaponRequire)p.value("weapon_require", 0);
            }
            // item bonuses.
            item.increase_HP  = e.value("increase_HP" , 0);
            item.increase_DMG = e.value("increase_DMG", 0);
            item.base_damage  = e.value("base_damage" , 0);
            item.health_bonus = e.value("health_bonus", 0);
            item.damage_bonus = e.value("damage_bonus", 0);
            item.resist_bonus = e.value("resist_bonus", 0);
            item.dodge_bonus  = e.value("dodge_bonus" , 0);

            itemDatabase[item.id] = item;
        }
        return true;
    }
    /**
     * @brief Looks up an item by its ID in the item database.
     * @param id The ID of the item to find.
     * @return An optional containing the Item if found, or std::nullopt if not found.
     */
    std::optional<Item> find(const std::string& id) const {
        auto cartesian = itemDatabase.find(id);
        if(cartesian != itemDatabase.end()) return cartesian->second;
        return std::nullopt;
    }
};
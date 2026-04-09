#ifndef ITEM_DATABASE_H
#define ITEM_DATABASE_H

#include "common.h"
#include <string>
#include <random>
#include <unordered_map>
#include <optional>

// Properties struct representing the equip type and requirements of an item.
struct Properties {
    bool          stackable      = false;                // Indicates if the item can be stacked.
    bool          sellable       = true;                 // Indicates if the item can be sold.
    bool          consumable     = false;                // Indicates if the item is consumed on use (e.g., potions).
    int           max_stack      = 99;                   // Determines on how much item clump or stack to one slot.
    EquipType     equip_type     = EquipType::None;      // Defines the type of equipment (e.g., weapon, armor).
    WeaponRequire weapon_require = WeaponRequire::None;  // For weapons, indicates if it's one-handed or two-handed.
    ArmorType     armor_type     = ArmorType::None;      // For armor, indicates if it's light, medium, heavy, etc.
};

// Item struct representing equippable items in the game.
struct Item {
    std::string name;
    std::string id;
    bool equipped = false;

    Attributes attribute{};
    Properties property{};

    double increase_HP  = 0.0;  // Adds health points by a percentage.
    double increase_DMG = 0.0;  // Adds flat damage by a percentage.
    double base_damage  = 0.0;  // Flat damage of a weapon.
    double health_bonus = 0.0;  // Extra health points.
    double damage_bonus = 0.0;  // Extra weapon damage.
    double resist_bonus = 0.0;  // Extra armor/resist.
    double dodge_bonus  = 0.0;  // Extra dodge chance.

    // TODO: Create a method that calculates the total value of the item based on its properties and attributes.
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
    LootTable(std::vector<std::pair<std::string, double>> list);

    /**
     * @brief Randomly selects an item ID based on the weights.
     * @return A randomly selected item ID from the loot table.
     * @warning Undefined behavior if the loot table is empty (no items with weight > 0).
     */
    std::string dropItem();

    /**
     * @brief Drops multiple items based on the loot table.
     * @param x The number of items to drop.
     * @return A vector of randomly selected item IDs from the loot table.
     * @warning Undefined behavior if the loot table is empty (no items with weight > 0).
     */
    std::vector<std::string> dropItem(const size_t x);
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
    static ItemDatabase &instance();

    /**
     * @brief Loads items from a JSON file into the item database.
     * @param FILE_NAME The path to the JSON file containing item data.
     * @return true if loading was successful, false otherwise.
     */
    bool load(const std::string &FILE_NAME = "json/items.json");
    /**
     * @brief Looks up an item by its ID in the item database.
     * @param id The ID of the item to find.
     * @return An optional containing the Item if found, or std::nullopt if not found.
     */
    std::optional<Item> find(const std::string &id) const;
};

#endif
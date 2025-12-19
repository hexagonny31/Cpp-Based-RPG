#include "hutils.h"
#include "item_database.hpp"
#include "json.hpp"

#include <fstream>
#include <optional>
#include <vector>
#include <unordered_map>

namespace fs = std::filesystem;
using namespace std::string_literals;
using json = nlohmann::json;

static std::unordered_map<std::string, Item> itemDatabase;
bool load(const std::string& FILE_NAME) {
    std::ifstream file("json/items.json");
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
        item.id               = entry["id"].get<std::string>();
        item.name             = entry["name"].get<std::string>();
        item.equippable       = entry["equippable"].get<bool>();

        item.vigor_mod        = entry.value("vigor_mod", 0);
        item.strength_mod     = entry.value("strength_mod", 0);
        item.endurance_mod    = entry.value("endurance_mod", 0);
        item.intelligence_mod = entry.value("intelligence_mod", 0);
        item.dexterity_mod    = entry.value("dexterity_mod", 0);

        item.increase_HP      = entry.value("increase_HP", 0);
        item.health_bonus     = entry.value("health_bonus", 0);
        item.damage_bonus     = entry.value("damage_bonus", 0);
        item.resist_bonus     = entry.value("resist_bonus", 0);
        item.dodge_bonus      = entry.value("dodge_bonus", 0);

        itemDatabase[item.id] = item;
    }
    return true;
}

std::optional<Item> find(const std::string& item_id) {
    auto cartesian = itemDatabase.find(item_id);
    if(cartesian != itemDatabase.end()) return cartesian->second;
    return std::nullopt;
}
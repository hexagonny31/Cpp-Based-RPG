#include "hutils.h"
#include "item_database.h"
#include "json.hpp"

#include <fstream>
#include <utility>

using nj = nlohmann::json;

LootTable::LootTable(std::vector<std::pair<std::string, double>> list) : item_ids(), weights(), gen(std::random_device{}())
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

std::string LootTable::dropItem()
{
    return item_ids[dist(gen)];
}

std::vector<std::string> LootTable::dropItem(const size_t x)
{
    std::vector<std::string> it;
    for(size_t i = 0; i < x; ++i) it.push_back(dropItem());
    return it;
}

ItemDatabase &ItemDatabase::instance()
{
    static ItemDatabase db;
    return db;
}

bool ItemDatabase::load(const std::string &FILE_NAME)
{
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
    int i = 0;
    for(const auto &e : json) {
        hUtils::text.clearAll();
        if(e.contains("name") && e.contains("id")) {
            Item item;
            item.id   = e["id"].get<std::string>();
            item.name = e["name"].get<std::string>();

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
        hUtils::bar.setBar("Loading items", i, json.size());
        ++i;
        if(i >= json.size()) {
            hUtils::text.clearAll();
            hUtils::bar.setBar("Loading items (Done)", json.size(), json.size());
            hUtils::Sleep(500);
        }
    }
    return true;
}

std::optional<Item> ItemDatabase::find(const std::string &id) const
{
    auto cartesian = itemDatabase.find(id);
    if(cartesian != itemDatabase.end()) return cartesian->second;
    return std::nullopt;
}
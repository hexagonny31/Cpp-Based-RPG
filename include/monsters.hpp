#pragma once

#include "hutils.h"
#include "entity.h"
#include "common.h"
#include "item_database.hpp"
#include "json.hpp"

namespace fs = std::filesystem;
using nj = nlohmann::json;

struct MonsterDatabase {
private:
    MonsterDatabase() = default;
    static std::unordered_map<std::string, Monster> monsterDatabase;
public:
    static MonsterDatabase &instance() {
        static MonsterDatabase db;
        return db;
    }

    bool load(const std::string& FILE_NAME = "json/monsters.json") {
        std::ifstream file(FILE_NAME);
        if(!file.is_open()) return false;

        nj json;
        try {
            file >> json;
        } catch(const nj::parse_error&) {
            return false;
        }
        if(!json.is_array()) return false;

        monsterDatabase.clear();
        int i = 0;
        for(const auto &e : json) {
            hUtils::text.clearAll();
            if(e.contains("id")) {
                Monster monster;
                monster.setID(e["id"].get<std::string>());
                if(!e.contains("name")) monster.setName(e["id"].get<std::string>());
                else monster.setName(e["name"].get<std::string>());

                if(e.contains("attribute") && e["attribute"].is_object()) {
                    const auto& a = e["attribute"];
                    monster.setVigor(a.value("vigor", 0));
                    monster.setStrength(a.value("strength", 0));
                    monster.setEndurance(a.value("endurance", 0));
                    monster.setIntelligence(a.value("intelligence", 0));
                    monster.setDexterity(a.value("dexterity", 0));
                }
                if(e.contains("loot") && e["loot"].is_object()) {
                    std::vector<std::pair<std::string, double>> list;
                    for(const auto &item : e["loot"]) {
                        if(item.is_object() &&
                        item.contains("id") && item["id"].is_string() &&
                        item.contains("weight") && item["weight"].is_number()) {
                            
                            std::string id = item["id"].get<std::string>();
                            double weight = item["weight"].get<double>();

                            if(weight <= 0) continue;

                            list.emplace_back(id, weight);
                        };
                    }
                    LootTable lt{std::move(list)};
                    monster.setLootTable(lt);
                }
                monsterDatabase[monster.getID()] = monster;
            }
            hUtils::bar.setBar("Loading monsters", i, json.size());
            ++i;
            if(i >= json.size()) {
                hUtils::text.clearAll();
                hUtils::bar.setBar("Loading monsters (Done)", json.size(), json.size());
                hUtils::Sleep(500);
            }
        }
        return true;
    }

    std::optional<Monster> find(const std::string &id) const {
        auto cartesian = monsterDatabase.find(id);
        if(cartesian != monsterDatabase.end()) return cartesian->second;
        return std::nullopt;
    }
};
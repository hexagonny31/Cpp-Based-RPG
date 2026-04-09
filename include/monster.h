#ifndef MONSTER_H
#define MONSTER_H

#include "entity.h"
#include "item_database.h"

#include <string>
#include <utility>

struct Monster : public Entity
{
private:
    std::string id = "";
    int lvl        = 1;
    int xp_reward  = 0;
    std::pair<int,int> gold_reward = {0,0};
    LootTable loot;

public:
    Monster() : loot({{"Nothing", 1.0}}) {}

    std::string        getID() const;
    int                getLvl() const;
    int                getXP() const;
    std::pair<int,int> getGold() const;
    LootTable          getLootTable() const;

    void setID       (const std::string new_id);
    void setLvl      (const int new_lvl);
    void setXP       (const int new_xp_reward);
    void setGold     (const std::pair<int,int> new_gold_reward);
    void setLootTable(const LootTable new_loot);
};

#endif
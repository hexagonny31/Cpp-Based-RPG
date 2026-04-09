#include "monster.h"

std::string Monster::getID() const
{
    return id;
}

int Monster::getLvl() const
{
    return lvl;
}

int Monster::getXP() const
{
    return xp_reward;
}

std::pair<int,int> Monster::getGold() const
{
    return gold_reward;
}
LootTable Monster::getLootTable() const 
{
    return loot;
}

void Monster::setID(const std::string new_id)
{
    id = new_id;
}

void Monster::setLvl(const int new_lvl)
{
    lvl = new_lvl;
}

void Monster::setXP(const int new_xp_reward)
{
    xp_reward = new_xp_reward;
}

void Monster::setGold(const std::pair<int,int> new_gold_reward)
{
    gold_reward = std::move(new_gold_reward);
}

void Monster::setLootTable(const LootTable new_loot)
{
    loot = std::move(new_loot);
}
#include "monster.h"
#include "player.h"

#include <iostream>
#include <random>

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

bool Monster::attack(Entity& target)
{
    double dmg = getDamage(false);
    std::pair<double, double> t_resists = {target.getPhysicalResist(false), target.getMagicalResist(false)};
    if(target.didDodge()) {
        if(dynamic_cast<Player*>(&target)) {
            std::cout << "You dodged the " << getName() << "'s attack!\n";
        } else {
            std::cout << "The " << target.getName() << " dodged the " << getName() << "'s attack!\n";
        }
        return false;
    }

    double block_mult = 1.0 - target.getBlockReduction();
    dmg *= block_mult;

    DamageType dmg_type = DamageType::Physical;  // temporary. soon to be something more cooler.
    double dmg_dealt = 0.0;
    switch(dmg_type) {
    case DamageType::Physical:
        dmg_dealt = dmg * (1.0 - t_resists.first);
        break;
    case DamageType::Magical:
        dmg_dealt = dmg * (1.0 - t_resists.second);
        break;
    default:
        std::cout << "The " << getName() << " cannot attack!\n";
        return false;
    }

    if(dmg_dealt < 0.0) dmg_dealt = 0.0;

    target.setCurrentHealth(target.getCurrentHealth() - dmg_dealt);
    if(target.getBlockReduction() > 0.0) {
        if(dynamic_cast<Player*>(&target)) std::cout << "You blocked its attack! ";
        else std::cout << "The " << target.getName() << " blocked its attack! ";
        std::cout << "The " << getName() << " dealt " << dmg_dealt << " damage! (" << target.getBlockReduction()*100 << "% blocked)\n";
    } else {
        std::cout << "The " << getName() << " dealt " << dmg_dealt << " damage!\n";
    }
    return true;
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

double Monster::getBlockChance() const
{
    double chance = 0.1;
    chance += (static_cast<double>(lvl) * 0.004);
    chance += block_chance_mod;
    chance += (getAttributes().endurance - 10) * 0.001;

    if(chance < 0.1) chance = 0.1;
    if(chance > 0.4) chance = 0.4;

    return chance;
}

void Monster::setGold(const std::pair<int,int> new_gold_reward)
{
    gold_reward = std::move(new_gold_reward);
}

void Monster::setLootTable(const LootTable new_loot)
{
    loot = std::move(new_loot);
}
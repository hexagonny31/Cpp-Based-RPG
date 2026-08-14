#include "entity.h"

#include <vector>

std::string Entity::getName      () const
{
    return name;
}

Attributes Entity::getAttributes() const
{
    return attribute;
}

std::vector<Item*> Entity::getEquipment () const
{
    return equipment;
}

Item* Entity::getEquipment(Slot slot) const
{
    return equipment[to_index(slot)];
}

std::string Entity::getEquipmentName(Slot slot) const
{
    Item* item = getEquipment(slot);
    return item ? item->name : "Empty";
}

// health/mana manipulators n' shit
double Entity::getCurrentHealth() const
{
    return curr_hp;
}

double Entity::getTotalHealth(const bool ignore_equipment) const
{
    double total_vigor = attribute.vigor;
    double bonus_hp = 0.0;
    
    if(!ignore_equipment) {
        for(const Item* item : equipment) {
            if(item && item->equipped) {
                bonus_hp += item->health_bonus;
                total_vigor += item->attribute.vigor;
            }
        }
        return df_hp + bonus_hp + (10.0 * total_vigor);
    } else {
        return (10.0 * total_vigor);
    }
}

double Entity::getCurrentMana() const
{
    return curr_mp;
}

double Entity::getTotalMana(const bool ignore_equipment) const 
{ 
    double bonus_mp = 15.0 * attribute.intelligence;
    
    if(!ignore_equipment) {
        for(const Item* item : equipment) {
            if(item && item->equipped) bonus_mp += item->attribute.intelligence;
        }
        return df_mp + bonus_mp;
    } else {
        return bonus_mp;
    }
}

// actual stats n' shit.
double Entity::getDamage(const bool ignore_equipment) const
{
    double base_dmg = dmg;
    int total_str = attribute.strength;
    
    if(!ignore_equipment) {
        for(const Item* item : equipment) {
            if(item && item->equipped) total_str += item->attribute.strength;
        }
        Item* weapon = getEquipment(Slot::MainHand);
        if(weapon && weapon->equipped) {
            base_dmg += weapon->base_damage;
        }
        double total_weapon_dmg = base_dmg;
        return total_weapon_dmg * (1.0 + ((max_bonus * total_str) / (100.0 * (max_bonus + total_str))));
    } else {
        return (max_bonus * total_str) / (100.0 * (max_bonus + total_str));
    }
}

double Entity::getPhysicalResist(const bool ignore_equipment) const
{
    int total_end = attribute.endurance;
    double total_resist_bonus = 0.0;

    if(!ignore_equipment) {
        for(const Item* item : equipment) {
            if(item && item->equipped) {
                total_end += item->attribute.endurance;    // from any equipment
                total_resist_bonus += item->resist_bonus;  // from armor and shields
            }
        }
        return total_resist_bonus + ((max_bonus * total_end) / (100.0 * (max_bonus + total_end)));
    } else {
        return (max_bonus * total_end) / (100.0 * (max_bonus + total_end));
    }
}

double Entity::getDodgeChance(const bool ignore_equipment) const
{
    int total_dex = attribute.dexterity;
    double total_dodge_bonus = 0.0;
    
    if(!ignore_equipment) {
        for(const Item* item : equipment) {
            if(item && item->equipped) {
                total_dex += item->attribute.dexterity;  // from any equipment
                total_dodge_bonus += item->dodge_bonus;  // from armor and shields
            }
        }
    }

    double base_dodge = (max_bonus * total_dex) / (100.0 * (max_bonus + total_dex));
    return (std::min)(1.0, (ignore_equipment ? 0 : 0.05) + total_dodge_bonus + base_dodge);
}

void Entity::setName(const std::string& newName)
{
    name = newName;
}

void Entity::setDefaultVitals(const double new_df_hp, const double new_df_mp)
{
    df_hp = new_df_hp;
    df_mp = new_df_mp;
    updateHealth();
    updateMana();
}

void Entity::setCurrentHealth(const double new_hp)       
{
    curr_hp = new_hp;
}

void Entity::setCurrentMana(const double new_mp)       
{
    curr_mp = new_mp;
}

void Entity::setAttributes(const Attributes new_attr) 
{
    attribute = new_attr;
}

bool Entity::isAlive() const
{
    return curr_hp > 0.0;
}

void Entity::updateHealth()
{
    curr_hp = getTotalHealth(false); 
}

void Entity::updateMana()
{
    curr_mp = getTotalMana(false); 
}

bool Entity::didDodge() const
{
    double total = getDodgeChance(false);
    double random = static_cast<double>(rand())/RAND_MAX;
    return random < total;
}
#ifndef ENTITYOBJECT_H
#define ENTITYOBJECT_H

#include "common.h"
#include "item_database.h"

#include <vector>

struct Entity
{
protected:
    std::string name;
    Attributes attribute;
    std::vector<Item*> equipment;
    double dmg = 1.0;

    Entity() : equipment(to_index(Slot::COUNT), nullptr) {}

    static constexpr double max_bonus = 250.0;
    double df_hp = 100.0;  // fallback value. just incase if it doesn't update initially.
    double df_mp = 100.0;
    double curr_hp;
    double curr_mp;

public:
    std::string        getName      () const;
    Attributes         getAttributes() const;
    std::vector<Item*> getEquipment () const;
    Item*       getEquipment    (Slot slot) const;
    std::string getEquipmentName(Slot slot) const;

    // health/mana manipulators n' shit
    double getCurrentHealth() const;
    double getCurrentMana()   const;
    double getTotalHealth(const bool ignore_equipment) const; // added bool flag for displaying raw stats.
    double getTotalMana  (const bool ignore_equipment) const;
    // actual stats n' shit.
    double getDamage(const bool ignore_equipment) const;
    double getPhysicalResist(const bool ignore_equipment) const;
    double getDodgeChance(const bool ignore_equipment) const;

    void setName         (const std::string& newName);
    void setDefaultVitals(const double new_df_hp, const double new_df_mp);
    void setCurrentHealth(const double new_hp);
    void setCurrentMana  (const double new_mp);
    void setAttributes   (const Attributes new_attr);

    void updateHealth();
    void updateMana();
    bool isAlive()  const;
    bool didDodge() const;
};

#endif
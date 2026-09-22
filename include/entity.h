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

    // blockstatus
    int base_max_blocks = 3;
    int current_block_uses = base_max_blocks;
    int consecutive_blocks = 0;
    bool is_blocking = false;
    double block_bonus = 0.0;  // percentage of damage reduction when blocking. (e.g. 0.5 = 50% damage reduction)

public:
    std::string        getName      () const;
    Attributes         getAttributes() const;
    std::vector<Item*> getEquipment () const;
    Item*       getEquipment    (Slot slot) const;
    std::string getEquipmentName(Slot slot) const;

    void setName(const std::string& newName);

    // health/mana manipulators n' shit
    double getCurrentHealth() const;
    double getCurrentMana()   const;
    double getTotalHealth(const bool ignore_equipment) const; // added bool flag for displaying raw stats.
    double getTotalMana  (const bool ignore_equipment) const;

    void setDefaultVitals(const double new_df_hp, const double new_df_mp);
    void setCurrentHealth(const double new_hp);
    void setCurrentMana  (const double new_mp);

    // actual stats n' shit.
    double getDamage(const bool ignore_equipment) const;
    double getPhysicalResist(const bool ignore_equipment) const;
    double getDodgeChance(const bool ignore_equipment) const;

    void setAttributes   (const Attributes new_attr);

    // blocking n' shit
    virtual bool startBlocking();
    void   endBlocking();
    void   regainBlockUse();
    double getBlockReduction() const;
    int    getMaxBlockUses() const;
    int    getCurrentBlockUses() const;

    void updateHealth();
    void updateMana();
    void updateBlockUses();
    void resetConsecutiveBlocks();
    bool isAlive() const;
    bool isCurrentlyBlocking() const;
    bool didDodge() const;
};

#endif
#ifndef PLAYER_H
#define PLAYER_H

#include "entity.h"

struct Player : public Entity
{
private:
    int allocation_pts = 0;
    std::vector<Item> inventory;

public:
    Player() = default;
    Player(const Player&) = delete;
    Player& operator=(const Player&) = delete;

    Player(Player&&) = default;
    Player& operator=(Player&&) = default;

    int         getAllocationPts()            const;
    Item        getItem         (size_t slot) const;
    std::string getItemName     (size_t slot) const;
    std::vector<Item>&       getInventory();
    const std::vector<Item>& getInventory() const;

    void setAllocation(int newAllocation);
    bool setAttribute();

    bool addToInventory(const std::string& id);
    void equipItem(Item* item, Slot slot);
    void unequipItem(Slot slot);
};

#endif
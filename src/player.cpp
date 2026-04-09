#include "player.h"
#include "hutils.h"

#include <iostream>

int Player::getAllocationPts() const
{
    return allocation_pts;
}

Item Player::getItem(size_t slot) const
{
    return inventory[slot];
}

std::string Player::getItemName(size_t slot) const
{
    if(slot < 0 || slot >= inventory.size()) return "Empty";
    return inventory[slot].name;
}

std::vector<Item> &Player::getInventory()
{
    return inventory;
}

const std::vector<Item> &Player::getInventory() const
{
    return inventory;
}

void Player::setAllocation(int newAllocation)
{
    allocation_pts = newAllocation;
}

bool Player::setAttribute()
{
    if(allocation_pts <= 0) return false;
    while(true) {
        char c = '\0';
        hUtils::text.clearAll(500);
        std::cout << "Choose an attribute to increase:\n";
        hUtils::table.setElements(
            " [1] Vigor",     " [4] Intelligence",
            " [2] Strength",  " [5] Dexterity",
            " [3] Endurance", " [E] Cancel"
        );
        hUtils::table.toColumn("left", 16, 2);
        c = hUtils::GetInputKeymap({'1','2','3','4','5','E'});
        
        if(c == 'E') return false;

        int allocation = hUtils::GetIntegerInput(
            "How many points would you like to allocate? (avail: "
            + std::to_string(allocation_pts) + ")\n", 
            1, allocation_pts);

        switch(c) {
        case '1': attribute.vigor        += allocation; break;
        case '2': attribute.strength     += allocation; break;
        case '3': attribute.endurance    += allocation; break;
        case '4': attribute.intelligence += allocation; break;
        case '5': attribute.dexterity    += allocation; break;
        default:     continue;
        }
        allocation_pts -= allocation;
        std::cout << "Points allocated!\n";
        return true;
    }
}

bool Player::addToInventory(const std::string& id)
{
    auto init = ItemDatabase::instance().find(id);
    if(!init) return false;
    inventory.push_back(init.value());
    return true;
}

void Player::equipItem(Item* item, Slot slot)
{
    if(!item || item->property.equip_type == EquipType::None) return;
    Item* current = getEquipment(slot);
    if(current) current->equipped = false;
    equipment[to_index(slot)] = item;
    item->equipped = true;
}

void Player::unequipItem(Slot slot)
{
    Item* current = getEquipment(slot);
    if(!current) return;
    current->equipped = false;
    equipment[to_index(slot)] = nullptr;
}
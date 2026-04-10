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
        int pos = 0;
        int sel = 0;
        while(true) {
            hUtils::text.clearAll(15);
            std::string opt[6] = {"Vigor","Strength","Endurance","Intelligence","Dexterity"};
            std::cout << "Choose an attribute to increase:\n";
            for(size_t i(0); i != 5; ++i) {
                if(i == pos)
                    std::cout << hUtils::text.bgColor(45) << (int)i+1 << ". " << opt[i] << hUtils::text.defaultText() << '\n';
                else std::cout << (int)i+1 << ". " << opt[i] << '\n';
            }
            char c = hUtils::GetInputKeymap({'W', 'S', 'E', '\x0D'});
            if(c == 'E') return false;

            switch(c) {
            case 'W':
                if(!(pos - 1 < 0)) --pos;
                continue;
            case 'S':
                if(!(pos + 1 >= 6)) ++pos;
                continue;
            case '\x0D':
                sel = pos;
                break;
            default:
                continue;
            }
            break;
        }

        int allocation = hUtils::GetIntegerInput(
            "How many points would you like to allocate? (avail: "
            + std::to_string(allocation_pts) + ")\n", 
            1, allocation_pts);

        switch(sel) {
        case '1': attribute.vigor        += allocation; break;
        case '2': attribute.strength     += allocation; break;
        case '3': attribute.endurance    += allocation; break;
        case '4': attribute.intelligence += allocation; break;
        case '5': attribute.dexterity    += allocation; break;
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
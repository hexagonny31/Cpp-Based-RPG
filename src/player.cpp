#include "player.h"
#include "hutils.h"

#include <iostream>
#include <random>

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

bool Player::attack(Entity& target)
{
    Item* weapon = getEquipment(Slot::MainHand);
    if(!weapon) {
        std::cout << "You have no weapon equipped! You cannot attack!\n";
        return false;
    }

    double dmg = getDamage(false);
    std::pair<double, double> t_resists = {target.getPhysicalResist(false), 0.01};

    if(target.didDodge()) {
        std::cout << "The " << target.getName() << " dodged your attack!\n";
        return false;
    }

    double dmg_dealt = 0.0;
    switch(weapon->property.damage_type) {
    case DamageType::Physical:
        dmg_dealt = dmg * (1.0 - t_resists.first);
        break;
    case DamageType::Magical:
        dmg_dealt = dmg * (1.0 - t_resists.second);
        break;
    default:
        std::cout << "You cannot attack with this weapon!\n";
        return false;
    }

    double block_reduc = target.getBlockReduction();
    if(block_reduc > 0.0) {
        if(getAttributes().strength > target.getAttributes().endurance) {
            static std::random_device rd;
            static std::mt19937 gen(rd());
            std::uniform_real_distribution<double> dis(0.0, 1.0);

            if(dis(gen) < 0.80) {
                block_reduc = 0.0;
                std::cout << "You broke the " << target.getName() << "'s block!\n";
            } else {
                block_reduc *= 0.80;
            }
        }
        dmg_dealt *= (1.0 - block_reduc);
    }

    if(dmg_dealt < 0.0) dmg_dealt = 0.0;

    target.setCurrentHealth(target.getCurrentHealth() - dmg_dealt);
    if(block_reduc > 0.0) {
        std::cout << "The " << target.getName() << " blocked! You dealt " << dmg_dealt << " damage! (" << block_reduc*100 << "% blocked)\n";
    } else {
        std::cout << "You dealt " << dmg_dealt << " damage to the " << target.getName() << "!\n";
    }
    return true;
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

bool Player::addToInventory(const std::vector<std::string>& ids)
{
    bool added = false;
    for(const std::string& id : ids) if(addToInventory(id)) added = true;
    return added;
}

bool Player::removeFromInventory(const std::string& id)
{
    for(auto it = inventory.begin(); it != inventory.end(); ++it) {
        if(it->id != id) continue;
        if(it->equipped) {
            for(size_t i = 0; i < (size_t)Slot::COUNT; ++i) {
                Slot slot = (Slot)i;
                if(getEquipment(slot) == &(*it)) {
                    unequipItem(slot);
                    break;
                }
            }
        }
        inventory.erase(it);
        return true;
    }
    return false;
}

bool Player::removeFromInventory(const std::vector<std::string>& ids)
{
    bool removed = false;
    for(const std::string& id : ids) if(removeFromInventory(id)) removed = true;
    return removed;
}

void Player::equipItem(Item* item, Slot slot)
{
    if(!item || item->property.equip_type == EquipType::None) return;
    Item* current = getEquipment(slot);
    if(current) current->equipped = false;
    equipment[to_index(slot)] = item;
    item->equipped = true;
    updateBlockUses();
}

void Player::unequipItem(Slot slot)
{
    Item* current = getEquipment(slot);
    if(!current) return;
    current->equipped = false;
    equipment[to_index(slot)] = nullptr;
    updateBlockUses();
}
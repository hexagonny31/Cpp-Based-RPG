#pragma once

#include "hutils.h"
#include "item_database.hpp"
#include "save_manager.h"

#include <iostream>
#include <vector>
#include <algorithm>
#include <unordered_map>

bool equip(Player &player){
    struct EquipOption {
        const Item* item;
        std::string name;
        size_t i;
    };
    std::vector<EquipOption> opt;
    for(size_t i = 0; i != player.inventory.size(); ++i) {
        const  Item &it = player.inventory[i];
        if(it.property.equip_type != EquipType::None && !it.equipped) opt.push_back({&it, it.name, i});
    }

    if(opt.empty()) return false;
    
    while(true) {
        std::cout << "Choose an item to equip:\n";
        for(size_t i = 0; i != opt.size(); ++i) std::cout << (int)i+1 << ". " << opt[i].name <<  '\n';
        std::string x = "";
        std::cout << "\n> ";
        std::getline(std::cin, x);

        if(hUtils::text.toLowerCase(x) == "exit" || hUtils::text.toLowerCase(x) == "e") return false;
        size_t y;
        try {
            y = std::stoul(x);
        } catch(...) {
            hUtils::text.reject("Unable to find item.", opt.size() + 3);
            continue;
        }
        if(y == 0 || y > opt.size()) {
            hUtils::text.reject("No such item.", opt.size() + 3);
            continue;            
        }
        EquipOption &selected = opt[y-1];
        const Item &equip = *selected.item;
        Slot slot;
        switch(equip.property.equip_type) {
        case EquipType::Weapon: {
            while(true) {
                std::cout << "\nPick a hand slot: [Q] Main Hand | [W] Off-Hand | [E] Exit\n";
                char z = hUtils::GetInputKeymap({'E','Q','W'});
                if(z == 'E') return false;
                else if(z == 'Q') { slot = Slot::MainHand; break; }
                else if(z == 'W') { slot = Slot::OffHand;  break; }
            }
            break;
        }
        case EquipType::Helmet:     slot = Slot::Helmet;     break;
        case EquipType::Chestplate: slot = Slot::Chestplate; break;
        case EquipType::Boots:      slot = Slot::Boots;      break;
        }
        player.equipItem(&player.inventory[selected.i], slot);
        std::cout << "\nEquipped " << player.inventory[selected.i].name << ".\n";
        hUtils::Sleep(2500);
        return true;
    }
}

bool unEquip(Player &player) {
    if(player.getEquipment().empty()) return false;
    std::vector<Slot> opt;

    for(size_t i = 0; i != to_index(Slot::COUNT); ++i) {
        Slot s = static_cast<Slot>(i);
        if(player.getEquipment(s) == nullptr) continue;
        opt.push_back(s);
    }

    while(true) {
        std::cout << "Choose an item to unequip:\n";
        for(int i = 0; i != player.getEquipment().size(); ++i) std::cout << i+1 << ". " << player.getEquipmentName((Slot)i) <<  '\n';
        std::string x = "";
        std::cout << "\n> ";
        std::getline(std::cin, x);

        if(hUtils::text.toLowerCase(x) == "exit" || hUtils::text.toLowerCase(x) == "e") return false;
        size_t y;
        try {
            y = std::stoul(x);
        } catch(...) {
            hUtils::text.reject("Unable to find item.", opt.size() + 3);
            continue;
        }
        if(y == 0 || y > opt.size()) {
            hUtils::text.reject("No such item.", opt.size() + 3);
            continue;            
        }
        Slot selected = opt[y-1];
        std::cout << "\nUnequipped " << player.getEquipmentName(selected) << ".\n";
        player.unequipItem(selected);
        hUtils::Sleep(1800);
        return true;
    }
}

bool Player::setAttribute() {
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
        case '1':    attribute.vigor        += allocation; break;
        case '2':    attribute.strength     += allocation; break;
        case '3':    attribute.endurance    += allocation; break;
        case '4':    attribute.intelligence += allocation; break;
        case '5':    attribute.dexterity    += allocation; break;
        default:     continue;
        }
        allocation_pts -= allocation;
        std::cout << "Points allocated!\n";
        return true;
    }
}

void statistics(Player &player) {
    while(true) {
        char c ='\0';
        hUtils::text.clearAll(500);
        std::cout << "Player Info:\n"
                << "  Name:   " << player.getName() << '\n'
                << "  Points: " << player.getAllocationPts() << '\n';
        hUtils::bar.setBar(player.getCurrentHealth(), player.getTotalHealth(), 124);
        hUtils::bar.setBar(player.getCurrentMana(),   player.getTotalMana());
        hUtils::text.toLine();
        std::cout << "Equipment:\n"
                << "  Main Hand:  " << player.getEquipmentName(Slot::MainHand)   << '\n'
                << "  Off-Hand:   " << player.getEquipmentName(Slot::OffHand)    << '\n'
                << "  Helmet:     " << player.getEquipmentName(Slot::Helmet)     << '\n'
                << "  Chestplate: " << player.getEquipmentName(Slot::Chestplate) << '\n'
                << "  Boots:      " << player.getEquipmentName(Slot::Boots)      << '\n';
        hUtils::text.toLine();
        std::cout << "Attributes:\n"
                << "  Vigor:        " << player.getVigor()        << '\n'
                << "  Strength:     " << player.getStrength()     << '\n'
                << "  Endurance:    " << player.getEndurance()    << '\n'
                << "  Intelligence: " << player.getIntelligence() << '\n'
                << "  Dexterity:    " << player.getDexterity()    << '\n';
        hUtils::text.toLine();
        std::cout << "[Q] Allocate | [A] Equip | [S] Unequip | [E] Exit\n";

        c = hUtils::GetInputKeymap({'Q','A','S','E'});

        switch(std::toupper(c)) {
        case 'Q': player.setAttribute(); break;
        case 'A': equip(player);          break;
        case 'S': unEquip(player);        break;
        case 'E': return;
        }
    }
}

void inventory(Player &player) {
    const int ITEM_LIMIT = 10;
    int total_items  = player.inventory.size();
    int total_pages  = (total_items + ITEM_LIMIT - 1) / ITEM_LIMIT;
    int current_page = 1;
    std::cout << "Showing inventory...\n\n";
    while(true) {
        hUtils::text.clearAll();
        int start = (current_page - 1) * ITEM_LIMIT;
        int end   = (start + ITEM_LIMIT < total_items) ? (start + ITEM_LIMIT) : total_items;
        std::cout << "Inventory (Page " << current_page << " of " << total_pages << ")\n";
        for(int i = start; i < end; ++i) std::cout << i+1 << ". '" << player.getItemName((size_t)i) << "'\n";
        hUtils::table.setElements(
            " [Q] Next",    " [W] Previous",
            " [A] Equip",   " [S] Unequip",
            " [D] Sort by", " [E] Exit Inventory"
        );
        hUtils::table.toColumn("left", 13, 2);

        char c = hUtils::GetInputKeymap({'Q','W','A','S','D','E'});

        switch(std::toupper(c)) {
        case 'Q':
            if(current_page < total_pages) ++current_page;
            break;
        case 'W':
            if(current_page > 1) --current_page;
            break;
        case 'A':
            equip(player);
            break;
        case 'S':
            unEquip(player);
            break;
        case 'D': {
            std::cout << "Sort by: [1] Name | [2] Damage\n";
            char option = hUtils::GetInputKeymap({'1','2'});
            if(option == '1')
                std::sort(player.inventory.begin(), player.inventory.end(),
                          [](const Item &a, const Item &b) { return a.name < b.name; });
            else std::sort(player.inventory.begin(), player.inventory.end(),
                           [](const Item &a, const Item &b) { return a.increase_DMG < b.increase_DMG; });
            total_items  = player.inventory.size();
            total_pages  = (total_items + ITEM_LIMIT - 1) / ITEM_LIMIT;
            current_page = 1;  // we go back to the first index :)
            break;
        }
        case 'E':
            return;
        }
    }
}
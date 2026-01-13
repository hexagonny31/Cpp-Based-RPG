#pragma once

#include "hutils.h"
#include "item_database.hpp"
#include "save_manager.h"

#include <iostream>
#include <vector>
#include <algorithm>
#include <unordered_map>

void equip(Player &player) {
    std::unordered_map<std::string, int> lookup;
    std::vector<std::string> names;
    int i = 0;
    for(const Item item : player.inventory) {
        if(item.equippable && item.equip_type != EquipType::None) {
            lookup[item.name] = i;
            names.push_back(item.name);
        }
        ++i;
    }
    size_t selected;
    while(true) {
        std::cout << "Choose an item to equip (enter its value):\n";
        for(int i = 0; i < names.size(); ++i) std::cout << i+1 << ". " << names[i] << '\n';
        std::string input;
        std::cout << "> "; 
        std::getline(std::cin, input);

        if(hUtils::text.toLowerCase(input) == "exit" || hUtils::text.toLowerCase(input) == "e")
            throw UserCancelled("Operation canceld by user.");
        
        size_t choice;
        try {
            choice = std::stoul(input);
        } catch(...) {
            hUtils::text.reject("Unable to find item.", names.size() + 2);
            continue;
        }
        if(choice >= names.size()) {
            hUtils::text.reject("No such item.", names.size() + 2);
            continue;
        }
        selected = lookup.at(names[choice]);
        if(selected >= player.inventory.size() || !player.inventory[selected].equippable) {
            hUtils::text.reject("Item is not available.", names.size() + 2);
            continue;
        }
        break;
    }
    Slot slot;
    switch(player.inventory[selected].equip_type) {
    case EquipType::Weapon:
        while(true) {
            char w = charIn("Pick a hand slot: [Q] Main Hand | [W] Off-Hand | [E] Exit");
            if(w == 'e') throw UserCancelled("Operation cancelled by user.");
            if(w == 'q') { slot = Slot::MainHand; break; }
            if(w == 'w') { slot = Slot::OffHand;  break; }
            hUtils::text.reject("Invalid choice.", 2);
        }
        break;
    case EquipType::Helmet:     slot = Slot::Helmet;     break;
    case EquipType::Chestplate: slot = Slot::Chestplate; break;
    case EquipType::Boots:      slot = Slot::Boots;      break;
    }
    player.equipItem(&player.inventory[selected], slot);
    std::cout << "Equipped " << player.inventory[selected].name << ".\n";
    hUtils::sleep(2500);
    hUtils::text.clearAll();
}

void Player::setAttributes() {
    std::cout << "Modyfying Attributes...\n\n";
    hUtils::text.clearAll(500);
    std::cout << "Choose an attribute to increase:\n";
    hUtils::table.setElements(
        " [1] Vigor",     " [4] Intelligence",
        " [2] Strength",  " [5] Dexterity",
        " [3] Endurance", " [6] Cancel"
    );
    hUtils::table.toColumn("left", 16, 2);
    int choice = intIn("\n", 1, 6);
    if(choice == 6) throw UserCancelled("Operation cancelled by user.");
    int allocation = intIn("How many points would you like to allocate?\n", 1, allocation_pts);
    switch(choice) {
        case 1: attribute.vigor        += allocation; break;
        case 2: attribute.strength     += allocation; break;
        case 3: attribute.endurance    += allocation; break;
        case 4: attribute.intelligence += allocation; break;
        case 5: attribute.dexterity    += allocation; break;
    }
    allocation_pts -= allocation;
    std::cout << "Points allocated!\n";
}

void statistics(Player &player) {
    std::cout << "Showing stats...\n\n";
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
              << "  Main Hand:  " << player.getEquipmentName(Slot::Boots)      << '\n';
    hUtils::text.toLine();
    std::cout << "Attributes:\n"
              << "  Vigor:        " << player.getVigor()        << '\n'
              << "  Strength:     " << player.getStrength()     << '\n'
              << "  Endurance:    " << player.getEndurance()    << '\n'
              << "  Intelligence: " << player.getIntelligence() << '\n'
              << "  Dexterity:    " << player.getDexterity()    << '\n';
    hUtils::text.toLine();
    char choice = charIn("[A] Allocate | [S] Equip | [E] Exit\n");

    switch(choice) {
    case 'q': player.setAttributes(); break;
    case 'w': equip(player); break;
    }
    hUtils::text.clearAll();
}

void inventory(Player &player) {
    const int ITEM_LIMIT = 10;
    int total_items  = player.inventory.size();
    int total_pages  = (total_items + ITEM_LIMIT - 1) / ITEM_LIMIT;
    int current_page = 1;
    hUtils::text.clearAll(500);
    while(true) {
        int start = (current_page - 1) * ITEM_LIMIT;
        int end   = (start + ITEM_LIMIT < total_items) ? (start + ITEM_LIMIT) : total_items;
        hUtils::text.clearAll();
        std::cout << "Inventory (Page " << current_page << " of " << total_pages << ")\n";
        for(int i = start; i < end; ++i) std::cout << i + 1 << ". '" << player.getItemName(i) << "'\n";
        char choice = charIn("[Q] Next | [W] Previous | [A] Sort by | [E] Exit\n");
        
        if(choice == 'e') break;
        if(choice == 'q' && current_page < total_pages) {
            ++current_page;
        } else if(choice == 'w' && current_page > 1) {
            --current_page;
        } else if(choice == 'a') {
            int option = intIn("Sort by: [1] Name | [2] Damage | [3] Rarity\n", 1, 2);
            if((option == 1)) 
                std::sort(player.inventory.begin(), player.inventory.end(),
                          [](const Item &a, const Item &b) { return a.name < b.name; });
            else std::sort(player.inventory.begin(), player.inventory.end(),
                           [](const Item &a, const Item &b) { return a.increase_DMG < b.increase_DMG; });
            total_items  = player.inventory.size();
            total_pages  = (total_items + ITEM_LIMIT - 1) / ITEM_LIMIT;
            current_page = 1;  // we go back to the first page.
        }
    }
    hUtils::text.clearAll();
}
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
        for(int i = 0; i != opt.size(); ++i) std::cout << i+1 << ". " << opt[i].name <<  '\n';
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
                char z = charIn("\nPick a hand slot: [Q] Main Hand | [W] Off-Hand | [E] Exit\n");
                if(z == 'e') return false;
                if(z == 'q') { slot = Slot::MainHand; break; }
                if(z == 'w') { slot = Slot::OffHand;  break; }
                hUtils::text.reject("Invalid choice!", 4);
            }
            break;
        }
        case EquipType::Helmet:     slot = Slot::Helmet;     break;
        case EquipType::Chestplate: slot = Slot::Chestplate; break;
        case EquipType::Boots:      slot = Slot::Boots;      break;
        }
        player.equipItem(&player.inventory[selected.i], slot);
        std::cout << "\nEquipped " << player.inventory[selected.i].name << ".\n";
        hUtils::sleep(2500);
        hUtils::text.clearAll();
        break;
    }
    return true;
}

bool Player::setAttributes() {
    std::cout << "Modyfying Attributes...\n\n";
    hUtils::text.clearAll(500);

    if(allocation_pts <= 0) return false;
    
    std::cout << "Choose an attribute to increase:\n";
    hUtils::table.setElements(
        " [1] Vigor",     " [4] Intelligence",
        " [2] Strength",  " [5] Dexterity",
        " [3] Endurance", " [6] Cancel"
    );
    hUtils::table.toColumn("left", 16, 2);
    int choice = intIn("", 1, 6);

    if(choice == 6) return false;

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
    return true;
}

void statistics(Player &player) {
    std::cout << "Showing stats...\n\n";
    char choice;
    while(true) {
        choice ='\0';
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
        choice = charIn("[A] Allocate | [S] Equip | [E] Exit\n\n");

        if(choice == 'e') break;
        else if(choice == 'a') {
            if(!player.setAttributes()) hUtils::text.reject("Failed to modify attributes.");
        }
        else if(choice == 's') {
            if(!equip(player)) hUtils::text.reject("Failed to equip an item.");
        }
        else {
            hUtils::text.reject("Invalid option!");
        }
    }
    hUtils::text.clearAll();
}

void inventory(Player &player) {
    const int ITEM_LIMIT = 10;
    int total_items  = player.inventory.size();
    int total_pages  = (total_items + ITEM_LIMIT - 1) / ITEM_LIMIT;
    int current_page = 1;
    std::cout << "Showing inventory...\n\n";
    hUtils::text.clearAll(500);
    while(true) {
        int start = (current_page - 1) * ITEM_LIMIT;
        int end   = (start + ITEM_LIMIT < total_items) ? (start + ITEM_LIMIT) : total_items;
        hUtils::text.clearAll();
        std::cout << "Inventory (Page " << current_page << " of " << total_pages << ")\n";
        for(int i = start; i < end; ++i) std::cout << i + 1 << ". '" << player.getItemName(i) << "'\n";
        char choice = charIn("[Q] Next | [W] Previous | [A] Sort by | [S] Equip | [E] Exit\n");
        
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
        } else if(choice == 's') {
            if(!equip(player)) hUtils::text.reject("Failed to equip an item.");
        }
    }
    hUtils::text.clearAll();
}
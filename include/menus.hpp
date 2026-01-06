#pragma once

#include "hutils.h"
#include "item_database.hpp"
#include "save_manager.h"

#include <iostream>
#include <vector>

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
    std::cout << "How many points would you like to allocate?\n";
    int allocation = intIn("", 1, allocation_pts);
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

void allocationOption(Player &player) {
    std::cout << "You have " << player.getAllocationPts() << " point/s unallocated.\n";
    hUtils::sleep(500);
    if(proceed("Would you like to spend your points?")) player.setAttributes();
    else std::cout << "Make sure to spend it later.\n";
}

void statistics(Player &player) {
    std::cout << "Showing stats...\n\n";
    hUtils::text.clearAll(500);
    std::cout << "Player Info:\n"
              << "  " << player.getName() << '\n';
    hUtils::bar.setBar(player.getCurrentHealth(), player.getTotalHealth(), 124);
    hUtils::bar.setBar(player.getCurrentMana(),   player.getTotalMana());
    
    hUtils::text.toLine();
    
    std::cout << "Equipment:\n"
              << "  " << std::setw(12) << std::left << "Main Hand:"  << player.getEquipmentName(Slot::MainHand)   << '\n'
              << "  " << std::setw(12) << std::left << "Off-Hand:"   << player.getEquipmentName(Slot::OffHand)    << '\n'
              << "  " << std::setw(12) << std::left << "Helmet:"     << player.getEquipmentName(Slot::Helmet)     << '\n'
              << "  " << std::setw(12) << std::left << "Chestplate:" << player.getEquipmentName(Slot::Chestplate) << '\n'
              << "  " << std::setw(12) << std::left << "Main Hand:"  << player.getEquipmentName(Slot::Boots)      << '\n';

    hUtils::text.toLine();

    std::cout << "Attribuites:\n"
              << "  " << std::setw(14) << std::left << "Vigor:"        << player.getVigor()        << '\n'
              << "  " << std::setw(14) << std::left << "Strength:"     << player.getStrength()     << '\n'
              << "  " << std::setw(14) << std::left << "Endurance:"    << player.getEndurance()    << '\n'
              << "  " << std::setw(14) << std::left << "Intelligence:" << player.getIntelligence() << '\n'
              << "  " << std::setw(14) << std::left << "Dexterity:"    << player.getDexterity()    << '\n';

    hUtils::text.toLine();

    hUtils::sleep(2000);
    if(player.getAllocationPts() > 0) allocationOption(player);
    hUtils::pause();
    hUtils::text.clearAll();
}

void inventory(Player &player) {
    const int ITEM_LIMIT = 10;
    int total_items = player.inventory.size();
    int total_pages = (total_items + ITEM_LIMIT - 1) / ITEM_LIMIT;
    int current_page = 1;
    hUtils::text.clearAll(500);
    while(true) {
        int start = (current_page - 1) * ITEM_LIMIT;
        int end   = (start + ITEM_LIMIT < total_items) ? (start + ITEM_LIMIT) : total_items;
        hUtils::text.clearAll();
        std::cout << "Inventory (Page " << current_page << " of " << total_pages << ")\n";
        for(int i = start; i < end; ++i) std::cout << i + 1 << ". '" << player.getItemName(i) << "'\n";
        std::cout << "[Q] Next | [W] Previous | [E] Exit\n";
        char choice = charIn();
        
        if(choice == 'e') break;
        if(choice == 'q' && current_page < total_pages) {
            current_page++;
        } else if(choice == 'w' && current_page > 1) {
            current_page--;
        }
    }
    hUtils::text.clearAll();
}
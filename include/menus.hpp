#pragma once

#include "hutils.h"
#include "item_database.hpp"
#include "save_manager.h"

#include <iostream>
#include <vector>

void allocationOption(Player &player) {
    std::cout << "You have " << player.getAllocationPts() << " point/s unallocated.\n";
    hUtils::sleep(1000);
    if(proceed("Would you like to spend your points?")) player.setAttributes();
    else std::cout << "Make sure to spend it later.\n";
}

void statistics(Player &player) {
    std::cout << "Showing stats...\n\n";
    hUtils::text.clearAll();
    std::cout << "Player Info:\n"
              << "  " << player.getName();
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

    hUtils::sleep(2000);
    if(player.getAllocationPts() > 0) allocationOption(player);
    hUtils::pause();
}

void inventory(Player &player) {
    const int PAGE_LIMIT = 10;
    int total_items = player.inventory.size();
    int total_pages = (total_items + PAGE_LIMIT - 1) / PAGE_LIMIT;
    int current_page = 1;
    while(true) {
        int start = (current_page - 1) * PAGE_LIMIT;
        int end   = (start + PAGE_LIMIT < total_items) ? (start + PAGE_LIMIT) : total_items;
        hUtils::text.clearAll();
        std::cout << "Inventory (Page " << current_page << " of " << total_pages << ")\n";
        for(int i = start; i < end; ++i) std::cout << i + 1 << player.getItemName(i) << '\n';
        std::cout << "[Q] Next Page | [W] Previous | [E] Exit\n";
        char choice = charIn();
        
        if(choice == 'e') break;
        if(choice == 'q' && current_page < total_pages) {
            current_page++;
        } else if(choice == 'p' && current_page > 1) {
            current_page--;
        }
    }
}
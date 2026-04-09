#pragma once

#include "hutils.h"
#include "item_database.h"
#include "save_manager.h"
#include "player.h"

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
    const auto &inv = player.getInventory();
    for(size_t i = 0; i != inv.size(); ++i) {
        const  Item &it = inv[i];
        if(it.property.equip_type != EquipType::None && !it.equipped) opt.push_back({&it, it.name, i});
    }

    if(opt.empty()) return false;

    while(true) {
        int pos = 0;
        EquipOption &selected = opt[0];
        while(true) {
            hUtils::text.clearAll(15);
            std::cout << "Choose an item to equip:\n";
            for(size_t i(0); i != opt.size(); ++i) {
                if(i == pos)
                    std::cout << hUtils::text.bgColor(45) << (int)i+1 << ". " << opt[i].name << hUtils::text.defaultText() << '\n';
                else std::cout << (int)i+1 << ". " << opt[i].name << '\n';
            }
            char c = hUtils::GetInputKeymap({'W', 'S', 'E', '\x0D'});
            if(c == 'E') return false;
            
            switch(c) {
            case 'W':
                if(!(pos - 1 < 0)) --pos;
                continue;
            case 'S':
                if(!(pos + 1 >= (int)opt.size())) ++pos;
                continue;
            case '\x0D':
                selected = opt[pos];
                break;
            default:
                continue;
            }
            break;
        }

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
        player.equipItem(&player.getInventory()[selected.i], slot);
        std::cout << "\nEquipped " << player.getInventory()[selected.i].name << ".\n";
        hUtils::Sleep(2500);
        return true;
    }
}

bool unEquip(Player &player) {
    if(player.getEquipment().empty()) return false;

    Slot selected;
    while(true) {
        size_t pos = 0;
        selected = Slot::COUNT;
        while(true) {
            hUtils::text.clearAll(15);
            std::cout << "Choose an item to unequip:\n";
            for(size_t i(0); i != player.getEquipment().size(); ++i) {
                std::string temp = player.getEquipmentName((Slot)i);
                if(i == pos)
                    std::cout << hUtils::text.bgColor(45) << (int)i+1 << ". " << temp << hUtils::text.defaultText() << '\n';
                else std::cout << (int)i+1 << ". " << temp << '\n';
            }
            char c = hUtils::GetInputKeymap({'W', 'S', 'E', '\x0D'});
            if(c == 'E') return false;

            switch(c) {
            case 'W':
                if(!(pos - 1 < 0)) --pos;
                continue;
            case 'S':
                if(!(pos + 1 >= (int)Slot::COUNT)) ++pos;
                continue;
            case '\x0D':
                if(player.getEquipment((Slot)pos) == nullptr) continue;
                else selected = (Slot)pos;
                break;
            default:
                continue;
            }
            break;
        }
        std::cout << "\nUnequipped " << player.getEquipmentName(selected) << ".\n";
        player.unequipItem(selected);
        hUtils::Sleep(1800);
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
        hUtils::bar.setBar("HP", player.getCurrentHealth(), player.getTotalHealth(false), 124);
        hUtils::bar.setBar("MP", player.getCurrentMana(),   player.getTotalMana(false));
        hUtils::text.toLine();
        std::cout << "Equipment:\n"
                << "  Main Hand:  " << player.getEquipmentName(Slot::MainHand)   << '\n'
                << "  Off-Hand:   " << player.getEquipmentName(Slot::OffHand)    << '\n'
                << "  Helmet:     " << player.getEquipmentName(Slot::Helmet)     << '\n'
                << "  Chestplate: " << player.getEquipmentName(Slot::Chestplate) << '\n'
                << "  Boots:      " << player.getEquipmentName(Slot::Boots)      << '\n';
        hUtils::text.toLine();
        const Attributes &attribute = player.getAttributes();
        std::cout << std::setprecision(2) << "Attributes:\n"
                << "  Vigor:        " << attribute.vigor        << " (" << player.getTotalHealth(true)        << ")\n"
                << "  Strength:     " << attribute.strength     << " (" << player.getDamage(true)*100         << "%)\n"
                << "  Endurance:    " << attribute.endurance    << " (" << player.getPhysicalResist(true)*100 << "%)\n"
                << "  Intelligence: " << attribute.intelligence << " (" << player.getTotalMana(true)          << ")\n"
                << "  Dexterity:    " << attribute.dexterity    << " (" << player.getDodgeChance(true)*100    << "%)\n";
        hUtils::text.toLine();
        std::cout << "[Q] Allocate | [A] Equip | [S] Unequip | [E] Exit\n";

        c = hUtils::GetInputKeymap({'Q','A','S','E'});

        switch(std::toupper(c)) {
        case 'Q': player.setAttribute(); break;
        case 'A': equip(player);         break;
        case 'S': unEquip(player);       break;
        case 'E': return;
        }
    }
}

void inventory(Player &player) {
    const int ITEM_LIMIT = 10;
    int total_items  = player.getInventory().size();
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
            auto &inv = player.getInventory();
            if(option == '1')
                std::sort(inv.begin(), inv.end(),
                          [](const Item &a, const Item &b) { return a.name < b.name; });
            else std::sort(inv.begin(), inv.end(),
                           [](const Item &a, const Item &b) { return a.increase_DMG < b.increase_DMG; });
            total_items  = inv.size();
            total_pages  = (total_items + ITEM_LIMIT - 1) / ITEM_LIMIT;
            current_page = 1;  // we go back to the first index :)
            break;
        }
        case 'E':
            return;
        }
    }
}
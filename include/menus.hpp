#pragma once

#include "hutils.h"
#include "item_database.h"
#include "save_manager.h"
#include "player.h"
#include "monster.h"
#include "monsters.hpp"

#include <iostream>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <random>

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

void statistics(Player &player, bool exit_only = false) {
    while(true) {
        char c ='\0';
        hUtils::text.clearAll(500);
        std::cout << "Player Info:\n"
                << "  Name:       " << player.getName() << '\n'
                << "  Points:     " << player.getAllocationPts() << '\n'
                << "  Block Uses: " << player.getCurrentBlockUses() << "/" << player.getMaxBlockUses() << '\n';
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

        if(exit_only) {
            std::cout << "[E] Exit\n";
            c = hUtils::GetInputKeymap({'E'});
            if(std::toupper(c) == 'E') return;
        } else {
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

enum class BattleState {
    PlayerTurn,
    MonsterTurn,
    Victory,
    Defeat,
    Retreat,
};

BattleState battle(Player &player, Monster &monster)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dis(0.0, 1.0);

    unsigned char see_flags = 0;

    int p_intel = player.getAttributes().intelligence;
    int m_intel = monster.getAttributes().intelligence;

    // battle intro, show monster info

    // if the player has high enough intelligence, they can see the monster's level.
    // else if the player's intelligence is more than 20% lower than the monster's intelligence, they can see the monster's level.
    if(p_intel >= m_intel || p_intel < m_intel * 0.8) see_flags = see_flags | (1u << 0);

    // seeing the monster's level can give the player an idea of how strong the monster is, since levels are directly proportional to the monster's stats. (e.g. player's can learn the approximate calculation to plan their battle strategy.)

    // if the player has higher or equal intelligence than the monster, they can see the monster's exact health points, mana points.
    if(p_intel >= m_intel) see_flags = see_flags | (1u << 1);

    // if the player has a special equipment, they have a guaranteed chance to see the monster's attributes and stats. (it sacrifices an armor slot, but it can be a game changer.)
    // else if the player has higher intelligence than the monster, they can have a chance to see the monster's attributes and stats.
    // the chance can be influenced by the player's intelligence (if the player has 50% higher intelligence, it increases by 50% between base and cap), but it won't be a guaranteed win for the player. 
    bool can_see_full_stats = false;
    bool has_reveal_gear = false;
    Item* equip = player.getEquipment(Slot::Helmet);
    if(equip && equip->id == "monocle_of_true_sight") has_reveal_gear = true;

    if(has_reveal_gear) {
        see_flags = see_flags | (1u << 2) | (1u << 3);
    } else if(p_intel > m_intel) {
        double base_reveal_chance = 0.3;
        double bonus_chance = ((double)p_intel / m_intel) - 1.0;
        if(bonus_chance > 0.5) bonus_chance = 0.5;
        double total_reveal_chance = base_reveal_chance + bonus_chance;
        if(dis(gen) < total_reveal_chance) see_flags = see_flags | (1u << 2) | (1u << 3);
    }

    hUtils::text.clearAll();

    // a 20% chance to either player attack first or the monster attack first. (chance can be influenced by player's dexterity, but it won't be a guaranteed win for the player.)
    // a 80% chance to not happen.
    double preemptive_chance = 0.2;
    if(dis(gen) < preemptive_chance) {
        double player_first_prob = 0.5 + player.getDodgeChance(true);
        if(player_first_prob > 0.85) player_first_prob = 0.85;
        
        bool player_strikes_first = dis(gen) < player_first_prob;
        // if its successful, player attacks first, calculate damage, apply to monster, check if monster is alive, if not, give rewards.
        // monster attacks first, calculate damage, apply to player, check if player is alive, if not, game over.
        if(player_strikes_first) {
            std::cout << "Preemptive Strike! You strike first!\n";
            player.attack(monster);
        } else {
            std::cout << "Ambush! The enemy lunges forward!\n";
        }
    }

    // battle loop, player and monster take turns attacking each other until one of them is dead.
    // player's actions also have random chances of events or outcomes, such as critical hits, dodges, and misses. (these can be influenced by player's stats and equipment.)
    std::cout << "You have encountered a " << monster.getID() << "!\n\n";
    while(true) {
        if(!player.isAlive()) {
            bool revived = false;
            // check if player has a revive item or something, if so, give them a chance to use it and come back to life.
            if(player.hasItem("revive_potion")) {
                std::cout << "You have a revive potion! Do you want to use it? [Y/N]\n";
                char choice = hUtils::GetInputKeymap({'Y','N'});
                if(std::toupper(choice) == 'Y') {
                    player.setCurrentHealth(player.getTotalHealth(false) * 0.5); // revive with 50% health
                    player.removeFromInventory("revive_potion");
                    std::cout << "You used the revive potion and came back to life with " << player.getCurrentHealth() << " HP!\n";
                    hUtils::Sleep(2500);
                    revived = true;
                }
            }

            if(!revived) {
                if(player.getCurrentHealth() < 0) player.setCurrentHealth(0);
                std::cout << "You have been defeated by the " << monster.getName() << "...\n";
                return BattleState::Defeat;
            }
        }

        if(!monster.isAlive()) {
            static std::random_device rd;
            static std::mt19937 gen(rd());
            std::uniform_int_distribution<> dist(1, 3);
            std::vector<std::string> rewards = monster.getLootTable().dropItem(dist(gen));
            if(player.addToInventory(rewards)) {
                for(const std::string& reward : rewards) std::cout << "You obtained: " << reward << '\n';
            } else {
                std::cout << "Your inventory is full! You couldn't pick up the rewards...\n";
            }
            hUtils::Sleep(2500);
            return BattleState::Victory;
        }
        // seeing the monster's attributes and stats can give the player a huge advantage, since they can plan their battle strategy accordingly. (e.g. if the monster has high physical resist, the player can choose to use magic damage instead of physical damage.)
        std::cout << "Monster: " << monster.getName();
        if(can_see_level) std::cout << "(Lvl " << monster.getLvl() << ")";
        else std::cout << "(Lvl ??\?)"; 
        std::cout << '\n';

        hUtils::bar.setBar("HP", monster.getCurrentHealth(), monster.getTotalHealth(false), 124, {}, !can_see_exact_stats);
        hUtils::bar.setBar("MP", monster.getCurrentMana(),   monster.getTotalMana(false), {}, {}, !can_see_exact_stats);

        // this is where player can choose to attack, use item, or flee.
        hUtils::table.setElements(
            " [Q] Attack",   " [W] Block",
            " [A] Use Item", " [S] Flee",
            " [D] See stats"
        );
        hUtils::table.toColumn("left", 15, 2);

        static std::random_device turn_rd;
        static std::mt19937 turn_gen(turn_rd());
        std::uniform_real_distribution<double> turn_dis(0.0, 1.0);

        bool turn_completed = false;
        while(!turn_completed) {
            char c = hUtils::GetInputKeymap({'Q','W','A','S','D','E'});

            // if player flees, there's a chance of failure, which can lead to a re-encounter.
            // if player uses an item.. player looses a chance to attack.
            // if player attacks.. calculate damage, apply to monster, check if monster is alive, if not, give rewards.
            // if player blocks.. calculate damage reduction, apply to player, check if player is alive, if not, game over.
            switch(std::toupper(c)) {
            case 'Q':    // attack
                player.endBlocking();
                player.regainBlockUse();
                player.attack(monster);
                turn_completed = true;
                break;
            case 'W':    // block
                if(player.startBlocking()) {
                    std::cout << "You raise your guard. (Block uses left: " << player.getCurrentBlockUses() << ")\n";
                    turn_completed = true;
                } else {
                    std::cout << "You have no block uses left!\n";
                }
                break;
            case 'A':    // use item
                break;
            case 'S': {  // flee
                player.endBlocking();
                double flee_chance = 0.25 + (player.getDodgeChance(true) * 0.3);
                if(flee_chance > 0.85) flee_chance = 0.85;
                if(turn_dis(gen) < flee_chance) {
                    std::cout << "You successfully fled from the " << monster.getName() << ".\n";
                    return BattleState::Retreat;
                }
                std::cout << "You failed to flee from the " << monster.getName() << ".\n";
                player.regainBlockUse();
                turn_completed = true;
                break;
            }
            case 'D': // check player stats
                statistics(player, true);
                break;
            }
        }

        if(monster.isAlive()) {
            if(turn_dis(turn_gen) < monster.getBlockChance()) {
                if(monster.startBlocking()) {
                    std::cout << "The " << monster.getName() << " raises its guard.\n";
                } else {
                    std::cout << "The " << monster.getName() << " attempted to block but failed!\n";
                    monster.attack(player);
                }
            } else {
                monster.endBlocking();
                monster.regainBlockUse();
                monster.attack(player);
            }
        }
    }
}

void encounter(Player &player) {
    // all the randomizer stuff happens.
    // battle is a bool function. which means its possible to do a re-encounter if player fails to flee. (which can be brutal)
    auto monster = MonsterDatabase::instance().find("target_dummy");
    if(!monster) return;

    // with BattleSate, we can determine the outcome and create the player's statistics.
    BattleState outcome = battle(player, *monster);
    switch(outcome) {
    case BattleState::Victory:
        std::cout << "You defeated the " << monster->getName() << "!\n";
        break;
    case BattleState::Defeat:
        std::cout << "You were defeated by the " << monster->getName() << "...\n";
        break;
    case BattleState::Retreat:
        std::cout << "You successfully fled from the " << monster->getName() << ".\n";
        break;
    default:
        break;
    }
}
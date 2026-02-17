#include "hutils.h"
#include "item_database.hpp"
#include "menus.hpp"
#include "save_manager.h"

#include <bitset>
#include <iostream>
#include <windows.h>

using std::cout;

std::unordered_map<std::string, Item> ItemDatabase::itemDatabase;

int main() {
    ItemDatabase::instance().load();
    std::string FILE_NAME;

    //  create/select progress.
    Player player;
    char c;
    while(true) {
        hUtils::text.clearAll();
        c = '\0';
        cout << "[Q]   Create new save\n" <<
                "[W]   Select existing save\n" <<
                "[Esc] Exit program\n";
        c = GetInputKeymap({'Q','W','\x1B'});
        switch(std::toupper(c)) {
        case 'Q':
            player = newCharacterSave();
            saveToFile(player);
            break;
        case 'W':
            player = loadToFile();
            break;
        case '\x1B':
            return 0;
        default:
            continue;
        }
        break;
    }

    cout << player.getName() << "'s save file selected.\n";
    cout << player.getEquipmentName(Slot::MainHand) << '\n';
    hUtils::sleep(2500);

    //  action menu.
    while(true) {
        hUtils::text.clearAll();
        c = '\0';
        std::cout << "Action Menu:\n";
        hUtils::table.setElements(
            " [A] Character Stats", " [S] Inventory",
            " [Q] Gather Items",    " [W] Adventure",
            " [D] Save Character",  " [Esc] Exit Game"
        );
        hUtils::table.toColumn("left", 22, 2);
        c = GetInputKeymap({'Q','W','A','S','D','\x1B'});
        switch(std::toupper(c)) {
        case 'A':    statistics(player);
                     break;
        case 'S':    inventory(player);
                     break;
        case 'W':    break;
        case 'Q':    break;
        case 'D':    saveToFile(player);
                     break;
        case '\x1B': return 0;
        default:     continue;
        }
    }
    
    std::cout << "\nExited the menu unexpectedly.";
    hUtils::sleep(10000);

    return -1;
}
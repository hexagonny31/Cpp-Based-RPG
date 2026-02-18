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
        try {
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
        } catch(const std::exception &e) {
            hUtils::text.reject(e.what());
            continue;
        }
        break;
    }

    cout << player.getName() << "'s save file selected.\n";
    cout << player.getEquipmentName(Slot::MainHand) << '\n';
    hUtils::sleep(2500);

    /*  this is where you probably will spend time the most.

        to anyone seeing this, i'm still thinking of just joining
        all the void functions into the cases but for now, i'll
        keep them separated so i can just focus on functionality
        and not all that readability bullshit.  */
    while(true) {
        hUtils::text.clearAll();
        c = '\0';
        std::cout << "Action Menu:\n";
        hUtils::table.setElements(
            " [A] Character Stats", " [S] Inventory",
            " [Q] Gather Items",    " [W] Adventure",
            " [D] Save Character",  " [E] Load Character",
            " [Esc] Exit program"
        );
        hUtils::table.toColumn("left", 21, 2);
        c = GetInputKeymap({'Q','W','A','S','D','E','\x1B'});
        try {
            switch(std::toupper(c)) {
            case 'A':
                statistics(player);
                break;
            case 'S':
                inventory(player);
                break;
            case 'W':
                break;
            case 'Q':
                break;
            case 'D':
                saveToFile(player);
                break;
            case 'E':
                saveToFile(player);
                player = loadToFile();
                break;
            case '\x1B':
                return 0;
            }
        } catch(const std::exception &e) {
            hUtils::text.reject(e.what());
        }
    }
    
    /*  this should be logically impossible to reach but added this
        anyway to let me know that it did the impossible or i did
        something astronomically stupid.  */
    std::cout << "\nExited the menu unexpectedly!";
    hUtils::sleep(10000);

    return -1;
}
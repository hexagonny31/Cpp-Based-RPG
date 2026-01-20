#include "hutils.h"
#include "item_database.hpp"
#include "menus.hpp"
#include "save_manager.h"

#include <iostream>

using std::cout;

std::unordered_map<std::string, Item> ItemDatabase::itemDatabase;

int main() {
    ItemDatabase::instance().load();
    std::string FILE_NAME;

    //  create/select progress.
    char choice;
    Player player;
    cout << "[Q] Create new save\n" <<
            "[W] Select existing save\n" <<
            "[E] Exit program\n";
    while(true) {
        choice = charIn("\n");
        try {
            switch(choice) {
            case 'q': //  new save.
                player = newCharacterSave();
                saveToFile(player);
                break;
            case 'w': // select save.
                player = loadToFile();
                break;
            case 'e':
                return 0;
            default:
                hUtils::text.reject("Invalid option!", 2);
                continue;
            }
        } catch(const std::exception &e) {
            hUtils::text.reject(e.what(), 2);
            continue;
        }
        break;
    }

    cout << player.getName() << "'s save file selected.\n";
    cout << player.getEquipmentName(Slot::MainHand) << '\n';
    hUtils::sleep(2500);

    //  action menu.
    
    while(true) {
        choice = '\0';
        hUtils::text.clearAll();
        std::cout << "Action Menu:\n";
        hUtils::table.setElements(
            " [A] Character Stats", " [S] Inventory",
            " [Q] Gather Items",    " [W] Adventure",
            " [E] Exit Game"
        );
        hUtils::table.toColumn("left", 22, 2);
        choice = charIn("\n");
        try {
            switch(choice) {
            case 'a': 
                statistics(player); break;
            case 's': 
                inventory(player); break;
            case 'w': // adventuring
                break;
            case 'q': // gathering
                break;
            case 'e':
                return 0;
            default:
                hUtils::text.reject("Invalid option!", 5);
                continue;
            }
        } catch(const std::exception &e) {
            hUtils::text.reject(e.what(), 5);
            continue;
        }
    }
    
    hUtils::sleep(10000);

    return 0;
}
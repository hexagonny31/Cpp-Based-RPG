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
    char input;
    Player player;
    while(true) {
        hUtils::text.clearAll();
        cout << "[Q] Create new save\n" <<
                "[W] Select existing save\n" <<
                "[E] Exit program\n";
        input = charIn();
        try {
            switch(input) {
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
                hUtils::text.reject("Invalid option!", 4);
                continue;
            }
        } catch(const LoadFailed &e) {
            hUtils::text.reject(e.what(), 4);
            continue;
        } catch(const UserCancelled &e) {
            hUtils::text.reject(e.what(), 4);
            continue;
        } catch(const std::exception &e) {
            hUtils::text.reject(e.what(), 4);
            continue;
        }
        break;
    }

    cout << player.getName() << "'s save file selected.\n";
    cout << player.getEquipmentName(Slot::MainHand) << '\n';
    hUtils::sleep(2500);

    //  action menu.
    
    while(true) {
        input = '\0';
        hUtils::text.clearAll();
        std::cout << "Action Menu:\n";
        hUtils::table.setElements(
            " [A] Character Stats", " [S] Inventory",
            " [Q] Gather Items",    " [W] Adventure",
            " [E] Exit Game"
        );
        hUtils::table.toColumn("left", 22, 2);
        input = charIn("\n");
        try {
            switch(input) {
            case 'a': 
                statistics(player);
                break;
            case 's': 
                inventory(player);
                break;
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
    int inputing = intIn("", 1, 5);
    
    hUtils::sleep(10000);
    //  parse save files.

    return 0;
}
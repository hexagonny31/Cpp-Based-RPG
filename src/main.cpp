#include "hutils.h"
#include "item_database.hpp"
#include "save_manager.h"

#include <iostream>

using std::cout;
using std::cin;

std::unordered_map<std::string, Item> ItemDatabase::itemDatabase;

int main() {
    ItemDatabase::instance().load();
    std::string FILE_NAME;

    //  create/select save.
    char input;
    Player player;
    while(true) {
        hUtils::text.clearAll();
        cout << "Create new save [Q]\n" <<
                "Select existing save [W]\n" <<
                "Exit program [E]\n";
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
        } catch(const LoadFailed& e) {
            hUtils::text.reject(e.what(), 4);
            continue;
        } catch(const UserCancelled& e) {
            hUtils::text.reject(e.what(), 4);
            continue;
        } catch(const std::exception& e) {
            hUtils::text.reject(e.what(), 4);
            continue;
        }
        break;
    }

    cout << player.getName() << "'s save file selected.\n";
    cout << player.getEquipmentName(Slot::MainHand) << '\n';

    int inputing = intIn("", 1, 5);
    
    hUtils::sleep(10000);
    //  parse save files.

    return 0;
}
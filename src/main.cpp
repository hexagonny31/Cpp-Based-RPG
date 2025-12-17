#include "hutils.h"

#include <iostream>
#include <filesystem>
#include <unordered_map>

using std::cout;
using std::cin;
namespace fs = std::filesystem;

int main() {
    std::string FILE_NAME;

    //  create/select save.
    char input;
    while(true) {
        cout << "Create new save [Q]\n" <<
                "Select existing save [W]\n" <<
                "Exit program [E]\n";
        input = charIn();

        if(input == 'e') return 0;    
        switch(input) {
            case 'q': //  new save.
                hUtils::text.clearAll();
                cout << "Creating new save...\n";
                while(true) {
                    std::string initName = "";
                    cout << "Enter your character name: ";
                    std::getline(cin, initName);
                    if(initName.length() > 64) {
                        hUtils::text.reject("Name can't be over 64 characters!");
                        continue;
                    } else if(initName.empty()) {
                        hUtils::text.reject("Argument is empty!");
                        continue;
                    }
                    cout << '\n';

                    FILE_NAME = initName + ".save";
                    if(!fs::exists(FILE_NAME)) break;

                    char input;
                    hUtils::text.clearAll();
                    cout << "'" << FILE_NAME << "' already exists!\n" <<
                            "Overwrite save [Q]\n" <<
                            "Enter new save name [W]\n";
                    input = charIn();

                    if(input == 'q') break;
                    if (input == 'w') continue;

                    hUtils::text.reject("Invalid option!");
                }
                //  creating action.
                
                break;
            case 'w': // select save.
                break;
            default: hUtils::text.reject("Invalid option!", 4);
        }
    }

    //  parse save files.

    while(true) {
        char input;
        cout << "Menu";
        if(input == 'e') break;
    }
    return 0;
}
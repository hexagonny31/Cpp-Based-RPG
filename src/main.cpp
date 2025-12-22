#include "hutils.h"
#include "item_database.hpp"

#include <iostream>

using std::cout;
using std::cin;

int main() {
    ItemDatabase::instance().load();
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
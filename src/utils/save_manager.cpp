#include "hutils.h"
#include "class_presets.h"
#include "item_database.hpp"
#include "entity.h"

#include <unordered_map>
#include <filesystem>

namespace fs = std::filesystem;

std::optional<Player> newSave() {
    using namespace std::string_literals;

    //  deciding for the character name.
    hUtils::text.clearAll();
    std::cout << "Creating new save...\n";
    std::string init_name;
    while(true) {
        init_name = "";
        std::cout << "Enter your character name (2 - 64 characters)\n> ";
        std::getline(std::cin, init_name);
        hUtils::text.trim(init_name);
        if(init_name.length() > 64) {
            hUtils::text.reject("Name can't be over 64 characters!", 2);
            continue;
        } else if(init_name.length() < 2) {
            hUtils::text.reject("Name can't be under 2 characters!", 2);
            continue;
        }
        std::cout << '\n';
        break;
    }
    //  creating action.
    const std::string PRESET_JSON_NAME = "class_preset.json";
    std::optional<std::vector<ClassPresets>> init = parsePresets(PRESET_JSON_NAME);
    if(!init) {
        hUtils::text.reject("Failed to load "s + PRESET_JSON_NAME);
        return std::nullopt;
    }
    const std::vector<ClassPresets> class_presets = *init;
    
    std::unordered_map<std::string, int> lookup;
    std::vector<std::string> class_names;
    for(int i = 0; i < class_presets.size(); i++) {
        std::string temp = class_presets[i].class_name;
        if(temp.empty()) {
            hUtils::text.reject("Preset name is missing at index "s + std::to_string(i));
            return std::nullopt;
        }
        lookup[hUtils::text.toLowerCase(temp)] = i;
        class_names.push_back(temp);
    }
    //  the actual user interaction goes here.
    Player new_player;
    ClassPresets class_preset;
    while(true) {
        std::string input = "";
        std::cout << "Choose a preset for your character: ";
        hUtils::table.setElements(class_names);
        hUtils::table.toColumn("left", 16, 3);
        std::cout << "> ";
        std::getline(std::cin, input);
        hUtils::text.trim(input);
        input = hUtils::text.toLowerCase(input);

        if(input == "exit" || input == "e") return std::nullopt;

        std::unordered_map<std::string, int>::iterator cartesian = lookup.find(input);
        if(cartesian == lookup.end()) {
            hUtils::text.reject("Unable to find class preset.", 4);
            continue;
        }
        int index = cartesian->second;
        class_preset = class_presets[index];
        break;
    }
    //  setting the preset to the new player object.
    new_player.name                    = init_name;
    new_player.allocation_pts          = class_preset.starting_pts;
    new_player.attributes.vigor        = class_preset.attributes.vigor;
    new_player.attributes.strength     = class_preset.attributes.strength;
    new_player.attributes.endurance    = class_preset.attributes.endurance;
    new_player.attributes.intelligence = class_preset.attributes.intelligence;
    new_player.attributes.dexterity    = class_preset.attributes.dexterity;

    new_player.stats.updateHealth();
    //  equipment here very soon.

    return new_player;
}

//  saving progress into a file. once i'm done with the new save utility.
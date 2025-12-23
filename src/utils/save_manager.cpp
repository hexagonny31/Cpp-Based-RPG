#include "hutils.h"
#include "class_presets.h"
#include "save_manager.h"
#include "item_database.hpp"
#include "entity.h"

#include <iostream>
#include <unordered_map>
#include <filesystem>
#include <stdexcept>

using namespace std::string_literals;
namespace fs = std::filesystem;

Player newCharacterSave() {
    //  deciding for the character name.
    hUtils::text.clearAll();
    std::cout << "Creating new save...\n";
    std::string init_name;
    while(true) {
        init_name = "";
        std::cout << "Enter your character name (2 - 64 characters)\n> ";
        std::getline(std::cin, init_name);
        if(init_name.length() > 64) {
            hUtils::text.reject("Name can't be over 64 characters!", 2);
            continue;
        } else if(init_name.length() < 2) {
            hUtils::text.reject("Name can't be under 2 characters!", 2);
            continue;
        } else {
            std::cout << '\n';
            break;
        }
    }
    //  creating action.
    const std::string PRESET_JSON_NAME = "class_preset.json";
    std::optional<std::vector<ClassPreset>> init = parsePresets();
    if(!init) {
        throw LoadFailed("Failed to load "s + PRESET_JSON_NAME);
    }
    const std::vector<ClassPreset> class_presets = *init;
    
    std::unordered_map<std::string, int> lookup;
    std::vector<std::string> class_names;
    for(int i = 0; i < class_presets.size(); i++) {
        std::string temp = class_presets[i].class_name;
        if(temp.empty()) {
            hUtils::text.reject("Preset name is missing at index "s + std::to_string(i));
            continue;
        }
        lookup[hUtils::text.toLowerCase(temp)] = i;
        class_names.push_back(temp);
    }
    //  the actual user interaction goes here.
    Player new_player;
    ClassPreset class_preset;
    while(true) {
        std::string input = "";
        std::cout << "Choose a preset for your character: \n";
        hUtils::table.setElements(class_names);
        hUtils::table.toColumn("left", 16, 3);
        std::cout << "> ";
        std::getline(std::cin, input);
        hUtils::text.trim(input);
        input = hUtils::text.toLowerCase(input);

        if(input == "exit" || input == "e") throw UserCancelled("Character creation cancelled by user.");

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
    new_player.setName(init_name);
    new_player.setAllocation(class_preset.starting_pts);

    new_player.attribute.vigor        = class_preset.attribute.vigor;
    new_player.attribute.strength     = class_preset.attribute.strength;
    new_player.attribute.endurance    = class_preset.attribute.endurance;
    new_player.attribute.intelligence = class_preset.attribute.intelligence;
    new_player.attribute.dexterity    = class_preset.attribute.dexterity;

    std::cout << "Checkpoint\n";
    hUtils::sleep(2000);

    new_player.updateHealth();

    if (!class_preset.main_hand.empty()) {
        new_player.addToInventory(class_preset.main_hand);
    } else {
        std::cout << "Warning: main_hand is empty, skipping.\n";
    }

    if (!class_preset.off_hand.empty()) {
        new_player.addToInventory(class_preset.off_hand);
    } else {
        std::cout << "Warning: off_hand is empty, skipping.\n";
    }

    return new_player;
}

//  saving progress into a file. once i'm done with the new save utility.
void saveToFile(const Player &player) {
    std::string name = player.getName();
    hUtils::text.trim(name);
    const std::string& FILE_NAME = "saves/" + name + ".save";
    json j;

    j["name"] = player.getName();
    j["allocation_pts"] = player.getAllocationPts();
    j["current_health"] = player.getCurrentHealth();
    j["current_mana"]   = player.getCurrentMana();

    j["attribute"]["vigor"]        = player.attribute.vigor;
    j["attribute"]["strength"]     = player.attribute.strength;
    j["attribute"]["endurance"]    = player.attribute.endurance;
    j["attribute"]["intelligence"] = player.attribute.intelligence;
    j["attribute"]["dexterity"]    = player.attribute.dexterity;

    j["equipment"] = json::array();
    for(const auto& item : player.equipment) {
        if(item != nullptr) {
            j["equipment"].push_back(item->id);
        } else {
            j["equipment"].push_back(nullptr);
        }
    }
    j["inventory"] = json::array();
    for(const auto& item : player.inventory) {
        j["inventory"].push_back(item.id);
    }

    std::ofstream output(FILE_NAME);
    if(output.is_open()) output << j.dump(4);
    std::cout << "Game saved successfully!\n";
}

//  loading and parsing save files are next here.
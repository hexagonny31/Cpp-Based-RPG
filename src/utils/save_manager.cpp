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
        if(init_name == "exit" || init_name == "e") throw UserCancelled("Load cancelled by user.");

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
    const std::string PRESET_JSON_NAME = "json/class_preset.json";
    std::optional<std::vector<ClassPreset>> init = parsePresets(PRESET_JSON_NAME);
    if(!init) {
        throw LoadFailed("Failed to load '"s + PRESET_JSON_NAME + "'"s);
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
        class_preset = class_presets[cartesian->second];
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

    new_player.updateHealth();

    if (!class_preset.main_hand.empty()) new_player.addToInventory(class_preset.main_hand);
    if (!class_preset.off_hand.empty()) new_player.addToInventory(class_preset.off_hand);

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
    for(const auto &item : player.equipment) {
        if(item != nullptr) {
            j["equipment"].push_back(item->id);
        } else {
            j["equipment"].push_back(nullptr);
        }
    }
    j["inventory"] = json::array();
    for(const auto &item : player.inventory) j["inventory"].push_back(item.id);

    std::ofstream output(FILE_NAME);
    if(output.is_open()) output << j.dump(4);
    std::cout << "Game saved successfully!\n";
}

//  loading and parsing save files are next here.
Player loadToFile() {
    //  deciding what save to load.
    hUtils::text.clearAll();
    std::cout << "Loading save file...\n";
    std::string load_name;
    try {
        std::cout << "Pick a file to load:\n";
        for(const auto &entry : fs::directory_iterator("saves/")) {
            if(entry.is_regular_file() && entry.path().extension() == ".save") {
                std::cout << "- " << entry.path().stem().string() << '\n';
            }
        }
    } catch(const fs::filesystem_error& e) {
        throw LoadFailed("Failed to access 'saves/' directory: "s + e.what());
    }
    while(true) {
        load_name = "";
        std::cout << "> ";  // must be case sensitive when inputing names. (less hassle)
        std::getline(std::cin, load_name);
        if(load_name == "exit" || load_name == "e") throw UserCancelled("Load cancelled by user.");

        if(load_name.length() > 64) {
            hUtils::text.reject("Name can't be over 64 characters!", 2);
            continue;
        } else if(load_name.length() < 2) {
            hUtils::text.reject("Name can't be under 2 characters!", 2);
            continue;
        } else {
            std::cout << '\n';
            break;
        }
    }
    const std::string& FILE_NAME = "saves/" + load_name + ".save";
    std::ifstream file(FILE_NAME);
    if(!file.is_open()) throw LoadFailed("Failed to open save file '"s + FILE_NAME + "'"s);

    json j;
    try {
        file >> j;
    } catch(const json::parse_error&) {
        throw LoadFailed("Failed to parse save file '"s + FILE_NAME + "'"s);
    }
    Player loaded_player;
    if(!j.contains("name") || !j["name"].is_string()) throw LoadFailed("Save file '"s + FILE_NAME + "' is missing 'name' field."s);

    loaded_player.setName(j["name"]);
    loaded_player.setAllocation(j.value("allocation_pts", 0));
    loaded_player.setCurrentHealth(j.value("current_health", 100.0));
    loaded_player.setCurrentMana(j.value("current_mana", 100.0));

    if(j.contains("attribute") && j["attribute"].is_object()) {
        auto attr = j["attribute"];
        loaded_player.attribute.vigor        = attr.value("vigor", 0);
        loaded_player.attribute.strength     = attr.value("strength", 0);
        loaded_player.attribute.endurance    = attr.value("endurance", 0);
        loaded_player.attribute.intelligence = attr.value("intelligence", 0);
        loaded_player.attribute.dexterity    = attr.value("dexterity", 0);
    }
    std::unordered_map<std::string, int> lookup;
    if(j.contains("inventory") && j["inventory"].is_array()) {
        int i = 0;
        for(const auto &item_name : j["inventory"]) {
            if(!item_name.is_string()) continue;
            loaded_player.addToInventory(item_name);
            lookup[item_name.get<std::string>()] = i;
            i++;
        }
    }
    if(j.contains("equipment") && j["equipment"].is_array()) {
        size_t i = 0;
        for(const auto &item_name_json : j["equipment"]) {
            if(!item_name_json.is_string()) continue;
            if(i >= static_cast<size_t>(Slot::COUNT)) break;

            const std::string item_name = item_name_json.get<std::string>();
            std::unordered_map<std::string, int>::iterator cartesian = lookup.find(item_name);
            if(cartesian == lookup.end()) continue;
            Item *item = &loaded_player.inventory[cartesian->second];

            loaded_player.equipItem(item, static_cast<Slot>(i));
            i++;
        }
    }

    return loaded_player;
}
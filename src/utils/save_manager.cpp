#include "hutils.h"
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
    hUtils::text.clearAll();
    FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
    std::cout << "Creating new save...\n\n";
    //  deciding for the character name.
    std::string init_name = strIn("Enter your character name (2 - 64 characters)\n", 2, 64);
    if(init_name == "e") throw std::runtime_error("Load cancelled by user.");
    //  creating action.
    const std::string PRESET_JSON_NAME = "json/class_presets.json";
    std::optional<std::vector<ClassPreset>> init = parsePresets(PRESET_JSON_NAME);

    Player new_player;
    ClassPreset class_preset;

    /*  if the program (somehow) fails to parse its contents, it instead use its default values.
        to anyone reading this, please do not- in any shape or form, modify or remove this statement.*/
    if(!init) {
        new_player.setName(init_name);
        new_player.setAllocation(class_preset.starting_pts);
        return new_player;
    }

    const std::vector<ClassPreset> class_presets = *init;
    //  constructing a lookup table.
    std::unordered_map<std::string, int> lookup;
    std::vector<std::string> class_names;
    for(int i = 0; i < class_presets.size(); i++) {
        std::string temp = class_presets[i].class_name;
        if(temp.empty()) continue;
        lookup[hUtils::text.toLowerCase(temp)] = i;
        class_names.push_back(temp);
    }
    //  the actual user interaction goes here.
    while(true) {
        std::string input = "";
        std::cout << "Choose a preset for your character: \n";
        hUtils::table.setElements(class_names);
        hUtils::table.toColumn("left", {}, 3);
        std::cout << "\n> ";
        std::getline(std::cin, input);
        hUtils::text.trim(input);
        input = hUtils::text.toLowerCase(input);

        if(input == "exit" || input == "e") throw std::runtime_error("Character creation cancelled by user.");

        std::unordered_map<std::string, int>::iterator cartesian = lookup.find(input);
        if(cartesian == lookup.end()) continue;
        class_preset = class_presets[cartesian->second];
        break;
    }
    std::cout << '\n';
    //  setting the preset to the new player object.
    new_player.setName(init_name);

    // get total attribute points from the preset and assign it to the player.
    Attributes attr = class_preset.attribute;
    int total_pts = attr.vigor + attr.strength + attr.endurance + attr.intelligence + attr.dexterity;
    new_player.attribute = attr;
    new_player.setAllocation(class_preset.starting_pts - total_pts);

    new_player.updateHealth();

    if(!class_preset.main_hand.empty()) new_player.addToInventory(class_preset.main_hand);
    if(!class_preset.off_hand.empty()) new_player.addToInventory(class_preset.off_hand);

    return new_player;
}

//  saving progress into a file. once i'm done with the new save utility.
void saveToFile(const Player &player) {
    FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
    std::string name = player.getName();
    hUtils::text.trim(name);
    std::string FILE_NAME = "saves/" + name + ".save";

    while(fs::exists(FILE_NAME)) {
        char c = '\0';
        std::cout << "'" << FILE_NAME << "' already exists!\n" <<
                     "[Q] Overwrite save\n" <<
                     "[W] Enter new save name\n" <<
                     "[E] Cancel\n";
        c = GetInputKeymap({'Q','W','E'});

        if(c == 'q') {
            if(!proceed()) continue;
            break;
        } else if(c == 'e') {
            throw std::runtime_error("Load cancelled by user.");
        } else if(c == 'w') {
            std::string new_name = strIn("Enter a new name\n");
            hUtils::text.trim(new_name);
            FILE_NAME = "saves/" + new_name + ".save";
        }
    }

    nj json;

    json["name"]           = player.getName();
    json["allocation_pts"] = player.getAllocationPts();
    json["current_health"] = player.getCurrentHealth();
    json["current_mana"]   = player.getCurrentMana();

    json["attribute"]["vigor"]        = player.attribute.vigor;
    json["attribute"]["strength"]     = player.attribute.strength;
    json["attribute"]["endurance"]    = player.attribute.endurance;
    json["attribute"]["intelligence"] = player.attribute.intelligence;
    json["attribute"]["dexterity"]    = player.attribute.dexterity;

    json["equipment"] = nj::array();
    for(const auto &item : player.equipment) {
        if(item != nullptr) json["equipment"].push_back(item->id);
        else json["equipment"].push_back(nullptr);
    }
    json["inventory"] = nj::array();
    for(const auto &item : player.inventory) json["inventory"].push_back(item.id);

    std::ofstream output(FILE_NAME);
    if(!output.is_open()) throw std::runtime_error("Failed to open output save file '"s + FILE_NAME + "'"s);
    output << json.dump(4);
    std::cout << "Game saved successfully!\n\n";
}

//  loading and parsing save files are next here.
Player loadToFile() {
    //  deciding what save to load.
    hUtils::text.clearAll();
    FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
    std::cout << "Loading save file...\n\n";
    try {
        std::cout << "Pick a file to load:\n";
        for(const auto &entry : fs::directory_iterator("saves/")) {
            if(entry.is_regular_file() && entry.path().extension() == ".save") {
                std::cout << "- " << entry.path().stem().string() << '\n';
            }
        }
    } catch(const fs::filesystem_error& e) {
        throw std::runtime_error("Failed to access 'saves/' directory: "s + e.what());
    }
    std::string load_name = strIn("\n");
    if(load_name == "exit" || load_name == "e") throw std::runtime_error("Load cancelled by user.");

    const std::string& FILE_NAME = "saves/" + load_name + ".save";
    std::ifstream file(FILE_NAME);
    if(!file.is_open()) throw std::runtime_error("Failed to open save file '"s + FILE_NAME + "'"s);

    nj json;
    try {
        file >> json;
    } catch(const nj::parse_error&) {
        throw std::runtime_error("Failed to parse save file '"s + FILE_NAME + "'"s);
    }
    Player loaded_player;
    if(!json.contains("name") || !json["name"].is_string())
        throw std::runtime_error("Save file '"s + FILE_NAME + "' is missing 'name' field."s);

    loaded_player.setName(json["name"]);
    loaded_player.setAllocation(json.value("allocation_pts", 0));
    loaded_player.setCurrentHealth(json.value("current_health", 100.0));
    loaded_player.setCurrentMana(json.value("current_mana", 100.0));

    if(json.contains("attribute") && json["attribute"].is_object()) {
        auto attr = json["attribute"];
        loaded_player.attribute.vigor        = attr.value("vigor", 0);
        loaded_player.attribute.strength     = attr.value("strength", 0);
        loaded_player.attribute.endurance    = attr.value("endurance", 0);
        loaded_player.attribute.intelligence = attr.value("intelligence", 0);
        loaded_player.attribute.dexterity    = attr.value("dexterity", 0);
    }
    std::unordered_map<std::string, int> lookup;
    if(json.contains("inventory") && json["inventory"].is_array()) {
        int i = 0;
        for(const auto &item_name : json["inventory"]) {
            if(!item_name.is_string()) continue;
            loaded_player.addToInventory(item_name);
            lookup[item_name.get<std::string>()] = i;
            ++i;
        }
    }
    if(json.contains("equipment") && json["equipment"].is_array()) {
        size_t i = 0;
        for(const auto &item_name_json : json["equipment"]) {
            if(i >= static_cast<size_t>(Slot::COUNT)) break;
            if(item_name_json.is_string()) {
                const std::string item_name = item_name_json.get<std::string>();
                std::unordered_map<std::string, int>::iterator cartesian = lookup.find(item_name);
                if(cartesian == lookup.end()) continue;
                Item *item = &loaded_player.inventory[cartesian->second];

                loaded_player.equipItem(item, static_cast<Slot>(i));
            }
            ++i;
        }
    }
    return loaded_player;
}
#include "hutils.h"
#include "save_manager.h"
#include "json.hpp"

#include <iostream>
#include <fstream>
#include <optional>
#include <vector>

namespace fs = std::filesystem;
using nj = nlohmann::json;

/*  i did everything i can so that this have as much error handling as possible.
    hell, even made this fucking optional. there's something within me, telling
    me that some random person will not do what he needs to do (which could just
    probably me in the far future).  */
std::optional<std::vector<ClassPreset>> parsePresets(const std::string &FILE_NAME) {
    std::ifstream file(FILE_NAME);
    if(!file.is_open()) {
        std::cout << "Failed to open " << FILE_NAME << '\n';
        hUtils::sleep(2000);
        return std::nullopt;
    }

    nj json;
    try {
        file >> json;
    } catch(const nj::parse_error&) {
        std::cout << "Failed to parse " << FILE_NAME << '\n';
        hUtils::sleep(2000);
        return std::nullopt;  // if JSON file is not valid.
    }
    if(!json.is_array()) return std::nullopt;
    std::vector<ClassPreset> result;
    for(auto& entry : json) {
        ClassPreset preset;
        if(!entry.contains("class_name")) continue;
        if(!entry.contains("main_hand")) continue;
        
        preset.class_name   = entry["class_name"];
        preset.main_hand    = entry["main_hand"];
        preset.off_hand     = entry.value("off_hand", "");
        preset.starting_pts = entry.value("starting_points", 5);

        if(!entry.contains("attribute") || !entry["attribute"].is_object()) continue;
        auto attr = entry["attribute"];
        Attributes a;
        a.vigor        = attr.value("vigor", 0);
        a.strength     = attr.value("strength", 0);
        a.endurance    = attr.value("endurance", 0);
        a.intelligence = attr.value("intelligence", 0);
        a.dexterity    = attr.value("dexterity", 0);
        preset.attribute = a;
        result.push_back(preset);
    }
    if(result.empty()) {
        std::cout << "No valid presets found in " << FILE_NAME << '\n';
        hUtils::sleep(2000);
        return std::nullopt;  // if JSON is valid but logically wrong.
    }
    return result;
}
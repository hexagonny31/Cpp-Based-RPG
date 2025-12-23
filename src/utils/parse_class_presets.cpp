#include "hutils.h"
#include "class_presets.h"
#include "json.hpp"

#include <iostream>
#include <fstream>
#include <optional>
#include <vector>

namespace fs = std::filesystem;
using json = nlohmann::json;

std::optional<std::vector<ClassPreset>> parsePresets(const std::string &FILE_NAME) {
    std::ifstream file(FILE_NAME);
    if(!file.is_open()) {
        std::cout << "Failed to open " << FILE_NAME << '\n';
        hUtils::sleep(2000);
        return std::nullopt;
    }

    json j;
    try {
        file >> j;
    } catch(const json::parse_error&) {
        std::cout << "Failed to parse " << FILE_NAME << '\n';
        hUtils::sleep(2000);
        return std::nullopt;  // if JSON file is not valid.
    }
    if(!j.is_array()) return std::nullopt;
    std::vector<ClassPreset> result;
    for(auto& entry : j) {
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
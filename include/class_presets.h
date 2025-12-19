#ifndef PARSERS_H
#define PARSERS_H

#include "entity.h"

#include <string>
#include <optional>

struct ClassPresets
{
    std::string class_name;
    std::string main_hand;
    std::string off_hand;
    Attributes attributes;
    int starting_pts = 5;
};

std::optional<std::vector<ClassPresets>> parsePresets(const std::string &FILE_NAME = "json/class_presets.json");

// save manager.
std::optional<Player> newSave();

#endif
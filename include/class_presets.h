#ifndef PARSERS_H
#define PARSERS_H

#include "entity.h"

#include <string>
#include <optional>

struct ClassPreset
{
    std::string class_name;
    std::string main_hand;
    std::string off_hand = "";
    Attributes attribute;
    int starting_pts = 5;
};

std::optional<std::vector<ClassPreset>> parsePresets(const std::string &FILE_NAME = "json/class_presets.json");

#endif
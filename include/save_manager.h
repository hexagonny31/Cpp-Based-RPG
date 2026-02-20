#ifndef SAVE_MANAGER_H
#define SAVE_MANAGER_H

#include "entity.h"

#include <optional>
#include <stdexcept>

struct ClassPreset
{
    std::string class_name = "";
    std::string main_hand  = "";
    std::string off_hand   = "";
    Attributes attribute   = {};
    int starting_pts       = 15;
};

std::optional<std::vector<ClassPreset>> parsePresets(const std::string &FILE_NAME = "json/class_presets.json");

Player newCharacterSave();
void saveToFile(const Player &player);
Player loadToFile();

#endif
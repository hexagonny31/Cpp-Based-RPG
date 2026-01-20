#ifndef SAVE_MANAGER_H
#define SAVE_MANAGER_H

#include "entity.h"

#include <optional>
#include <stdexcept>

Player newCharacterSave();
void saveToFile(const Player &player);
Player loadToFile();

#endif
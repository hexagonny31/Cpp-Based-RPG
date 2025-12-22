#ifndef SAVE_MANAGER_H
#define SAVE_MANAGER_H

#include "entity.h"

#include <optional>
#include <stdexcept>

struct UserCancelled : std::runtime_error
{
    using std::runtime_error::runtime_error;
};

struct LoadFailed : std::runtime_error
{
    using std::runtime_error::runtime_error;
};

Player newCharacterSave();
void saveToFile(const Player &player);

#endif
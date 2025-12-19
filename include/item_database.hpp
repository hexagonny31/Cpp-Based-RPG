#pragma once

#include <string>
#include <optional>
#include <unordered_map>
#include "entity.h"

struct ItemDatabase {
public:
    static bool load(const std::string& FILE_NAME = "json/items.json");
    static std::optional<Item> find(const std::string& item_id);
private:
    static std::unordered_map<std::string, Item> items;
};
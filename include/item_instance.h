#ifndef ITEM_INSTANCE_H
#define ITEM_INSTANCE_H

#include "item_database.h"
#include "json.hpp"

#include <algorithm>

struct ItemInstance {
private:
    const Item* item = nullptr;
    int quantity = 1;

    ItemInstance() = default;

    explicit ItemInstance(const Item* item_ptr, int qty = 1);
    ItemInstance(const std::string& item_id, int qty = 1);
public:
    const Item        &getItem()       const;
    const std::string &getName()       const;
    const Properties  &getProperties() const;

    bool isStackable() const;
    int maxStackSize() const;
    int add(int amount);
    // ItemInstance split(int amount);
};

#endif
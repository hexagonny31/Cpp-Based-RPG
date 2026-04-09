#pragma once

#include "hutils.h"
#include "common.h"
#include "json.hpp"
#include "item_database.h"

#include <algorithm>

struct ItemInstance {
    const Item* item = nullptr;
    int quantity = 1;

    ItemInstance() = default;

    explicit ItemInstance(const Item* item_ptr, int qty = 1) : item(item_ptr), quantity(qty)
    {
        if (quantity < 1) quantity = 1;
    }

    ItemInstance(const std::string& item_id, int qty = 1)
    {
        auto init = ItemDatabase::instance().find(item_id);
        if (init) *this = ItemInstance(&*init, qty);
    }

    const Item        &getItem()       const { return *item; }
    const std::string &getName()       const { return item ? item->name : ""; }
    const Properties  &getProperties() const { return item ? item->property : Properties{}; }

    bool isStackable() const
    {
        return item && item->property.stackable && item->property.max_stack > 1;
    }

    int maxStackSize() const
    {
        return item ? item->property.max_stack : 1;
    }

    int add(int amount)
    {
        if (!isStackable()) return 0;
        int space = maxStackSize() - quantity;
        int added = (std::min)(amount, (std::max)(0, space));
        quantity += added;
        return added;
    }

    // ItemInstance split(int amount)
    // {
    //     if (!item || amount <= 0 || amount >= quantity) return {};
    //     ItemInstance newStack(item, amount);
    //     quantity -= amount;
    //     return newStack;
    // }
};
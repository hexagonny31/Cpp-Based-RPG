#include "item_instance.h"

#include <fstream>
#include <utility>

using nj = nlohmann::json;

explicit ItemInstance::ItemInstance(const Item* item_ptr, int qty) : item(item_ptr), quantity(qty)
{
    if(quantity < 1) quantity = 1;
}

ItemInstance::ItemInstance(const std::string& item_id, int qty = 1)
{
    auto init = ItemDatabase::instance().find(item_id);
    if(init) *this = ItemInstance(&*init, qty);
}

const Item &ItemInstance::getItem() const
{
    return *item;
}

const std::string &ItemInstance::getName() const 
{
    return item ? item->name : "";
}

const Properties &ItemInstance::getProperties() const
{
    return item ? item->property : Properties{};
}

bool ItemInstance::isStackable() const
{
    return item && item->property.stackable && item->property.max_stack > 1;
}

int ItemInstance::add(int amount)
{
    if(!isStackable()) return 0;
    int space = maxStackSize() - quantity;
    int added = (std::min)(amount, (std::max)(0, space));
    quantity += added;
    return added;
}

// ItemInstance ItemInstance::split(int amount)
// {
//     if(!item || amount <= 0 || amount >= quantity) return {};
//     ItemInstance newStack(item, amount);
//     quantity -= amount;
//     return newStack;
// }
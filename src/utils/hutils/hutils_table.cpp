#include "hutils.h"

#include <iostream>
#include <iomanip>
#include <algorithm>

namespace hUtils {
    Table table;

    unsigned int Table::findMaxWidth() const
    {
        int width = 0;
        for(const auto& element : elements) {
            std::string visible = text.stripAnsi(element);
            width = (std::max)(width, static_cast<int>(visible.length()));
        }
        return width;
    }

    void Table::toColumn(std::string orientation, unsigned int givenWidth, unsigned int columns)
    {
        if(elements.empty()) {
            std::cerr << "No elements to display.\n";
            return;
        }

        unsigned int maxWidth = findMaxWidth();
        unsigned int padding = (givenWidth != 0) ? (std::max)(givenWidth, maxWidth) : maxWidth;

        auto rows = (elements.size() + columns) / columns;
        for(size_t i = 0; i < rows; ++i) {
            for(size_t j = 0; j < columns; ++j){
                size_t pos = j + i * columns;

                if(pos < elements.size()) {
                    std::cout << std::setw(padding)
                              << ((orientation == "right") ? std::right : std::left)
                              << elements[pos];
                    if(j < columns - 1) std::cout << "|";
                }
            }
            std::cout << '\n';
        }
    }
}
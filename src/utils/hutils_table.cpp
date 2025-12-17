#include "hutils.h"

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <iomanip>

namespace hUtils {
    Table table;

    int Table::calculateMaxWidth() const
    {
        int width = 0;
        for (const auto& element : elements) {
            std::string visible = stripAnsi(element);
            width = (std::max)(width, static_cast<int>(visible.length()));
        }
        return width;
    }

    void Table::toColumn(std::string orientation, int givenWidth, int numberOfColumns)
    {
        if(elements.empty()){
            std::cerr << "No elements to display.\n";
            return;
        }

        orientation = hUtils::text.toLowerCase(orientation);

        int maxWidth = calculateMaxWidth();
        int finalWidth = (givenWidth != 0) ? (std::max)(givenWidth - 1, maxWidth) : maxWidth;

        int rows = (elements.size() + numberOfColumns - 1) / numberOfColumns;
        for(int i = 0; i < rows; ++i){
            for(int j = 0; j < numberOfColumns; ++j){
                int index = j + i * numberOfColumns;

                if(index < static_cast<int>(elements.size())){
                    std::string visible = stripAnsi(elements[index]);
                    int padding = finalWidth - visible.length();

                    if(orientation == "right"){
                        std::cout << std::setw(padding);
                        if(j < numberOfColumns) std::cout << "|";
                        std::cout << elements[index];
                    }
                    else if(orientation == "left"){
                        std::cout << elements[index];
                        std::cout << std::setw(padding);
                        if(j < numberOfColumns - 1) std::cout << "|";
                    }
                }
            }
            std::cout << '\n';
        }
    }
}
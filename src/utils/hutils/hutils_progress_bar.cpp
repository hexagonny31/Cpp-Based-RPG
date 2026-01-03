#include "hutils.h"

#include <iostream>
#include <algorithm>

constexpr int BAR_LENGTH = 40;

namespace hUtils {
    Bar bar;
    
    void Bar::printBar(double value, double maxPoints, int filledColor, int emptyColor)
    {
        double percentage = value / maxPoints;
        int filled = static_cast<int>(percentage * BAR_LENGTH);
        int empty = BAR_LENGTH - filled;
        std::string bar;

        for (int i = 0; i < filled; ++i) {
            bar.append("\033[38;5;" + std::to_string(filledColor) + "m█\033[0m");
        }
        for (int i = 0; i < empty; ++i) {
            bar.append("\033[38;5;" + std::to_string(emptyColor) + "m▒\033[0m");
        }
        
        int appliedScreenWidth = SCREEN_WIDTH;
        int padding = (appliedScreenWidth - BAR_LENGTH) / 2;
        if (padding < 0) padding = 0;

        std::cout << std::string(padding, ' ') << bar << '\n';

        std::string percentageString = hUtils::text.toString(percentage * 100, 2) + "/100.00 % (" +
                                       hUtils::text.toString(value, 2) + "/" +
                                       hUtils::text.toString(maxPoints, 2) + ")";

        hUtils::text.toCentered(percentageString);
    }

    void Bar::setBar(double value, double maxPoints, int filledColor, int emptyColor)
    {
        value = std::clamp(value, 0.0, maxPoints);
        printBar(value, maxPoints, filledColor, emptyColor);
        sleep(200);
    }
}
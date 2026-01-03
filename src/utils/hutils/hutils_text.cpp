#include "hutils.h"

#include <iostream>
#include <iomanip>
#include <algorithm>

using std::cout;
using std::string;

using namespace std::string_literals;

std::string colorLabel(std::string label, const int color, bool use356) {
    return hUtils::text.fgColor(color, use356) + label + hUtils::text.defaultText();
}

bool proceed() {
    char choice;
    string input;
    do {
        cout << "This action cannot be undone.\n"
             << "Do you want to continue? (y/n): ";
        std::getline(std::cin, input);
        
        if(!input.empty()) choice = std::tolower(input[0]);
        else choice = '\0';

        if(choice != 'y' && choice != 'n') {
            hUtils::text.reject("\n\nInvalid choice! Please try again!", 3);
        }
    } while(choice != 'y' && choice != 'n');
    hUtils::text.clearAbove(2);
    return choice == 'y';
}

char charIn(std::string prompt) {
    char input;
    cout << prompt << "\n> ";
    std::cin >> input;
    std::cin.ignore(256, '\n');
    input = tolower(input);
    return input;
}

std::string strIn(std::string prompt, int min, int max) {
    std::string input = "";
    while(true) {
        input = "";
        cout << prompt << "> ";
        std::getline(std::cin, input);
        if(input == "exit" || input == "e") return input;
        if(input.length() < min) {
            hUtils::text.reject("Name can't be under " + std::to_string(min) + " characters!"s, 2);
            continue;
        } else if(input.length() > max) {
            hUtils::text.reject("Name can't be over " + std::to_string(max) + " characters!"s, 2);
            continue;
        } else {
            cout << '\n';
            return input;
        }
    }
}

int intIn(std::string prompt, int min, int max) {
    int input;
    while(true) {
        cout << prompt << "\n> ";
        std::cin >> input;
        if(input < min) {
            hUtils::text.reject("Input is too low!", 2);
            continue;
        } else if(input > max) {
            hUtils::text.reject("Input is too high!", 2);
            continue;
        } else {
            cout << '\n';
            return input;
        }
    }
}

namespace hUtils {

    Text text;

    void Text::reject(const string& msg, int lines)
    {
        std::cout << msg;
        sleep(2000);
        text.clearAbove(lines);
    }

    void Text::trim(string& text)
    {
        const string whitespace = " \t\n\r\f\v";
        size_t first = text.find_first_not_of(whitespace);
        if(std::string::npos == first) {
            text.clear();
            return;
        }
        size_t last = text.find_last_not_of(whitespace);
        text.erase(0, first);
        text.erase(last - first + 1);
    }

    void Text::toLine(char character)
    {
        cout << string(SCREEN_WIDTH - 1, character) << '\n';
    }

    void Text::toCentered(string text, int colorCode, int number, bool use256)
    {
        int appliedScreenWidth = SCREEN_WIDTH;
        int padding = (appliedScreenWidth - text.length()) / 2;
        if(padding < 0) padding = 0;

        if (number >= 1) {
            text = std::to_string(number) + ". " + text;
        }
    
        cout << fgColor(colorCode, use256)
             << std::string(padding, ' ')
             << text
             << defaultText()
             << '\n';
    }

    void Text::toRight(string text, int colorCode, bool use256)
    {
        cout << fgColor(colorCode, use256)
             << std::setw(SCREEN_WIDTH) << text << '\n'
             << defaultText();
    }

    void Text::toLeft(string text, int colorCode, int number, bool use256)
    {
        cout << fgColor(colorCode, use256);

        if(number >= 1){
            text = std::to_string(number) + ". " + text;
        }

        cout << text << '\n'
             << defaultText();
    }

    string Text::toLowerCase(string text)
    {
        transform(text.begin(), text.end(), text.begin(), ::tolower);
        return text;
    }

    string Text::toUpperCase(string text)
    {
        transform(text.begin(), text.end(), text.begin(), ::toupper);
        return text;
    }

    string Text::fgColor(int textColor, bool use256)
    {
        if(use256){
            // 256-color mode (0-255)
            if (textColor >= 0 && textColor <= 255){
                return "\033[38;5;" + std::to_string(textColor) + "m";
            }
        } 
        else{
            // Standard ANSI 16 colors
            if ((textColor >= 30 && textColor <= 37) || (textColor >= 90 && textColor <= 97)){
                return "\033[" + std::to_string(textColor) + "m";
            }
        }
    
        return "";
    }

    string Text::bgColor(int textColor, bool use256)
    {
        if(use256){
            // 256-color mode (0-255)
            if (textColor >= 0 && textColor <= 255){
                return "\033[48;5;" + std::to_string(textColor) + "m";
            }
        }
        else{
            // Standard ANSI 16 colors
            if ((textColor >= 40 && textColor <= 47) || (textColor >= 100 && textColor <= 107)){
                return "\033[" + std::to_string(textColor) + "m";
            }
        }

        return "";
    }

    string Text::defaultText()
    {
        return "\033[0m";
    }

    void Text::clearAll()
    {
    #ifdef _WIN32
        if(std::getenv("TERM")){ 
            // Use ANSI escape codes if the terminal supports it
            std::cout << "\033[2J\033[H" << std::flush;
        }
        else{
            system("cls");
        }
    #else
        std::cout << "\033[2J\033[H" << std::flush;
    #endif
    }

    void Text::clearBelow(int line)
    {
        cout << "\033[" << line << ";1H";
        cout << "\033[J";
    }

    void Text::clearAbove(int line) {
        cout << "\033[2K";
        for (int i = 0; i < line; i++) {
            cout << "\033[1A" << "\033[G" << "\033[2K";
        }
        cout.flush();
    }
}
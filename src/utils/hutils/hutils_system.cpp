#include "hutils.h"

#include <iostream>
#include <thread>
#include <chrono>

using std::cout;
using std::string;

namespace hUtils {
    #ifdef _WIN32
    /**
     * @brief Waits for a key press and returns the corresponding character.
     * @param keys A list of virtual key codes to listen for (e.g., {'A', 'B', 'C'}).
     * @return The character corresponding to the key pressed, or '\x1B' for Escape.
     * @warning This function is Windows-specific and uses the WinAPI. It will not compile on non-Windows platforms.
     */
    char GetInputKeymap(std::initializer_list<unsigned char> keys) {
        while(true) {
            FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
            if((GetAsyncKeyState(VK_ESCAPE) >> 8) & 0x80) return '\x1B';
            for(const unsigned char c : keys) {
                if((GetAsyncKeyState(c) >> 8) & 0x80) {
                    while((GetAsyncKeyState(c) >> 8) & 0x80) Sleep(10);
                    FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
                    return c;
                }
            }
            Sleep(8);
        }
    }
    #endif

    /**
     * @brief Adjusts the console window size to predefined dimensions and disables resizing.
     * 
     * On Windows, it sets the console buffer and window size using the WinAPI and disables
     * resizing by modifying the window style. On Unix-like systems, it uses ioctl to set the terminal size
     * and sends an ANSI escape code to adjust the terminal window size.
     */
    void SetConsoleWindowSize() {
        #ifdef _WIN32
            // Windows: Set console size and disable resizing
            HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
            if(hOut == INVALID_HANDLE_VALUE){
                std::cerr << "Error: Could not get console handle." << std::endl;
                return;
            }

            //  Temporary because is so ass.
            SMALL_RECT tempRect = {0, 0, 1, 1};
            SetConsoleWindowInfo(hOut, TRUE, &tempRect);
        
            // Set buffer size (prevents scrolling)
            COORD bufferSize = { static_cast<SHORT>(SCREEN_WIDTH), static_cast<SHORT>(SCREEN_HEIGHT * 30) };
            SetConsoleScreenBufferSize(hOut, bufferSize);
        
            // Set the window size
            SMALL_RECT DisplayArea = { 0, 0, static_cast<SHORT>(SCREEN_WIDTH - 1), static_cast<SHORT>(SCREEN_HEIGHT - 1) };
            SetConsoleWindowInfo(hOut, TRUE, &DisplayArea);
        
            // Disable window resizing (removes maximize and resize options)
            HWND hwnd = GetConsoleWindow();
            LONG style = GetWindowLong(hwnd, GWL_STYLE);
            style &= ~(WS_SIZEBOX | WS_MAXIMIZEBOX);
            SetWindowLong(hwnd, GWL_STYLE, style);
            SetWindowPos(hwnd, nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);
        
        #elif defined(__unix__) || defined(__APPLE__)
            // Unix (Linux/macOS): Resize terminal using ioctl
            struct winsize w;
            w.ws_row = SCREEN_HEIGHT;
            w.ws_col = SCREEN_WIDTH;
            ioctl(STDOUT_FILENO, TIOCSWINSZ, &w);
            
            cout << "\033[8;" << SCREEN_HEIGHT << ";" << SCREEN_WIDTH << "t";  
        #endif
    }

    /**
     * @brief Prompts the user with a yes/no question and returns their response as a boolean.
     * @param prompt The question to present to the user. Defaults to "Do you want to continue?".
     * @return true if the user responds with 'y' or 'Y', false if they respond with 'n' or 'N'.
     * @warning The function will continue to prompt until a valid response is received. It also
     *          clears the console above the prompt after a valid response.
     */
    bool Proceed(std::string prompt) {
        char choice;
        string input;
        do {
            cout << "This action cannot be undone.\n"
                 << prompt << " (y/n): ";
            std::getline(std::cin, input);
            
            if(!input.empty()) choice = std::tolower(input[0]);
            else choice = '\0';

            if(choice != 'y' && choice != 'n')
                hUtils::text.reject("\n\nInvalid choice! Please try again!", 3);
        } while(choice != 'y' && choice != 'n');
        hUtils::text.clearAbove(2);
        return choice == 'y';
    }

    /**
     * @brief Prompts the user for a single character input and returns it.
     * @param prompt The message to display to the user before the input. Defaults to an empty string.
     * @return The character entered by the user, converted to lowercase.
     */
    char GetCharacterInput(std::string prompt) {
        char input;
        cout << prompt << "> ";
        std::cin >> input;
        std::cin.ignore(255,'\n');
        input = tolower(input);
        return input;
    }

    /**
     * @brief Prompts the user for a string input within specified length constraints and returns it.
     * @param prompt The message to display to the user before the input. Defaults to an empty string.
     * @param min The minimum length of the input string. Defaults to 2.
     * @param max The maximum length of the input string. Defaults to 100.
     * @return The string entered by the user that meets the length requirements. If the user enters
     *         "exit" or "e", that string is returned immediately.
     * @warning The function will continue to prompt until a valid response is received. It also
     *          clears the console above the prompt after a valid response.
     */
    std::string GetStringInput(std::string prompt, int min, int max) {
        using namespace std::string_literals;
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

    /**
     * @brief Prompts the user for an integer input within specified range constraints and returns it.
     * @param prompt The message to display to the user before the input. Defaults to an empty string.
     * @param min The minimum value of the input integer. Defaults to 0.
     * @param max The maximum value of the input integer. Defaults to 10.
     * @return The integer entered by the user that meets the range requirements.
     * @warning The function will continue to prompt until a valid response is received. It also
     *          clears the console above the prompt after a valid response.
     */
    int GetIntegerInput(std::string prompt, int min, int max) {
        int input;
        while(true) {
            cout << prompt << "> ";
            std::cin >> input;
            std::cin.ignore(255,'\n');
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

    /*
        std::cin.get() is so primitive that I have to add a check(bool) if the last input ended with a new line('\n').
        Otherwise, the whole premise refuses to work.

        To any knows how to fix this problem PLS can I get a comment. I SUCK AT CODING!!!!!1
    */
    void Pause(bool clearBuffer) {
        std::cout << "Press Enter to continue. . .";
        if (clearBuffer) std::cin.ignore(255,'\n');
        std::cin.get();
        hUtils::text.clearAbove(1);
    }

    void Sleep(int miliseconds) {
        std::this_thread::sleep_for(std::chrono::milliseconds(miliseconds));
    }

}
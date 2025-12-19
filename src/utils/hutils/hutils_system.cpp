#include "hutils.h"

#include <iostream>
#include <thread>
#include <chrono>


using std::cout;
using std::string;

namespace hUtils {

    void setConsoleWindowSize() {
        #ifdef _WIN32
            // Windows: Set console size and disable resizing
            HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
            if(hOut == INVALID_HANDLE_VALUE){
                std::cerr << "Error: Could not get console handle." << std::endl;
                return;
            }
        
            // Set buffer size (prevents scrolling)
            COORD bufferSize = { static_cast<SHORT>(SCREEN_WIDTH), static_cast<SHORT>(SCREEN_HEIGHT) };
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

    /*
        std::cin.get() is so primitive that I have to add a check(bool) if the last input ended with a new line('\n').
        Otherwise, the whole premise refuses to work. So please understand.
    */
    void pause(bool clearBuffer)
    {
        std::cout << "Press Enter to continue. . .";
        if (clearBuffer) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
        std::cin.get();
        hUtils::text.clearAbove(1);
    }

    void sleep(int miliseconds)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(miliseconds));
    }

}
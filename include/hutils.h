#ifndef HUTILS_H
#define HUTILS_H

#ifdef _WIN32
    #include <windows.h>
#else
    #include <cstdlib>
    #include <sys/ioctl.h>
    #include <unistd.h>
#endif

#include <string>
#include <sstream>
#include <regex>
#include <iomanip>

#ifndef HUTIL_API
#define HUTIL_API
#endif  

constexpr int SCREEN_WIDTH  = 75;
constexpr int SCREEN_HEIGHT = 30;

std::string colorLabel(std::string label, const int color, bool use356 = true);
bool proceed(std::string prompt = "Do you want to continue?");
char charIn(std::string prompt = "");
std::string strIn(std::string prompt = "", int min = 2, int max = 64);
int intIn(std::string prompt = "", int min = 0, int max = 10);

namespace hUtils {

    // --- SYSTEM UTILITIES ---
    HUTIL_API void setConsoleWindowSize();          //  Adjust console size.
    HUTIL_API void pause(bool clearBuffer = false); //  Cross-platform system pause.
    HUTIL_API void sleep(int milliseconds);         //  Sleep for a given duration.

    // --- TEXT UTILITIES ---
    struct Text {
    public:
        HUTIL_API void reject            (const std::string& msg);
        HUTIL_API void reject            (const std::string& msg,
                                          int lines);
        HUTIL_API void trim              (std::string& text);
        HUTIL_API void toLine            (char character = '-'); //  Print a line of repeated characters
        HUTIL_API void toCentered        (std::string text,      //  Prints centered text.
                                          int colorCode = 0,
                                          int number = 0,
                                          bool use256 = false);      
        HUTIL_API void toRight           (std::string text,      //  Moves text to the right.
                                          int colorCode = 0,
                                          bool use256 = false);  
        HUTIL_API void toLeft            (std::string text,      //  Moves text to the left.
                                          int colorCode = 0,
                                          int number = 0,
                                          bool use256 = false);
        HUTIL_API std::string toLowerCase(std::string text);     //  Convert string to lowercase.
        HUTIL_API std::string toUpperCase(std::string text);     //  Convert string to uppercase.
        template <typename T>
        HUTIL_API std::string toString(const T& value,           //  Converts int and doubles to string.
                                       int precision)
        {
            std::ostringstream oss;
            if(precision >= 0 && std::is_floating_point<T>::value) {
                oss << std::fixed << std::setprecision(precision);
            }
            oss << value;
            return oss.str();
        }

        HUTIL_API std::string fgColor    (int textColor = 0,     //  Get ANSI color codes.
                                          bool use256 = false);
        HUTIL_API std::string bgColor    (int textColor = 0,
                                          bool use256 = false);
        HUTIL_API std::string defaultText();                     //  Reset text color.
        HUTIL_API std::string stripAnsi(const std::string& text) const
        {
            return std::regex_replace(text, std::regex("\033\\[[0-9;]*m"), "");
        }

        HUTIL_API void clearAll          (int delay = 0);                     //  Clears every output in the terminal.
        HUTIL_API void clearBelow        (int line);             //  Clears an assigned line below it.
        HUTIL_API void clearAbove        (int line);             //  Clears an assigned line above it.
    };

    struct Table {
    private:
        std::vector<std::string> elements;

        template <typename T>
        std::string toString (const T& value)
        {
            std::ostringstream oss;
            oss << value;
            return oss.str();
        }
        unsigned int findMaxWidth() const;
    
    public:
        template <typename... Args>
        HUTIL_API void setElements(Args... args)
        {
            elements.clear();
            (elements.push_back(toString(args)), ...);
        }
        template <typename T>
        HUTIL_API void setElements(const std::vector<T>& vec) {
            elements.clear();
            for (const auto& item : vec) {
                elements.push_back(toString(item));
            }
        }
        
        HUTIL_API void toColumn   (std::string  orientation     = "left",
                                   unsigned int givenWidth      = 0,
                                   unsigned int numberOfColumns = 2);
    };

    struct Bar {
    private:
        void printBar(double value,
                      double maxPoints,
                      int filledColor = 255,
                      int emptyColor  = 237);
    public:
        HUTIL_API void setBar  (double value,
                                double maxPoints,
                                int filledColor = 252,
                                int emptyColor  = 237);
    };
    
    extern Text   text;
    extern Table  table;  
    extern Bar    bar;
}

#endif
#include <Arduino.h>
#include <LiquidCrystal.h>

class LCDDebug {
    public: 
        LiquidCrystal raw;
        LCDDebug(uint8_t rs, uint8_t e, uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7);
        void println (const char* string);
        void println (String string);
        void printPos (uint8_t pos, uint8_t row, const char* string);
    private:
        uint8_t cursorPos;
        uint8_t cursorRow;
        String lastString = String();
        void setCursorPos ();
};
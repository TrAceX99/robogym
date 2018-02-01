#include "LCDDebug.h"

LCDDebug::LCDDebug (uint8_t rs, uint8_t e, uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7) : raw(LiquidCrystal(rs, e, d4, d5, d6, d7)) {
    raw.begin(16, 2);
    raw.clear();
}

void LCDDebug::setCursorPos() {
    raw.setCursor(cursorPos, cursorRow);
}

void LCDDebug::println (const char* string) {
    raw.clear();
    raw.print(lastString);
    cursorPos = 0;
    cursorRow = 1;
    setCursorPos();
    raw.print(string);
    lastString = String(string);
}

void LCDDebug::println (String string) {
    raw.clear();
    raw.print(lastString);
    cursorPos = 0;
    cursorRow = 1;
    setCursorPos();
    raw.print(string);
    lastString = string;
}

void LCDDebug::printPos (uint8_t pos, uint8_t row, const char* string) {
    cursorPos = pos;
    cursorRow = row;
    setCursorPos();
    String _string = String(string) + "               ";
    lastString = _string;
    raw.print(_string);
}
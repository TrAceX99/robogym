#include <iostream>
#include "WinBase.h"
#include "Windows.h"
#include "uFCoder.h"
#include <conio.h>

#define ESC 27
#define MIFARE_LINEAR_CAPACITY 752

using namespace std;

uint8_t cardType;
uint32_t cardID;

void WaitForCard() {
    cout << "Waiting for card ";
    while(GetCardId(&cardType, &cardID)) {
        cout << ".";
        Sleep(100);
        if(GetKeyState(VK_ESCAPE) & 0x8000) break;
    }
}

main () {
    const uint8_t allow = 0xFF;

    uint8_t* data;
    uint16_t dalMeZezas;
    
    if(ReaderOpen() == 0) {
        uint32_t type;
        GetReaderType(&type);
        cout << "Reader OK. Type:" << uppercase << showbase << hex << type << endl;
        Sleep(1000);
        while(true) {
            cout << "Press any key to continue, W to write access code to card, F to format card, esc to exit" << endl;
            char a = getch();

            if (a == ESC) {
                ReaderClose();
                return 0;
            } else if (a == 'w') {
                if (GetCardId(&cardType, &cardID))
                    WaitForCard();
                ReaderUISignal(3, 0);

                LinearWrite(&allow, 0, 1, &dalMeZezas, MIFARE_AUTHENT1A, 0);

                cout << " Card access granted";
            } else if (a == 'f') {
                WaitForCard();
                ReaderUISignal(3, 0);

                data = new byte[MIFARE_LINEAR_CAPACITY];
                for(int i = 0; i < MIFARE_LINEAR_CAPACITY; i++) data[i] = 0x00;
                LinearWrite(data, 0, MIFARE_LINEAR_CAPACITY, &dalMeZezas, MIFARE_AUTHENT1A, 0);
                delete[] data;

                cout << " Bytes written: " << dalMeZezas;
            } else break;

            cout << endl;
        }
        while(true) {
            cout << "Press esc to exit program" << endl;
            WaitForCard();
            if(GetKeyState(VK_ESCAPE) & 0x8000) break;

            cout << hex << showbase << uppercase << "\nType: " << (int)cardType << "\nID:" << cardID << endl;

            data = new uint8_t[1];
            LinearRead(data, 0, 1, &dalMeZezas, MIFARE_AUTHENT1A, 0);

            cout << "Bytes returned: " << dalMeZezas << endl << "Data: ";
            for (int i = 0; i < dalMeZezas; i++) {
                cout << (int)data[i];
            }

            if (data[0] == allow) {
                cout << "\nAccess allowed!" << endl;
                ReaderUISignal(1, 0);
            } else {
                cout << "\nAccess denied!" << endl;
                ReaderUISignal(2, 0);
            }

            delete[] data;
            cout << endl;
            Sleep(2000);
        }
        SetSpeakerFrequency(1000);
        Sleep(10);
        SetSpeakerFrequency(0);
    } else {
        cout << "Reader opening error!" << endl;
    }
    ReaderClose();
    return 0;
}
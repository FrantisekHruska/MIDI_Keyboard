#include <Arduino.h>

#define COLUMNS 5
#define ROWS 5

struct Keyboard{
    uint8_t size = 0x00; // 0xROWS COLUMNS 
    uint8_t output[ROWS]; // ROW output
};



void readKeyboard(struct Keyboard* _keyboard){

    for (uint8_t i = 0; i < COLUMNS; i++){
        DDRC = 0x1f;
        PORTC = 0 << i;

        _keyboard->output[i] = PINA;
        
        
        // PORTC = 0x1f;

        DDRC = 0x00; //INPUT
        PORTC = 0x00;

    }
}
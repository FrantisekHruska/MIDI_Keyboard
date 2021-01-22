#include <Arduino.h>

#define COLUMNS 5
#define ROWS 5

struct Keyboard{
    uint8_t size = 0x00; // 0xROWS COLUMNS 
    uint8_t output[ROWS]; // ROW output
};



void readKeyboard(struct Keyboard* _keyboard){

    // Sets COLUMN to HIGH and saves row state to output
    for (uint8_t i = 0; i < COLUMNS; i++){

        PORTA = 0;
        PORTA = 1 << i;
        _keyboard->output[i] = PINC;
    }
    
    // _delay_ms(1);
    // PORTA = 0x01;
    
    // _keyboard->output[0] = PINC;
    // _delay_ms(1);

}
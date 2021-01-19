#include <Arduino.h>

#define COLUMNS 5
#define ROWS 5

struct Keyboard{
    uint8_t size = 0x00; //0xROWS COLUMNS 
    uint8_t output[COLUMNS];
};

void readKeyboard(struct Keyboard* _keyboard){

    for (uint8_t i = 0; i < (_keyboard->size & 0x0f); i++){
        PORTA = 1 << i;
        _keyboard->output[i] = PINC;
        
    }
}
#include <Arduino.h>

#define COLUMNS 5
#define ROWS 5

struct Keyboard
{
    // 0xRADEK SLOUPEC (0xROWS COLUMNS)
    uint8_t output[ROWS]; // ROW output
    uint8_t output_1[ROWS];
    uint8_t regs = 0x00;
};

uint8_t compareRow(struct Keyboard *_keyboard, uint8_t rownum)
{
    return _keyboard->output[rownum] ^ _keyboard->output_1[rownum];
}

uint8_t *getArray(struct Keyboard *_keyboard)
{
    if (_keyboard->regs == (_keyboard->regs & 0x01))
    {
        return _keyboard->output_1;
    }
    else
    {
        return _keyboard->output;
    }
}

uint8_t *getArrayOld(struct Keyboard *_keyboard)
{
    if (_keyboard->regs == (_keyboard->regs & 0x01))
    {
        return _keyboard->output;
    }
    else
    {
        return _keyboard->output_1;
    }
}
uint8_t *getRow(struct Keyboard *_keyboard, uint8_t rownum)
{
    return;
}

void switchArray(struct Keyboard *_keyboard)
{
    _keyboard->regs ^= 0x01;
    return;
};

uint8_t getOn(struct Keyboard *_keyboard, uint8_t rownum)
{
    return getArrayOld(_keyboard)[rownum] & (getArray(_keyboard)[rownum] ^ 0xff);
}
uint8_t getOff(struct Keyboard *_keyboard, uint8_t rownum)
{
    return getArray(_keyboard)[rownum] & (getArrayOld(_keyboard)[rownum] ^ 0xff);
}

void readKeyboard(struct Keyboard *_keyboard)
{
    switchArray(_keyboard);
    for (uint8_t i = 0; i < COLUMNS; i++)
    {

        DDRC = 1 << i;
        PORTC = 0 << i;

        getArray(_keyboard)[i] = PINA ^ 0xff;

        // PORTC = 1 << i;

        DDRC = 0; //INPUT
        PORTC = 0;
    }
}
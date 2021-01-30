#include <Arduino.h>

#define COLUMNS 5
#define ROWS 5
#define OCTAVE 12

struct Keyboard
{
    // 0xRADEK SLOUPEC (0xROWS COLUMNS)
    uint8_t output[ROWS];
    uint8_t output_1[ROWS];
    uint8_t regs = 0x00;
    int8_t transpose = 0;
    uint8_t transpose_buttons_state = 0x00;
};

uint8_t *getArray(struct Keyboard *_keyboard)
{
    if (_keyboard->regs & 0x01)
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
    if (_keyboard->regs & 0x01)
    {
        return _keyboard->output;
    }
    else
    {
        return _keyboard->output_1;
    }
}
// strida promenou regs ktera urcuje do ktereho pole ulozit
void switchArray(struct Keyboard *_keyboard)
{
    _keyboard->regs = _keyboard->regs ^ 0x01;
    return;
}
// Najde co se zmenilo z 0 na 1
uint8_t getOn(struct Keyboard *_keyboard, uint8_t rownum)
{
    return getArrayOld(_keyboard)[rownum] & (getArray(_keyboard)[rownum] ^ 0xff);
}
// Najde co se zmenilo z 1 na 0
uint8_t getOff(struct Keyboard *_keyboard, uint8_t rownum)
{
    return getArray(_keyboard)[rownum] & (getArrayOld(_keyboard)[rownum] ^ 0xff);
}
void readTranspose(struct Keyboard *_keyboard)
{
  _keyboard->transpose_buttons_state = _keyboard->transpose_buttons_state + (PINB & 0b00000011);

  if ((_keyboard->transpose_buttons_state & 0x01) != ((_keyboard->transpose_buttons_state >> 2) & 0x01) || (_keyboard->transpose_buttons_state & 0x02) != ((_keyboard->transpose_buttons_state >> 2) & 0x02))
  {
    _delay_us(150);
    if (_keyboard->transpose_buttons_state & 0x01)
      _keyboard->transpose--;
    
    
    if ((_keyboard->transpose_buttons_state & 0x02)>>1)
      _keyboard->transpose++;
    
    // writeKeymap();
  }
  _keyboard->transpose_buttons_state <<= 2;
  _keyboard->transpose_buttons_state &= 0b00001111;
}
// budi sloupce a cte radky
void readKeyboard(struct Keyboard *_keyboard)
{
    readTranspose(_keyboard);
    switchArray(_keyboard);
    for (uint8_t i = 0; i < COLUMNS; i++)
    {

        DDRC = 1 << i;
        // PORTC = 0;

        _delay_us(150); // debounce delay

        getArray(_keyboard)[i] = PINA ^ 0xff; // XOR zneguje bity protoze 0 ma byt off a 1 ma byt ON
        // PORTC = 1 << i;

        DDRC = 0;  // INPUT
        PORTC = 0; //
    }
}

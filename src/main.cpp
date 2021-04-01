#include <Arduino.h>
#include "keyreader.h"
extern "C"
{
#include <uart.h>
}

struct Keyboard keyboard;

void processOutput(struct Keyboard *_keyboard)
{
  uint8_t temp = 0x00;

  for (uint8_t i = 0; i < COLUMNS; i++)
  {
    temp = getOff(_keyboard, i);

    for (uint8_t j = 0; j < ROWS; j++)
    {
      if ((temp & (0x01 << j)) >> j)
        if (_keyboard->keymap[(j << 4) | i] >= 0 && _keyboard->keymap[(j << 4) | i] <= 127)
          sendNoteOff(CHANNEL, _keyboard->keymap[(j << 4) | i]);
    }

    temp = getOn(_keyboard, i);

    for (uint8_t j = 0; j < ROWS; j++)
    {
      if ((temp & (0x01 << j)) >> j)
        if (_keyboard->keymap[(j << 4) | i] >= 0 && _keyboard->keymap[(j << 4) | i] <= 127)
          sendNoteOn(CHANNEL, _keyboard->keymap[(j << 4) | i]);
    }
  }
}

void setup()
{
  // Nastaveni pinu
  DDRB = 0x00; // ROWS INPUT
  PORTB = 0x00;

  DDRD = 0b11111000; // COLUMNS OUTPUT

  DDRC |= 0b00000111; // Nastavime LED piny jako output a zbytek nechame jako input
  PORTC = 0b00000011;

  uart0_init(UART_BAUD_SELECT(115200, 16000000L));
  writeKeymap(&keyboard);
}

void loop()
{
  readKeyboard(&keyboard);
  processOutput(&keyboard);
}

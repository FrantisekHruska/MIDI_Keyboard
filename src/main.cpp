#include <Arduino.h>
#include "keyreader.h"
extern "C"
{
#include <uart.h>
}

struct Keyboard keyboard;

void sendMIDI(uint8_t status, uint8_t key, uint8_t velocity = 127, uint8_t channel = 1)
{
  switch (status)
  {
  case 1: // NoteOn
    uart0_putc(0x90 | channel);
    break;
  case 0: // NoteOff
    uart0_putc(0x80 | channel);
    velocity = 0;
    break;
  default:
    return;
  }
  uart0_putc(key);
  uart0_putc(velocity);
}

void processOutput(struct Keyboard *_keyboard)
{
  uint8_t temp = 0x00;
  // uint8_t keystate;

  for (uint8_t i = 0; i < COLUMNS; i++)
  {
    temp = getOn(_keyboard, i);

    for (uint8_t j = 0; j < ROWS; j++)
    {
      // keystate = ((temp & (0x01 << j)) >> j);
      if ((temp & (0x01 << j)) >> j)
        if (_keyboard->keymap[(j << 4) | i] >= 0 && _keyboard->keymap[(j << 4) | i] <= 127)
          sendMIDI(0, _keyboard->keymap[(j << 4) | i], 127);
    }

    temp = getOff(_keyboard, i);

    for (uint8_t j = 0; j < ROWS; j++)
    {
      // keystate = ((temp & (0x01 << j)) >> j);
      if ((temp & (0x01 << j)) >> j)
        if (_keyboard->keymap[(j << 4) | i] >= 0 && _keyboard->keymap[(j << 4) | i] <= 127)
          sendMIDI(1, _keyboard->keymap[(j << 4) | i], 127);
    }
  }
}

void setup()
{
  // Nastaveni pinu
  DDRD = 0x00; // ROWS INPUT
  PORTD = 0x00;

  DDRB = 0b11111000; // COLUMNS OUTPUT

  DDRC = 0x00; // TRANSPOSE INPUT
  PORTC = 0x00;

  uart0_init(UART_BAUD_SELECT(115200, 16000000L));

  writeKeymap(&keyboard);
}

void loop()
{
  readKeyboard(&keyboard);
  processOutput(&keyboard);
}

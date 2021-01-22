#include <Arduino.h>
#include "keyreader.h"
extern "C"
{
#include <uart.h>
}

uint8_t keymap[0xff];
struct Keyboard keyboard;

void sendMIDI(uint8_t status, uint8_t key, uint8_t velocity = 127)
{
  switch (status)
  {
  case 1:
    uart0_putc(0x9c);
    break;
  case 0:
    uart0_putc(0x8c);
    break;
  default:
    return;
  }
  uart0_putc(key);
  uart0_putc(velocity);
}

int processOutput(struct Keyboard *_keyboard)
{
  uint8_t temp = 0x00;
  for (uint8_t i = 0; i < COLUMNS; i++)
  {
    temp = _keyboard->output[i];

    for (uint8_t j = 0; j < ROWS; j++)
    {
      sendMIDI(temp & (1 << j) >> j, keymap[(j << 4) | i], 127);
      //temp = temp >> 1;
    }
  }
}

void setup()
{
  DDRC = 0x00;
  PORTC = 0x00;
  DDRA = 0xff;

  uart0_init(UART_BAUD_SELECT(9600, 16000000L));

  keyboard.size = (ROWS << 4) | COLUMNS;

  // create keymap
  uint8_t tone = 60;
  for (uint8_t i = 0; i < COLUMNS; i++)
  {
    for (uint8_t j = 0; j < ROWS; j++)
    {
      keymap[(j << 4) | i] = tone;
      tone++;
    }
  }
}

void loop()
{
  readKeyboard(&keyboard);
  processOutput(&keyboard);
}

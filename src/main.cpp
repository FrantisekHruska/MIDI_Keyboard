#include <Arduino.h>
#include "keyreader.h"
extern "C"
{
#include <uart.h>
}

uint8_t keymap[0xff];
struct Keyboard keyboard;

uint8_t testvar;

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

int processOutput(struct Keyboard *_keyboard)
{
  uint8_t temp = 0x00;
  uint8_t keystate;

  for (uint8_t i = 0; i < COLUMNS; i++)
  {
    temp = getOn(_keyboard, i);

    for (uint8_t j = 0; j < ROWS; j++)
    {
      keystate = ((temp & (0x01 << j)) >> j);
      if (keystate)
        sendMIDI(0, keymap[(j << 4) | i], 127, 1);
    }

    temp = getOff(_keyboard, i);

    for (uint8_t j = 0; j < ROWS; j++)
    {
      keystate = ((temp & (0x01 << j)) >> j);
      if (keystate)
        sendMIDI(1, keymap[(j << 4) | i], 127, 1);
    }
    keystate = 0;
  }
}

void createKeymap()
{
  // create keymap
  const uint8_t rowmap[ROWS] = {0, 4, 3, 2, 1};
  uint8_t tone = 60;
  for (uint8_t i = 0; i < ROWS; i++)
  {
    for (uint8_t j = 0; j < COLUMNS; j++)
    {
      keymap[(rowmap[i] << 4) | (COLUMNS - 1) - j] = tone;
      tone++;
    }
  }
}

void setup()
{
  DDRA = 0x00; // ROWS INPUT
  PORTA = 0x00;

  DDRC = 0x1f; // COLUMNS OUTPUT

  uart0_init(UART_BAUD_SELECT(9600, 16000000L));

  createKeymap();
}

void loop()
{
  readKeyboard(&keyboard);
  //switchArray(&keyboard);
  //testvar = !testvar;
  //getArray(&keyboard)[0] = testvar;

  processOutput(&keyboard);
  //uart0_putc(getArrayOld(&keyboard)[0]);
  //uart0_putc(getOn(&keyboard,0));
  //_delay_ms(500);
}

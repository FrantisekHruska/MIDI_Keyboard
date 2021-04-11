#include <Arduino.h>
#include "keyreader.h"


/* Tento soubor je hlavni cast programu celeho keyboardu */


struct Keyboard keyboard;

void processOutput(struct Keyboard *_keyboard)// Funkce co zpracuje vystup keyboardu
{
  uint8_t temp = 0x00; // Docasna promena

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

void setup() // Zde se nastavuji piny pomoci registru
{
// Porty matice
  DDRB = 0x00; // Radky nastavime jako INPUT
  PORTB = 0x00;
  DDRD = 0b11111000; // Sloupce nastavime OUTPUT

// Nastavime portC tak aby piny na kterych je LED byl output a na pinech pro cteni tlacitek INPUT
  DDRC |= 0b00000111; 
  PORTC = 0b00000011;

  uart0_init(UART_BAUD_SELECT(115200, 16000000L)); // Nastaveni baud rate UART
  writeKeymap(&keyboard); 
}

void loop() // Hlavni cyklus programu
{
  readKeyboard(&keyboard); // Precte keyboard
  processOutput(&keyboard); // Zpracuje vystup a odesle prislusne MIDI zpravy
}

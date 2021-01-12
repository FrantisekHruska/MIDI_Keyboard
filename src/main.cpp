#include <Arduino.h>
#include <Keypad.h>
#include <MIDI.h>
#include "keyreader.h"
#include "uart.h"

uint8_t keymap[0xff];

void sendMIDI(uint8_t status, uint8_t key, uint8_t velocity = 127){
  switch (status){
    case 1:
      uart0_putc(0x9c);
      break;
    case 0:
      uart0_putc(0x8c);
      break;
    default:
      return;
  }

}


char keymap[ROWS][COLUMNS];
uint8_t row_pins[ROWS] = {25, 24};
uint8_t col_pins[COLUMNS] = {53, 52};

Keypad keyboard_matrix = Keypad(makeKeymap(keymap), row_pins, col_pins, ROWS, COLUMNS);

struct MySettings : public midi::DefaultSerialSettings
{
  static const long BaudRate = 9600;
};

MIDI_CREATE_CUSTOM_INSTANCE(HardwareSerial, Serial, MIDI, MySettings);

const uint8_t OCTAVE = 12;
uint8_t TRANSPOSE = 0;

void sendNotes()
{
  for (int i = 0; i < LIST_MAX; i++)
  {
    if (keyboard_matrix.key[i].stateChanged)
    {
      switch (keyboard_matrix.key[i].kstate)
      {
      case PRESSED:
        switch (keyboard_matrix.key[i].kchar)
        {
        case 'A':
          MIDI.sendNoteOn(60 , 127, 1);
          break;
        case 'B':
          MIDI.sendNoteOn(61, 127, 1);
          break;
        case 'C':
          MIDI.sendNoteOn(62, 127, 1);
          break;
        case 'D':
          MIDI.sendNoteOn(63, 127, 1);
          break;
        default:
          break;
        }
        break;
      case RELEASED:
        switch (keyboard_matrix.key[i].kchar)
        {
        case 'A':
          MIDI.sendNoteOff(60 , 0, 1);
          break;
        case 'B':
          MIDI.sendNoteOff(61, 0, 1);
          break;
        case 'C':
          MIDI.sendNoteOff(62, 0, 1);
          break;
        case 'D':
          MIDI.sendNoteOff(63, 0, 1);
          break;
        default:
          break;
        }
        break;
      case IDLE:
      case HOLD:
        break;
      }
    }
  }
}



void setup()
{

  uart0_init(31250);
}

void loop()
{
  // returns True if any key is pressed
  // fills 'key' array with keystates
  if (keyboard_matrix.getKeys())
  {
    sendNotes();
  }

}

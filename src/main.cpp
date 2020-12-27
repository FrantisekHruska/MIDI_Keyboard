#include <Arduino.h>
#include <Keypad.h>
#include <MIDI.h>

const uint8_t ROWS = 2;
const uint8_t COLUMNS = 2;

char keymap[ROWS][COLUMNS] = {{'A', 'B'}, {'C', 'D'}};
uint8_t row_pins[ROWS] = {25, 24};
uint8_t col_pins[COLUMNS] = {53, 52};

Keypad keyboard_matrix = Keypad(makeKeymap(keymap), row_pins, col_pins, ROWS, COLUMNS);

MIDI_CREATE_DEFAULT_INSTANCE();

void setup()
{
  Serial.begin(9600);
  // MIDI.begin(1);
}

void loop()
{
  // returns True if any key is pressed
  // fills 'key' array with keystates
  if (keyboard_matrix.getKeys())
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
            MIDI.sendNoteOn(60, 127, 1);
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
            MIDI.sendNoteOff(60, 0, 1);
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
}
#include <Arduino.h>
// #include <Keypad.h>
// #include <MIDI.h>
#include "keyreader.h"
extern "C"{
  #include <uart.h>
}

uint8_t keymap[0xff];
struct Keyboard keyboard;


// const uint8_t ROWS = 5;
// const uint8_t COLUMNS = 5;


// char keymap[ROWS][COLUMNS] = {
//     {'A', 'B', 'C', 'D', 'E'},
//     {'F', 'G', 'H', 'I', 'J'},
//     {'K', 'L', 'M', 'N', 'O'},
//     {'P', 'Q', 'R', 'S', 'T'},
//     {'U', 'V', 'W', 'X', 'Y'}};

// uint8_t row_pins[ROWS] = {53, 51, 49, 47, 45};
// uint8_t col_pins[COLUMNS] = {37, 39, 43, 35, 41};

// Keypad keyboard_matrix = Keypad(makeKeymap(keymap), row_pins, col_pins, ROWS, COLUMNS);

// struct MySettings : public midi::DefaultSerialSettings
// {
//   static const long BaudRate = 9600;
// };

// MIDI_CREATE_CUSTOM_INSTANCE(HardwareSerial, Serial, MIDI, MySettings);

// const uint8_t OCTAVE = 12;
// uint8_t TRANSPOSE = 0;

// void sendNotes()
// {
//   for (int i = 0; i < LIST_MAX; i++)
//   {
//     if (keyboard_matrix.key[i].stateChanged)
//     {
//       switch (keyboard_matrix.key[i].kstate)
//       {
//       case PRESSED:
//         switch (keyboard_matrix.key[i].kchar)
//         {
//         case 'A':
//           MIDI.sendNoteOn(60, 127, 1);
//           break;
//         case 'B':
//           MIDI.sendNoteOn(61, 127, 1);
//           break;
//         case 'C':
//           MIDI.sendNoteOn(62, 127, 1);
//           break;
//         case 'D':
//           MIDI.sendNoteOn(63, 127, 1);
//           break;
//         default:
//           break;
//         }
//         break;
//       case RELEASED:
//         switch (keyboard_matrix.key[i].kchar)
//         {
//         case 'A':
//           MIDI.sendNoteOff(60, 0, 1);
//           break;
//         case 'B':
//           MIDI.sendNoteOff(61, 0, 1);
//           break;
//         case 'C':
//           MIDI.sendNoteOff(62, 0, 1);
//           break;
//         case 'D':
//           MIDI.sendNoteOff(63, 0, 1);
//           break;
//         default:
//           break;
//         }
//         break;
//       case IDLE:
//       case HOLD:
//         break;
//       }
//     }
//   }
// }
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
  uart0_putc(key);
  uart0_putc(velocity);
}

int processOutput(struct Keyboard* _keyboard){
  uint8_t temp = 0;
  for (uint8_t i = 0; i < COLUMNS; i++){
    temp = _keyboard->output[i];

    for (uint8_t j = 0; j < ROWS; j++){
      sendMIDI(temp & 0x01, keymap[(j << 4)|i], 127);
      temp = temp >> 1;
    }
  }
}

void setup()
{
  DDRC = 0x00;
  DDRA = 0xff;
  
  uart0_init(UART_BAUD_SELECT(9600, 16000000L));

  keyboard.size = (ROWS << 4) | COLUMNS;
  
  uint8_t tone = 60;
  for (uint8_t i = 0; i < COLUMNS; i++){
    for (uint8_t j = 0; j < ROWS; j++){
      keymap[(j << 4)|i] = tone++;

    }
  }
}

void loop()
{
  // returns True if any key is pressed
  // fills 'key' array with keystates
  // if (keyboard_matrix.getKeys())
  // {
  //   sendNotes();
  // }
  // uart0_putc(0x3c);
  readKeyboard(&keyboard);
  processOutput(&keyboard);
}

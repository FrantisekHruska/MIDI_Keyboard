#include <Arduino.h>
#include <MIDI.h>

// PINS Array Setup
byte BUTTON_PIN[3] = {52, 50, 48};
byte LED_PIN[3] = {3, 2, 4};
byte BUTTON_COUNT = sizeof(BUTTON_PIN) / sizeof(BUTTON_PIN[0]);

MIDI_CREATE_DEFAULT_INSTANCE();

void setup()
{
  Serial.begin(9600);
  for (int i : BUTTON_PIN)
  {
    pinMode(i, INPUT_PULLUP);
  }

  for (int i : LED_PIN)
  {
    pinMode(i, OUTPUT);
  }
}

void loop()
{
  for (int i = 0; i < BUTTON_COUNT; i++)
  {
    digitalWrite(LED_PIN[i], LOW);
  }
  
  for (int i = 0; i < BUTTON_COUNT; i++)
  {
    if (digitalRead(BUTTON_PIN[i]))
    {
      // MIDI.sendNoteOn(60, 127, 1);
      digitalWrite(LED_PIN[i], HIGH);
    }
  }
}
#include <Arduino.h>
extern "C"
{
#include <uart.h>
}

#define CHANNEL 0
#define OCTAVE 12

/*Tato knihovna obsahuje vsechny MIDI zpravy pouzite v keyboardu*/

void sendNoteOff(uint8_t channel, uint8_t key, uint8_t velocity = 64)
{
    uart0_putc(0x80 | channel);
    uart0_putc(0x00 | key);
    uart0_putc(0x00 | velocity);
}
void sendNoteOn(uint8_t channel, uint8_t key, uint8_t velocity = 64)
{
    uart0_putc(0x90 | channel);
    uart0_putc(0x00 | key);
    uart0_putc(0x00 | velocity);
}
void allNotesOff(uint8_t channel)
{
    uart0_putc(0xB0 | channel);
    uart0_putc(0x7B);
    uart0_putc(0x00);
}
void sendSustainOn(uint8_t channel)
{
    uart0_putc(0xB0 | channel);
    uart0_putc(0x40);
    uart0_putc(0x40);
}
void sendSustainOff(uint8_t channel)
{
    uart0_putc(0xB0 | channel);
    uart0_putc(0x40);
    uart0_putc(0x3F);
}
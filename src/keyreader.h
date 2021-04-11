#include <Arduino.h>
#include "messages.h"

/* Tato knihovna obsahuje funkce dulezite pro cteni stavu keyboardu */

#define COLUMNS 5 // Definuje pocet sloupcu matice
#define ROWS 5 // Definuje pocet radku matice

// Hlavni struktura keyboardu obsahujici promene a pole se kterymi program pracuje
struct Keyboard
{
    uint8_t keymap[0xff]; // Pole pro mapu, podle ktere program urcuje jaky ton je na jake klavese 
    // 0xRADEK SLOUPEC (0xROWS COLUMNS)

    // Dve pole na ukladani stareho a noveho stavu tlacitek matice
    uint8_t output_1[ROWS];
    uint8_t output_2[ROWS];

    // Pomocna promena regs 
    uint8_t regs = 0x00;

    // Pomocne promene pro transpose a sustain tlacitka
    int8_t transpose = 0;
    uint8_t transpose_buttons_state = 0x00;
    uint8_t sustain_button_state = 0x00;
};

// Tato funkce zapise do keymapu tony
void writeKeymap(struct Keyboard *_keyboard)
{
    // Premapovani radku kvuli zapojeni
    const uint8_t rowmap[ROWS] = {4, 0, 1, 2, 3};

    // Vytvori zakladni ton (60) a posune ho o tolik oktav kolik je definovano v promene transpose
    uint8_t tone = 60 + (_keyboard->transpose * OCTAVE);

    // Tento cyklus zapise ton na prislusny radek a sloupec a zvysi ton o 1 v kazde iteraci
    for (uint8_t i = 0; i < ROWS; i++)
    {
        for (uint8_t j = 0; j < COLUMNS; j++)
        {
            _keyboard->keymap[(rowmap[i] << 4) | j] = tone++;
        }
    }
    return;
}

// Tyto dve funcke vraci pri kazdem precteni jine pole v zavislosti na regs
uint8_t *getArray(struct Keyboard *_keyboard)
{
    if (_keyboard->regs & 0x01)
        return _keyboard->output_2;
    else
        return _keyboard->output_1;
}
uint8_t *getArrayOld(struct Keyboard *_keyboard)
{
    if (_keyboard->regs & 0x01)
        return _keyboard->output_1;
    else
        return _keyboard->output_2;
}

// funkce ktera invertuje promenou regs ktera urcuje do ktereho pole ulozit
void switchArray(struct Keyboard *_keyboard)
{
    _keyboard->regs = _keyboard->regs ^ 0x01;
    return;
}

// Najde co se zmenilo z 1 na 0 v radku stareho a noveho polem pomoci logicke funkce
uint8_t getOff(struct Keyboard *_keyboard, uint8_t rownum)
{
    return getArrayOld(_keyboard)[rownum] & (getArray(_keyboard)[rownum] ^ 0xff);
}

// Najde co se zmenilo z 1 na 0 v radku stareho a noveho polem pomoci logicke funkce
uint8_t getOn(struct Keyboard *_keyboard, uint8_t rownum)
{
    return getArray(_keyboard)[rownum] & (getArrayOld(_keyboard)[rownum] ^ 0xff);
}

// Funkce limituje transpozici aby neprekrocila limit
void limitTranspose(struct Keyboard *_keyboard, int8_t lowerLimit, int8_t upperLimit)
{
    if (_keyboard->transpose > upperLimit)
        _keyboard->transpose = upperLimit;
    if (_keyboard->transpose < lowerLimit)
        _keyboard->transpose = lowerLimit;
    return;
}

void writeTransposeLED(struct Keyboard *_keyboard)
{
    if (_keyboard->transpose > 0)
    {
        PORTC &= 0b00000101;
        PORTC |= 0b00000001;
    }
    else if (_keyboard->transpose < 0)
    {
        PORTC &= 0b00000110;
        PORTC |= 0b00000010;
    }
    else
    {
        PORTC &= 0b00000111;
        PORTC |= 0b00000011;
    }
    return;
}

// Cte tlacitka na transpozici
void readTranspose(struct Keyboard *_keyboard)
{
    _keyboard->transpose_buttons_state |= ((PINC & 0b00011000) >> 3);

    if (((_keyboard->transpose_buttons_state ^ 0xff) & 0x01) & ((_keyboard->transpose_buttons_state >> 2) & 0x01))
    {
        allNotesOff(CHANNEL);
        _keyboard->transpose++;
    }
    if (((_keyboard->transpose_buttons_state ^ 0xff) & 0x02) & ((_keyboard->transpose_buttons_state >> 2) & 0x02))
    {
        allNotesOff(CHANNEL);
        _keyboard->transpose--;
    }
    writeTransposeLED(_keyboard);
    _delay_us(150);
    writeKeymap(_keyboard);
    limitTranspose(_keyboard, -5, 4);

    _keyboard->transpose_buttons_state <<= 2;
    _keyboard->transpose_buttons_state &= 0b00001111;
    return;
}

void readSustain(struct Keyboard *_keyboard)
{
    _keyboard->sustain_button_state += ((PINC & 0b00100000) >> 5);

    if ((((_keyboard->sustain_button_state >> 1) ^ 0xff) & 0x01) & ((_keyboard->sustain_button_state) & 0x01))
    {
        sendSustainOff(CHANNEL);
        PORTC &= 0b00000011;
    }
    if ((((_keyboard->sustain_button_state >> 1)) & 0x01) & (((_keyboard->sustain_button_state) ^ 0xff) & 0x01))
    {
        sendSustainOn(CHANNEL);
        PORTC |= 0b00000100;
    }
    _keyboard->sustain_button_state <<= 1;
    _keyboard->sustain_button_state &= 0b00000011;
    return;
}

// Funkce na cteni keyboardu
void readKeyboard(struct Keyboard *_keyboard)
{
    // Prectu stav tlacitek transpozice
    readTranspose(_keyboard);
    // Prectu stav sustain tlacitka
    readSustain(_keyboard);
    // Zmenim pole do ktereho ukladam
    switchArray(_keyboard);
    // Cyklus ktery cte vystup na PINB a ulozi do pole
    for (uint8_t i = 0; i < COLUMNS; i++)
    {
        DDRD = (1 << i) << 3; // Nastavim jeden radek jako OUTPUT LOW

        _delay_us(150); // debounce delay

        getArray(_keyboard)[i] = ((PINB & 0b00011111) ^ 0xff); // Prectu co je na PINB a ulozim do pole ktere je zrovna aktivni

        DDRD = 0; // Nastavim radek jako INPUT
        PORTD = 0;
    }
    return;
}

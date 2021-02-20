#include <Arduino.h>

#define COLUMNS 5
#define ROWS 5

#define OCTAVE 12

// Hlavni struktura obsahujici promene a pole se kterymi program pracuje
struct Keyboard
{
    // 0xRADEK SLOUPEC (0xROWS COLUMNS)

    // Dve pole na ukladani stavu matice
    uint8_t output_1[ROWS];
    uint8_t output_2[ROWS];

    // Promena regs ktera se meni z nuly na jednicku a tim meni do jakeho pole se uklada prectena matice (ouput_1 output_2)
    uint8_t regs = 0x00;

    int8_t transpose = 0;
    uint8_t transpose_buttons_state = 0x00;
};

// Tyto dve funcke vraci pole pri kazdem precteni jine pole v zavislosti na regs
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

// Najde co se zmenilo z 0 na 1 pomoci logicke funkce
uint8_t getOn(struct Keyboard *_keyboard, uint8_t rownum)
{
    return getArrayOld(_keyboard)[rownum] & (getArray(_keyboard)[rownum] ^ 0xff);
}

// Najde co se zmenilo z 1 na 0 pomoci logicke funkce
uint8_t getOff(struct Keyboard *_keyboard, uint8_t rownum)
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
}

// Cte tlacitka na transpozici
void readTranspose(struct Keyboard *_keyboard)
{
    _keyboard->transpose_buttons_state += (PINB & 0b00000011);

    if ((_keyboard->transpose_buttons_state & 0x01) != ((_keyboard->transpose_buttons_state >> 2) & 0x01) || (_keyboard->transpose_buttons_state & 0x02) != ((_keyboard->transpose_buttons_state >> 2) & 0x02))
    {
        // _delay_us(150);
        if (_keyboard->transpose_buttons_state & 0x01)
            _keyboard->transpose--;

        if ((_keyboard->transpose_buttons_state & 0x02) >> 1)
            _keyboard->transpose++;

        // writeKeymap();
    }
    _keyboard->transpose_buttons_state <<= 2;
    _keyboard->transpose_buttons_state &= 0b00001111;
    limitTranspose(_keyboard, -5, 4);
}

// Funkce na cteni keyboardu
void readKeyboard(struct Keyboard *_keyboard)
{
    // Prectu transpozici
    readTranspose(_keyboard);
    // Zmenim pole do ktereho ukladam
    switchArray(_keyboard);
    // Cyklus ktery cte vystup na PINA a ulozi do pole
    for (uint8_t i = 0; i < COLUMNS; i++)
    {

        DDRC = 1 << i; // Nastavim jeden radek jako OUTPUT LOW
        // PORTC = 0;

        _delay_us(150); // debounce delay

        getArray(_keyboard)[i] = PINA ^ 0xff; // Prectu co je na PINA a ulozim do pole ktere je zrovna aktivni podle promene regs
        // PORTC = 1 << i;

        DDRC = 0; // Nastavim radek jako INPUT
        PORTC = 0;
    }
}

#include <Arduino.h>

#define COLUMNS 5
#define ROWS 5

#define OCTAVE 12

// Hlavni struktura obsahujici promene a pole se kterymi program pracuje
struct Keyboard
{
    uint8_t keymap[0xff];
    // 0xRADEK SLOUPEC (0xROWS COLUMNS)

    // Dve pole na ukladani dvou stavu matice
    uint8_t output_1[ROWS];
    uint8_t output_2[ROWS];

    // Promena regs ktera se meni z nuly na jednicku a tim meni do jakeho pole se uklada prectena matice (ouput_1 output_2)
    uint8_t regs = 0x00;

    int8_t transpose = 0;
    uint8_t transpose_buttons_state = 0x00;
};

// Tato funkce zapise do keymapu tony
void writeKeymap(struct Keyboard *_keyboard)
{
    // Premapovani radku kvuli zapojeni
    const uint8_t rowmap[ROWS] = {4, 0, 1, 2, 3};

    // Vytvori zakladni ton
    // posune ho o tolik oktav kolikrat byla zmacknuta transpozice
    uint8_t tone = 60 + (_keyboard->transpose * OCTAVE);

    // Zapise na prislusny radek a sloupec ton a zvysi ho o 1
    for (uint8_t i = 0; i < ROWS; i++)
    {
        for (uint8_t j = 0; j < COLUMNS; j++)
        {
            _keyboard->keymap[(rowmap[i] << 4) | ((COLUMNS - 1) - j)] = tone++;
        }
    }
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
    _keyboard->transpose_buttons_state += ((PINC & 0b00011000) >> 3);

    if ((_keyboard->transpose_buttons_state & 0x01) != ((_keyboard->transpose_buttons_state >> 2) & 0x01) || (_keyboard->transpose_buttons_state & 0x02) != ((_keyboard->transpose_buttons_state >> 2) & 0x02))
    {
        if (_keyboard->transpose_buttons_state & 0x01)
            _keyboard->transpose--;
        _delay_us(150);

        if ((_keyboard->transpose_buttons_state & 0x02) >> 1)
            _keyboard->transpose++;
            
        if (!_keyboard->transpose)
        {
            PORTC = 0b00000011;
        }
        else if (_keyboard->transpose < 0)
        {
            PORTC = 0b00000010;
        }
        else
        {
            PORTC = 0b00000001;
        }
        writeKeymap(_keyboard);
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

        DDRD = (1 << i) << 3; // Nastavim jeden radek jako OUTPUT LOW
        // PORTC = 0;

        _delay_us(150); // debounce delay

        getArray(_keyboard)[i] = ((PINB & 0b00011111) ^ 0xff); // Prectu co je na PINA a ulozim do pole ktere je zrovna aktivni podle promene regs
        // PORTC = 1 << i;

        DDRD = 0; // Nastavim radek jako INPUT
        PORTD = 0;
    }
}

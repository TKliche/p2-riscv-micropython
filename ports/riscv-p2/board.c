#include <stdint.h>
#include <stdio.h>

#include <propeller2.h>
#include "board.h"

void pin_init(void) {
}

void pin_state(int pin, unsigned state) {
    _pinw(pin, state);
}

void pin_toggle(int pin)
{
    _pinnot(pin);
}

void pin_setdir(int pin, unsigned direction)
{
    if (direction) {
        _dirh(pin);
    } else {
        _dirl(pin);
    }
}

void pin_setxval(int pin, unsigned xval)
{
    _wxpin(pin, xval);
}

void pin_setyval(int pin, unsigned yval)
{
    _wypin(pin, yval);
}

void pin_setmode(int pin, unsigned mode)
{
    _wrpin(pin, mode);
}

unsigned pin_getzval(int pin)
{
    return _rdpin(pin);
}

unsigned pin_read(int pin)
{
    return _pin(pin);
}

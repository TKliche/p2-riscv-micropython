#include <stdint.h>
#include <stdio.h>

#include "board.h"
#include "riscv.h"

void pin_init(void) {
}

void pin_state(int pin, unsigned state) {
    setpin(pin, state);
}

void pin_toggle(int pin)
{
    togglepin(pin);
}

void pin_setdir(int pin, unsigned direction)
{
    if (direction) {
        dirh_(pin);
    } else {
        dirl_(pin);
    }
}

void pin_setxval(int pin, unsigned xval)
{
    pinwx(pin, xval);
}

void pin_setyval(int pin, unsigned yval)
{
    pinwy(pin, yval);
}

void pin_setmode(int pin, unsigned mode)
{
    pinwr(pin, mode);
}

unsigned pin_getzval(int pin)
{
    return pinrdr(pin);
}

unsigned pin_read(int pin)
{
    return getpin(pin);
}

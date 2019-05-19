#ifndef MICROPY_INCLUDED_UPDUINO_BOARD_H
#define MICROPY_INCLUDED_UPDUINO_BOARD_H

void pin_init(void);
void pin_state(int pin, unsigned state);
void pin_toggle(int pin);

void pin_setdir(int pin, unsigned direction);
void pin_setxval(int pin, unsigned xval);
void pin_setyval(int pin, unsigned yval);
void pin_setmode(int pin, unsigned mode);

unsigned pin_getzval(int pin);
unsigned pin_read(int pin);

#endif // MICROPY_INCLUDED_UPDUINO_BOARD_H

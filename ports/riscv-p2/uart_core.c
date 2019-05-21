#include <stdio.h>
#include <unistd.h>
#include "py/mpconfig.h"
#include "vgatext.h"
#include "OneCogKbM.h"
#include "riscv.h"

#define VGA_BASEPIN 48

/*
 * Core UART functions to implement for a port
 */

extern int _getbyte();
extern int _putbyte(int c);
extern unsigned int _getcnt();
extern void _waitcnt(unsigned int n);

vgatext vga;
OneCogKbM usb1;
static volatile uint8_t usb1_status[4];
static int32_t usb1_eventa;

void mp_hal_io_init(void) {
    int cog;
    usb1_status[0] = usb1_status[1] = usb1_status[2] = usb1_status[3] = 0;
    vgatext_start(&vga, VGA_BASEPIN);
    OneCogKbM_start(&usb1, (int32_t)&usb1_status); // FIXME: not working yet
    cog = usb1_status[0] - 1;
    if (cog >= 0) {
        usb1_eventa = usb1_status[1];
        printf("started USB on cog %d\n", cog);
    } else {
        printf("USB not started\n");
    }
}

static void usb_event(int event)
{
    switch(event) {
    case ONECOGKBM_DEV_DISCONNECT:
        usb1_status[2] = usb1_status[3] = 0;
        break;
    case ONECOGKBM_KB_READY:
    case ONECOGKBM_KBM_READY:
        usb1_status[2] = 1;
        break;
    default:
        break;
    }
}

int getrawbyte() {
    int ci;
    int event;
    ci = _getbyte();
    if (ci < 0) {
        event = getpin(usb1_eventa);
        if (event) {
            event = pinrdr(usb1_eventa);
            usb_event(event);
        }
        if (usb1_status[2] != 0) {
            ci = OneCogKbM_key() & 0xff;
            if (ci == 0) {
                ci = -1;
            }
        }
    }
    if (ci <= 0) {
        ci = -1;
    } else {
        ci = ci & 0xff;
    }
    return ci;
}

// Receive single character
int mp_hal_stdin_rx_chr(void) {
    unsigned char c;
    int32_t flip = 0;
    
#if MICROPY_MIN_USE_STDOUT
    int r = read(0, &c, 1);
    (void)r;
#else
    int ci;
    do {
        ci = getrawbyte();
        if (ci < 0 && !flip) {
            vgatext_invertcurchar(&vga);
            flip = 1;
        }
    } while (ci < 0);
    if (flip) {
        vgatext_invertcurchar(&vga);
    }
    c = ci;
#endif
    return c;
}

void _pausems(unsigned numms)
{
    unsigned cycle = _getcnt() + numms * 160000;
    _waitcnt(cycle);
}

// Send string of given length
void mp_hal_stdout_tx_strn(const char *str, mp_uint_t len) {
#if MICROPY_MIN_USE_STDOUT
    int r = write(1, str, len);
    (void)r;
#else
    unsigned int c;
    unsigned maxlen = len;
//    if (maxlen > 256) maxlen = 256;
    while (maxlen--) {
        c = (*str++) & 0xff;
        _putbyte(c);
        vgatext_tx(&vga, c);
    }
//    _pausems(500);
#endif
}

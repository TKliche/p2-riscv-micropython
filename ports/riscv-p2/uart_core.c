#include <stdio.h>
#include <unistd.h>
#include "py/mpconfig.h"
#include "vgatext.h"
#include "OneCogKbM.h"
#include "BufferSerial.h"
#include <propeller2.h>

#define VGA_BASEPIN 48

/*
 * Core UART functions to implement for a port
 */

extern int p2_getbyte();
extern void p2_putbyte(int c);

vgatext vga;
OneCogKbM usb1;
BufferSerial ser1;
static volatile uint8_t usb1_status[4];
static int32_t usb1_eventa;

void mp_hal_io_init(void) {
    int cog;
    usb1_status[0] = usb1_status[1] = usb1_status[2] = usb1_status[3] = 0;
    vgatext_start(&vga, VGA_BASEPIN);
    OneCogKbM_start(&usb1, (int32_t)&usb1_status);
    cog = usb1_status[0] - 1;
    if (cog >= 0) {
        usb1_eventa = usb1_status[1];
        printf("started USB on cog %d\n", cog);
    } else {
        printf("USB not started\n");
    }
    BufferSerial_start(&ser1);
}

#if 0
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
#endif

static int getrawbyte();

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
            vgatext_showcursor(&vga);
            flip = 1;
        }
    } while (ci < 0);
    if (flip) {
        vgatext_hidecursor(&vga);
    }
    c = ci;
#endif
    return c;
}

#define ESC "\x1b"

/* table organized by scan codes */
static const char *escape_sequences[] = {
    0, 0, 0, 0,  
    0, 0, 0, 0,    /* A-D */
    0, 0, 0, 0, 0, 0, 0, 0, /* E-L */
    0, 0, 0, 0, 0, 0, 0, 0, /* M-T */
    0, 0, 0, 0, 0, 0, 0, 0, /* U-2 */
    0, 0, 0, 0, 0, 0, 0, 0, /* 3-0 */
    0, 0, 0, 0, 0, 0, 0, 0, /* Enter Esc BkSpc Tab Spc _ + { */
    0, 0, 0, 0, 0, 0, 0, 0, /* more symbols */
    0, 0, /* ? CapsLock */
    ESC "OP", ESC "OQ", ESC "OR", ESC "OS", /* F1-F4 */
    ESC "[15~", ESC "[17~", ESC "[18~", ESC "[19~", /* F5-F8 */
    ESC "[20~", ESC "[21~",  /* F9-F10 */
    ESC "[23~", ESC "[24~", 0, 0, /* F11-F12 PrtSc ScrLk */
    0, 0, ESC "[H", ESC "[5~",  /* Pause Ins Home PgUp */
    0, ESC "[F", ESC "[6~", ESC "[C",  /* Del End PgDn Right */
    ESC "[D", ESC "[B", ESC "[A", 0,  /* Left Down Up KpdNumLck */
    0, 0, 0, 0,  /*Kp/ Kp* Kp- Kp+ */
    0, 0, 0, 0, 0, 0, 0, 0, /* more keypad */
    0, 0, 0, 0, 0, 0, 0, 0  /* more keypad */
};

static const char *kbd_stuff = NULL;

/* check for need to generate a special escape sequence */
static int checkscancode(unsigned scancode)
{
    if (scancode > 0x65) {
        return 0;
    }
    kbd_stuff = escape_sequences[scancode];
    return kbd_stuff != 0;
}

static int getrawbyte() {
    int ci;
    int event;
    int scan;
    
    if (kbd_stuff) {
        ci = *kbd_stuff++;
        if (ci) {
            return ci;
        } else {
            kbd_stuff = 0;
        }
    }
    event = ser1.data;
    if (event != 0) {
        ser1.data = 0;
        ci = event & 0xff;
    } else {
        event = OneCogKbM_key();
        if (event != 0) {
            ci = event & 0xff;
            scan = (event>>8) & 0xff;
            if (checkscancode(scan)) {
                ci = *kbd_stuff++;
            }
            if (ci == 0) {
                ci = -1;
            }
            else if (event & 0x010000) {
                ci = ci & 0x1f;  // control key
            }
        } else {
            ci = -1;
        }
    }
    return ci;
}

void _pausems(unsigned numms)
{
    unsigned cycles_per_millis = _clockfreq() / 1000;
    unsigned cycles = numms * cycles_per_millis;
    _waitx(cycles);
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
#if 0        
        if (c < 0x20) {
            p2_putbyte('^');
            c += '@';
        }
#endif        
        p2_putbyte(c);
        vgatext_tx(&vga, c);
    }
//    _pausems(500);
#endif
}

void
p2_putbyte(int c)
{
    _csr_write(_UART_CSR, c);
}

uint64_t
p2_getcntll()
{
    uint32_t hi1, lo, hi2;
    do {
        hi1 = _cnth();
        lo = _cnt();
        hi2 = _cnth();
    } while (hi1 != hi2);
    return lo | (((uint64_t)hi1) << 32LL);
}

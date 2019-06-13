# Overview

This is a basic port of MicroPython to the Parallax P2 Eval board.

To run it, load the upython.binary file into the P2 board. It will
talk on the standard serial line at 230400 baud. It will also try to
talk to a VGA board based at pin 48 and a USB keyboard based at pin 16
(using the standard P2 eval A/V and serial host expansion boards). The
USB driver used is garryj's excellent single COG keyboard/mouse driver
(although I haven't implemented mouse support yet).

## Pins

There is a P2 PIN class supported in the standard pyb module. You can
toggle pin 56 on the P2 board, for example, with:
```
>>> import pyb
>>> p = pyb.Pin(56)
>>> p.off()
>>> p.toggle()
```
(The LEDs on the P2 board are active low, weirdly enough, so p.off()
actually turns the LED on by pulling the pin low.)

The simple methods are:
```
  p.on()   # drives pin high
  p.off()  # drives pin low
  p.toggle() # toggles pin
  p.read() # reads input value of pin, returns 0 or 1
```

For dealing with smartpins there are several more methods:
```
p.makeinput()   # makes the pin an input, do this before setting up smartpin
p.mode(0x4c)    # set to NCO frequency mode
p.xval(16000)   # set bit period
p.yval(858993)  # set increment
p.makeoutput()  # enables smartpin
p.readzval()    # reads the Z value (input) of the smartpin
```
Note that p.makeinput() is implied by p.read(), and p.makeoutput() is
implied by p.on(), p.off(), and p.toggle(), so the makeinput() and
makeoutput() methods are only really needed for smart pin manipulation

Also note that this version supports long integers, so effectively the
smart pin registers are 32 bit unsigned values. Only the lower 32 bits
of any value passed to them are used. For example:
```
import pyb
p=pyb.Pin(1)
p.mode(2)
p.makeoutput()
p.xval(-1)
hex(p.readzval())
'0xffffffff'
```
## Timing

pyb also has millis() and micros() methods to return current
milliseconds and microseconds since boot. These use a 64 bit cycle
timer, so they do not roll over nearly as quickly as they used to.

## SD Card

There is a standard SDCard type implemented in the `pyb` module. If an
SD card is inserted when micropython is started up, it is
automatically mounted.

The `os` module is implemented, so you can get a listing of the SD
card contents via:
```
import os
os.listdir()
```

To mount an SD card after boot, do:
```
import pyb
import os
sd=pyb.SDCard()
os.mount(sd, '/sd')
os.chdir('/sd')
```

## Other Notes

### Performance

```
import pyb
def perfTest():
  millis = pyb.millis
  endTime = millis() + 10000
  count = 0
  while millis() < endTime:
    count += 1
  print("Count: ", count)


p2.binary:    53057
p2emu.binary: 29742
p2trace:      96691
```

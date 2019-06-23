#include "propeller.h"

unsigned int _getclockfreq();
void _waitcnt(unsigned int);
uint64_t _getcntll();
extern unsigned int _getcnt();

static inline mp_uint_t mp_hal_ticks_ms(void) {
#ifdef OLD    
    unsigned int freq = _getclockfreq() / 1000;
    uint64_t now = _getcntll();
    return (uint32_t) (now / freq);
#else
    return getmillis();
#endif    
}
static inline mp_uint_t mp_hal_ticks_us(void) {
    unsigned int freq = _getclockfreq() / 1000000;
    uint64_t now = _getcntll();
    return (uint32_t)(now / freq);
}
static inline void mp_hal_delay_ms(mp_uint_t ms) {
    unsigned int freq = _getclockfreq();
    unsigned int delay = ms * (freq / 1000);
    _waitcnt(_getcnt() + delay);
}
static inline void mp_hal_set_interrupt_char(char c) {}

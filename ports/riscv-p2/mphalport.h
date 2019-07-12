#include <propeller2.h>

extern uint64_t p2_getcntll();

static inline mp_uint_t mp_hal_ticks_ms(void) {
#ifdef OLD    
    unsigned int freq = _getclockfreq() / 1000;
    uint64_t now = _getcntll();
    return (uint32_t) (now / freq);
#else
    return _csr_read(_MILLIS_CSR);
#endif    
}
static inline mp_uint_t mp_hal_ticks_us(void) {
    unsigned int freq = _clockfreq() / 1000000;
    uint64_t now = p2_getcntll();
    return (uint32_t)(now / freq);
}
static inline void mp_hal_delay_ms(mp_uint_t ms) {
    unsigned int freq = _clockfreq();
    unsigned int delay = ms * (freq / 1000);
    _waitx(delay);
}
static inline void mp_hal_set_interrupt_char(char c) {}

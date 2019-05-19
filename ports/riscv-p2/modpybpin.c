/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Damien P. George
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "py/runtime.h"
#include "board.h"
#include "modpyb.h"

typedef struct _pyb_pin_obj_t {
    mp_obj_base_t base;
    mp_uint_t xval;
    mp_uint_t yval;
    mp_uint_t mode;
} pyb_pin_obj_t;

STATIC const pyb_pin_obj_t pyb_pin_obj[] = {
    {{&pyb_pin_type}},
    {{&pyb_pin_type}},
    {{&pyb_pin_type}},
    {{&pyb_pin_type}},
    {{&pyb_pin_type}},
    {{&pyb_pin_type}},
    {{&pyb_pin_type}},
    {{&pyb_pin_type}},
    
    {{&pyb_pin_type}},
    {{&pyb_pin_type}},
    {{&pyb_pin_type}},
    {{&pyb_pin_type}},
    {{&pyb_pin_type}},
    {{&pyb_pin_type}},
    {{&pyb_pin_type}},
    {{&pyb_pin_type}},

    {{&pyb_pin_type}},
    {{&pyb_pin_type}},
    {{&pyb_pin_type}},
    {{&pyb_pin_type}},    
    {{&pyb_pin_type}},
    {{&pyb_pin_type}},
    {{&pyb_pin_type}},
    {{&pyb_pin_type}},
    
    {{&pyb_pin_type}},
    {{&pyb_pin_type}},
    {{&pyb_pin_type}},
    {{&pyb_pin_type}},
    {{&pyb_pin_type}},
    {{&pyb_pin_type}},
    {{&pyb_pin_type}},
    {{&pyb_pin_type}},
////
    {{&pyb_pin_type}},
    {{&pyb_pin_type}},
    {{&pyb_pin_type}},
    {{&pyb_pin_type}},
    {{&pyb_pin_type}},
    {{&pyb_pin_type}},
    {{&pyb_pin_type}},
    {{&pyb_pin_type}},
    
    {{&pyb_pin_type}},
    {{&pyb_pin_type}},
    {{&pyb_pin_type}},
    {{&pyb_pin_type}},
    {{&pyb_pin_type}},
    {{&pyb_pin_type}},
    {{&pyb_pin_type}},
    {{&pyb_pin_type}},

    {{&pyb_pin_type}},
    {{&pyb_pin_type}},
    {{&pyb_pin_type}},
    {{&pyb_pin_type}},    
    {{&pyb_pin_type}},
    {{&pyb_pin_type}},
    {{&pyb_pin_type}},
    {{&pyb_pin_type}},
    
    {{&pyb_pin_type}},
    {{&pyb_pin_type}},
    {{&pyb_pin_type}},
    {{&pyb_pin_type}},
    {{&pyb_pin_type}},
    {{&pyb_pin_type}},
    {{&pyb_pin_type}},
    {{&pyb_pin_type}},
};

#define NUM_PIN MP_ARRAY_SIZE(pyb_pin_obj)
#define PIN_ID(obj) ((obj) - &pyb_pin_obj[0])

void pyb_pin_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    pyb_pin_obj_t *self = self_in;
    mp_printf(print, "PIN(%u)", PIN_ID(self));
}

STATIC mp_obj_t pyb_pin_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, 1, false);
    mp_int_t pin_id = mp_obj_get_int(args[0]);
    if (!(0 <= pin_id && pin_id < NUM_PIN)) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "PIN %d does not exist", pin_id));
    }
    return (mp_obj_t)&pyb_pin_obj[pin_id];
}

mp_obj_t pyb_pin_makeinput(mp_obj_t self_in) {
    pyb_pin_obj_t *self = self_in;
    pin_setdir(PIN_ID(self), 0);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_pin_makeinput_obj, pyb_pin_makeinput);

mp_obj_t pyb_pin_makeoutput(mp_obj_t self_in) {
    pyb_pin_obj_t *self = self_in;
    pin_setdir(PIN_ID(self), 1);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_pin_makeoutput_obj, pyb_pin_makeoutput);

mp_obj_t pyb_pin_read(mp_obj_t self_in) {
    pyb_pin_obj_t *self = self_in;
    mp_uint_t val;
    val = pin_read(PIN_ID(self));
    return MP_OBJ_NEW_SMALL_INT(val);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_pin_read_obj, pyb_pin_read);

mp_obj_t pyb_pin_readzval(mp_obj_t self_in) {
    pyb_pin_obj_t *self = self_in;
    mp_uint_t val;
    val = pin_getzval(PIN_ID(self));
    return mp_obj_new_int_from_uint(val);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_pin_readzval_obj, pyb_pin_readzval);

mp_obj_t pyb_pin_on(mp_obj_t self_in) {
    pyb_pin_obj_t *self = self_in;
    pin_state(PIN_ID(self), 1);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_pin_on_obj, pyb_pin_on);

mp_obj_t pyb_pin_off(mp_obj_t self_in) {
    pyb_pin_obj_t *self = self_in;
    pin_state(PIN_ID(self), 0);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_pin_off_obj, pyb_pin_off);

mp_obj_t pyb_pin_toggle(mp_obj_t self_in) {
    pyb_pin_obj_t *self = self_in;
    pin_toggle(PIN_ID(self));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_pin_toggle_obj, pyb_pin_toggle);

mp_obj_t pyb_pin_setxval(size_t n_args, const mp_obj_t *args) {
    pyb_pin_obj_t *self = args[0];
    uint mode;
    int pin = PIN_ID(self);

    if (n_args == 1) {
        return mp_obj_new_int_from_uint(self->xval);
    }
    mode = mp_obj_get_int_truncated(args[1]);
    self->xval = mode;
    pin_setxval(pin, mode);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_pin_setxval_obj, 1, 2, pyb_pin_setxval);

mp_obj_t pyb_pin_setyval(size_t n_args, const mp_obj_t *args) {
    pyb_pin_obj_t *self = args[0];
    mp_uint_t mode;
    int pin = PIN_ID(self);

    if (n_args == 1) {
        return mp_obj_new_int_from_uint(self->yval);
    }
    mode = mp_obj_get_int_truncated(args[1]);
    self->yval = mode;
    pin_setyval(pin, mode);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_pin_setyval_obj, 1, 2, pyb_pin_setyval);

mp_obj_t pyb_pin_setmode(size_t n_args, const mp_obj_t *args) {
    pyb_pin_obj_t *self = args[0];
    mp_uint_t mode;
    int pin = PIN_ID(self);

    if (n_args == 1) {
        return mp_obj_new_int_from_uint(self->mode);
    }
    mode = mp_obj_get_int_truncated(args[1]);
    self->mode = mode;
    pin_setmode(pin, mode);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_pin_setmode_obj, 1, 2, pyb_pin_setmode);

STATIC const mp_rom_map_elem_t pyb_pin_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_on), MP_ROM_PTR(&pyb_pin_on_obj) },
    { MP_ROM_QSTR(MP_QSTR_off), MP_ROM_PTR(&pyb_pin_off_obj) },
    { MP_ROM_QSTR(MP_QSTR_toggle), MP_ROM_PTR(&pyb_pin_toggle_obj) },

    { MP_ROM_QSTR(MP_QSTR_makeinput), MP_ROM_PTR(&pyb_pin_makeinput_obj) },
    { MP_ROM_QSTR(MP_QSTR_makeoutput), MP_ROM_PTR(&pyb_pin_makeoutput_obj) },

    { MP_ROM_QSTR(MP_QSTR_mode), MP_ROM_PTR(&pyb_pin_setmode_obj) },
    { MP_ROM_QSTR(MP_QSTR_xval), MP_ROM_PTR(&pyb_pin_setxval_obj) },
    { MP_ROM_QSTR(MP_QSTR_yval), MP_ROM_PTR(&pyb_pin_setyval_obj) },
    
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&pyb_pin_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_readzval), MP_ROM_PTR(&pyb_pin_readzval_obj) },
};

STATIC MP_DEFINE_CONST_DICT(pyb_pin_locals_dict, pyb_pin_locals_dict_table);

const mp_obj_type_t pyb_pin_type = {
    { &mp_type_type },
    .name = MP_QSTR_PIN,
    .print = pyb_pin_print,
    .make_new = pyb_pin_make_new,
    .locals_dict = (mp_obj_t)&pyb_pin_locals_dict,
};

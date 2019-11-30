/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Total Spectrum Software Inc.
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
#include <propeller2.h>

//////////////////////////////////////////////////////
typedef struct _pyb_cpu_obj_t {
    mp_obj_base_t base;
    mp_int_t id;
    mp_obj_t data;
} pyb_cpu_obj_t;

#define CPU_ID(obj) ((obj)->id)

void pyb_cpu_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    pyb_cpu_obj_t *self = self_in;
    mp_printf(print, "CPU(%d)", CPU_ID(self));
}

STATIC mp_obj_t pyb_cpu_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 0, false);
    pyb_cpu_obj_t *self = m_new_obj(pyb_cpu_obj_t);
    self->base.type = type;
    self->id = -1;
    self->data = NULL;
    return self;
}

mp_obj_t pyb_cpu_id(mp_obj_t self_in) {
    pyb_cpu_obj_t *self = self_in;
    mp_int_t val;
    val = CPU_ID(self);
    return mp_obj_new_int(val);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_cpu_id_obj, pyb_cpu_id);

mp_obj_t pyb_cpu_start(size_t n_args, const mp_obj_t *args) {
    pyb_cpu_obj_t *self = args[0];
    void *codeptr, *dataptr;
    mp_buffer_info_t bufinfo;
    mp_buffer_info_t datinfo;
    mp_int_t cogid;

    mp_get_buffer_raise(args[1], &bufinfo, MP_BUFFER_READ);
    codeptr = (void *)bufinfo.buf;
    if (n_args == 2) {
        dataptr = NULL;
        self->data = NULL;
    } else {
        self->data = args[2];
        mp_get_buffer_raise(args[2], &datinfo, MP_BUFFER_RW);
        dataptr = (void *)datinfo.buf;
    }
    cogid = _cognew(codeptr, dataptr);
    self->id = cogid;
    return mp_obj_new_int(cogid);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_cpu_start_obj, 2, 3, pyb_cpu_start);

mp_obj_t pyb_cpu_stop(mp_obj_t self_in) {
    pyb_cpu_obj_t *self = self_in;
    int val = CPU_ID(self);
    _cogstop(val);
    self->id = -1;
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_cpu_stop_obj, pyb_cpu_stop);

STATIC const mp_rom_map_elem_t pyb_cpu_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_id), MP_ROM_PTR(&pyb_cpu_id_obj) },
    { MP_ROM_QSTR(MP_QSTR_start), MP_ROM_PTR(&pyb_cpu_start_obj) },
    { MP_ROM_QSTR(MP_QSTR_stop), MP_ROM_PTR(&pyb_cpu_stop_obj) },
};

STATIC MP_DEFINE_CONST_DICT(pyb_cpu_locals_dict, pyb_cpu_locals_dict_table);

const mp_obj_type_t pyb_cpu_type = {
    { &mp_type_type },
    .name = MP_QSTR_CPU,
    .print = pyb_cpu_print,
    .make_new = pyb_cpu_make_new,
    .locals_dict = (mp_obj_t)&pyb_cpu_locals_dict,
};

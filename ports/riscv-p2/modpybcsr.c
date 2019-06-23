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
#include "propeller.h"

//////////////////////////////////////////////////////
// helper functions
// we need these because csr_read() and csr_write() only
// work with immediate values (the instructions only
// have an immediate field)
//////////////////////////////////////////////////////
static unsigned int do_csr_read(unsigned id)
{
    switch (id) {
    case 0xbc0:
        return csr_read(0xbc0);
    case 0xbc1:
        return csr_read(0xbc1);
    case 0xbc2:
        return csr_read(0xbc2);
    case 0xbc3:
        return csr_read(0xbc3);
    case 0xbc4:
        return csr_read(0xbc4);
    case 0xbc5:
        return csr_read(0xbc5);
    case 0xbc6:
        return csr_read(0xbc6);
    case 0xbc7:
        return csr_read(0xbc7);
    case CNT_CSR:
        return csr_read(CNT_CSR);
    case CNTH_CSR:
        return csr_read(CNTH_CSR);
    default:
        return (unsigned)-1;
    }
}
static void do_csr_write(unsigned int id, unsigned int val)
{
    switch (id) {
    case 0xbc0:
        csr_write(0xbc0, val);
        break;
    case 0xbc1:
        csr_write(0xbc1, val);
    case 0xbc2:
        csr_write(0xbc2, val);
    case 0xbc3:
        csr_write(0xbc3, val);
    case 0xbc4:
        csr_write(0xbc4, val);
    case 0xbc5:
        csr_write(0xbc5, val);
    case 0xbc6:
        csr_write(0xbc6, val);
    case 0xbc7:
        csr_write(0xbc7, val);
    default:
        break;
    }
}

        
//////////////////////////////////////////////////////
typedef struct _pyb_csr_obj_t {
    mp_obj_base_t base;
    mp_uint_t id;
} pyb_csr_obj_t;

#define CSR_ID(obj) ((obj)->id)

void pyb_csr_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    pyb_csr_obj_t *self = self_in;
    mp_printf(print, "CSR(0x%x)", CSR_ID(self));
}

STATIC mp_obj_t pyb_csr_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, 1, false);
    mp_int_t csr_id = mp_obj_get_int(args[0]);
    if (!(0 <= csr_id && csr_id < 0x0fff)) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "CSR 0x%x does not exist", csr_id));
    }
    pyb_csr_obj_t *self = m_new_obj(pyb_csr_obj_t);
    self->base.type = type;
    self->id = csr_id;
    return self;
}

mp_obj_t pyb_csr_id(mp_obj_t self_in) {
    pyb_csr_obj_t *self = self_in;
    mp_uint_t val;
    val = CSR_ID(self);
    return mp_obj_new_int_from_uint(val);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_csr_id_obj, pyb_csr_id);

mp_obj_t pyb_csr_read(mp_obj_t self_in) {
    pyb_csr_obj_t *self = self_in;
    mp_uint_t val;
    int csr_id = CSR_ID(self);

    val = do_csr_read(csr_id);
    return mp_obj_new_int_from_uint(val);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_csr_read_obj, pyb_csr_read);

mp_obj_t pyb_csr_write(mp_obj_t self_in, mp_obj_t val_in) {
    pyb_csr_obj_t *self = self_in;
    uint val;
    int csr_id = CSR_ID(self);

    val = mp_obj_get_int_truncated(val_in);
    do_csr_write(csr_id, val);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(pyb_csr_write_obj, pyb_csr_write);

STATIC const mp_rom_map_elem_t pyb_csr_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_id), MP_ROM_PTR(&pyb_csr_id_obj) },
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&pyb_csr_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&pyb_csr_write_obj) },
};

STATIC MP_DEFINE_CONST_DICT(pyb_csr_locals_dict, pyb_csr_locals_dict_table);

const mp_obj_type_t pyb_csr_type = {
    { &mp_type_type },
    .name = MP_QSTR_CSR,
    .print = pyb_csr_print,
    .make_new = pyb_csr_make_new,
    .locals_dict = (mp_obj_t)&pyb_csr_locals_dict,
};

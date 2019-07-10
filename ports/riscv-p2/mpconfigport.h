#include <stdint.h>

#define MICROPY_PY_SYS_PLATFORM "prop2"

#define MP_SSIZE_MAX (0x7fffffff)

// options to control how MicroPython is built

// Whether to enable the SD card interface, exposed as pyb.SDCard
#define MICROPY_HW_ENABLE_SDCARD    (1)

// disk I/O
#define MICROPY_READER_VFS          (MICROPY_HW_ENABLE_SDCARD)
#define MICROPY_VFS                 (MICROPY_HW_ENABLE_SDCARD)
#define MICROPY_VFS_FAT             (MICROPY_HW_ENABLE_SDCARD)

// You can disable the built-in MicroPython compiler by setting the following
// config option to 0.  If you do this then you won't get a REPL prompt, but you
// will still be able to execute pre-compiled scripts, compiled with mpy-cross.
#define MICROPY_ENABLE_COMPILER     (1)
#define MICROPY_ENABLE_FINALISER    (0)

#define MICROPY_ALLOC_PATH_MAX      (256)
#define MICROPY_ALLOC_PARSE_CHUNK_INIT (16)
#define MICROPY_EMIT_X86            (0)
#define MICROPY_EMIT_X64            (0)
#define MICROPY_EMIT_THUMB          (0)
#define MICROPY_EMIT_INLINE_THUMB   (0)
#define MICROPY_COMP_MODULE_CONST   (1)
#define MICROPY_COMP_CONST          (1)
#define MICROPY_COMP_DOUBLE_TUPLE_ASSIGN (1)
#define MICROPY_COMP_TRIPLE_TUPLE_ASSIGN (0)
#define MICROPY_COMP_RETURN_IF_EXPR (1)
#define MICROPY_MALLOC_USES_ALLOCATED_SIZE (1)
#define MICROPY_MEM_STATS           (1)
#define MICROPY_DEBUG_PRINTERS      (0)
#define MICROPY_ENABLE_GC           (1)
#define MICROPY_READER_POSIX        (0)
#define MICROPY_USE_READLINE_HISTORY (1)
#define MICROPY_GC_ALLOC_THRESHOLD  (0)

#define MICROPY_HELPER_REPL         (1)
#define MICROPY_REPL_EMACS_KEYS     (1)
#define MICROPY_REPL_AUTO_INDENT    (1)
#define MICROPY_ENABLE_SOURCE_LINE  (1)
//#define MICROPY_ENABLE_DOC_STRING   (0)
#define MICROPY_ERROR_REPORTING     (MICROPY_ERROR_REPORTING_NORMAL)
#define MICROPY_BUILTIN_METHOD_CHECK_SELF_ARG (0)
#define MICROPY_PY_ASYNC_AWAIT      (0)
#define MICROPY_PY_BUILTINS_COMPILE    (1)
#define MICROPY_PY_BUILTINS_EXECFILE (1)
#define MICROPY_PY_BUILTINS_BYTEARRAY (1)
#define MICROPY_PY_BUILTINS_MEMORYVIEW (1)
#define MICROPY_PY_BUILTINS_ENUMERATE (1)
#define MICROPY_PY_BUILTINS_FILTER  (1)
#define MICROPY_PY_BUILTINS_FROZENSET (1)
#define MICROPY_PY_BUILTINS_REVERSED (1)
#define MICROPY_PY_BUILTINS_SET     (1)
#define MICROPY_PY_BUILTINS_SLICE   (1)
#define MICROPY_PY_BUILTINS_PROPERTY (1)
#define MICROPY_PY_BUILTINS_MIN_MAX (1)
#define MICROPY_PY_BUILTINS_HELP            (1)
#define MICROPY_PY_BUILTINS_HELP_MODULES    (1)
#define MICROPY_PY_MICROPYTHON_MEM_INFO (1)
#define MICROPY_PY___FILE__         (0)
#define MICROPY_PY_GC               (1)
#define MICROPY_PY_ARRAY            (1)
#define MICROPY_PY_ATTRTUPLE        (1)
#define MICROPY_PY_COLLECTIONS      (1)
#define MICROPY_PY_MATH             (1)
#define MICROPY_PY_CMATH            (0)
#define MICROPY_PY_IO               (1)
#define MICROPY_PY_IO_IOBASE        (1)
#define MICROPY_PY_IO_FILEIO        (MICROPY_VFS_FAT)
#define MICROPY_PY_STRUCT           (1)
#define MICROPY_PY_SYS              (1)
#define MICROPY_PY_SYS_MAXSIZE      (1)
#define MICROPY_PY_SYS_STDIO_BUFFER (1)
#define MICROPY_PY_SYS_STDFILES     (1)
#define MICROPY_PY_MACHINE          (1)
#define MICROPY_CPYTHON_COMPAT      (0)
#define MICROPY_LONGINT_IMPL        (MICROPY_LONGINT_IMPL_MPZ)
#define MICROPY_FLOAT_IMPL          (MICROPY_FLOAT_IMPL_FLOAT)
#define MICROPY_MODULE_WEAK_LINKS   (1)
#define MICROPY_CAN_OVERRIDE_BUILTINS (1)
#define MICROPY_USE_INTERNAL_ERRNO  (1)


// extended modules
#define MICROPY_PY_UCTYPES          (1)
#define MICROPY_PY_UZLIB            (1)
#define MICROPY_PY_UJSON            (1)
#define MICROPY_PY_URE              (1)
#define MICROPY_PY_URE_SUB          (1)
#define MICROPY_PY_UHEAPQ           (1)
#define MICROPY_PY_UHASHLIB         (1)
#define MICROPY_PY_UBINASCII        (1)
//#define MICROPY_PY_URANDOM          (1)
//#define MICROPY_PY_URANDOM_EXTRA_FUNCS (1)
//#define MICROPY_PY_USELECT          (1)
//#define MICROPY_PY_UTIMEQ           (1)
//#define MICROPY_PY_UTIME_MP_HAL     (1)
//#define MICROPY_PY_OS_DUPTERM       (3)
//#define MICROPY_PY_UOS_DUPTERM_BUILTIN_STREAM (1)
//#define MICROPY_PY_MACHINE          (1)
//#define MICROPY_PY_MACHINE_PULSE    (1)
//#define MICROPY_PY_MACHINE_PIN_MAKE_NEW mp_pin_make_new
//#define MICROPY_PY_MACHINE_I2C      (1)
//#if MICROPY_HW_ENABLE_HW_I2C
//#define MICROPY_PY_MACHINE_I2C_MAKE_NEW machine_hard_i2c_make_new
//#endif
//#define MICROPY_PY_MACHINE_SPI      (1)
//#define MICROPY_PY_MACHINE_SPI_MSB  (SPI_FIRSTBIT_MSB)
//#define MICROPY_PY_MACHINE_SPI_LSB  (SPI_FIRSTBIT_LSB)
//#define MICROPY_PY_MACHINE_SPI_MAKE_NEW machine_hard_spi_make_new
//#define MICROPY_HW_SOFTSPI_MIN_DELAY (0)
//#define MICROPY_HW_SOFTSPI_MAX_BAUDRATE (HAL_RCC_GetSysClockFreq() / 48)
//#define MICROPY_PY_FRAMEBUF         (1)
//#ifndef MICROPY_PY_USOCKET
//#define MICROPY_PY_USOCKET          (1)
//#endif
//#ifndef MICROPY_PY_NETWORK
//#define MICROPY_PY_NETWORK          (1)
//#endif

// fatfs configuration used in ffconf.h
#define MICROPY_FATFS_ENABLE_LFN       (1)
#define MICROPY_FATFS_LFN_CODE_PAGE    437 /* 1=SFN/ANSI 437=LFN/U.S.(OEM) */
#define MICROPY_FATFS_USE_LABEL        (1)
#define MICROPY_FATFS_RPATH            (2)
#define MICROPY_FATFS_MULTI_PARTITION  (1)

// TODO these should be generic, not bound to fatfs
#define mp_type_fileio mp_type_vfs_fat_fileio
#define mp_type_textio mp_type_vfs_fat_textio

// use vfs's functions for import stat and builtin open
#define mp_import_stat mp_vfs_import_stat
#define mp_builtin_open mp_vfs_open
#define mp_builtin_open_obj mp_vfs_open_obj

// extra built in names to add to the global namespace
#define MICROPY_PORT_BUILTINS \
    { MP_ROM_QSTR(MP_QSTR_open), MP_ROM_PTR(&mp_builtin_open_obj) },

// Whether to automatically mount (and boot from) the SD card if it's present
#ifndef MICROPY_HW_SDCARD_MOUNT_AT_BOOT
#define MICROPY_HW_SDCARD_MOUNT_AT_BOOT (MICROPY_HW_ENABLE_SDCARD)
#endif

// TODO these should be generic, not bound to fatfs
#define mp_type_fileio mp_type_vfs_fat_fileio
#define mp_type_textio mp_type_vfs_fat_textio

// use vfs's functions for import stat and builtin open
#define mp_import_stat mp_vfs_import_stat
#define mp_builtin_open mp_vfs_open
#define mp_builtin_open_obj mp_vfs_open_obj

// type definitions for the specific machine

#define MICROPY_MAKE_POINTER_CALLABLE(p) ((void*)((mp_uint_t)(p) | 1))

// This port is intended to be 32-bit, but unfortunately, int32_t for
// different targets may be defined in different ways - either as int
// or as long. This requires different printf formatting specifiers
// to print such value. So, we avoid int32_t and use int directly.
#define UINT_FMT "%u"
#define INT_FMT "%d"
typedef int mp_int_t; // must be pointer size
typedef unsigned mp_uint_t; // must be pointer size

typedef long mp_off_t;

#define MP_PLAT_PRINT_STRN(str, len) mp_hal_stdout_tx_strn_cooked(str, len)

// extra built in names to add to the global namespace
#define MICROPY_PORT_BUILTINS \
    { MP_ROM_QSTR(MP_QSTR_open), MP_ROM_PTR(&mp_builtin_open_obj) },

// extra builtin modules to add to the list of known ones
extern const struct _mp_obj_module_t pyb_module;
extern const struct _mp_obj_module_t mp_module_ubinascii;
extern const struct _mp_obj_module_t mp_module_ure;
extern const struct _mp_obj_module_t mp_module_uzlib;
extern const struct _mp_obj_module_t mp_module_ujson;
extern const struct _mp_obj_module_t mp_module_uheapq;
extern const struct _mp_obj_module_t mp_module_uhashlib;
extern const struct _mp_obj_module_t mp_module_uos;
extern const struct _mp_obj_module_t mp_module_utime;

#define MICROPY_PORT_BUILTIN_MODULES \
    { MP_ROM_QSTR(MP_QSTR_pyb), MP_ROM_PTR(&pyb_module) }, \
    { MP_ROM_QSTR(MP_QSTR_uos), MP_ROM_PTR(&mp_module_uos) }, \

#define MICROPY_PORT_BUILTIN_MODULE_WEAK_LINKS \
    { MP_ROM_QSTR(MP_QSTR_binascii), MP_ROM_PTR(&mp_module_ubinascii) }, \
    { MP_ROM_QSTR(MP_QSTR_collections), MP_ROM_PTR(&mp_module_collections) }, \
    { MP_ROM_QSTR(MP_QSTR_re), MP_ROM_PTR(&mp_module_ure) }, \
    { MP_ROM_QSTR(MP_QSTR_zlib), MP_ROM_PTR(&mp_module_uzlib) }, \
    { MP_ROM_QSTR(MP_QSTR_json), MP_ROM_PTR(&mp_module_ujson) }, \
    { MP_ROM_QSTR(MP_QSTR_heapq), MP_ROM_PTR(&mp_module_uheapq) }, \
    { MP_ROM_QSTR(MP_QSTR_hashlib), MP_ROM_PTR(&mp_module_uhashlib) }, \
    { MP_ROM_QSTR(MP_QSTR_io), MP_ROM_PTR(&mp_module_io) }, \
    { MP_ROM_QSTR(MP_QSTR_os), MP_ROM_PTR(&mp_module_uos) }, \

#if 0

    { MP_ROM_QSTR(MP_QSTR_errno), MP_ROM_PTR(&mp_module_uerrno) }, \
    { MP_ROM_QSTR(MP_QSTR_random), MP_ROM_PTR(&mp_module_urandom) },  \
    { MP_ROM_QSTR(MP_QSTR_time), MP_ROM_PTR(&mp_module_utime) },      \

#endif
        
// We need to provide a declaration/definition of alloca()
#include <alloca.h>

#define MICROPY_HW_BOARD_NAME "P2-Eval-Board"
#define MICROPY_HW_MCU_NAME "p2-cpu"

#define MP_STATE_PORT MP_STATE_VM

#define MICROPY_PORT_ROOT_POINTERS \
    const char *readline_hist[8];

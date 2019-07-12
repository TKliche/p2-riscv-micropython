include ../../py/mkenv.mk

# Using standard RISC-V toolchain
#RV32_BASE = /opt/riscv/riscv32-unknown-elf/
#RV32_GCCLIB=/opt/riscv/lib/gcc/riscv32-unknown-elf/8.2.0/libgcc.a
#RV32_PREFIX=riscv32-unknown-elf-

# Using GNU MCU toolchain
RV32_BASE = /opt/riscv/riscv-none-embed
RV32_GCCLIB=/opt/riscv/lib/gcc/riscv-none-embed/8.2.0/rv32imac/ilp32/libgcc.a
RV32_PREFIX=riscv-none-embed-

#LDFILE ?= riscv_p2_local.ld
LDFILE=riscvp2.ld

CROSS = 1

RV32_MATHLIB=$(RV32_BASE)/lib/libm.a

HEADERSIZE=16384
RV32ARCH=rv32imc

# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h

# include py core make definitions
include $(TOP)/py/py.mk

ifeq ($(CROSS), 1)
CROSS_COMPILE = $(RV32_PREFIX)
endif

INC += -I.
INC += -I$(TOP)
INC += -I$(BUILD)

ifeq ($(CROSS), 1)
LD=$(CC)
CFLAGS_RISCV = -march=$(RV32ARCH)
CFLAGS = $(INC) -Wall -Werror -std=c99 $(CFLAGS_RISCV) $(COPT) $(CFLAGS_MOD)
#LDFLAGS = -nostdlib -T $(LDFILE) -Wl,-Map=$@.map,--cref,--gc-sections
LDFLAGS = -specs=nano.specs -T $(LDFILE) -Wl,-Map=$@.map,--cref 
#CRT0 = $(BUILD)/crt0.o $(BUILD)/setjmp.o

$(BUILD)/%.o: %.S
	$(CC) $(CFLAGS) -o $@ -c $^
else
LD = gcc
CFLAGS = -m32 -g $(INC) -Wall -Werror -std=c99 $(COPT)
LDFLAGS = -m32 -g -Wl,-Map=$@.map,--cref -Wl,--gc-sections
endif

# Tune for Debugging or Optimization
ifeq ($(DEBUG), 1)
CFLAGS += -O0 -ggdb
else
CFLAGS += -Os -DNDEBUG
CFLAGS += -fdata-sections -ffunction-sections
endif

LIBS = $(RV32_MATHLIB) $(RV32_GCCLIB)

SRC_C = \
	main.c \
	uart_core.c \
	modpyb.c \
	modpybcsr.c \
	modpybpin.c \
	moduos.c \
	board.c \
	sdcard.c \
	vgatext.c \
	vga_tile_driver.c \
	OneCogKbM.c \
	BufferSerial.c \
	lib/utils/pyexec.c \
	lib/utils/stdout_helpers.c \
	lib/utils/sys_stdio_mphal.c \
	lib/libc/string0.c \
	lib/mp-readline/readline.c \
	lib/oofatfs/ff.c \
	lib/oofatfs/ffunicode.c \
	lib/timeutils/timeutils.c \

OBJ = $(CRT0) $(PY_CORE_O) $(PY_EXTMOD_O) $(addprefix $(BUILD)/, $(SRC_C:.c=.o))

ifeq ($(CROSS), 1)
all: $(BUILD)/firmware.bin
else
all: $(BUILD)/firmware.elf
endif

$(BUILD)/firmware.elf: $(OBJ)
	$(ECHO) "LINK $@"
	$(Q)$(LD) $(LDFLAGS) -o $@ $^ $(LIBS)
	$(Q)$(SIZE) $@

$(BUILD)/firmware.bin: $(BUILD)/firmware.elf
	$(Q)$(OBJCOPY) -O binary $^ $(BUILD)/firmware.bin
# Run emulation build on a POSIX system with suitable terminal settings
run:
	stty raw opost -echo
	build/firmware.elf
	@echo Resetting terminal...
# This sleep is useful to spot segfaults
	sleep 1
	reset

test: $(BUILD)/firmware.elf
	$(Q)/bin/echo -e "print('hello world!', list(x+1 for x in range(10)), end='eol\\\\n')\\r\\n\\x04" | $(BUILD)/firmware.elf | tail -n2 | grep "^hello world! \\[1, 2, 3, 4, 5, 6, 7, 8, 9, 10\\]eol"

# List of sources for qstr extraction
SRC_QSTR += $(SRC_C) mpconfigport.h
# Append any auto-generated sources that are needed by sources listed in
# SRC_QSTR
SRC_QSTR_AUTO_DEPS +=

include $(TOP)/py/mkrules.mk

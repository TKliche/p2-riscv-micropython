include ../../py/mkenv.mk

LDFILE ?= riscv_p2.ld

FASTSPIN = /home/ersmith/Parallax/spin2cpp/build/fastspin
P2HEADER = p2jitheader.bin
P2EMUHEADER = p2emuheader.bin
CROSS = 1

RV32_MATHLIB=/opt/riscv/riscv32-unknown-elf/lib/libm.a
RV32_GCCLIB=/opt/riscv/lib/gcc/riscv32-unknown-elf/8.2.0/libgcc.a

# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h

# include py core make definitions
include $(TOP)/py/py.mk

ifeq ($(CROSS), 1)
CROSS_COMPILE = riscv32-unknown-elf-
endif

INC += -I.
INC += -I$(TOP)
INC += -I$(BUILD)

ifeq ($(CROSS), 1)
DFU = $(TOP)/tools/dfu.py
PYDFU = $(TOP)/tools/pydfu.py
CFLAGS_RISCV = -march=rv32im
CFLAGS = $(INC) -Wall -Werror -std=c99 -nostdlib $(CFLAGS_RISCV) $(COPT)
#LDFLAGS = -nostdlib -T $(LDFILE) -Map=$@.map --cref --gc-sections
LDFLAGS = -nostdlib -T $(LDFILE) -Map=$@.map --cref 
CRT0 = $(BUILD)/crt0.o $(BUILD)/setjmp.o

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
	modpybpin.c \
	board.c \
	lib/utils/printf.c \
	lib/utils/stdout_helpers.c \
	lib/utils/pyexec.c \
	lib/libc/string0.c \
	lib/mp-readline/readline.c \

OBJ = $(CRT0) $(PY_CORE_O) $(addprefix $(BUILD)/, $(SRC_C:.c=.o))

ifeq ($(CROSS), 1)
all: $(BUILD)/p2.binary $(BUILD)/p2emu.binary
else
all: $(BUILD)/firmware.elf
endif

$(BUILD)/firmware.elf: $(OBJ)
	$(ECHO) "LINK $@"
	$(Q)$(LD) $(LDFLAGS) -o $@ $^ $(LIBS)
	$(Q)$(SIZE) $@

$(BUILD)/firmware.bin: $(BUILD)/firmware.elf
	$(Q)$(OBJCOPY) -O binary $^ $(BUILD)/firmware.bin
## Use next line to padd to a known size
##	dd if=/dev/null of=$(BUILD)/firmware.bin bs=1 count=0 seek=262144

$(BUILD)/p2.binary: $(BUILD)/$(P2HEADER) $(BUILD)/firmware.bin
	cat $^ > $@
$(BUILD)/p2emu.binary: $(BUILD)/$(P2EMUHEADER) $(BUILD)/firmware.bin
	cat $^ > $@

$(BUILD)/p2jitheader.bin: riscvjit_p2.spin
	mkdir -p $(BUILD)
	$(FASTSPIN) -2 -c -o $(BUILD)/p2jitheader.bin riscvjit_p2.spin
	dd if=/dev/null of=$(BUILD)/p2jitheader.bin bs=1 count=1 seek=8192

$(BUILD)/p2emuheader.bin: debug.spin riscvemu_p2.spin
	mkdir -p $(BUILD)
	$(FASTSPIN) -2 -o $(BUILD)/p2emuheader.bin debug.spin
	dd if=/dev/null of=$(BUILD)/p2emuheader.bin bs=1 count=1 seek=8192

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

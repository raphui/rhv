export

TARGET=rhv
HV_BASE=$(shell pwd)

CROSS_COMPILE = aarch64-none-elf-

ifeq (${MAKELEVEL}, 0)
INCLUDES	+= -I$(HV_BASE)/include
INCLUDES	+= -I$(HV_BASE)/lib
INCLUDES	+= -I$(HV_BASE)/third_party/lib/tlsf


ASFLAGS	:= -g $(INCLUDES)
CFLAGS  :=  -Wall -g $(INCLUDES) -MD -MP -nostdlib -nostdinc -fno-builtin -ffunction-sections -march=armv8-a+nosimd+nofp
LDFLAGS	:= -g $(INCLUDES) -Trhv.lds -nostartfiles -nostdlib -Wl,-Map=rhv.map

CC := $(CROSS_COMPILE)gcc
AS := $(CROSS_COMPILE)as
AR := $(CROSS_COMPILE)ar
LD := $(CROSS_COMPILE)ld
LDS := $(CROSS_COMPILE)gcc -E -P -C
CPP := cpp
MKDIR := mkdir -p

endif

obj-y := start.o
obj-y += exceptions.o
obj-y += interrupts.o
obj-y += hv.o

subdirs-y := mm
subdirs-y += lib
subdirs-y += third_party

OBJS_DIR := $(HV_BASE)/objs

.PHONY: all clean $(TARGET)

ifdef DEBUGMAKE
else
PREFIX=@
endif

build = echo "$1 $@"

ifeq (${MAKELEVEL}, 0)
all: ${TARGET}
	$(MKDIR) $(OBJS_DIR)

$(TARGET):
	$(PREFIX)rm -f objects.lst
	$(PREFIX)rm -f extra_objects.lst
	$(PREFIX)$(MAKE) --no-print-directory -f Makefile.common dir=$(HV_BASE) all
	$(PREFIX)$(CC) -o $@ `cat objects.lst | tr '\n' ' '` $(LDFLAGS)
	$(PREFIX)$(call build,LD)

cscope:
	@@echo "GEN " $@
	$(PREFIX)cscope -b -q -k -R
 
include $(wildcard *.d)
 
clean:
	$(PREFIX)$(MAKE) -f Makefile.common dir=$(HV_BASE) $@
	-$(PREFIX)rm $(TARGET)

dist-clean: clean
	$(PREFIX)$(RM) `find . -name *.d`
endif

ifndef VERBOSE
.SILENT:
endif

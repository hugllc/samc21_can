# Changes should be made in the Package.mk file.  Changes here will get overwritten
# in the next update.
ROOTDIR    := $(abspath $(shell pwd))
BINDIR     := $(ROOTDIR)/toolchain/bin
NODEBINDIR := $(ROOTDIR)/node_modules/.bin
export PATH := ${HOME}/.platformio/packages/toolchain-gccarmnoneeabi/bin/:$(BINDIR):$(NODEBINDIR):$(PATH)
TEMPDIR := $(shell mktemp -d)

OPENOCD := ${HOME}/.platformio/packages/tool-openocd/bin/openocd -s ~.platformio/packages/tool-openocd/scripts -f board/atmel_samc21_xplained_pro.cfg

# This needs to be before include Package.mk
all: style docs assemble

ENV ?= ""
PACKAGE_NAME ?= "bootloader"


CC := arm-none-eabi-gcc
SIZE := arm-none-eabi-size

CFLAGS :=  -std=gnu11
CFLAGS += -gdwarf-2 -D__SAMC21J18A__
CFLAGS += -Isrc/ -I../../bootloader/
CFLAGS += -g -Os -ffunction-sections -fdata-sections -Wall -mthumb -mcpu=cortex-m0plus -nostdlib 
CFLAGS += --param max-inline-insns-single=500 -gdwarf-2 -Wall -Wextra -Wno-unused-parameter
CFLAGS += -Wall -Werror -Wformat-nonliteral -Wpointer-arith -Winline 
CFLAGS += -Wwrite-strings -Wno-unused-parameter -Wfloat-equal -DF_CPU=48000000L 
CFLAGS += -I${HOME}/.platformio/packages/framework-arduinosam/system/samd/CMSIS/CMSIS/Include
CFLAGS += -I${HOME}/.platformio/packages/framework-arduinosam/system/samd/CMSIS-Atmel/CMSIS/Device/ATMEL


LDFLAGS := 
LDFLAGS += -Wl,--gc-sections -Wl,--check-sections 
LDFLAGS += -Wl,--unresolved-symbols=report-all 
LDFLAGS += -Wl,--warn-common -Wl,--warn-section-align --specs=nosys.specs --specs=nano.specs 
LDFLAGS += -Wl,-Map,out.map -Wl,-Tbootloader_samc21x18.ld 
LDFLAGS += -mcpu=cortex-m0plus 
LDFLAGS += -L${HOME}/.platformio/packages/framework-arduinosam/system/samd/CMSIS/CMSIS/Lib/GCC 
LDFLAGS += -Wl,--start-group -lc -lgcc -lm -larm_cortexM0l_math -Wl,--end-group

EXTRA_TARGETS ?=
PACKAGE_FULL_VERSION :=  $(subst 0x,,$(PACKAGE_VERSION)).$(subst 0x,,$(PACKAGE_VERSION_SMALL)).$(subst 0x,,$(PACKAGE_VERSION_TINY))

APT        := $(shell command -v apt-get 2> /dev/null)
PIP        := $(shell command -v pip 2> /dev/null)
VIRTUALENV := $(shell command -v virtualenv 2> /dev/null)

PORT ?= /dev/ttyUSB0

VPATH := $(ROOTDIR)/src/ $(ROOTDIR)/../../bootloader/

OBJECTS := $(patsubst %.c,%.o,$(wildcard src/*.c)) $(patsubst %.c,%.o,$(wildcard ../../bootloader/*.c)) 

build:
	mkdir -p build

assemble: build ${PACKAGE_NAME}.elf
	mkdir -p build/${PACKAGE_NAME}
	cp README.md build/${PACKAGE_NAME}/
	cp ${PACKAGE_NAME}.elf build/${PACKAGE_NAME}/
	

${PACKAGE_NAME}.elf: $(OBJECTS)
	${CC} -o $@ $^ $(LDFLAGS) $(LIBS)
	${SIZE}  --format=sysv -t -x ${PACKAGE_NAME}.elf
	
	
update: platformio
	-platformio lib install
	-platformio platform update
	-platformio lib update;
	-$(BINDIR)/pip install -U jsmin


install: ${PACKAGE_NAME}.elf chiperase
	$(OPENOCD)  -c "telnet_port disabled; program {${PACKAGE_NAME}.elf} verify reset ; shutdown"

toolchain/bin/pip: | setup
	virtualenv toolchain
	
toolchain/bin/platformio: | toolchain/bin/pip
	$(BINDIR)/pip install -U platformio
	#$(BINDIR)/platformio platform install --with-package tool-espotapy https://github.com/platformio/platform-espressif32.git#feature/stage
	# The following line MUST be here, so that espotapy gets installed.
	$(BINDIR)/platformio platform install https://github.com/prices/platform-atmelsam.git#samc21
	
# Just a short cut
platformio: toolchain/bin/platformio
			
monitor:
	stty raw -echo 115200 < $(PORT); cat -vte $(PORT)

openocd: platformio
	~/.platformio/packages/tool-openocd/bin/openocd -s ~.platformio/packages/tool-openocd/scripts -f $(ROOTDIR)/debug/10820101.cfg
	
chiperase: platformio
	~/.platformio/packages/tool-openocd/bin/openocd -s ~.platformio/packages/tool-openocd/scripts -f interface/cmsis-dap.cfg -c "transport select swd" -f target/at91samdXX.cfg -c "init" -c "reset halt" -c "at91samd chip-erase" -c "shutdown"
	
fix: platformio
	~/.platformio/packages/tool-openocd/bin/openocd -s ~.platformio/packages/tool-openocd/scripts -f $(ROOTDIR)/debug/10820101.cfg -c "init; reset halt;"

	
debug: ${PACKAGE_NAME}.elf
	kdbg -t kdbg.out ${PACKAGE_NAME}.elf

distclean:
	rm -Rf toolchain boards rel build 
	find . -iname "*.old" -delete
	find . -iname "*.orig" -delete
	find . -iname "*.map" -delete
	find . -iname ".piolibdeps" -type d -prune -exec rm -Rf "{}" \;
	find . -iname ".pioenvs" -type d -prune -exec rm -Rf "{}" \;

clean:
	rm -Rf rel build
	find . -iname "*.old" -delete
	find . -iname "*.orig" -delete
	find . -iname "*.map" -delete
	find . -iname "*.o" -delete
	find . -iname "*.elf" -delete
	find . -iname "*.bin" -delete
	
.FORCE:
.PHONY: distclean platformio clean docs archive setup monitor build node web


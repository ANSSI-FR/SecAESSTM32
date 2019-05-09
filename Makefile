SRC_DIR   = src
BUILD_DIR = build

#### The masked AES and its glue
LIBAES_SRC_DIR = $(SRC_DIR)/aes
LIBAES_C_SOURCES = $(LIBAES_SRC_DIR)/aes.c
LIBAES_ASM_SOURCES = $(LIBAES_SRC_DIR)/affine_aes.S
LIBAES_OBJECTS = $(patsubst $(LIBAES_SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(LIBAES_C_SOURCES))
LIBAES_OBJECTS += $(patsubst $(LIBAES_SRC_DIR)/%.S, $(BUILD_DIR)/%.o, $(LIBAES_ASM_SOURCES))

#### Printf and strings related stuff
LIBPRINTF_SRC_DIR = $(SRC_DIR)/printf
LIBPRINTF_C_SOURCES = $(LIBPRINTF_SRC_DIR)/printf.c
LIBPRINTF_OBJECTS = $(patsubst $(LIBPRINTF_SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(LIBPRINTF_C_SOURCES))

#### Tests related stuff 
LIBTESTS_SRC_DIR = $(SRC_DIR)/tests
LIBTESTS_C_SOURCES = $(LIBTESTS_SRC_DIR)/test.c
LIBTESTS_OBJECTS = $(patsubst $(LIBTESTS_SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(LIBTESTS_C_SOURCES))

#### The core files handling the 'main' tests functions
CORE_SRC_DIR = $(SRC_DIR)
CORE_C_SOURCES   = $(CORE_SRC_DIR)/main.c  $(CORE_SRC_DIR)/platform.c
CORE_OBJECTS  = $(patsubst  $(CORE_SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(CORE_C_SOURCES))

# Bare metal 'OS' sources for the STM32 Discovery F407 board
OS_SRC_DIR = $(SRC_DIR)/stm32discovery_F407/
OS_C_SOURCES = $(call wildcard, $(OS_SRC_DIR)/*.c)
OS_ASM_SOURCES = $(call wildcard, $(OS_SRC_DIR)/*.s)
OS_OBJECTS = $(patsubst $(OS_SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(OS_C_SOURCES))
OS_OBJECTS += $(patsubst $(OS_SRC_DIR)/%.s, $(BUILD_DIR)/%.o, $(OS_ASM_SOURCES))

ifeq ($(CC),clang)
COMPILER = clang
endif

ifneq ($(COMPILER),)
FORCE_COMPILER=1
endif
COMPILER ?= arm-none-eabi-gcc
AR ?= arm-none-eabi-ar
OBJCOPY ?= arm-none-eabi-objcopy

MEM_LAYOUT_DEF=$(OS_SRC_DIR)/layout.def
MEM_LAYOUT=$(BUILD_DIR)/layout.ld

STFLASH ?= st-flash

GCC_CFLAGS = -fPIC -fno-builtin -g3 -DDEBUG_LVL=0 -Os -mthumb-interwork -mthumb -mcpu=cortex-m4  -mno-long-calls -std=gnu99
GCC_LDFLAGS = -nostartfiles -fno-builtin -g3 -Os -mthumb-interwork -mthumb -mcpu=cortex-m4  -mno-long-calls

CLANG_CFLAGS = -fPIC -fno-builtin -Os -g -mcpu=cortex-m4 -march=armv7e-m
CLANG_LDFLAGS = -nostartfiles -fno-builtin -Os -g -mcpu=cortex-m4 -march=armv7e-m

ifeq ($(COMPILER),clang)
CC = clang
endif

ifeq ($(CC),clang)
COMPILER = clang
CFLAGS ?= $(CLANG_CFLAGS)
LDFLAGS ?= $(CLANG_LDFLAGS)
else
CFLAGS ?= $(GCC_CFLAGS)
LDFLAGS ?= $(GCC_LDFLAGS)
endif

# Do we have static tests to include
STATIC_TESTS = ./generated_tests/aes_tests.h
ifneq ($(wildcard $(STATIC_TESTS)),)
CFLAGS += -DSTATIC_TESTS_PRESENT -I"./generated_tests"
endif

# Necessary includes
CFLAGS += -I$(LIBAES_SRC_DIR) -I$(LIBPRINTF_SRC_DIR) -I$(LIBTESTS_SRC_DIR) -I$(CORE_SRC_DIR)

# Add the optional extra CFLAGS and LDFLAGS
CFLAGS += $(EXTRA_CFLAGS)
LDFLAGS += $(EXTRA_LDFLAGS)

all: help


help:
	@echo "Please choose the target you want to compile:"
	@echo "\tmake qemu"
	@echo "\t=> for qemu target compilation"
	@echo "\t==============================="
	@echo "\tmake firmware"
	@echo "\t=> for firmware target compilation, for the STM32Discovery F407 board"
	@echo "\t==============================="
	@echo "\tmake burn"
	@echo "\t=> for firmware burning on the STM32Discovery F407 board."
	@echo "\t   Burning uses the st-utils tools."
	@echo "\t==============================="
	@echo "\tThe masked AES static library can also be compiled using the dedicated target:"
	@echo "\tmake libaes"
	@echo "\tNOTE: use the UNROLL=1 environment variable to force the fully unrolled version compilation"

# Compile the OS sources
embedded_os_lib:        
	@mkdir -p $(BUILD_DIR)
	$(COMPILER) $(CFLAGS) -I$(CORE_SRC_DIR) -I$(LIBPRINTF_SRC_DIR) -I$(OS_SRC_DIR) $(OS_C_SOURCES) $(OS_ASM_SOURCES) -c        
	@mv *.o $(BUILD_DIR)
	$(AR) rcs $(BUILD_DIR)/libembeddedos.a $(OS_OBJECTS)

ifneq ($(FORCE_COMPILER),1)
firmware: COMPILER = arm-none-eabi-gcc
firmware: AR = arm-none-eabi-ar
firmware: OBJCOPY = arm-none-eabi-objcopy
endif
ifeq ($(COMPILER),clang)
firmware: CFLAGS += --target=arm-none-eabi
firmware: LDFLAGS += --target=arm-none-eabi
endif
firmware: CFLAGS += -DMEASURE_PERFORMANCE -DUSE_STM32_DISCO_PLATFORM
firmware: libprintf libtests libaes embedded_os_lib
	# Generate memory layout
	$(COMPILER) -E -x c $(MEM_LAYOUT_DEF) -I$(OS_SRC_DIR) | grep -v '^#' > $(MEM_LAYOUT)
	cp  $(OS_SRC_DIR)/standalone.ld $(BUILD_DIR)/
	# Link elements
	$(COMPILER) $(CFLAGS) $(LDFLAGS) -I$(OS_SRC_DIR) -I$(LIBPRINTF_SRC_DIR) $(CORE_C_SOURCES) $(BUILD_DIR)/libtests.a $(BUILD_DIR)/libprintf.a $(BUILD_DIR)/libmaskedaes.a $(BUILD_DIR)/libembeddedos.a -Xlinker -T$(BUILD_DIR)/standalone.ld -o $(BUILD_DIR)/firmware.elf
	# Generate the hex file
	$(OBJCOPY) -O ihex $(BUILD_DIR)/firmware.elf $(BUILD_DIR)/firmware.hex
	$(OBJCOPY) -I ihex --output-target=binary $(BUILD_DIR)/firmware.hex $(BUILD_DIR)/firmware.bin

burn:
	$(STFLASH) write $(BUILD_DIR)/firmware.bin 0x8000000

# Compile an independed printf static library
libprintf:
	@mkdir -p $(BUILD_DIR)
	$(COMPILER) $(CFLAGS) $(LIBPRINTF_C_SOURCES) -c
	@mv *.o $(BUILD_DIR)
	$(AR) rcs $(BUILD_DIR)/libprintf.a $(LIBPRINTF_OBJECTS)	

# Compile an independed tests static library
libtests:
	@mkdir -p $(BUILD_DIR)
	$(COMPILER) $(CFLAGS) $(LIBTESTS_C_SOURCES) -c
	@mv *.o $(BUILD_DIR)
	$(AR) rcs $(BUILD_DIR)/libtests.a $(LIBTESTS_OBJECTS)	

# Compile an independent AES static library
LIBAES_CFLAGS = $(CFLAGS)
ifeq ($(UNROLL),1)
LIBAES_CFLAGS += -DUSE_FULL_UNROLLING
endif
libaes:
	@mkdir -p $(BUILD_DIR)
	$(COMPILER) $(LIBAES_CFLAGS) $(LIBAES_C_SOURCES) $(LIBAES_ASM_SOURCES) -c
	@mv *.o $(BUILD_DIR)
	$(AR) rcs $(BUILD_DIR)/libmaskedaes.a $(LIBAES_OBJECTS)	

ifneq ($(FORCE_COMPILER),1)
qemu: COMPILER = arm-linux-gnueabi-gcc
qemu: AR = arm-linux-gnueabi-ar
qemu: OBJCOPY = arm-linux-gnueabi-objcopy
endif
ifeq ($(COMPILER),clang)
qemu: CFLAGS += --target=arm-linux-gnueabi
qemu: LDFLAGS += --target=arm-linux-gnueabi
endif
qemu: CFLAGS += -DUSE_QEMU_PLATFORM
qemu: libprintf libtests libaes
	@mkdir -p $(BUILD_DIR)
	$(COMPILER) $(CFLAGS) $(CORE_C_SOURCES) -c
	@mv *.o $(BUILD_DIR)/
	# Link
	$(COMPILER) $(LDFLAGS) $(CORE_OBJECTS) $(BUILD_DIR)/libtests.a  $(BUILD_DIR)/libprintf.a $(BUILD_DIR)/libmaskedaes.a -static -o $(BUILD_DIR)/affine_aes

clean_tmp:
	@rm -rf *.o $(BUILD_DIR)/*.o $(BUILD_DIR)/*.a

clean:
	@rm -rf *.o $(BUILD_DIR)/

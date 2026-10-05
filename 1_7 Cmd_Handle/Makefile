PROJECT = stm32f407
BUILD_DIR = build

TOOLCHAIN_DIR ?= D:/STM32CubeIDE_2.2.0/STM32CubeIDE/plugins/com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.14.3.rel1.win32_1.0.100.202602081740/tools/bin
PREFIX = $(TOOLCHAIN_DIR)/arm-none-eabi-

CC = $(PREFIX)gcc
AS = $(PREFIX)gcc
OBJCOPY = $(PREFIX)objcopy
SIZE = $(PREFIX)size

MCU = -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard
DEFINES = -DSTM32F407xx -DSTM32F40_41xxx -DUSE_STDPERIPH_DRIVER -DHSE_VALUE=8000000
INCLUDES = -Iapp -Idriver -Ifirmware/cmsis/core -Ifirmware/cmsis/device -Ifirmware/driver/inc

CFLAGS = $(MCU) $(DEFINES) $(INCLUDES) -std=gnu11 -O0 -g3 -Wall -ffunction-sections -fdata-sections
ASFLAGS = $(MCU) $(DEFINES) $(INCLUDES) -x assembler-with-cpp -g3
LDFLAGS = $(MCU) -TSTM32F407VETx_FLASH.ld -specs=nano.specs -Wl,--gc-sections -Wl,-Map=$(BUILD_DIR)/$(PROJECT).map

C_SOURCES = \
app/main.c \
driver/CMD.c \
driver/DMA.c \
driver/Parser.c \
driver/RingBuffer.c \
driver/USART.c \
syscalls.c \
firmware/cmsis/device/system_stm32f4xx.c \
$(filter-out firmware/driver/src/stm32f4xx_fmc.c,$(wildcard firmware/driver/src/*.c))

ASM_SOURCES = firmware/cmsis/device/startup_stm32f407xx_gcc.s

OBJECTS = $(addprefix $(BUILD_DIR)/,$(C_SOURCES:.c=.o))
OBJECTS += $(addprefix $(BUILD_DIR)/,$(ASM_SOURCES:.s=.o))

DEPS = $(OBJECTS:.o=.d)

.PHONY: all clean

all: $(BUILD_DIR)/$(PROJECT).elf $(BUILD_DIR)/$(PROJECT).hex $(BUILD_DIR)/$(PROJECT).bin

$(BUILD_DIR)/$(PROJECT).elf: $(OBJECTS) STM32F407VETx_FLASH.ld
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	$(SIZE) $@

$(BUILD_DIR)/%.o: %.c Makefile
	@if not exist "$(subst /,\,$(@D))" mkdir "$(subst /,\,$(@D))"
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

$(BUILD_DIR)/%.o: %.s Makefile
	@if not exist "$(subst /,\,$(@D))" mkdir "$(subst /,\,$(@D))"
	$(AS) $(ASFLAGS) -MMD -MP -c $< -o $@

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf
	$(OBJCOPY) -O ihex $< $@

$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf
	$(OBJCOPY) -O binary -S $< $@

clean:
	@if exist "$(BUILD_DIR)" rmdir /S /Q "$(BUILD_DIR)"

-include $(DEPS)

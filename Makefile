STM32CUBE_ROOT=../STM32Cube_FW_F4_V1.1.0

CC = arm-none-eabi-gcc
OBJCOPY= arm-none-eabi-objcopy

INCLUDES += .
INCLUDES += $(STM32CUBE_ROOT)/Drivers/STM32F4xx_HAL_Driver/Inc/
INCLUDES += $(STM32CUBE_ROOT)/Drivers/CMSIS/Device/ST/STM32F4xx/Include/
INCLUDES += $(STM32CUBE_ROOT)/Drivers/CMSIS/Include/
INCLUDES += $(STM32CUBE_ROOT)/Drivers/BSP/STM32F4-Discovery/

CFLAGS = -mcpu=cortex-m4 -mthumb -DSTM32F407xx -nostartfiles -std=c99 $(addprefix -I, $(INCLUDES))
CFLAGS += -Wall -g
LDFLAGS = -TSTM32F407VG_FLASH.ld

OBJ = main startup_stm32f4xx system_stm32f4xx syscalls stm32f4xx_it

TARGET=test

# external
## HAL
HAL_ROOT=$(STM32CUBE_ROOT)/Drivers/STM32F4xx_HAL_Driver/

OBJ_HAL += stm32f4xx_hal 
OBJ_HAL += stm32f4xx_hal_cortex 
OBJ_HAL += stm32f4xx_hal_gpio 
OBJ_HAL += stm32f4xx_hal_spi 
OBJ_HAL += stm32f4xx_hal_i2c 
OBJ_HAL += stm32f4xx_hal_dma 
OBJ_HAL += stm32f4xx_hal_rcc
OBJ_HAL += stm32f4xx_hal_tim
OBJ_HAL += stm32f4xx_hal_tim_ex

## BSP
DISCOVERY_ROOT=$(STM32CUBE_ROOT)/Drivers/BSP/STM32F4-Discovery/
OBJ_DISCOVERY = stm32f4_discovery

# generated
OBJ_O = $(addsuffix .o, $(OBJ))
OBJ_HAL_O = $(addprefix HAL_,$(addsuffix .o,$(OBJ_HAL)))
OBJ_DISCOVERY_O = $(addprefix DISCOVERY_,$(addsuffix .o,$(OBJ_DISCOVERY)))
OBJ_ALL_O = $(OBJ_O) $(OBJ_DISCOVERY_O) $(OBJ_HAL_O)

# rules 
all: $(TARGET).bin

$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) -O binary $< $@

$(TARGET).hex: $(TARGET).elf
	$(OBJCOPY) -O ihex $< $@

$(TARGET).elf: $(OBJ_ALL_O)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

HAL_%.o: $(HAL_ROOT)/Src/%.c 
	$(CC) $(CFLAGS) $< -c -o $@

DISCOVERY_%.o: $(DISCOVERY_ROOT)/%.c 
	$(CC) $(CFLAGS) $< -c -o $@

%.o : %.c
	$(CC) $(CFLAGS) $< -c -o $@

clean:
	rm -f *.o *.elf *~

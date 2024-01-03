projectName = $(shell basename $(shell pwd))
TOOLCHAIN =#/home/mahmoud/arm-gnu-toolchain-12.2.rel1-x86_64-arm-none-eabi/bin

# cpu
CPU = -mcpu=cortex-m4

# fpu
FPU = -mfpu=fpv4-sp-d16

# float-abi
FLOAT-ABI = -mfloat-abi=hard

# mcu
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI) 

#no exceptions , no runtime type information (no virtual pointers table) each function linked individualy
#at the cost of increase linker time
CC_OPTIONS = -fno-exceptions -fno-rtti -ffunction-sections -fno-use-cxa-atexit
#no start files from the system explicitly use the start files in one of the .o files
LD_OPTIONS = -nostartfiles --specs=nano.specs --specs=nosys.specs -fno-use-cxa-atexit
#link math library
LIBS = -lm

INCLUDES = ../Include ../Include/AML

all: compile link

compile: src/$(projectName).cpp 
	$(TOOLCHAIN)arm-none-eabi-g++ -g -c $(MCU) $(CC_OPTIONS) src/main.cpp -o Build/main.o -I $(INCLUDES)
	$(TOOLCHAIN)arm-none-eabi-g++ -g -c $(MCU) $(CC_OPTIONS) src/$(projectName).cpp -o ../Drivers/$(projectName).o -I $(INCLUDES)

assemble:
	$(TOOLCHAIN)arm-none-eabi-as -g -mcpu=cortex-m4 -mthumb ../Startup/startup_stm32f401rctx.s -o Build/startup_stm32f401rctx.o

link:	compile	assemble
	$(TOOLCHAIN)arm-none-eabi-g++ $(MCU) $(LD_OPTIONS)  Build/*.o ../drivers/*.o -o Build/$(projectName).elf -T ../STM32F401RCTX_FLASH.ld  $(LIBS)

upload:	compile link 
	openocd -f interface/stlink.cfg -f target/stm32f4x.cfg -c "program Build/$(projectName).elf verify reset exit"

debug: compile link
	openocd -f interface/stlink.cfg -f target/stm32f4x.cfg

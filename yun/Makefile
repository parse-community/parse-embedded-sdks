#******************************************************************************
#
# Makefile - Rules for building Arduino Yun
#
#*****************************************************************************

TOOLCHAIN_PATH=$(HOME)/devtools/avr-gcc/bin
CC=$(TOOLCHAIN_PATH)/avr-gcc
CXX=$(TOOLCHAIN_PATH)/avr-g++
OBJCOPY=$(TOOLCHAIN_PATH)/avr-objcopy
AVRDUDE=$(TOOLCHAIN_PATH)/avrdude

ARDUINO_STUDIO=/Applications/Arduino.app/Contents/Resources/Java
ARDUINO_STUDIO2=/Applications/Arduino.app/Contents/Java
SKETCH_PATH=$(ARDUINO_STUDIO2)

ifeq ("$(wildcard $(ARDUINO_STUDIO2))","")
	SKETCH_PATH=$(ARDUINO_STUDIO)
endif

OBJ_DIR=obj
EXE_DIR=exe

LIBS=-L ~/lib -lm
MCU=-mmcu=atmega32u4
CPU_SPEED=-DF_CPU=16000000L
PORT=$(shell ls /dev/cu.usbmodem* 2> /dev/null)
BOARD=arduino
INCDIR=~/include/$(BOARD)

CFLAGS=$(MCU) $(CPU_SPEED)
CFLAGS+=-c -g -Os -w -ffunction-sections -fdata-sections -MMD
CFLAGS+=-DARDUINO=10600 -DARDUINO_AVR_YUN -DARDUINO_ARCH_AVR
CFLAGS+=-DUSB_MANUFACTURER= -DUSB_PRODUCT="\"Arduino Yun\"" -DUSB_VID=0x2341 -DUSB_PID=0x8041

VPATH+=libraries/Parse/src/internal
VPATH+=$(SKETCH_PATH)/libraries/Bridge/src
VPATH+=$(SKETCH_PATH)/hardware/arduino/avr/cores/arduino

INCLUDE=\
-I$(SKETCH_PATH)/hardware/arduino/avr/variants/yun \
-I$(SKETCH_PATH)/hardware/arduino/avr/cores/arduino \
-I$(SKETCH_PATH)/libraries/Bridge/src \
-I./libraries/Parse/src/internal \
-I./libraries/Parse/src

ifeq (upload,$(firstword $(MAKECMDGOALS)))
  TARGET_BIN:=$(wordlist 2,$(words $(MAKECMDGOALS)),$(MAKECMDGOALS))
  $(eval $(TARGET_BIN):;@:)
endif

default: directory inofile build

directory:
	@touch Blink.cpp
	@touch QuickStart.cpp
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(EXE_DIR)

inofile: Blink.cpp QuickStart.cpp
	@cp libraries/Parse/examples/blink/Blink.ino Blink.cpp
	@cp libraries/Parse/examples/quickstart/QuickStart.ino QuickStart.cpp

BLINK_OBJS+= $(OBJ_DIR)/Blink.o
QS_OBJS+= $(OBJ_DIR)/QuickStart.o

OBJS+= $(OBJ_DIR)/ParseClient.o \
  $(OBJ_DIR)/ParseCloudFunction.o \
  $(OBJ_DIR)/ParseObjectCreate.o \
  $(OBJ_DIR)/ParseObjectDelete.o \
  $(OBJ_DIR)/ParseObjectGet.o \
  $(OBJ_DIR)/ParsePush.o \
  $(OBJ_DIR)/ParseQuery.o \
  $(OBJ_DIR)/ParseRequest.o \
  $(OBJ_DIR)/ParseResponse.o \
  $(OBJ_DIR)/ParseTrackEvent.o \
  $(OBJ_DIR)/ParseObjectUpdate.o
OBJS+= $(OBJ_DIR)/Bridge.o \
  $(OBJ_DIR)/Console.o \
  $(OBJ_DIR)/FileIO.o \
  $(OBJ_DIR)/HttpClient.o \
  $(OBJ_DIR)/Mailbox.o \
  $(OBJ_DIR)/Process.o \
  $(OBJ_DIR)/YunClient.o \
  $(OBJ_DIR)/YunServer.o

CORE_OBJS+= $(OBJ_DIR)/hooks.o \
  $(OBJ_DIR)/WInterrupts.o \
  $(OBJ_DIR)/wiring.o \
  $(OBJ_DIR)/wiring_analog.o \
  $(OBJ_DIR)/wiring_digital.o \
  $(OBJ_DIR)/wiring_pulse.o \
  $(OBJ_DIR)/wiring_shift.o
CORE_OBJS+= $(OBJ_DIR)/abi.o \
  $(OBJ_DIR)/CDC.o \
  $(OBJ_DIR)/HardwareSerial.o \
  $(OBJ_DIR)/HardwareSerial0.o \
  $(OBJ_DIR)/HardwareSerial1.o \
  $(OBJ_DIR)/HardwareSerial2.o \
  $(OBJ_DIR)/HardwareSerial3.o \
  $(OBJ_DIR)/HID.o \
  $(OBJ_DIR)/IPAddress.o \
  $(OBJ_DIR)/main.o \
  $(OBJ_DIR)/new.o \
  $(OBJ_DIR)/Print.o \
  $(OBJ_DIR)/Stream.o \
  $(OBJ_DIR)/Tone.o \
  $(OBJ_DIR)/USBCore.o \
  $(OBJ_DIR)/WMath.o \
  $(OBJ_DIR)/WString.o

build: $(EXE_DIR)/Blink.hex $(EXE_DIR)/QuickStart.hex

$(EXE_DIR)/Blink.elf: $(BLINK_OBJS) $(OBJS) $(CORE_OBJS)
	$(CC) -w -Os -Wl,--gc-sections -mmcu=atmega32u4 $^ -o $@ $(LIBS)

$(EXE_DIR)/Blink.hex: $(EXE_DIR)/Blink.elf
	$(OBJCOPY) -O ihex $< $@

$(EXE_DIR)/QuickStart.elf: $(QS_OBJS) $(OBJS) $(CORE_OBJS)
	$(CC) -w -Os -Wl,--gc-sections -mmcu=atmega32u4 $^ -o $@ $(LIBS)

$(EXE_DIR)/QuickStart.hex: $(EXE_DIR)/QuickStart.elf
	$(OBJCOPY) -O ihex $< $@

upload:
ifeq (,$(TARGET_BIN))
	@echo "Please type 'make upload Blink' or 'make upload QuickStart'"
endif

ifeq ($(PORT),)
	@echo "Please plugin Yun"
else
	@echo "Yun's serial port is $(PORT), and your target is $(TARGET_BIN)"
	$(AVRDUDE) -C$(TOOLCHAIN_PATH)/../etc/avrdude.conf -v -patmega32u4 -cavr109 -P$(PORT) -b57600 -D -Uflash:w:$(EXE_DIR)/$(TARGET_BIN).hex:i
endif

clean:
	@rm -f Blink.cpp
	@rm -f QuickStart.cpp
	@rm -rf $(EXE_DIR)
	@rm -rf $(OBJ_DIR)

$(OBJ_DIR)/%.o: %.c
	$(CC) $< $(CFLAGS) $(INCLUDE) -c -o $@

$(OBJ_DIR)/%.o: %.cpp
	$(CXX) $< $(CFLAGS) -fno-exceptions -fno-threadsafe-statics $(INCLUDE) -c -o $@

.PHONY: upload build clean

SDK:=/home/kkrosman/sdk/esp8266/esp-open-sdk/ESP8266_NONOS_SDK_V2.0.0_16_08_10
CC = xtensa-lx106-elf-gcc
CFLAGS = -DICACHE_FLASH  -I$(SDK)/driver_lib/include -I. -mlongcalls -O2
LDLIBS = -I$(SDK)/driver_lib/include -nostdlib -Wl,--start-group -DICACHE_FLASH -lmain -lnet80211 -lwpa -llwip -lpp -lphy -lc -Wl,--end-group -lgcc -L$(SDK)/lib
LDFLAGS = -Teagle.app.v6.ld

main-0x00000.bin: main
	esptool.py elf2image $^

main: main.o ws2812.o data.o driver/uart.o wifi.o storage.o

ws2812.o: ws2812.c
main.o: main.c
data.o: data.c
driver/uart.o: driver/uart.c
wifi.o: wifi.c wifi.h
storage.o: storage.c storage.h

flash: main-0x00000.bin
	../../esptool/esptool.py write_flash 0 main-0x00000.bin 0x10000 main-0x10000.bin

clean:
	rm -f main main.o main-0x00000.bin main-0x10000.bin

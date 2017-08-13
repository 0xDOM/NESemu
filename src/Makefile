CC		= g++
CFLAGS	= -I../include -g -Wall $(shell sdl2-config --cflags) $(shell sdl2-config --libs) -Wc++11-extensions

OBJ = emu.o CPU.o Memory.o Cartridge.o PPU.o APU.o Pulse.o

emu: $(OBJ)
	$(CC) $(CFLAGS) -o emu $(OBJ)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $<

clean:
	rm *.o emu

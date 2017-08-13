#ifndef MEMORY_H
#define MEMORY_H

#include <cstdint>
#include "Cartridge.h"

class Memory {
public:
	Memory(Cartridge* c);
	uint8_t read(uint16_t adress);
	void write(uint16_t adress, uint8_t value);
	Cartridge* cartridge;

	uint8_t mapper0_Read(uint16_t address);
	uint8_t mapper1_Read(uint16_t address);
	uint8_t unrom_Read(uint16_t address);
	
private:
	uint8_t _memory[0xFFFF];
	uint8_t activeBank;

	//mmc1
	uint8_t shiftReg;
	uint8_t writeCounter;
	uint8_t control;
	uint8_t chr0;
	uint8_t chr1;
	uint8_t prg;
	uint8_t bank0, bank1;
	uint8_t* _ram;

};

#endif
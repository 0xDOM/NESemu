#include "Memory.h"

Memory::Memory(Cartridge* c) : cartridge(c), bank0(0), bank1(0){
	_ram = new uint8_t[0x1FFF];
	bank1 = c->prgRomSize-1;
} 

uint8_t Memory::read(uint16_t address) {

	uint8_t result = 0;
	//std::cout << "read from addr" << (int) address << std::endl;

	if (address >= 0x6000 && address <= 0x7FFF) {
		_ram[address-0x6000];
	} else {
		if (cartridge->mapper == 2) {
			result = unrom_Read(address);
		} else if (cartridge->mapper == 1) {
			result = mapper1_Read(address);
		} else if (cartridge->mapper == 0) {
			result = mapper0_Read(address);
		}
	}

	return result;
}



void Memory::write(uint16_t adress, uint8_t value){

	if (cartridge->mapper == 2 && (adress > 0x8000)) {
		activeBank = value;	
		std::cout << "Bank " << (int) activeBank << std::endl;
	} else if (cartridge->mapper == 1) {
			if (adress >= 0x6000 && adress <= 0x7FFF) {
		_ram[adress-0x6000] = value;
	} else if (adress >= 0x8000 && adress <= 0xFFFF) {	

	if (value & 0x80) {
		writeCounter = 0;
		shiftReg = 0;
		bank1 = cartridge->prgRomSize-1;
		return;
	} else {
		shiftReg |= (value & 0x01) << (writeCounter);
		shiftReg &= 0x1F;
	}


	writeCounter++;

	
	if (writeCounter == 5) {

		uint8_t regSelect = (adress & 0x6000) >> 13;

		switch (regSelect) {
			case 0: {
				uint8_t mode = (shiftReg & 0x0C) >> 2;
				std::cout << "adsf" <<(int) mode << std::endl;
				bank1 = cartridge->prgRomSize-1;
				}
				break;
			case 1:
				std::cout << "1a" << std::endl;
				break;
			case 2:
				std::cout << "2" << std::endl;
				break;
			case 3: {
				uint8_t bank = (shiftReg & 0x0F);
				//std::cout << "Switchint to bank " << (int) bank << std::endl;
				bank0 = bank;
				}
				break;
		}
		shiftReg = 0;
		writeCounter = 0;
	}

	}

}
}

uint8_t Memory::mapper1_Read(uint16_t address) {
	uint8_t result = 0;

	// mapper 1
	if (address >= 0x8000 && address < 0x8000 + 0x4000) {
		result =  cartridge->banks[bank0][address-0x8000];
	} else if (address >= 0xC000) {
		result = cartridge->banks[bank1][address-0x8000-0x4000];
	}

	return result;
}

uint8_t Memory::unrom_Read(uint16_t address) {
	uint8_t result = 0;

	// mapper 0
	if (address >= 0x8000 && address < 0x8000 + 0x4000) {
		result =  cartridge->banks[activeBank][address-0x8000];
	} else if (address >= 0xC000) {
		result = cartridge->banks[cartridge->prgRomSize-1][address-0x8000-0x4000];
	}

	return result;
}

uint8_t Memory::mapper0_Read(uint16_t address) {

	uint8_t result = 0;

	// mapper 0
	if (address >= 0x8000 && address < 0x8000 + 0x4000) {
		result =  cartridge->banks[0][address-0x8000];
	} else if (address >= 0xC000) {
		result = cartridge->banks[1][address-0x8000-0x4000];
	}

	return result;
}

#ifndef APU_H
#define APU_H 

#include <cstdint>
#include <fstream>
#include <iostream>
#include "Pulse.h"

class APU {

private:
	int counter;
	uint16_t pulseTimer1;
	uint16_t pulseTimer2;
	Pulse pulse1;
	Pulse pulse2;
	std::ofstream _file;
	int flip;

public:
	void tick();
	void setApuReg(uint16_t addr, uint8_t value);
	APU();
};
#endif

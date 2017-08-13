#include "APU.h"

#include <iostream>

APU::APU() : counter(0) {

	_file.open("sound.dat", std::ios::binary);


}

void APU::tick() {

	pulse1.tick();
	pulse2.tick();

	counter++;

	if (counter > 20) {
		_file.put(0.5*(pulse1.getSample() + pulse2.getSample()));
		counter = 0;	
	}

}

void APU::setApuReg(uint16_t addr, uint8_t value) {

	if (addr == 0x4000) {
		int dutyCycle = (value & 0xC0) >> 6;
		int volume = (value & 0x0F);
		pulse1.setDutyCycle(dutyCycle);
		pulse1.setVolume(volume);

	} else if (addr == 0x4002) {
		pulseTimer1 = value;
	} else if (addr == 0x4003) {
		pulseTimer1 |= (value & 0x07) << 8;
		pulse1.setTimer(pulseTimer1);
	} else if (addr == 0x4004) {
		int dutyCycle = (value & 0xC0) >> 6;
		int volume = (value & 0x0F);
		pulse2.setDutyCycle(dutyCycle);
		pulse2.setVolume(volume);

	} else if (addr == 0x4006) {
		pulseTimer2 = value;
	} else if (addr == 0x4007) {
		pulseTimer2 |= (value & 0x07) << 8;
		pulse2.setTimer(pulseTimer2);
	}
	
}
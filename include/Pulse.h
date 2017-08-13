#ifndef PULSE_H
#define PULSE_H

#include <cstdint>

class Pulse {

private:
	int dutyCycles[4][8] = {
		{0, 1, 0, 0, 0, 0, 0, 0},
		{0, 1, 1, 0, 0, 0, 0, 0},
		{0, 1, 1, 1, 1, 0, 0, 0},
		{1, 0, 0, 1, 1, 1, 1, 1}
	};
	int dutyIndex;
	int counter;
	int timerValue;
	int sequenceIndex;
	uint8_t volume;
public:
	Pulse();
	void tick();
	uint8_t getSample();
	void setTimer(uint16_t timerValue);
	void setVolume(uint8_t volume);
	void setDutyCycle(int dutyIndex);

};

#endif
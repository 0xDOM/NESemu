#ifndef SQUARE_H
#define SQUARE_H

#include <cstdint>

class Square {

public:
	Square(int rate);
	void setVolume(uint8_t vol);
	void setFrequency(double frequency);
	void setDutyCycle(double dutyCycle);
	uint8_t tick();

private:
	uint8_t volume;
	int counter;
	int sampleRate;
	double dutyCycle;
	int highCounter;
	int periode;

};

#endif
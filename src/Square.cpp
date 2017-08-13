#include "Square.h"
#include <iostream>

Square::Square(int rate) : sampleRate(rate), counter(0), highCounter(0), dutyCycle(0.5), periode(0) {}

uint8_t Square::tick() 
{

	uint8_t result = 0;

	if (counter <= highCounter)
	{
		result = volume;
	}
	else 
	{
		result = 0;
	}

	counter++;

	if (counter > periode) {
		counter = 0;
	}
	return result;
}

void Square::setVolume(uint8_t vol)
{
	volume = vol;
}

void Square::setFrequency(double frequency) 
{
	periode = sampleRate / frequency;
	highCounter = periode * dutyCycle;
}
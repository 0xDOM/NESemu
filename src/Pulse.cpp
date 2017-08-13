#include "Pulse.h"

Pulse::Pulse() : dutyIndex(0), counter(0), timerValue(0), sequenceIndex(0) {}

void Pulse::tick() {

	if (counter > 0) {
		counter--;
	} else {
		counter = timerValue;
		sequenceIndex--;
		if (sequenceIndex < 0) {
			sequenceIndex = 7;
		}
	}
}

void Pulse::setDutyCycle(int dutyIndex) {
	this->dutyIndex = dutyIndex;
}

uint8_t Pulse::getSample() {
	return volume * dutyCycles[dutyIndex][sequenceIndex] * 20;
}

void Pulse::setVolume(uint8_t volume) {
	this->volume = volume;
}

void Pulse::setTimer(uint16_t timerValue) {
	this->timerValue = timerValue;
	this->counter = timerValue;
	this->sequenceIndex = 0;
}
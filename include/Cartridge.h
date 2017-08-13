#ifndef CARTRIDGE_H
#define CARTRIDGE_H

#include <string>
#include <iostream>
#include <fstream>

class Cartridge {

public:
	Cartridge(std::string filename);
	void printInfo();
	
public:
	std::ifstream _file;
	uint8_t prgRomSize;
	uint8_t chrRomSize;
	uint8_t mapper;
	
	uint8_t *rom;
	uint8_t *vrom;
	uint8_t **banks;
};

#endif
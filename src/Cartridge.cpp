#include "Cartridge.h"

Cartridge::Cartridge(std::string filename) {
	_file.open(filename, std::ios::binary);
	
	if (!_file)
		std::cerr << "No file";
	
	 if (!(_file.get() == 'N' && _file.get() == 'E' && _file.get() == 'S' && _file.get() == '\32' )) {
 		std::cerr << "Invalid file";
	 }
	 
	 prgRomSize = _file.get();
	 chrRomSize = _file.get();
	 
	 std::cout << "PRG " << (int) prgRomSize;
	 
	 // flag bytes
	 int byte6 = _file.get(); // 6
	 int byte7 = _file.get(); // 7
	 _file.get(); // 8
	 _file.get(); // 9
	 _file.get(); // 10
	 _file.get(); // 11
	 _file.get(); // 12
	 _file.get(); // 13
	 _file.get(); // 14
	if (_file.get() != 0) // 15
		std::cerr << "Error 0";
	
	
	mapper = (byte7 & 0xF0) | (byte6 >> 4);

	std::cout << "mapper " << (int) mapper << std::endl;

	std::cout << (int) prgRomSize << std::endl;
	
	rom = new uint8_t[prgRomSize * 0x4000];
	if (chrRomSize == 0) chrRomSize = 1;

	vrom = new uint8_t[chrRomSize * 0x2000];
		
	banks = new uint8_t*[prgRomSize];

	for (int i=0; i<prgRomSize; i++) {
		banks[i] = new uint8_t[0x4000];
		_file.read((char*)banks[i], 0x4000);
	}


	std::cout << (int) chrRomSize << std::endl;
	_file.read((char*)vrom, chrRomSize * 0x2000);
	
	_file.close();
}
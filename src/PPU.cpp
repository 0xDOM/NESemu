#include "PPU.h"
#include "CPU.h"
#include <iostream>
#include <iomanip>
#include "SDL.h"


uint8_t PPU::handleRead(uint16_t addr) {
	
	uint8_t retval = _readBuffer;
	
	if (addr == 0x2002) {
		_readBuffer = handlePpuStatus();
	} else if (addr == 0x2007) {
		_readBuffer = _cartridge->vrom[_vAddress];
		_vAddress++;
	}
	

//	std::cout << "reading from ppu " << addr << "  " << (int) retval << std::endl;
	
	
	return retval;

}

uint8_t PPU::internalRead(uint16_t addy) {
	if (addy <= 0x1FFF) {
		return _cartridge->vrom[addy];
	} else if (addy <= 0x2FFF) {
		// Name tables
		return _vram[(addy - 0x2000) & ~0x800];
	} else if (addy >= 0x3F00) {
		return _pal[(addy - 0x3F00) % 32];
	}
}

void PPU::flushScreen() {
	SDL_UpdateTexture(texture, NULL, pixels, 256 * sizeof(Uint32));

	SDL_RenderClear(renderer);
	        SDL_RenderCopy(renderer, texture, NULL, NULL);
	        SDL_RenderPresent(renderer);	
				



SDL_UpdateTexture(texture2, NULL, pixels2, 256 * 2 * sizeof(Uint32));

	SDL_RenderClear(renderer2);
	        SDL_RenderCopy(renderer2, texture2, NULL, NULL);
	        SDL_RenderPresent(renderer2);
}

void PPU::processPixel(uint8_t x, uint8_t y) {

	// Handle background
	//if (!_showBackground) {
	
	bool nonTransBg = false;
		
		
	if (_scrollXfine > 7) {
		_scrollXfine = 0;
		_scrollXcoarse++;

		if (_scrollXcoarse > 0x1F) {
			_scrollXcoarse = 0;
			//_scrollNt ^= 0x400;				
			
			_baseNameTableAddress ^= 0x400;



		}
	}
	
	//_scrollNt += 0x400;
	
	
		int tileNr = (_scrollXcoarse + 0x20 * (y / 8) );// + _scrollNt; //(x / 8) + _scrollXcoarse + 0x20 * (y / 8);		 
	
		int tileIndex = internalRead(_baseNameTableAddress + tileNr);//_vram[(_baseNameTableAddress - 0x2000 + tileNr) & ~0x800];
	 
		 int row = y % 8;
		 
		uint8_t t1 = _cartridge->vrom[_backgroundTableAddress+tileIndex*16 + row];
		uint8_t t2 = _cartridge->vrom[_backgroundTableAddress+tileIndex*16 + row+8];

		
		uint16_t tt = (_scrollXcoarse + 0x20 * (y / 8) );

		uint8_t attributeByte = internalRead(_baseNameTableAddress + _scrollNt + 0x3C0 + (tt % 32)/4 + 8*(tt / (32*4)));

		uint8_t attribShift = ((tt % 4) & 2) | ((((tt/32)%4)&2)<<1);


		int bitpos = 7 - _scrollXfine;//(x % 8);
		
//		if (x < _scrollXfine) {
//			bitpos = 7 - _scrollXfine - (x % 8);
//		}
		

		
		uint8_t color = (((t1 >> (bitpos))&1))|(((t2 >> (bitpos))&1)<<1);		
		
		
		uint8_t palIndex = internalRead(0x3F00);

		if (color > 0) {
			palIndex = internalRead(0x3F00 + color + 4*((attributeByte >> attribShift)&3));
			if (!nonTransBg)
				nonTransBg = true;
		}

		
		
			pixels[x + 256*y] = pal[palIndex][0] << 16 | pal[palIndex][1] << 8 | pal[palIndex][2];
		/*
		if ( color == 0 ) {
			pixels[x + 256*y] = 6579300;
		} else if (color == 1){	
			pixels[x + 256*y] = 255;
		} else if (color == 2) {
			pixels[x + 256*y] = 255<<8;
		} else {
			pixels[x + 256*y] = 255<<16;
		}
		*/
		_scrollXfine++;
		
		//}
	
		
		
	// handle sprites
	if (_showSprites) {
		
		for (int sprite=0; sprite<64; sprite++) {
			uint8_t yPos = _oam[sprite*4+0];
			uint8_t tileIndex = _oam[sprite*4+1];
			uint8_t attribs = _oam[sprite*4+2];
			uint8_t xPos = _oam[sprite*4+3];
		
		
		
			if (yPos < 0xEF && x >= xPos && x < xPos + 8 ) {
			
				uint16_t spriteAddr = 0;
				if (_spriteHeight == 16) {
													
					if (tileIndex & 1) {
						spriteAddr = (tileIndex-1)*16;
						spriteAddr += 0x1000;
					} else {
						spriteAddr = (tileIndex)*16;
					}
				} else {
					spriteAddr = _spritePatternTableAddress+tileIndex*16;
				}
	
			if (x >= xPos && x < xPos + 8 && y >= yPos && y < yPos + _spriteHeight) {
				int row = y - yPos;
				
				if (row > 7) {
					row -= 8;
					spriteAddr += 16;
				}
				
				uint8_t t1 = _cartridge->vrom[spriteAddr + row];
				uint8_t t2 = _cartridge->vrom[spriteAddr + row+8];

				
				
				int bitpos = 0;
				if (attribs & (1<<6)) {
						bitpos = x - xPos;
				} else {
					bitpos = 7 - (x - xPos);
				}


				uint8_t color = (((t1 >> (bitpos))&1))|(((t2 >> (bitpos))&1)<<1);
				uint8_t spritePal = attribs & 0x03;
				
				uint8_t palIndex = internalRead(0x3F11 + spritePal*4 + color-1);
				
				if (color > 0) {
					pixels[x + 256*y] = pal[palIndex][0] << 16 | pal[palIndex][1] << 8 | pal[palIndex][2];
					if (sprite == 0 && nonTransBg) {
						_spriteZeroHit = true;
					}
					//break;
				}
			}
		}
	}}
		
		
		
		
}

void PPU::performDMA(uint8_t* addr) {
	for (int i=0; i<256; i++) {
		_oam[i] = addr[i];
	}
}


void PPU::handleWrite(uint16_t addr, uint8_t value) {
	if (addr == 0x2000) {
		handlePpuCtrl(value);
	} else if (addr == 0x2001) {
		handlePpuMask(value);
	} else if (addr == 0x2003) {
		_oamAddress = value;
	} else if (addr == 0x2004) {
			std::cout << "Write to 2004 " << (int) value;//exit(1);
	} else if (addr == 0x2005) {
		handleScrolling(value);
	} else if (addr == 0x2006) {
		handlePpuAddress(value);
	} else if (addr == 0x2007) {
		handlePpuData(value);
	}
	
	//std::cout << "writing to ppu: " << std::hex << addr << " " << (int) value << " " << std::endl;
}

void PPU::handleScrolling(uint8_t value) {
	
	if (_nextScrollX) {
		
		_scrollXfine = value & 0x07;
		_scrollXfine_orig = value & 0x07;
		_scrollXcoarse_orig = value >> 3;		
		_nextScrollX = false;


		//std::cout << (int) _scrollXcoarse_orig << std::endl;

	} else {
		
		_nextScrollX = true;
	}
	
}


void PPU::tick() {
	
//		std::cout << "SL: " << (int) _scanline << " _x; " << (int) _x << std::endl;
	
	if (_x > 0 && _x <= 256 && _scanline < 240) {
		processPixel(_x, _scanline);
	}


	//if (_scanline == 30)
	//	_spriteZeroHit = true;

	//	if (_scanline == 261)
	//		_spriteZeroHit = false;
	
	if (_scanline  == 241 && _x == 1) {
		_vBlank = true;
				outputNameTables();

		if (_generateNmi) {
			nmi = true;
		}
	}

	if (_scanline == 261 && _x == 1) {
		_vBlank = false;	
		_spriteZeroHit = false;		

	}

	if (_x == 340) {	
		_x = 0;
		_scanline++;
		_scrollXfine = _scrollXfine_orig;
		_scrollXcoarse = _scrollXcoarse_orig;
		_baseNameTableAddress = _baseNameTableAddress_orig;
		//_scrollNt = _baseNameTableAddress & 0x400;
		if (_scanline > 261) {
			_scanline = 0;
			flushScreen();
		}		
	}
	
	_x++;

}

PPU::PPU(Cartridge* c) : _cartridge(c),
		_baseNameTableAddress(0x2000),
		_vramIncrement(1),
		_spritePatternTableAddress(0x0000),
		_backgroundTableAddress(0x0000),
		_spriteHeight(8),
		_generateNmi(true),
		_vBlank(true),
		_scanline(0),
		_vAddress(0),
		_showBackground(false),
		_showSprites(false),
		_oamAddress(0),
		_spriteZeroHit(false),
		_nextScrollX(true),
		_ppuAddressFF(true),
		_scrollXfine(0),
		_scrollXcoarse(0),
		_x(0)
		 {
			 std::cout << "created" << std::endl;
			 

		     SDL_Init(SDL_INIT_VIDEO);              // Initialize SDL2

		     // Create an application window with the following settings:
		     window = SDL_CreateWindow(
		         "NES",                  // window title
		         SDL_WINDOWPOS_UNDEFINED,           // initial x position
		         SDL_WINDOWPOS_UNDEFINED,           // initial y position
		         256,                               // width, in pixels
		         240,                               // height, in pixels
		         SDL_WINDOW_OPENGL                  // flags - see below
		     );

		     // Check that the window was successfully made
		     if (window == NULL) {
		         // In the event that the window could not be made...
		         printf("Could not create window: %s\n", SDL_GetError());
   
		     }
			 
		 	renderer = SDL_CreateRenderer(window, -1, 0);
		 	texture = SDL_CreateTexture(renderer,
		 	        SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, 256, 240);
			
		 			
							 memset(pixels, 100, 256 * 240 * sizeof(uint32_t));
			 
			




// debug
					     window2 = SDL_CreateWindow(
		         "NameTable DebugView",                  // window title
		         SDL_WINDOWPOS_UNDEFINED,           // initial x position
		         SDL_WINDOWPOS_UNDEFINED,           // initial y position
		         256*2,                               // width, in pixels
		         240,                               // height, in pixels
		         SDL_WINDOW_OPENGL                  // flags - see below
		     );
					     renderer2 = SDL_CreateRenderer(window2, -1, 0);
		 	texture2 = SDL_CreateTexture(renderer2,
		 	        SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, 2*256, 240);
			
		 			
							 memset(pixels2, 100, 2*256 * 240 * sizeof(uint32_t));

		 }

void PPU::handlePpuMask(uint8_t value) {
	
	if (value & 0x10)
		_showSprites = true;
	else
		_showSprites = false;
	
	if (value & 0x08)
		_showBackground = true;
	else
		_showBackground = false;	
}

void PPU::handlePpuAddress(uint8_t value) {
	if (_ppuAddressFF) {


		uint8_t nt = (value & 0x0C) >> 2;
		switch(nt) {
		case 0:
			_baseNameTableAddress = 0x2000;
			break;
		case 1:
			_baseNameTableAddress = 0x2400;
			break;
		case 2:
			_baseNameTableAddress = 0x2800;
			break;
		case 3:
			_baseNameTableAddress = 0x2C00;
			break;
	}
	
	_baseNameTableAddress_orig = _baseNameTableAddress;

		_ppuAddressFF = false;
	} else {
		
		_ppuAddressFF = true;	
	}
	_vAddress = (_vAddress << 8) | value;
	
}

void PPU::handlePpuData(uint8_t value) {
	if (_vAddress > 0x1FFF && _vAddress < 0x3F00) {
		_vAddress &= ~0x800;
		_vram[_vAddress - 0x2000] = value;
	} else if (_vAddress >= 0x3F00) {
		if (_vAddress == 0x3F10)
			_pal[0] = value;
		_pal[(_vAddress - 0x3F00) % 32] = value;
	} else if (_vAddress < 0x1FFF) {
		_cartridge->vrom[_vAddress] = value;
	}

	//std::cout << std::hex << _vAddress << std::endl;
	_vAddress += _vramIncrement;	
	
}

uint8_t PPU::handlePpuStatus() {
	uint8_t retval = 0;
	
	if (_vBlank) 
		retval |= 0x80;
			
	if (_spriteZeroHit) 
		retval |= 0x40;
	else
		retval &= 0xBF;
	
	// clear vlank
	_vBlank = false;
	
	return retval;
}

void PPU::handlePpuCtrl(uint8_t value) {

	switch(value & 0x03) {
		case 0:
			_baseNameTableAddress = 0x2000;
			break;
		case 1:
			_baseNameTableAddress = 0x2400;
			break;
		case 2:
			_baseNameTableAddress = 0x2800;
			break;
		case 3:
			_baseNameTableAddress = 0x2C00;
			break;
	}
	
	_baseNameTableAddress_orig = _baseNameTableAddress;
		
	if (value & 0x04)
		_vramIncrement = 32;
	else
		_vramIncrement = 1;
	
	if (value & 0x08)
		_spritePatternTableAddress = 0x1000;
	else
		_spritePatternTableAddress = 0;
	
	if (value & 0x10)
		_backgroundTableAddress = 0x1000;
	else
	 	_backgroundTableAddress = 0;
	
	if (value & 0x20)
		_spriteHeight = 16;
	else
		_spriteHeight = 8;
	
	if (value & 0x80) 
		_generateNmi = true;
	else
		_generateNmi = false;
			
}

void PPU::outputNameTables() {


uint16_t basent = 0x2000;

for (uint8_t row = 0; row < 30; row++) {

	for (uint8_t col = 0; col < 32; col++) {

uint16_t tile = row * 32 + col;

int tileIndex = internalRead(basent + tile);

		for (int j=0; j<8; j++) {

			uint8_t t1 = _cartridge->vrom[_backgroundTableAddress+tileIndex*16 + j];
			uint8_t t2 = _cartridge->vrom[_backgroundTableAddress+tileIndex*16 + j+8];
	
			for (int i=0; i<8; i++) {
	
				uint8_t attributeByte = internalRead(basent + 0x3C0 + (tile % 32)/4 + 8*(tile / (32*4)));
	
				uint8_t attribShift = ((tile % 4) & 2) | ((((tile/32)%4)&2)<<1);
				int bitpos = 7 - i;
	
			
				uint8_t color = (((t1 >> (bitpos))&1))|(((t2 >> (bitpos))&1)<<1);		
			
			
				uint8_t palIndex = internalRead(0x3F00);
	
				if (color > 0) {
					palIndex = internalRead(0x3F00 + color + 4*((attributeByte >> attribShift)&3));
				}
	
				pixels2[col * 8 + i + row * 512 * 8 + j*512] = pal[palIndex][0] << 16 | pal[palIndex][1] << 8 | pal[palIndex][2];
	
			}
		}
	}

	}



basent = 0x2400;

for (uint8_t row = 0; row < 30; row++) {

	for (uint8_t col = 0; col < 32; col++) {

uint16_t tile = row * 32 + col;

int tileIndex = internalRead(basent + tile);

		for (int j=0; j<8; j++) {

			uint8_t t1 = _cartridge->vrom[_backgroundTableAddress+tileIndex*16 + j];
			uint8_t t2 = _cartridge->vrom[_backgroundTableAddress+tileIndex*16 + j+8];
	
			for (int i=0; i<8; i++) {
	
				uint8_t attributeByte = internalRead(basent + 0x3C0 + (tile % 32)/4 + 8*(tile / (32*4)));
	
				uint8_t attribShift = ((tile % 4) & 2) | ((((tile/32)%4)&2)<<1);
				int bitpos = 7 - i;
	
			
				uint8_t color = (((t1 >> (bitpos))&1))|(((t2 >> (bitpos))&1)<<1);		
			
			
				uint8_t palIndex = internalRead(0x3F00);
	
				if (color > 0) {
					palIndex = internalRead(0x3F00 + color + 4*((attributeByte >> attribShift)&3));
				}
	
				pixels2[256 + col * 8 + i + row * 512 * 8 + j*512] = pal[palIndex][0] << 16 | pal[palIndex][1] << 8 | pal[palIndex][2];
	
			}
		}
	}

	}

	// draw frame
	

	int startX = 0;
	for (int row = 0; row < 240; row++) {
		startX =  (_baseNameTableAddress_orig == 0x2000 ? 0 : 256) + _scrollXcoarse * 8 + _scrollXfine;
		
		for (int col = 0; col < 256; col++) {
			pixels2[startX + row*512] |= 0xAA0000;

			startX = (startX+1) % 512;
		}
	}



startX = (_baseNameTableAddress_orig == 0x2000 ? 0 : 256) + _scrollXcoarse * 8 + _scrollXfine;
	for (int y=0; y<240; y++) {
		pixels2[startX + y*512] = 0xFF0000;
	}

	startX = (startX + 256) % 512;

	for (int y=0; y<240; y++) {
		pixels2[startX + y*512] = 0xFF0000;
	}

}



	
	




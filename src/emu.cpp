#include "CPU.h"
#include "Cartridge.h"
#include "PPU.h"
#include "SDL.h"

int main() {
	
	Cartridge c("mm2.nes");
	Memory m(&c);
	PPU p(&c);
	APU a;
	
	CPU cpu(&m, &p, &a);
	
	p._cpu = &cpu;
	
	for (;;) {
		cpu.processInstruction();
		if (p.nmi) {
			cpu.nmi();
			p.nmi = false;
		}
	}
		
	
}
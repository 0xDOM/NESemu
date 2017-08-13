#ifndef CPU_H
#define CPU_H 

#include <cstdint>
#include <iostream>
#include "Memory.h"
#include "PPU.h"
#include "APU.h"

struct Controller {
	int a;
	int b;
	int start;
	int select;
	int left;
	int right;
	int up;
	int down;
};

class CPU {

public:
	CPU(Memory* m, PPU* p, APU* apu);
	void tick();
	void processInstruction();
	
	void nmi();
	
		
private:

int controllerCounter;
int blub;
Controller controller1;

	// CPU Registers
	uint16_t _pc;
	uint8_t _sp;
	uint8_t _ac;
	uint8_t _x;
	uint8_t _y;
	
	// CPU Flags
	bool _carry;
	bool _zero;
	bool _interruptDisable;
	bool _decimalMode;
	bool _breakCmd;
	bool _overflow;
	bool _negative;
	
	uint8_t Ram[0x800];
	Memory* _memory;
	PPU* _ppu;
	APU* _apu;
	
	uint8_t readByte(uint16_t addr);
	void writeByte(uint16_t addr, uint8_t value);
	void push(uint8_t value);
	uint8_t pop();
	
	void lda(uint16_t addr);
	void ldx(uint16_t addr);
	void ldy(uint16_t addr);
	
	void sta(uint16_t addr);
	void stx(uint16_t addr);
	void sty(uint16_t addr);
	
	void land(uint16_t addr);
	void lor(uint16_t addr);
	void lxor(uint16_t addr);
	void lbit(uint16_t addr);

	void cmp(uint16_t addr);
	void cpx(uint16_t addr);
	void cpy(uint16_t addr);

	void adc(uint16_t addr);
	void sbc(uint16_t addr);
	void asl();
	void asl_mem(uint16_t addr);
	void lsr();
	void lsr_mem(uint16_t addr);
	void rol_mem(uint16_t addr);
	void rol();
	void ror_mem(uint16_t addr);
	void ror();
	
	void inc(uint16_t addr);
	void dec(uint16_t addr);
	
	void jsr(uint16_t addr);
	void jmp(uint16_t addr);
	
	void brk();
	void rts();
	void rti();
	void php();
	void plp();
	void pha();
	void pla();
	
	void inx();
	void dex();
	void iny();
	void dey();
	void tax();
	void txa();
	void tay();
	void tya();
	void tsx();
	void txs();
	
	void sed();
	void cld();
	void sei();
	void cli();
	void sec();
	void clc();
	void clv();
	
	void beq(uint16_t addr);
	void bne(uint16_t addr);
	void bmi(uint16_t addr);
	void bpl(uint16_t addr);
	void bcc(uint16_t addr);
	void bcs(uint16_t addr);		
	void bvc(uint16_t addr);
	void bvs(uint16_t addr);
	void branch(uint16_t addr, bool doBranch);
	

	void handleInput();
	
	
//	void updateCarry(uint8_t value);
	void updateZero(uint8_t value);
	void updateNegative(uint8_t value);
	
	uint8_t getCpuStatusReg();
	void setCpuStatusReg(uint8_t status);
	
	
	
	std::string decode(uint8_t cmd) {
		if (cmd == 0xA9) return "LDA_IMM";
		if (cmd == 0xA2) return "LDX_IMM";
		if (cmd == 0xA0) return "LDY_IMM";
		if (cmd == 0x29) return "AND_IMM";
		if (cmd == 0x09) return "ORA_IMM";
		if (cmd == 0x49) return "EOR_IMM";
		if (cmd == 0xC9) return "CMP_IMM";
		if (cmd == 0xE0) return "CPX_IMM";
		if (cmd == 0xC0) return "CPY_IMM";
		if (cmd == 0x69) return "ADC_IMM";
		if (cmd == 0xE9) return "SBC_IMM";
		if (cmd == 0xA5) return "LDA_ZPAGE";
		if (cmd == 0x85) return "STA_ZPAGE";
		if (cmd == 0xA6) return "LDX_ZPAGE";
		if (cmd == 0x86) return "STX_ZPAGE";
		if (cmd == 0xA4) return "LDY_ZPAGE";
		if (cmd == 0x84) return "STY_ZPAGE";
		if (cmd == 0x25) return "AND_ZPAGE";
		if (cmd == 0x05) return "ORA_ZPAGE";
		if (cmd == 0x45) return "EOR_ZPAGE";
		if (cmd == 0x24) return "BIT_ZPAGE";
		if (cmd == 0xC5) return "CMP_ZPAGE";
		if (cmd == 0xE4) return "CPX_ZPAGE";
		if (cmd == 0xC4) return "CPY_ZPAGE";
		if (cmd == 0x65) return "ADC_ZPAGE";
		if (cmd == 0xE5) return "SBC_ZPAGE";
		if (cmd == 0x06) return "ASL_ZPAGE";
		if (cmd == 0x46) return "LSR_ZPAGE";
		if (cmd == 0x26) return "ROL_ZPAGE";
		if (cmd == 0x66) return "ROR_ZPAGE";
		if (cmd == 0xE6) return "INC_ZPAGE";
		if (cmd == 0xC6) return "DEC_ZPAGE";
		if (cmd == 0xB5) return "LDA_ZPAGEX";
		if (cmd == 0x95) return "STA_ZPAGEX";
		if (cmd == 0xB4) return "LDY_ZPAGEX";
		if (cmd == 0x94) return "STY_ZPAGEX";
		if (cmd == 0x35) return "AND_ZPAGEX";
		if (cmd == 0x15) return "ORA_ZPAGEX";
		if (cmd == 0x55) return "EOR_ZPAGEX";
		if (cmd == 0xD5) return "CMP_ZPAGEX";
		if (cmd == 0x75) return "ADC_ZPAGEX";
		if (cmd == 0xF5) return "SBC_ZPAGEX";
		if (cmd == 0x16) return "ASL_ZPAGEX";
		if (cmd == 0x56) return "LSR_ZPAGEX";
		if (cmd == 0x36) return "ROL_ZPAGEX";
		if (cmd == 0x76) return "ROR_ZPAGEX";
		if (cmd == 0xF6) return "INC_ZPAGEX";
		if (cmd == 0xD6) return "DEC_ZPAGEX";
		if (cmd == 0xAD) return "LDA_ABS";
		if (cmd == 0x8D) return "STA_ABS";
		if (cmd == 0xAE) return "LDX_ABS";
		if (cmd == 0x8E) return "STX_ABS";
		if (cmd == 0xAC) return "LDY_ABS";
		if (cmd == 0x8C) return "STY_ABS";
		if (cmd == 0x2D) return "AND_ABS";
		if (cmd == 0x0D) return "ORA_ABS";
		if (cmd == 0x4D) return "EOR_ABS";
		if (cmd == 0x2C) return "BIT_ABS";
		if (cmd == 0xCD) return "CMP_ABS";
		if (cmd == 0xEC) return "CPX_ABS";
		if (cmd == 0xCC) return "CPY_ABS";
		if (cmd == 0x6D) return "ADC_ABS";
		if (cmd == 0xED) return "SBC_ABS";
		if (cmd == 0x0E) return "ASL_ABS";
		if (cmd == 0x4E) return "LSR_ABS";
		if (cmd == 0x2E) return "ROL_ABS";
		if (cmd == 0x6E) return "ROR_ABS";
		if (cmd == 0xEE) return "INC_ABS";
		if (cmd == 0xCE) return "DEC_ABS";
		if (cmd == 0x4C) return "JMP_ABS";
		if (cmd == 0x20) return "JSR_ABS";
		if (cmd == 0xBD) return "LDA_ABSX";
		if (cmd == 0x9D) return "STA_ABSX";
		if (cmd == 0xBC) return "LDY_ABSX";
		if (cmd == 0x3D) return "AND_ABSX";
		if (cmd == 0x1D) return "ORA_ABSX";
		if (cmd == 0x5D) return "EOR_ABSX";
		if (cmd == 0xDD) return "CMP_ABSX";
		if (cmd == 0x7D) return "ADC_ABSX";
		if (cmd == 0xFD) return "SBC_ABSX";
		if (cmd == 0x1E) return "ASL_ABSX";
		if (cmd == 0x5E) return "LSR_ABSX";
		if (cmd == 0x3E) return "ROL_ABSX";
		if (cmd == 0x7E) return "ROR_ABSX";
		if (cmd == 0xFE) return "INC_ABSX";
		if (cmd == 0xDE) return "DEC_ABSX";
		if (cmd == 0xB9) return "LDA_ABSY";
		if (cmd == 0x99) return "STA_ABSY";
		if (cmd == 0xBE) return "LDX_ABSY";
		if (cmd == 0x39) return "AND_ABSY";
		if (cmd == 0x19) return "ORA_ABSY";
		if (cmd == 0x59) return "EOR_ABSY";
		if (cmd == 0xD9) return "CMP_ABSY";
		if (cmd == 0x79) return "ADC_ABSY";
		if (cmd == 0xF9) return "SBC_ABSY";
		if (cmd == 0xA1) return "LDA_INDX";
		if (cmd == 0x81) return "STA_INDX";
		if (cmd == 0x21) return "AND_INDX";
		if (cmd == 0x01) return "ORA_INDX";
		if (cmd == 0x41) return "EOR_INDX";
		if (cmd == 0xC1) return "CMP_INDX";
		if (cmd == 0x61) return "ADC_INDX";
		if (cmd == 0xE1) return "SBC_INDX";
		if (cmd == 0xB1) return "LDA_INDY";
		if (cmd == 0x91) return "STA_INDY";
		if (cmd == 0x31) return "AND_INDY";
		if (cmd == 0x11) return "ORA_INDY";
		if (cmd == 0x51) return "EOR_INDY";
		if (cmd == 0xD1) return "CMP_INDY";
		if (cmd == 0x71) return "ADC_INDY";
		if (cmd == 0xF1) return "SBC_INDY";
		if (cmd == 0xB6) return "LDX_ZPAGEY";
		if (cmd == 0x96) return "STX_ZPAGEY";
		if (cmd == 0x0A) return "ASL_ACCUM";
		if (cmd == 0x4A) return "LSR_ACCUM";
		if (cmd == 0x2A) return "ROL_ACCUM";
		if (cmd == 0x6A) return "ROR_ACCUM";
		if (cmd == 0x6C) return "JMP_INDIRECT_CODE1"; 
		if (cmd == 0xFF) return "JMP_INDIRECT_CODE2";
		if (cmd == 0x00) return "BRK";
		if (cmd == 0x60) return "RTS";
		if (cmd == 0x40) return "RTI";
		if (cmd == 0x08) return "PHP";
		if (cmd == 0x28) return "PLP";
		if (cmd == 0x48) return "PHA";
		if (cmd == 0x68) return "PLA";
		if (cmd == 0xE8) return "INX";
		if (cmd == 0xCA) return "DEX";
		if (cmd == 0xC8) return "INY";
		if (cmd == 0x88) return "DEY";
		if (cmd == 0xAA) return "TAX";
		if (cmd == 0x8A) return "TXA";
		if (cmd == 0xA8) return "TAY";
		if (cmd == 0x98) return "TYA";
		if (cmd == 0xBA) return "TSX";
		if (cmd == 0x9A) return "TXS";
		if (cmd == 0xF8) return "SED";
		if (cmd == 0xD8) return "CLD";
		if (cmd == 0x78) return "SEI";
		if (cmd == 0x58) return "CLI";
		if (cmd == 0x38) return "SEC";
		if (cmd == 0x18) return "CLC";
		if (cmd == 0xB8) return "CLV";
		if (cmd == 0xF0) return "BEQ";
		if (cmd == 0xD0) return "BNE";
		if (cmd == 0x30) return "BMI";
		if (cmd == 0x10) return "BPL";
		if (cmd == 0x90) return "BCC";
		if (cmd == 0xB0) return "BCS";
		if (cmd == 0x50) return "BVC";
		if (cmd == 0x70) return "BVS";
		if (cmd == 0xEA) return "NOP";
	}
};


#define LDA_IMM 0xA9
#define LDX_IMM 0xA2
#define LDY_IMM 0xA0
#define AND_IMM 0x29
#define ORA_IMM 0x09
#define EOR_IMM 0x49
#define CMP_IMM 0xC9
#define CPX_IMM 0xE0
#define CPY_IMM 0xC0
#define ADC_IMM 0x69
#define SBC_IMM 0xE9
#define LDA_ZPAGE 0xA5
#define STA_ZPAGE 0x85
#define LDX_ZPAGE 0xA6
#define STX_ZPAGE 0x86
#define LDY_ZPAGE 0xA4
#define STY_ZPAGE 0x84
#define AND_ZPAGE 0x25
#define ORA_ZPAGE 0x05
#define EOR_ZPAGE 0x45
#define BIT_ZPAGE 0x24
#define CMP_ZPAGE 0xC5
#define CPX_ZPAGE 0xE4
#define CPY_ZPAGE 0xC4
#define ADC_ZPAGE 0x65
#define SBC_ZPAGE 0xE5
#define ASL_ZPAGE 0x06
#define LSR_ZPAGE 0x46
#define ROL_ZPAGE 0x26
#define ROR_ZPAGE 0x66
#define INC_ZPAGE 0xE6
#define DEC_ZPAGE 0xC6
#define LDA_ZPAGEX 0xB5
#define STA_ZPAGEX 0x95
#define LDY_ZPAGEX 0xB4
#define STY_ZPAGEX 0x94
#define AND_ZPAGEX 0x35
#define ORA_ZPAGEX 0x15
#define EOR_ZPAGEX 0x55
#define CMP_ZPAGEX 0xD5
#define ADC_ZPAGEX 0x75
#define SBC_ZPAGEX 0xF5
#define ASL_ZPAGEX 0x16
#define LSR_ZPAGEX 0x56
#define ROL_ZPAGEX 0x36
#define ROR_ZPAGEX 0x76
#define INC_ZPAGEX 0xF6
#define DEC_ZPAGEX 0xD6
#define LDA_ABS 0xAD
#define STA_ABS 0x8D
#define LDX_ABS 0xAE
#define STX_ABS 0x8E
#define LDY_ABS 0xAC
#define STY_ABS 0x8C
#define AND_ABS 0x2D
#define ORA_ABS 0x0D
#define EOR_ABS 0x4D
#define BIT_ABS 0x2C
#define CMP_ABS 0xCD
#define CPX_ABS 0xEC
#define CPY_ABS 0xCC
#define ADC_ABS 0x6D
#define SBC_ABS 0xED
#define ASL_ABS 0x0E
#define LSR_ABS 0x4E
#define ROL_ABS 0x2E
#define ROR_ABS 0x6E
#define INC_ABS 0xEE
#define DEC_ABS 0xCE
#define JMP_ABS 0x4C
#define JSR_ABS 0x20
#define LDA_ABSX 0xBD
#define STA_ABSX 0x9D
#define LDY_ABSX 0xBC
#define AND_ABSX 0x3D
#define ORA_ABSX 0x1D
#define EOR_ABSX 0x5D
#define CMP_ABSX 0xDD
#define ADC_ABSX 0x7D
#define SBC_ABSX 0xFD
#define ASL_ABSX 0x1E
#define LSR_ABSX 0x5E
#define ROL_ABSX 0x3E
#define ROR_ABSX 0x7E
#define INC_ABSX 0xFE
#define DEC_ABSX 0xDE
#define LDA_ABSY 0xB9
#define STA_ABSY 0x99
#define LDX_ABSY 0xBE
#define AND_ABSY 0x39
#define ORA_ABSY 0x19
#define EOR_ABSY 0x59
#define CMP_ABSY 0xD9
#define ADC_ABSY 0x79
#define SBC_ABSY 0xF9
#define LDA_INDX 0xA1
#define STA_INDX 0x81
#define AND_INDX 0x21
#define ORA_INDX 0x01
#define EOR_INDX 0x41
#define CMP_INDX 0xC1
#define ADC_INDX 0x61
#define SBC_INDX 0xE1
#define LDA_INDY 0xB1
#define STA_INDY 0x91
#define AND_INDY 0x31
#define ORA_INDY 0x11
#define EOR_INDY 0x51
#define CMP_INDY 0xD1
#define ADC_INDY 0x71
#define SBC_INDY 0xF1
#define LDX_ZPAGEY 0xB6
#define STX_ZPAGEY 0x96
#define ASL_ACCUM 0x0A
#define LSR_ACCUM 0x4A
#define ROL_ACCUM 0x2A
#define ROR_ACCUM 0x6A
#define JMP_INDIRECT_CODE1 0x6C 
#define JMP_INDIRECT_CODE2 0xFF
#define BRK 0x00
#define RTS 0x60
#define RTI 0x40
#define PHP 0x08
#define PLP 0x28
#define PHA 0x48
#define PLA 0x68
#define INX 0xE8
#define DEX 0xCA
#define INY 0xC8
#define DEY 0x88
#define TAX 0xAA
#define TXA 0x8A
#define TAY 0xA8
#define TYA 0x98
#define TSX 0xBA
#define TXS 0x9A
#define SED 0xF8
#define CLD 0xD8
#define SEI 0x78
#define CLI 0x58
#define SEC 0x38
#define CLC 0x18
#define CLV 0xB8
#define BEQ 0xF0
#define BNE 0xD0
#define BMI 0x30
#define BPL 0x10
#define BCC 0x90
#define BCS 0xB0
#define BVC 0x50
#define BVS 0x70
#define NOP 0xEA

#endif
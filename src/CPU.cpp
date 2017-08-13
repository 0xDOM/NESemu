#include "CPU.h"



CPU::CPU(Memory* m, PPU* p, APU* apu) : 
 _pc(0x8000), _sp(0xFF), _ac(0), _x(0), _y(0),
 _carry(false), _zero(false), _interruptDisable(false), _decimalMode(false), 
 _breakCmd(false), _overflow(false), _negative(false), _memory(m), _ppu(p), _apu(apu)
	{
		
		_pc = readByte(0xFFFC) | (readByte(0xFFFD) << 8);

	};
 
void CPU::tick() {
	_ppu->tick();
	_ppu->tick();
	_ppu->tick();

static bool flip = true;
	if (flip) {
		_apu->tick();
		flip = false;	
	} else {
		flip = true;
	}

	static long myc = 0;
	if (myc++ % 10000 == 0)
	handleInput();
}

void CPU::processInstruction() {
	
//	std::cout << std::hex;
	
//		std::cout << "ac: " << (int) _ac << "  y:" << (int) _y << std::endl;
	
			//	std::cout << _pc << "   ";
	
	uint8_t instr = readByte(_pc++);
	tick();
	
	//	std::cout << (int) _pc << " " << (int) instr << "  " << decode(instr) << std::endl;
	
	uint16_t address = 0;
	
	// First, provide arguments to selected instruction
	switch(instr) {
		// Immediate memory access
		case LDA_IMM: case LDX_IMM: case LDY_IMM: case AND_IMM: case ORA_IMM: case EOR_IMM: 
		case CMP_IMM: case CPX_IMM: case CPY_IMM: case ADC_IMM: case SBC_IMM:
		{
			address = _pc++;
		}
			break;
			
		// Load via Zero Page
		case LDA_ZPAGE: case STA_ZPAGE: case LDX_ZPAGE: case STX_ZPAGE: case LDY_ZPAGE: case STY_ZPAGE: 
		case AND_ZPAGE: case ORA_ZPAGE: case EOR_ZPAGE: case BIT_ZPAGE: case CMP_ZPAGE: case CPX_ZPAGE: 
		case CPY_ZPAGE: case ADC_ZPAGE: case SBC_ZPAGE: case ASL_ZPAGE: case LSR_ZPAGE: case ROL_ZPAGE: 
		case ROR_ZPAGE: case INC_ZPAGE: case DEC_ZPAGE:
		{	
			address = readByte(_pc++);
		}
			break;
		
		case LDA_ZPAGEX: case STA_ZPAGEX: case LDY_ZPAGEX: case STY_ZPAGEX: case AND_ZPAGEX: case ORA_ZPAGEX: 
		case EOR_ZPAGEX: case CMP_ZPAGEX: case ADC_ZPAGEX: case SBC_ZPAGEX: case ASL_ZPAGEX: case LSR_ZPAGEX: 
		case ROL_ZPAGEX: case ROR_ZPAGEX: case INC_ZPAGEX: case DEC_ZPAGEX:
		{
			address = readByte(_pc++) + _x;
		}
			break;
			
		case LDA_ABS: case STA_ABS: case LDX_ABS: case STX_ABS: case LDY_ABS: case STY_ABS: case AND_ABS: 
		case ORA_ABS: case EOR_ABS: case BIT_ABS: case CMP_ABS: case CPX_ABS: case CPY_ABS: case ADC_ABS: 
		case SBC_ABS: case ASL_ABS: case LSR_ABS: case ROL_ABS: case ROR_ABS: case INC_ABS: case DEC_ABS: 
		case JSR_ABS: case JMP_ABS:
		{
			address = readByte(_pc++) | (readByte(_pc++) << 8);
		}
			break;
			
		case LDA_ABSX: case STA_ABSX: case LDY_ABSX: case AND_ABSX: case ORA_ABSX: case EOR_ABSX: 
		case CMP_ABSX: case ADC_ABSX: case SBC_ABSX: case ASL_ABSX: case LSR_ABSX: case ROL_ABSX: 
		case ROR_ABSX: case INC_ABSX: case DEC_ABSX:
		{
			uint16_t startpage = readByte(_pc++) | (readByte(_pc++) << 8);
			address = startpage + _x;
			// page crossing?
			if ((address & 0xFF00) != (startpage & 0xFF00))
				tick();
		}
			break;
			
		case LDA_ABSY: case STA_ABSY: case LDX_ABSY: case AND_ABSY: case ORA_ABSY: case EOR_ABSY: 
		case CMP_ABSY: case ADC_ABSY: case SBC_ABSY:
		{
			uint16_t startpage = readByte(_pc++) | (readByte(_pc++) << 8);
			address = startpage + _y;
			// page crossing?
			if ((address & 0xFF00) != (startpage & 0xFF00))
				tick();
		}
			break;
			
		case LDA_INDX: case STA_INDX: case AND_INDX: case ORA_INDX: case EOR_INDX: case CMP_INDX: 
		case ADC_INDX: case SBC_INDX:
		{
			uint8_t row = (readByte(_pc++) + _x) % 0xFF;
			address = (readByte(row)) | (readByte(row+1) << 8);
		}
			break;
			
		case LDA_INDY: case STA_INDY: case AND_INDY: case ORA_INDY: case EOR_INDY: case CMP_INDY: 
		case ADC_INDY: case SBC_INDY:
		{
			uint8_t zpageaddr = readByte(_pc++);
			uint16_t startpage = readByte(zpageaddr) | (readByte(zpageaddr+1) << 8);
			address = startpage + _y;
			// page crossing?
			if ((startpage & 0xFF00) != (address & 0xFF00))
				tick();
		}
			break;
			
		case LDX_ZPAGEY: case STX_ZPAGEY:
		{
			address = readByte(_pc++) + _y;
		}
			break;
			
		case ASL_ACCUM: case LSR_ACCUM: case ROL_ACCUM: case ROR_ACCUM:
		{
			tick();
		}
			break;
			
		case JMP_INDIRECT_CODE1: case JMP_INDIRECT_CODE2:
		{
			uint8_t lowIndirect = readByte(_pc++);
			uint8_t highIndirect = readByte(_pc++);
		
			uint8_t lowAddr = lowIndirect | (highIndirect << 8);
			lowIndirect++;
			uint8_t highAddr = lowIndirect | (highIndirect << 8);
			
			address = readByte(lowAddr) | (readByte(highAddr) << 8);
		}
			break;
			
		case BRK: case RTS: case RTI: case PHP: case PLP: case PHA: case PLA: case INX: case DEX: case INY: 
		case DEY: case TAX: case TXA: case TAY: case TYA: case TSX: case TXS: case SED: case CLD: case SEI: 
		case CLI: case SEC: case CLC: case CLV: case NOP:
		{
			tick();
		}
			break;
			
		case BEQ: case BNE: case BMI: case BPL: case BCC: case BCS: case BVC: case BVS:
		{
			address = _pc++;
		}
			break;
			
		default:
		{
			std::cerr << "Invalid instruction!" << std::endl;
			exit(1);
		}
	}
	
	switch (instr) {
		case LDA_INDY: case LDA_IMM: case LDA_ZPAGE: case LDA_ZPAGEX: case LDA_ABS: case LDA_ABSX: case LDA_ABSY: 
		case LDA_INDX:
		{
			lda(address);
		}
			break;
			
		case STA_INDY: case STA_ZPAGE: case STA_ZPAGEX: case STA_ABS: case STA_ABSX: case STA_ABSY: case STA_INDX:
		{
			sta(address);
		}
			break;
			
		case LDX_IMM: case LDX_ZPAGE: case LDX_ABS: case LDX_ABSY: case LDX_ZPAGEY:
		{
			ldx(address);
		}
			break;

		case STX_ZPAGE: case STX_ABS: case STX_ZPAGEY:
		{
			stx(address);
		}
		break;
		
		case LDY_IMM: case LDY_ZPAGE: case LDY_ZPAGEX: case LDY_ABS: case LDY_ABSX:
		{
			ldy(address);
		}
			break;
			
		case STY_ZPAGE: case STY_ZPAGEX: case STY_ABS:
		{
			sty(address);
		}
		break;
		
		case AND_INDY: case AND_IMM: case AND_ZPAGE: case AND_ZPAGEX: case AND_ABS: 
		case AND_ABSX: case AND_ABSY: case AND_INDX:
		{
			land(address);
		}
		break;
		
		case ORA_INDY: case ORA_IMM: case ORA_ZPAGE: case ORA_ZPAGEX: case ORA_ABS: 
		case ORA_ABSX: case ORA_ABSY: case ORA_INDX:
		{
			lor(address);
		}
		break;
		
		case EOR_INDY: case EOR_IMM: case EOR_ZPAGE: case EOR_ZPAGEX: case EOR_ABS: case EOR_ABSX: 
		case EOR_ABSY: case EOR_INDX:
		{
			lxor(address);
		}
		break;
		
		case BIT_ZPAGE: case BIT_ABS:
		{
			lbit(address);
		}
		break;
		
		case CMP_INDY: case CMP_IMM: case CMP_ZPAGE: case CMP_ZPAGEX: case CMP_ABS: case CMP_ABSX: 
		case CMP_ABSY: case CMP_INDX:
		{
			cmp(address);
		}
		break;
		
		case CPX_IMM: case CPX_ZPAGE: case CPX_ABS:
		{
			cpx(address);
		}
		break;
		
		case CPY_IMM: case CPY_ZPAGE: case CPY_ABS:
		{
			cpy(address);
		}
		break;
		
		case ADC_INDY: case ADC_IMM: case ADC_ZPAGE: case ADC_ZPAGEX: case ADC_ABS: case ADC_ABSX: 
		case ADC_ABSY: case ADC_INDX:
		{
			adc(address);
		}	
		break;
		
		case SBC_INDY: case SBC_IMM: case SBC_ZPAGE: case SBC_ZPAGEX:
		case SBC_ABS: case SBC_ABSX: case SBC_ABSY: case SBC_INDX:
		{
			sbc(address);
		}
		break;
		
		case ASL_ZPAGE: case ASL_ZPAGEX: case ASL_ABS: case ASL_ABSX:
		{
			asl_mem(address);
		}
		break;
		
		case LSR_ZPAGE: case LSR_ZPAGEX: case LSR_ABS: case LSR_ABSX:
		{
			lsr_mem(address);
		}	
		break;
		
		case ROL_ZPAGE: case ROL_ZPAGEX: case ROL_ABS: case ROL_ABSX:
		{
			rol_mem(address);
		}
		break;
		
		case ROR_ZPAGE: case ROR_ZPAGEX: case ROR_ABS: case ROR_ABSX:
		{
			ror_mem(address);
		}
		break;
		
		case INC_ZPAGE: case INC_ZPAGEX: case INC_ABS: case INC_ABSX:
		{
			inc(address);
		}
		break;
		
		case DEC_ZPAGE: case DEC_ZPAGEX: case DEC_ABS: case DEC_ABSX:
		{
			dec(address);
		}
		break;
		
		case JSR_ABS:
		{
			jsr(address);
		}
		break;
		
		case JMP_ABS:
		case JMP_INDIRECT_CODE1:
		case JMP_INDIRECT_CODE2:
		
		{
			jmp(address);
		}
		break;
		
		case ASL_ACCUM:
		{
			asl();
		}
		break;
		
		case LSR_ACCUM:
		{
			lsr();
		}
		break;
		
		case ROL_ACCUM:
		{
			rol();
		}
		break;
		
		case ROR_ACCUM:
		{
			ror();
		}
		break;
		
		case BRK:
		{
			brk();
		}
		break;
		
		case RTS: 
		{
			rts();
		}
		break;
		
		case RTI: 
		{
			rti();
		}
		break;
		
		case PHP:
		{
			php();
		}
		break;
		
		case PLP:
		{
			plp();
		}
		break;
		
		case PHA:
		{
			pha();
		}
		break;
		
		case PLA:
		{
			pla();
		}
		break;
		
		case INX:
		{
			inx();
		}
		break;
		
		case DEX:
		{
			dex();
		}
		break;
		
		case INY:
		{
			iny();
		}
		break;
		
		case DEY:
		{
			dey();
		}
		break;
		
		case TAX:
		{
			tax();
		}
		break;
		
		case TXA:
		{
			txa();
		}
		break;
		
		case TAY:
		{
			tay();
		}
		break;
		
		case TYA:
		{
			tya();
		}
		break;
		
		case TSX:
		{
			tsx();
		}
		break;
		
		case TXS:
		{
			txs();
		}
		break;
		
		case SED:
		{
			sed();
		}
		break;
		
		case CLD:
		{
			cld();
		}
		break;
		
		case SEI:
		{
			sei();
		}
		break;
		
		case CLI:
		{
			cli();
		}
		break;
		
		case SEC: 
		{
			sec();
		}
		break;
		
		case CLC:
		{
			clc();
		}
		break;
		
		case CLV:
		{
			clv();
		}
		break;
		
		case BEQ:
		{
			beq(address);
		}
		break;
		
		case BNE:
		{
			bne(address);
		}
		break;
		
		case BMI:
		{
			bmi(address);
		}
		break;
		
		case BPL:
		{
			bpl(address);
		}
		break;
		
		case BCC:
		{
			bcc(address);
		}
		break;
		
		case BCS:
		{
			bcs(address);
		}
		break;
		
		case BVC:
		{
			bvc(address);
		}
		break;
		
		case BVS:
		{
			bvs(address);
		}
		break;
		
		case NOP:
		break;
		
	}
}

void CPU::bvc(uint16_t addr) {
	branch(addr, !_overflow);
}

void CPU::bvs(uint16_t addr) {
	branch(addr, _overflow);
}


void CPU::bcs(uint16_t addr) {
	branch(addr, _carry);
}

void CPU::bcc(uint16_t addr) {
	branch(addr, !_carry);
}

void CPU::bpl(uint16_t addr) {
	branch(addr, !_negative);
}

void CPU::branch(uint16_t addr, bool doBranch) {
	int8_t data = readByte(addr);
		
		
		
	if (doBranch) {
		tick();
//		std::cout << "branching to " << (int) data << "  " << (int) (_pc + data) << std::endl;
		if ((_pc & 0xFF00) != ((_pc + data) & 0xFF00))
			tick();
		
//		std::cout << "X:" << (int)_x << "  Y:" << (int)_y << "  AC:" << (int)_ac << std::endl;
		
		_pc += data;
	}		
}


void CPU::bmi(uint16_t addr) {
	branch(addr, _negative);
}

void CPU::bne(uint16_t addr) {
	branch(addr, !_zero);
}

void CPU::beq(uint16_t addr) {
	branch(addr, _zero);
}

void CPU::clv() {
	_overflow = false;
}

void CPU::sec() {
	_carry = true;
}

void CPU::clc() {
	_carry = false;
}

void CPU::sei() {
	_interruptDisable = true;
}

void CPU::cli() {
	_interruptDisable = false;
}

void CPU::cld() {
	_decimalMode = false;
}

void CPU::sed() {
	_decimalMode = true;
}

void CPU::txs() {
	_sp = _x;
}

void CPU::tsx() {
	_x = _sp;
	updateZero(_x);
	updateNegative(_x);	
}

void CPU::tya() {
	_ac = _y;
	updateZero(_ac);
	updateNegative(_ac);	
}

void CPU::tay() {
	_y = _ac;
	updateZero(_y);
	updateNegative(_y);	
}

void CPU::txa() {
	_ac = _x;
	updateZero(_ac);
	updateNegative(_ac);	
}

void CPU::tax() {
	_x = _ac;
	updateZero(_x);
	updateNegative(_x);	
}

void CPU::dey() {
	_y--;
	updateZero(_y);
	updateNegative(_y);
}

void CPU::iny() {
	_y++;
	updateZero(_y);
	updateNegative(_y);
}

void CPU::dex() {
	_x--;
	updateZero(_x);
	updateNegative(_x);
}

void CPU::inx() {
	_x++;
	updateZero(_x);
	updateNegative(_x);
}

void CPU::pla() {
	tick();
	_ac = pop();
	updateZero(_ac);
	updateNegative(_ac);
}

void CPU::pha() {
	push(_ac);
}

void CPU::plp() {
	tick();
	setCpuStatusReg(pop());
}

void CPU::php() {
	// status pushed on stack has break flag set
	push(getCpuStatusReg() | (1<<4));
}

uint8_t CPU::getCpuStatusReg() {
	
	uint8_t status = 0;
	
	if (_carry) 
		status |= 1;
	
	if (_zero)
		status |= 1 << 1;
	
	if (_interruptDisable)
		status |= 1 << 2;
	
	if (_decimalMode)
		status |= 1 << 3;
	
	if (_breakCmd)
		status |= 1 << 4;
	
	if (_overflow)
		status |= 1 << 6;
	
	if (_negative)
		status |= 1 << 7;
	
	return status;
}

void CPU::setCpuStatusReg(uint8_t status) {
	
	if (status & 1)
		_carry = true;
	else 
		_carry = false;
	
	if (status & (1<<1))
		_zero = true;
	else
		_zero = false;
	
	if (status & (1<<2))
		_interruptDisable = true;
	else
		_interruptDisable = false;
	
	if (status & (1<<3))
		_decimalMode = true;
	else
		_decimalMode = false;
	
	if (status & (1<<4))
		_breakCmd = true;
	else 
		_breakCmd = false;
	
	if (status & (1<<6))
		_overflow = true;
	else
		_overflow = false;
	
	if (status & (1<<7))
		_negative = true;
	else
		_negative = false;
}

void CPU::rti() {
	
	setCpuStatusReg(pop());
	_pc = pop() | (pop() << 8);
	tick();
	
}

void CPU::rts() {
	_pc = pop() | (pop() << 8);
	_pc++;
	tick();
	tick();
}

void CPU::brk() {

	_breakCmd = true;	
	_interruptDisable = true;
	
	// skip "useless" argument
	_pc++;
	push((_pc) >> 8);
	push((_pc));
	push(getCpuStatusReg()),
	
	_pc = readByte(0xFFFF) | (readByte(0xFFFE) << 8);
	
}

void CPU::nmi() {
	push((_pc) >> 8);
	push((_pc));
	push(getCpuStatusReg()),
	_pc = readByte(0xFFFA) | (readByte(0xFFFB) << 8);
	
}

void CPU::ror() {
	uint16_t result = _ac >> 1;
	if (_carry) {
		result |= 0x80;
	}
	
	if ((_ac & 1) == 1) {
		_carry = true;
	} else {
		_carry = false;
	}
	
	_ac = result;
	updateZero(_ac);
	updateNegative(_ac);
}

void CPU::rol() {
	uint16_t result = _ac << 1;
	
	if (_carry) {
		result |= 1;
	}
	
	if ((_ac & 0x80) == 0x80) {
		_carry = true;
	} else {
		_carry = false;
	}

	_ac = result;
	updateZero(_ac);
	updateNegative(_ac);
	
}

void CPU::asl() {
	
	uint16_t result = _ac << 1;
	
	if ((_ac & 0x80) == 0x80) {
		_carry = true;
	} else {
		_carry = false;
	}
	
	_ac = result;
	updateZero(_ac);
	updateNegative(_ac);
	
}

void CPU::lsr() {
	
	uint16_t result = _ac >> 1;
	
	if ((_ac & 1) == 1) {
		_carry = true;
	} else {
		_carry = false;
	}
	
	_ac = result;
	updateZero(_ac);
	updateNegative(_ac);
}

void CPU::push(uint8_t value) {
	
	writeByte(0x0100 + _sp, value);
	_sp--;
}

uint8_t CPU::pop() {
	_sp++;
	return readByte(0x0100 + _sp);
}

void CPU::jmp(uint16_t addr) {
	_pc = addr;
}

void CPU::jsr(uint16_t addr) {
	
	uint16_t returnAddress = _pc - 1;
	
	push(returnAddress >> 8);
	push(returnAddress & 0xFF);
	tick();
	
	_pc = addr;
}

void CPU::dec(uint16_t addr) {
	uint8_t data = readByte(addr);
	data--;
	tick();
	writeByte(addr, data);
	updateZero(data);
	updateNegative(data);	
}

void CPU::inc(uint16_t addr) {
	uint8_t data = readByte(addr);
	data++;
	tick();
	writeByte(addr, data);
	updateZero(data);
	updateNegative(data);
}

void CPU::ror_mem(uint16_t addr) {
	uint8_t data = readByte(addr);
	
	uint16_t result = data >> 1;
	if (_carry) {
		result |= 0x80;
	}
	
	if ((data & 1) == 1) {
		_carry = true;
	} else {
		_carry = false;
	}
	updateZero(result);
	updateNegative(result);
	tick();
	writeByte(addr, result);
}

void CPU::rol_mem(uint16_t addr) {
	uint8_t data = readByte(addr);
	
	uint16_t result = data << 1;
	
	if (_carry) {
		result |= 1;
	}
	
	if ((data & 0x80) == 0x80) {
		_carry = true;
	} else {
		_carry = false;
	}
	
	writeByte(addr, result);
	tick();
	updateZero(result);
	updateNegative(result);
}

void CPU::lsr_mem(uint16_t addr) {
	
	uint8_t data = readByte(addr);
	
	uint16_t result = data >> 1;
	
	if ((data & 1) == 1) {
		_carry = true;
	} else {
		_carry = false;
	}
	
	updateZero(result);
	updateNegative(result);
	tick();
	writeByte(addr, result);
	
}


void CPU::asl_mem(uint16_t addr) {
	
	uint8_t data = readByte(addr);
	
	uint16_t result = data << 1;
	
	if ((data & 0x80) == 0x80) {
		_carry = true;
	} else {
		_carry = false;
	}
	
	writeByte(addr, result);
	tick();
	updateZero(result);
	updateNegative(result);
}

void CPU::sbc(uint16_t addr) {
	uint8_t data = readByte(addr);
	
	uint16_t result = _ac - data;
	if (_carry == false) {
		result--;
	}
	
	if ((result & 0x100) == 0x100) {
		_carry = false;
	} else {
		_carry = true;
	}
	
	if (((_ac ^ result) & 0x80) && ((_ac ^ data) & 0x80)) {
		_overflow = true;
	} else {
		_overflow = false;
	}
	
	_ac = result;
	updateZero(_ac);
	updateNegative(_ac);
}

void CPU::adc(uint16_t addr) {
	
	uint8_t data = readByte(addr);
	
	uint16_t result = _ac + data;
	if (_carry) {
		result++;
	}
	
	if ((result & 0x100) == 0x100) {
		_carry = true;
	} else {
		_carry = false;
	}
	
	if (((_ac ^ result) & 0x80) && !((_ac ^ data) & 0x80)) {
		_overflow = true;
	} else {
		_overflow = false;
	}
	
	_ac = result;
	updateZero(_ac);
	updateNegative(_ac);
}

void CPU::cpy(uint16_t addr) {
	
	uint8_t data = readByte(addr);
	
	if (_y >= data) {
		_carry = true;
	} else {
		_carry = false;
	}
	
	updateZero(_y - data);
	updateNegative(_y - data);
}

void CPU::cpx(uint16_t addr) {
	
	uint8_t data = readByte(addr);
	
	if (_x >= data) {
		_carry = true;
	} else {
		_carry = false;
	}
	
	updateZero(_x - data);
	updateNegative(_x - data);
}


void CPU::cmp(uint16_t addr) {
	
	uint8_t data = readByte(addr);
	
	if (_ac >= data) {
		_carry = true;
	} else {
		_carry = false;
	}
	
	updateZero(_ac - data);
	updateNegative(_ac - data);
}

void CPU::lbit(uint16_t addr) {
	uint8_t data = readByte(addr);

	updateZero(_ac & data);
	updateNegative(data);
	
	if ((data & 0x40) == 0x40) {
		_overflow = true;
	} else {
		_overflow = false;
	}
}


void CPU::lxor(uint16_t addr) {
	_ac ^= readByte(addr);
	updateZero(_ac);
	updateNegative(_ac);
}

void CPU::lor(uint16_t addr) {
	_ac |= readByte(addr);
	updateZero(_ac);
	updateNegative(_ac);
}

void CPU::land(uint16_t addr) {
	_ac &= readByte(addr);
	updateZero(_ac);
	updateNegative(_ac);
}

void CPU::sty(uint16_t addr) {
	writeByte(addr, _y);
}

void CPU::ldy(uint16_t addr) {
	_y = readByte(addr);
	updateZero(_y);
	updateNegative(_y);
}

void CPU::stx(uint16_t addr) {
	writeByte(addr, _x);
}

void CPU::ldx(uint16_t addr) {
	_x = readByte(addr);
	updateZero(_x);
	updateNegative(_x);
}

void CPU::sta(uint16_t addr) {
	writeByte(addr, _ac);
}

void CPU::lda(uint16_t addr) {
	_ac = readByte(addr);
	updateZero(_ac);
	updateNegative(_ac);
}

void CPU::updateZero(uint8_t value) {
	if (value == 0) {
		_zero = true;
	} else {
		_zero = false;
	}
}

void CPU::updateNegative(uint8_t value) {
	if ((value & 0x80) == 0x80) {
		_negative = true;
	} else {
		_negative = false;
	}
}

uint8_t CPU::readByte(uint16_t addr) {
	tick();
	
	if (addr < 0x2000) {
		return Ram[addr % 0x800];
	} else if (addr >= 0x2000 && addr <= 0x2007) {
		return _ppu->handleRead(addr);
	} else if (addr < 0x4000) {
		return 0xFF;
	}

	if (addr == 0x4016) {

		int ret = 0;

		switch (controllerCounter) {
			case 0:
				ret = controller1.a;
				break;
			case 3:
				ret = controller1.start;
				break;
			case 4:
				ret = controller1.up;
				break;
			case 5:
				ret = controller1.down;
				break;

			case 6:
				ret = controller1.left;
				break;

			case 7:
				ret = controller1.right;
				break;
		}


		controllerCounter++;
		if (controllerCounter > 7) controllerCounter = 0;

		return ret;
	}
	
	return _memory->read(addr);
}
void CPU::writeByte(uint16_t addr, uint8_t value) {
	tick();
	if (addr < 0x2000) {
		Ram[addr % 0x800] = value;
	} else if (addr >= 0x2000 && addr <= 0x2007) {
		_ppu->handleWrite(addr, value);
	} else if (addr < 0x4000) {
		
	} else if (addr < 0x4014) {
		_apu->setApuReg(addr, value);

	} else if (addr == 0x4014) {
//			std::cout << "Performing DMA transfer" << std::endl; 
			_ppu->performDMA(&Ram[0x100*value]);
	} else if (addr == 0x4016) {
		
	} else if (addr < 0x4020){
		
	} else {
		_memory->write(addr, value);
	}
	
}

void CPU::handleInput() {
	SDL_Event event;
  	while( SDL_PollEvent( &event ) ){
  		switch( event.type ){
                    /* Keyboard event */
                    /* Pass the event data onto PrintKeyInfo() */
                    case SDL_KEYDOWN: 
                    {
                    	switch(event.key.keysym.sym) {
                    	case SDLK_LEFT:
                    		controller1.left = 1;
                    		break;
                    	case SDLK_RIGHT:
                    		controller1.right = 1;
                    		break;
                    	case SDLK_UP:
                   			controller1.up = 1;
                   			break;
                   		case SDLK_DOWN:
                   			controller1.down = 1;
                   			break;
                   		case SDLK_RETURN:
                   			controller1.start = 1;
                   			break;
                   		case SDLK_SPACE:
                   			controller1.a = 1;
                   			break;
                   		}

                    }
                    break;
                    case SDL_KEYUP: 
                    {
                    	switch(event.key.keysym.sym) {
                    	case SDLK_LEFT:
                    		controller1.left = 0;
                    		break;
                    	case SDLK_RIGHT:
                    		controller1.right = 0;
                    		break;
                    	case SDLK_UP:
                   			controller1.up = 0;
                   			break;
                   		case SDLK_DOWN:
                   			controller1.down = 0;
                   			break;
                   		case SDLK_RETURN:
                   			controller1.start = 0;
                   			break;
                   		case SDLK_SPACE:
                   			controller1.a = 0;
                   			break;

                   		}

                    }
                    break;
                }
  	}
}



#include <map>
#include <vector>
#include <iostream>
using namespace std;

const uint8_t jump=0b0101<<4;
const uint8_t arith=0b0111<<4;
const uint8_t logic=0b1000<<4;
const uint8_t shift=0b1001<<4;

class InstructionOperator{

public:
enum TypeOfInstruction{
  jmp=jump,
  jeq=jump|1,
  jne=jump|2,
  jgt=jump|3,
  add=arith,
  sub=arith|1,
  mul=arith|2,
  div=arith|3,
  cmp=arith|4,
  nott=logic,
  andd=logic|1,
  orr=logic|2,
  xorr=logic|3,
  test=logic|4,
  shl=shift,
  shr=shift|1,
  push=0b1011<<4,
  pop=0b1010<<4,
  str=0b1011<<4,
  ldr=0b1010<<4,
  halt=0b0,
  intt=0b0001<<4,
  iret=0b0010<<4,
  call=0b0011<<4,
  ret=0b0100<<4,
  xchg=0b0110<<4

};

enum TypeOfAddresing{
  immed=0b0000,
  regdir=0b0001,
  regdirpom=0b00101,
  regind=0b0010,
  reginpom=0b0011,
  mem=0b0100
};



static void tranInstr(vector<uint8_t>& addrInstr,string instr,string typeofAddr,uint16_t value,uint8_t src=0b1111,uint8_t dst=0b1111,uint8_t up=0);
};
#include "../h/Emulator.hpp"
#include <sstream>
#include <FlexLexer.h>
#include <iomanip>
#include <bitset>

#include <fstream>
FlexLexer* Emulator::flexer;
vector<uint8_t> Emulator::MemTable;
const char *Emulator::inputBuff;

void printEmulator(int16_t reg[],int16_t psw){
  cout<<"Emulated processor executed halt instruction"<<endl;
  cout<<"Emulated processor state:0b"<<std::bitset<16>{(uint16_t)psw}<<endl;
  for(int i=0;i<8;i++){
    cout<<'r'<<i<<"=0x"<<hex<<setw(4)<<setfill('0')<<reg[i]<<"    ";
    if(i==3) cout<<endl;
  }
}


uint16_t literalToUint_16(string l)
{
  stringstream liter;
  liter << ((l.size() > 1 && l.at(1) == 'x') || (l.size() > 2 && l.at(2) == 'x') ? hex : dec) << l;
  int ret;
  liter >> ret;
  return ret;
}


Emulator::Emulator(string input){
  inpuHex=input;
  input_stream.open(input,std::ios::in);
  for(int i=0;i<0xFFFF;i++){
    memMap[i]=0;
  }

  flexer=new yyFlexLexer(&input_stream);
  for(int i=0;i<8;i++)registers[i]=0;
  registers[sp]=0xFF00;

}

void Emulator::analyzeHexFile(){
int read;
int numOfBytes;
uint16_t currAddress;


read=flexer->yylex();


while(!terminate && read){
switch (read)
{
  case ReadingType::address:
  {


   currAddress=literalToUint_16(inputBuff);


   numOfBytes=0;
   break;
   }
  case ReadingType::byte:
  {
    char analyze[50];
    strcpy(analyze,"0x");
    strcat(analyze,inputBuff);
    memMap[currAddress+numOfBytes++]=(uint8_t)literalToUint_16(analyze);
    break;
  }
  case ReadingType::eol:
   break;
  default:
  terminate=true;
  break;
}
read=flexer->yylex();
}
if(terminate){
  cout<<"Invalid format of instruction";
}

registers[sp]=memMap[currAddress+numOfBytes];
}


void Emulator::compileHexFile(){
  analyzeHexFile();

  while(!terminate && !finished){
    uint8_t instr=memMap[registers[pc]++];
    switch(instr){
     
      case(Instruction::add):
       
        handleAdd();
        break;
      case(Instruction::cmp):
        
        handleCmp();
        break;
      case(Instruction::div):
       

        handleDiv();
        break;
      case(Instruction::mul):
       
        handleMul();
        break;
      case(Instruction::sub):
       
        handleSub();
        break;
      case(Instruction::jmp):
      
        handleJmp();
        break;
      case(Instruction::jeq):
      

        handleJeq();
        break;
      case(Instruction::jne):
        
        handleJne();
        break;
      case(Instruction::jgt):
        

        handleJgt();
        break;
      case(Instruction::nott):
     

        handleNot();
        break;
      case(Instruction::andd):
       

        handleAnd();
        break;
      case(Instruction::orr):
       
        handleOr();
        break;
      case(Instruction::xorr):
      

        handleXor();
        break;
      case(Instruction::test):
       

        handleTest();
        break;
      case(Instruction::shl):
        

        handleShl();
        break;
      case(Instruction::shr):
       

        handleShr();
        break;
      case(Instruction::ldr):
     

        handleLdr();
        break;
      case(Instruction::str):
        

        handleStr();
        break;
      case(Instruction::intt):
       

        handleInt();
        break;
      case(Instruction::iret):
        
        handleIRet();
        break;
      case(Instruction::ret):
        handleRet();
        break;
      case(Instruction::call):
        
        handleCall();
        break;
      case(Instruction::xchg):
       
        handleXChg();
        break;
      case(Instruction::halt):
        handleHalt();
        break;
      default:
        terminate=true;
        break;
    }    
    if(terminate){
      cout<<"Program terminated";
      return;
    }
  }
  printEmulator(registers,psw);
}

//arith operations
void Emulator::handleAdd(){
  uint8_t reg=memMap[registers[pc]++];
  uint8_t regS=reg&0xF;
  uint8_t regD=(uint8_t)reg>>4;
  registers[regD]=registers[regD]+registers[regS];

}

void Emulator::handleSub(){
  uint8_t reg=memMap[registers[pc]++];
  uint8_t regS=reg&0xF;
  uint8_t regD=(uint8_t)reg>>4;
  registers[regD]=registers[regD]-registers[regS];
}


void Emulator::handleMul(){
  uint8_t reg=memMap[registers[pc]++];
  uint8_t regS=reg&0xF;
  uint8_t regD=(uint8_t)reg>>4;
  registers[regD]=registers[regD]*registers[regS];
}


void Emulator::handleDiv(){
  uint8_t reg=memMap[registers[pc]++];
  uint8_t regS=reg&0xF;
  uint8_t regD=(uint8_t)reg>>4;
  if(registers[regS]==0){
    terminate=true;
    return;
  }
  registers[regD]=registers[regD]/registers[regS];
}

void Emulator::handleCmp(){
  uint8_t reg=memMap[registers[pc]++];
  uint8_t regS=reg&0xF;
  uint8_t regD=(uint8_t)reg>>4;
  int16_t temp=registers[regD]-registers[regS];
  if(temp<0){
    psw|=negative;
  }else{
    psw&=(~negative);
  }
  if(temp==0){
    psw|=zero;
  }else{
    psw&=(~zero);
  }
  if((registers[regD]>0 && registers[regS]<0 && temp<0)||(registers[regD]<0 && registers[regS]>0 && temp<0)){
    psw|=overflow;
  }else{
    psw&=(~overflow);
  }
  if(registers[regD]<registers[regS]){
    psw|=carry;
  }else{
    psw&=(~carry);
  }
  //update psw bites
}

//logical operations
void Emulator::handleAnd(){
  uint8_t reg=memMap[registers[pc]++];
  uint8_t regS=reg&0xF;
  uint8_t regD=(uint8_t)reg>>4;
  registers[regD]=registers[regD]&registers[regS];
}

void Emulator::handleOr(){
  uint8_t reg=memMap[registers[pc]++];
  uint8_t regS=reg&0xF;
  uint8_t regD=(uint8_t)reg>>4;
  registers[regD]=registers[regD]|registers[regS];
}

void Emulator::handleXor(){
  uint8_t reg=memMap[registers[pc]++];
  uint8_t regS=reg&0xF;
  uint8_t regD=(uint8_t)reg>>4;
  registers[regD]=registers[regD]^registers[regS];
}

void Emulator::handleNot(){
  uint8_t reg=memMap[registers[pc]++];
  uint8_t regD=(uint8_t)reg>>4;
  registers[regD]=~registers[regD];
}

void Emulator::handleTest(){
   uint8_t reg=memMap[registers[pc]++];
  uint8_t regS=reg&0xF;
  uint8_t regD=(uint8_t)reg>>4;
  uint8_t temp=registers[regD]&registers[regS];

  if(temp==0){
    psw|=zero;
  }else{
    psw&=(~zero);
  }
  if(temp<0){
    temp|=negative;
  }else{
    temp&=(~negative);
  }
}
//shifting operations

void Emulator::handleShl(){
  uint8_t reg=memMap[registers[pc]++];
  uint8_t regS=reg&0xF;

  uint8_t regD=(uint8_t)reg>>4;
  uint8_t temp=regD;
  temp=temp<<(registers[regS]-1);
  registers[regD]=registers[regD]<<registers[regS];
  if(registers[regD]==0){
    psw|=zero;
  }else{
    psw&=(~zero);
  }
  if(registers[regD]<0){
    psw|=negative;
  }else{
    psw&=(~negative);
  }
  if(temp&0x80){
    psw|=carry;
  }else{
    psw&=(~carry);
  }



}

void Emulator::handleShr(){
   uint8_t reg=memMap[registers[pc]++];
  uint8_t regS=reg&0xF;
  uint8_t regD=(uint8_t)reg>>4;
  uint8_t temp=regD;
  registers[regD]=registers[regD]>>registers[regS];
  temp=temp>>(registers[regS]-1);
  if(registers[regD]==0){
    psw|=zero;
  }else{
    psw&=(~zero);
  }
  if(registers[regD]<0){
    psw|=negative;
  }else{
    psw&=(~negative);
  }
  if(temp&0x01){
    psw|=carry;
  }else{
    psw&=(~carry);
  }
}

void Emulator::handleXChg(){
  uint8_t reg=memMap[registers[pc]++];
  uint8_t regS=reg&0xF;
  uint8_t regD=(uint8_t)reg>>4;
  uint8_t temp=registers[regD];
  registers[regD]=registers[regS];
  registers[regS]=temp;
}

//treba nam za jump i ldr/str(promenljive velicine instrukcije)

void Emulator::handleJmp(){
  uint16_t adressToJump=gettingAddress();

  registers[pc]=adressToJump;
}

void Emulator::handleJeq(){
  uint16_t addressToJump=gettingAddress();

  if(psw&1){
    registers[pc]=addressToJump;
  }
}

void Emulator::handleJne(){
  uint16_t addressToJump=gettingAddress();

  if(!(psw&1)){
    registers[pc]=addressToJump;
  }
}

void Emulator::handleJgt(){
  uint16_t addressToJump=gettingAddress();

  if(!(psw&1)&&((psw&2)==(psw&8))){
    registers[pc]=addressToJump;
  }
}

void Emulator::handleCall(){
  uint16_t address=gettingAddress();

  push(registers[pc]);

  registers[pc]=address;
}

void Emulator::handleInt(){
  uint8_t reg=(memMap[registers[pc]++]>>4)&0xF;

  push(registers[pc]);
  push(psw);

  uint16_t newPC=(memMap[(registers[reg])*2+1]<<8)|memMap[(registers[reg])*2];

  registers[pc]=newPC;
}

void Emulator::handleIRet(){
  psw=pop();
  registers[pc]=pop();
}

void Emulator::handleRet(){
  registers[pc]=pop();
}

void Emulator::handleHalt(){
  finished=true;
}

void Emulator::handleLdr(){
  //int16_t val=gettingAddress();
  int16_t finalValue;
  uint8_t reg=memMap[registers[pc]++];
  uint8_t addr=memMap[registers[pc]++];
  uint8_t upMode=(addr>>4)&0xF;
  addr=addr&0xF;

  
  switch(addr){
    case(typeOfAddresing::immed):
  {
    uint8_t lower=memMap[registers[pc]++];
    uint8_t higher=memMap[registers[pc]++];
    finalValue=((higher<<8)|lower);
    break;
  }
    case(typeOfAddresing::regdir):
  {
    uint8_t regi=reg&0xF;
    finalValue=registers[regi];
    break;
  }
    case(typeOfAddresing::regind):
  {
    uint8_t regi=reg&0xF;
    registers[regi]+=upModeBeforeOper(upMode);
    
    uint8_t lower=memMap[registers[regi]];
    uint8_t higher=memMap[registers[regi]+1];

    registers[regi]+=upModeAfterOper(upMode);
    finalValue=((higher<<8)|lower);
    break;
  }
    case(typeOfAddresing::regindpom):
    {
    uint8_t regi=reg&0xF;
    registers[regi]+=upModeBeforeOper(upMode);

    uint8_t lower=memMap[registers[pc]++];
    uint8_t higher=memMap[registers[pc]++];

    uint8_t add=(higher<<8)|lower;

    lower=memMap[registers[regi]+add];
    higher=memMap[registers[regi]+add+1];

    registers[regi]+=upModeAfterOper(upMode);
    finalValue=((higher<<8)|lower);
    break;
    }
    case(typeOfAddresing::mem):
    {
     uint8_t lower=memMap[registers[pc]++];
    uint8_t higher=memMap[registers[pc]++];

    uint16_t add=(higher<<8)|lower;
    lower=memMap[add];
    higher=memMap[add+1];

    finalValue=((higher<<8)|lower);
    break;
    }
    case(typeOfAddresing::regdirpom):
    {
    uint8_t regi=reg&0xF;
    uint8_t lower=memMap[registers[pc]++];
    uint8_t higher=memMap[registers[pc]++];

    uint16_t add=(higher<<8)|lower;
    finalValue=(registers[regi]+add);
    }
    break;
    default:
    terminate=true;
    break;

  }
  registers[((reg>>4)&0xF)]=finalValue;

}

void Emulator::handleStr(){
  uint8_t reg=memMap[registers[pc]++];
  uint8_t addr=memMap[registers[pc]++];
  uint8_t regD=(reg>>4)&0xF;
  uint8_t upMode=(addr>>4)&0xF;
  addr=addr&0xF;

  switch(addr){
    case(typeOfAddresing::immed):
    terminate=true;
    break;
    case(typeOfAddresing::regdir):
  {
    uint8_t regi=reg&0xF;
    registers[regi]=registers[regD];
    break;
  }
    case(typeOfAddresing::regind):
  {
    uint8_t regi=reg&0xF;
    registers[regi]+=upModeBeforeOper(upMode);
    
    memMap[registers[regi]]=(registers[regD]&0xFF);
    memMap[registers[regi]+1]=((registers[regD]>>8)&0xFF);

    registers[regi]+=upModeAfterOper(upMode);
    break;
  }
    case(typeOfAddresing::regindpom):
  {
    uint8_t regi=reg&0xF;
    registers[regi]+=upModeBeforeOper(upMode);

    uint8_t lower=memMap[registers[pc]++];
    uint8_t higher=memMap[registers[pc]++];

    uint8_t add=(higher<<8)|lower;

    memMap[registers[regi]+add]=(registers[regD]&0xFF);
    memMap[registers[regi]+add+1]=((registers[regD]>>8)&0xFF);

    registers[regi]+=upModeAfterOper(upMode);
    break;
  }
    case(typeOfAddresing::mem):
    {
     uint8_t lower=memMap[registers[pc]++];
    uint8_t higher=memMap[registers[pc]++];

    uint16_t add=(higher<<8)|lower;
    memMap[add]=(registers[regD]&0xFF);
    memMap[add+1]=((registers[regD]>>8)&0xFF);

    break;
    }
    case(typeOfAddresing::regdirpom):
    terminate=true;
    break;
    default:
    terminate=true;
    break;

  }

}

void Emulator::inputInstr(const char *c)
{
  inputBuff = c;
}

void Emulator::push(int16_t val){
  memMap[(uint16_t)--registers[sp]]=val>>8;
  memMap[(uint16_t)--registers[sp]]=val&0xFF;

}

int16_t Emulator::pop(){
  uint16_t lower=(uint8_t)memMap[(uint16_t)registers[sp]++];
  uint16_t higher=(uint8_t)memMap[(uint16_t)registers[sp]++];
//  cout<<(uint16_t)registers[sp];
  return ((higher<<8)|lower);
}

int16_t Emulator::upModeBeforeOper(uint8_t up){
      if(up==1) return -2;
      else if(up==2) return 2;
      else return 0;
}

int16_t Emulator::upModeAfterOper(uint8_t up){
  if(up==3) return -2;
  else if(up==4) return 2;
  else return 0;
}

uint16_t Emulator::gettingAddress(){
  uint8_t reg=memMap[registers[pc]++];
  uint8_t addr=memMap[registers[pc]++];
  uint8_t upMode=(addr>>4)&0xF;
  addr=addr&0xF;

  switch(addr){
    case(typeOfAddresing::immed):
    {
    uint8_t lower=memMap[registers[pc]++];
    uint8_t higher=memMap[registers[pc]++];
    return ((higher<<8)|lower);
    break;
    }
    case(typeOfAddresing::regdir):
  {
    uint8_t regi=(reg)&0xF;
    return registers[regi];
    break;
  }
    case(typeOfAddresing::regind):
    {
    uint8_t regi=(reg)&0xF;
    registers[regi]+=upModeBeforeOper(upMode);
    
    uint8_t lower=memMap[registers[regi]];
    uint8_t higher=memMap[registers[regi]+1];

    registers[regi]+=upModeAfterOper(upMode);
    return ((higher<<8)|lower);
    break;
    }
    case(typeOfAddresing::regindpom):
  {
    uint8_t regi=(reg)&0xF;
    registers[regi]+=upModeBeforeOper(upMode);

    int16_t lower=memMap[registers[pc]++];
    int16_t higher=memMap[registers[pc]++];

    int16_t add=(higher<<8)|lower;

    lower=memMap[registers[regi]+add];
    higher=memMap[registers[regi]+add+1];

    registers[regi]+=upModeAfterOper(upMode);
    return ((higher<<8)|lower);
    break;
  }
    case(typeOfAddresing::mem):
  {
     uint8_t lower=memMap[registers[pc]++];
    uint8_t higher=memMap[registers[pc]++];

    uint16_t add=(higher<<8)|lower;
    lower=memMap[add];
    higher=memMap[add+1];

    return ((higher<<8)|lower);
    break;
  }
    case(typeOfAddresing::regdirpom):
  {
    uint8_t regi=(reg)&0xF;
    int16_t lower=memMap[registers[pc]++];
    int16_t higher=memMap[registers[pc]++];

    int16_t add=(higher<<8)|lower;
    return (registers[regi]+add);
    break;
  }
    default:
    terminate=true;
    break;

  }
  return 0;
}




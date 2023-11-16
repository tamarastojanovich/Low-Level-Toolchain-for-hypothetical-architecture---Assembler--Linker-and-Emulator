#include "../h/InstructionOperator.hpp"
#include "string.h"

void InstructionOperator::tranInstr(vector<uint8_t>& addrInstr,string instr,string typeofAddr,uint16_t value,uint8_t src,uint8_t dst,uint8_t up){
  addrInstr.push_back((uint8_t)((dst<<4)|src)); //dodajemo koji su nam registri potrebni
  //potrebno je da proverimo da li je data instrukcija instr soft prekida posto je potrebno da src bude F

const char* i=instr.c_str();
//zasto nigde nemam dodavanje registra
  if(!strcmp(i,"int")){
    addrInstr.back() |=(uint8_t) 0xF;
    //ili samo proslediti 0xF ako nam nista ne daju i onda samo to dodati na kraju
    return;
  }

  /*if(usingAddress(instr)){
    
    addrInstr.push_back((uint8_t)((up<<4)|(addresing[typeofAddr])));//mozda je bolje da idem po nazivu za program,ali videcemo
  }*/

  if(!strcmp(i,"call") || !strcmp(i,"int") || !strcmp(i,"ldr") ||!strcmp(i,"str")
||!strcmp(i,"jmp")|| !strcmp(i,"push") || !strcmp(i,"pop")){
  const char* a=typeofAddr.c_str();
  if(!strcmp(a,"immed")){
    addrInstr.push_back((uint8_t)((up<<4)|TypeOfAddresing::immed));
  }else if(!strcmp(a,"regdir")){
    addrInstr.push_back((uint8_t)((up<<4)|TypeOfAddresing::regdir));
  }else if(!strcmp(a,"regdirpom")){
    addrInstr.push_back((uint8_t)((up<<4)|TypeOfAddresing::regdirpom));
  }else if(!strcmp(a,"regind")){
    addrInstr.push_back((uint8_t)((up<<4)|TypeOfAddresing::regind));
  }else if(!strcmp(a,"regindpom")){
    addrInstr.push_back((uint8_t)((up<<4)|TypeOfAddresing::reginpom));
  }else if(!strcmp(a,"mem")){
    addrInstr.push_back((uint8_t)((up<<4)|TypeOfAddresing::mem));
  }
}

  const char* a=typeofAddr.c_str();


if(!strcmp(a,"immed") || !strcmp(a,"regdirpom") || !strcmp(a,"regindpom") ||!strcmp(a,"mem")){
  addrInstr.push_back((uint8_t)(value& 0xFF));
  addrInstr.push_back((uint8_t)((value>>8)&0xFF));
}
  
  cout<<instr<<addrInstr.size()<<endl;



}
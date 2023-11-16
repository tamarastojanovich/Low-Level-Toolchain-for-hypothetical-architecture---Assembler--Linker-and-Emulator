#include <iostream>
#include <string.h>
#include "../h/Emulator.hpp"

int main(int argc,char* argv[]){
  string input;
  if(argc!=2){
    cout<<"Program terminated";
  }
  input=argv[1];

  Emulator* emu=new Emulator(input);
  emu->compileHexFile();

  return 0;

}
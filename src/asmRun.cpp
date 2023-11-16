#include <iostream>
#include <string.h>
#include "../h/Assembler.hpp"

int main(int argc,char* argv[]){
  //koje duzine treba da bude argv
  cout<<"pikachu";
  string input;
  string output;
  if(argc!=2 && argc!=4){
    cout<<"Program terminated";
  }

  if(argc==2){
    input=argv[1];
    output="ASM.o";
  }else if(argc==4){   
     if(!strcmp(argv[1],"-o")){
    input=argv[3];
    output=argv[2];
  
  }else{
    cout<<"Program terminated";
  }
  }

  Assembler* a=new Assembler(input,output);
  a->first_pass();

  return 0;

}
#include <vector>
#include "string.h"
#include "../h/Linker.hpp"
#include <iostream>
using namespace std;
int main(int argc,char* argv[]){
  vector<string> files;
  string output="output.hex";
  bool hex=false;
  if(argc<2) cout<<"Error";
  /*for(int i=1;i<argc;i++){
    if(!strcmp(argv[i],"-hex")) hex=true;
    else if(!strcmp(argv[i],"-o") && (i+1)==argc)cout<<"ERROR";
    else if(i==3)output=argv[i];
    else{
      files.push_back(argv[i]);
    }
  }*/
  bool error=false;
  if(!strcmp(argv[1],"-hex")) hex=true;
  if(strcmp(argv[2],"-o")){
    
    error=true;
   
  }else if(
    !strcmp(argv[2],"-o")&&argc==3) {
      
      error=true;
     
    }
    
  if(!hex || error) {
    cout<<"Error: invalid form of instruction";}
    else{
      output=argv[3];
      for(int i=4;i<argc;i++){
        files.push_back(argv[i]);
      }
      Linker* linker=new Linker(files,output,true);
    }
    return 0;
}
#include <vector>
#include "string.h"
#include "SymbolTable.hpp"
#include "SectionTable.hpp"
#include "RelocationTable.hpp"
#include <iostream>
using namespace std;

class Linker{
  private:
  vector<string> inputfile;
  bool hex;
  string outputFile;
  SymbolTable* symbols;//mozda mi treba skup razresenih i nerazresenih,ali i ne mora ako pratim da li mi je ostalo nedefinisanih na kraju
  SectionTable* sections;
  vector<RelocationTable> relocatinTables;
  string currRelocSec;
  string machineCodeSec;
  int indexOfCurrSec;
  bool terminate=false;


  public:
  //KAKO DA CITAM FAJLOVEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE
  Linker(vector<string> files,string output,bool hex=false);//mozda da imamo listu sekcija? tabela simbola?
  void link();//metoda za razresavanje nedefinisanih simbola?
  void readFile();
  void insertInstruction(vector<uint8_t> info);
};
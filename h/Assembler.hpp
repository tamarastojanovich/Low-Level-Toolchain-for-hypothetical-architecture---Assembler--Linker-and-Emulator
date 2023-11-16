#ifndef _ASSEMBLER_H_
#define _ASSEMBLER_H_

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <map>
#include "SymbolTable.hpp"
#include "SectionTable.hpp"
#include "RelocationTable.hpp"
using namespace std;

class FlexLexer;

class Assembler{
private:

  string input_file;
  ifstream input_stream;
  string output_file;
  static const char* inputBuff;
  FlexLexer* flexer;

  static int id_section;
  static int id_symbol;
  bool error;
  bool finished;
  static bool newLine;

  int currLine;
  

  //mozda mi treba promenljiva za sve linije koje se nalaze u ulaznom fajlu
  //mozda mi treba neka celobrojna promenljiva koja ce da prati gde se tacno nalazimo




SectionTable* sectionTable;
SymbolTable* symbolTable;
vector<RelocationTable> relocationTable;


int locationCounter;


//funkcija za pretvaranje datog ulaznog fajla u citljiv string
bool readInputFile();



//posto je jednoprolazni treba da definisem i neku strukturu//check
//treba nam tabela simbola//check
//treba nam relokaciona tabela
//treba namgc tabela sekcija

public:

enum Instruction{
  Global=1,
  Extern,
  Section,
  WordAlloc,
  Skip,
  End,
  Operator,//sta ce nam ovo????????????????????????
  zeroArguments,
  oneOperand,
  oneRegister,
  TwoRegisters,
  OneRegandOneOperand,
  Eol//nivo C


};

enum StringType{
Label=Instruction::Eol+1,
Comment
};

enum AddressingType{
  SymbRelative=StringType::Comment+1,
  Indirect,
  Val,
  Regdir,
  RegRel

};

enum Operand{
  Register=AddressingType::RegRel+1,
  Literal,
  Comma,
  String,
  Symbol
};



  Assembler(string input,string output);
  static void inputInstr(const char* c);
  void first_pass();

  static void endOfLine();
  private:
  void exportSymbol();
  void importSymbol();
  void skip();
  void section();
  void wordAlloc();
  void equ();
  void ascii();
  void end();
  void cleanUpReloc();

  void handleZeroArg();
  void handleOneRegister();
  void handleTwoRegister();
  void handleOneRegandOneOper();
  void handleJumps();


  void handleLabels();
  void handleComments();

  void insertInstruction(vector<uint8_t> info);



};

#endif
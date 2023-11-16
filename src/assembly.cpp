//#include "assembly.h"

#include "../h/Assembler.hpp"
#include <FlexLexer.h>
#include "../h/InstructionOperator.hpp"
#include <vector>
#include <fstream>
#include <regex>

uint16_t literalToUint_16(string l)
{
  stringstream liter;
  liter << ((l.size() > 1 && l.at(1) == 'x') || (l.size() > 2 && l.at(2) == 'x') ? hex : dec) << l;
  int ret;
  liter >> ret;
  return ret;
}

extern void printTable(SymbolTable *symb, SectionTable *sect, vector<RelocationTable> rel, string output);

bool Assembler::newLine = false;
int Assembler::id_section = 0;
int Assembler::id_symbol = 0;
const char *Assembler::inputBuff;
Assembler::Assembler(string input, string output)
{

  this->input_file = input;
  output_file = output;
  input_stream.open(input, std::ios::in);
  if (!input_stream)
  {
    error = true;
  }
  else
    error = false;

  symbolTable = new SymbolTable();
  sectionTable = new SectionTable();

  this->output_file = output;
  flexer = new yyFlexLexer(&input_stream);

  this->locationCounter = 0;
  finished = false;
  newLine = true;
}

void Assembler::inputInstr(const char *c)
{
  // cout<<"OVDE";
  inputBuff = c;
}

void Assembler::first_pass()
{

  int instr = 0;
  instr = flexer->yylex();
  while (!error && instr && !finished)
  {

    if (!newLine && instr != StringType::Comment)
    {
      end();
      continue;
    }
    newLine = false;

    switch (instr)
    {
    case (Instruction::Global):
      exportSymbol();
      break;
    case (Instruction::Extern):
      importSymbol();
      break;
    case (Instruction::Section):
      section();
      break;
    case (Instruction::WordAlloc):
      wordAlloc();
      break;
    case (Instruction::Skip):
      skip();
      break;
    case (Instruction::End):
      end();
      break;
    case (Instruction::zeroArguments):
      handleZeroArg();
      break;
    case (Instruction::oneRegister):
      handleOneRegister();

      break;
    case (Instruction::oneOperand):
      handleJumps();
      break;
    case (Instruction::TwoRegisters):
      handleTwoRegister();
      break;
    case (Instruction::OneRegandOneOperand):

      handleOneRegandOneOper();
      break;
    case (Instruction::Eol):
      newLine = true;
      break;
    case (StringType::Label):

      handleLabels();

      break;
    case (StringType::Comment):

      handleComments();
      break;
    default:

      end();
      break;
    }
    instr = flexer->yylex();
  }
  if (error || finished)
  {
    input_stream.close();
    return;
  }

  input_stream.close();
  // sredjivanje relokacionih tabela
  cleanUpReloc();

  printTable(symbolTable, sectionTable, relocationTable, output_file);
}

void Assembler::exportSymbol()
{ // global

  int oper, operands = 0;

  oper = flexer->yylex();

  while (oper != Instruction::Eol)
  {
    if (operands == 0)
    {
      if (oper == Operand::Symbol)
        operands = Operand::Symbol;
      else
      {
        end();
        break;
      }
    }
    else
    {
      if ((oper == Operand::Symbol && operands != Operand::Comma) || (oper == Operand::Comma && operands != Operand::Symbol) || (oper == StringType::Comment && operands != Operand::Symbol))
      {
        end();
        break;
      }
      else
      {
        operands = oper;
      }
    }

    switch (operands)
    {
    case (Operand::Symbol):
    {
      if (sectionTable->findSection(inputBuff))
      {
        // ispisi gresku
        end();
      }
      else
      {
        symbolTable->globalize(inputBuff);
      }
      break;
    }
    case (Operand::Comma):
    {
      break;
    }
    case (StringType::Comment):
    {
      handleComments();
      break;
    }
    }
    oper = flexer->yylex();
  }
  if (operands == Operand::Comma || operands == 0)
    end();
}

void Assembler::importSymbol()
{ // rxtern
  int oper, operands = 0;

  oper = flexer->yylex();

  while (oper != Instruction::Eol)
  {
    if (operands == 0)
    {
      if (oper == Operand::Symbol)
        operands = Operand::Symbol;
      else
      {
        end();
        break;
      }
    }
    else
    {
      if ((oper == Operand::Symbol && operands != Operand::Comma) || (oper == Operand::Comma && operands != Operand::Symbol) || (oper == StringType::Comment && operands != Operand::Symbol))
      {
        end();
        break;
      }
      else
      {
        operands = oper;
      }
    }

    switch (operands)
    {
    case (Operand::Symbol):
    {
      if (symbolTable->contains(inputBuff))
      {
        cout << "Multiple definition of symbol " << inputBuff << endl;
        finished = true;
      }
      else if (sectionTable->findSection(inputBuff))
      {
        cout << "Cannot export/import section" << endl;
        end();
      }
      else
      {
        symbolTable->externSym(inputBuff);
      }

      break;
    }
    case (Operand::Comma):
    {
      break;
    }
    case (StringType::Comment):
    {
      handleComments();
      break;
    }
    }
    oper = flexer->yylex();
  }
  if (operands == Operand::Comma || operands == 0)
    end();
}

void Assembler::handleLabels()
{
  // ubacivanje u tabelu...
  if (symbolTable->contains(inputBuff))
  {
    if (symbolTable->isDefined(inputBuff))
    {
      finished = true;
      cout << "Multiple definitons of symbol " << inputBuff;
    }
    else
    {
      symbolTable->defineSymbol(inputBuff, locationCounter, SectionTable::currSec);
    }
  }
  else
  {
    symbolTable->insertSymbol(inputBuff, locationCounter, SectionTable::currSec);
  }
}

void Assembler::equ()
{
  // ovo nije nivo A
}

void Assembler::ascii()
{
  // ovo nije nivo A
}

void Assembler::wordAlloc()
{
  int oper, operands = 0;

  uint16_t w=0;
  oper = flexer->yylex();

  while (oper != Instruction::Eol)
  {
    if (operands == 0)
    {
      if (oper == Operand::Symbol || oper == Operand::Literal)
        operands = oper;
      else
      {
        end();
        break;
      }
    }
    else
    {
      if (((oper == Operand::Symbol || oper == Operand::Literal) && operands != Operand::Comma) || (oper == Operand::Comma && (operands != Operand::Symbol && operands != Operand::Literal)) || (oper == StringType::Comment && (operands != Operand::Symbol && operands != Operand::Literal)))
      {

        end();
        break;
      }
      else
      {
        operands = oper;
      }
    }

    switch (operands)
    {
    case (Operand::Literal):
    {
      w=literalToUint_16(inputBuff);
      bool found = false;
      for (int i = 0; i < relocationTable.size(); i++)
      {
        if (relocationTable.at(i).getSection() == SectionTable::currSec)
        {
          relocationTable.at(i).insertReloc("", 0, 0, locationCounter, inputBuff, true);
          

          found = true;
          break;
        }
      }
      if (!found)
      {
        RelocationTable reloc;
        reloc.setSection(SectionTable::currSec);
        reloc.insertReloc("", 0, 0, locationCounter, inputBuff, true);
      

        relocationTable.push_back(reloc);
      }

      break;
    }
    case (Operand::Symbol):
    {
      bool found = false;
      for (int i = 0; i < relocationTable.size(); i++)
      {
        if (relocationTable.at(i).getSection() == SectionTable::currSec)
        {

          bool need = symbolTable->relocNeeded(inputBuff, locationCounter, SectionTable::currSec, RelocationTable::RelocType::absolute);
          if (need)
          {
            relocationTable.at(i).insertReloc(inputBuff, RelocationTable::RelocType::absolute, 0, locationCounter);
          }
          found = true;
          break;
        }
      }
      if (!found)
      {
        RelocationTable reloc;
        reloc.setSection(SectionTable::currSec);
        bool need = symbolTable->relocNeeded(inputBuff, locationCounter, SectionTable::currSec, RelocationTable::RelocType::absolute);
        if (need)
        {
          reloc.insertReloc(inputBuff, RelocationTable::RelocType::absolute, 0, locationCounter);
        }
        relocationTable.push_back(reloc);
      }

      break;
    }
    case (Operand::Comma):
    {
      break;
    }
    case (StringType::Comment):
    {
      handleComments();
      break;
    }
    default:
      end();
      break;
    }
    oper = flexer->yylex();
  }
  if (operands == Operand::Comma || operands == 0)
    end();
    /* if(literal){
   w=literalToUint_16(word);
    vector<uint8_t> cont={(uint8_t)(w&0xFF),((uint8_t)(w>>8))};
    for(int i=0;i<cont.size();i++) {
      contentTable.push_back(cont.at(i));
    }
    //da li ima sta jos da se radi za literal
  }else{
    
    vector<uint8_t> cont={(uint8_t)(w&0xFF),((uint8_t)(w>>8))};

    for(int i=0;i<cont.size();i++) {
      contentTable.push_back(cont.at(i));
      }
      */
 // insertInstruction()
  vector<uint8_t> cont={(uint8_t)(w&0xFF),((uint8_t)(w>>8))};
  insertInstruction(cont);
  locationCounter += sizeof(int16_t);
  sectionTable->incSizeOfSection(SectionTable::currSec, sizeof(int16_t));
}

void Assembler::end()
{
  finished = true;
}

void Assembler::section()
{
  if (symbolTable->contains(inputBuff))
  {
    end();
    return;
  }
  else
  {
    if (!sectionTable->findSection(inputBuff))
    {
      sectionTable->insertSection(inputBuff);
      sectionTable->setSizeOfSection(SectionTable::currSec, locationCounter);
      locationCounter = 0;
      SectionTable::currSec = inputBuff;
    }
    else
    {
      sectionTable->setSizeOfSection(SectionTable::currSec, locationCounter);
      locationCounter = sectionTable->getSizeOfSection(inputBuff);

      SectionTable::currSec = inputBuff;
    }
    // i da ne prelazi slucajno granicu 0xFFFF
  }
}

void Assembler::skip()
{

  int i = literalToUint_16(inputBuff);
  vector<uint8_t> zeroes;
  for (int j = 0; j < i; j++)
    zeroes.push_back(0);
  // treba sad da ih ubacimo u kontekst
  insertInstruction(zeroes);
  sectionTable->incSizeOfSection(SectionTable::currSec, i);
  locationCounter += i;
  newLine = true;
}

void Assembler::handleComments()
{
  while (flexer->yylex() != Instruction::Eol)
    ;
}

void Assembler::endOfLine()
{
  Assembler::newLine = true;
  // da li je potrebno da pamtimo broj linije?
}

// ovo na kraju
void Assembler::handleOneRegister()
{
  vector<uint8_t> instr;
  if(!strcmp(inputBuff,"push")){
      instr.push_back(InstructionOperator::TypeOfInstruction::push);
  }else if(!strcmp(inputBuff,"pop")){
    instr.push_back(InstructionOperator::TypeOfInstruction::pop);
  }else if(!strcmp(inputBuff,"not")){
      instr.push_back(InstructionOperator::TypeOfInstruction::nott);
  }else if(!strcmp(inputBuff,"int")){
    instr.push_back(InstructionOperator::TypeOfInstruction::intt);
  }

  //vector<uint8_t> instr = {InstructionOperator::instruction[inputBuff]};
  string instruction = inputBuff;
  uint8_t inc;
  if (!strcmp(inputBuff,"push"))
  {
    cout<<"HOK";
    inc = 0b0001;
  }
  else
  {
    inc = 0b0100;
  }
  if (flexer->yylex() != Operand::Register)
  {
    end();
    return;
  }

  int regD = literalToUint_16(inputBuff);

  int sp = 6;

  InstructionOperator::tranInstr(instr, instruction, "regind", 0, sp, regD, inc);
  insertInstruction(instr);
  sectionTable->incSizeOfSection(SectionTable::currSec, instr.size());
  locationCounter += instr.size();
}

void Assembler::handleOneRegandOneOper()
{
  vector<uint8_t> instr;
  if(!strcmp(inputBuff,"ldr")){
      instr.push_back(InstructionOperator::TypeOfInstruction::ldr);
  }else if(!strcmp(inputBuff,"str")){
    instr.push_back(InstructionOperator::TypeOfInstruction::str);
  }

 // vector<uint8_t> instr = {InstructionOperator::instruction[inputBuff]};
  string instruction = inputBuff;
  int reg = flexer->yylex();
  int regD=literalToUint_16(inputBuff);
  int comma = flexer->yylex();

  if (reg != Operand::Register || comma != Operand::Comma)
  {
    end();
    return;
  }
  int operand = flexer->yylex();
  switch (operand)
  {
  case AddressingType::Val:
  {
    int oper = flexer->yylex();
    switch (oper)
    {
    case (Operand::Literal):
    {

      InstructionOperator::tranInstr(instr, instruction, "immed", literalToUint_16(inputBuff), 0b1111, regD);
    
      break;
    }
    case (Operand::Symbol):
    {

      if (!symbolTable->isDefined(inputBuff))
      {
        bool found = false;
        for (int i = 0; i < relocationTable.size(); i++)
        {
          if (relocationTable.at(i).getSection() == SectionTable::currSec)
          {

            bool need = symbolTable->relocNeeded(inputBuff, locationCounter, SectionTable::currSec, RelocationTable::RelocType::absolute);
            if (need)
            {
              relocationTable.at(i).insertReloc(inputBuff, RelocationTable::RelocType::absolute, 0, locationCounter + 3);
            }
            found = true;
            break;
          }
        }
        if (!found)
        {
          RelocationTable reloc;
          reloc.setSection(SectionTable::currSec);
          bool need = symbolTable->relocNeeded(inputBuff, locationCounter, SectionTable::currSec, RelocationTable::RelocType::absolute);
          if (need)
          {
            reloc.insertReloc(inputBuff, RelocationTable::RelocType::absolute, 0, locationCounter + 3);
          }
          relocationTable.push_back(reloc);
        }
      }
      InstructionOperator::tranInstr(instr, instruction, "immed", 0, 0b1111, regD);
      break;
    }

    default:
      end();
      break;
    }
    break;
  }
  case (AddressingType::RegRel):
  {

    int reg2 = flexer->yylex();
    int regS=literalToUint_16(inputBuff);

    if (reg2 != Operand::Register)
    {

      end();
      break;
    }
    int afterReg2 = flexer->yylex();
    if (afterReg2 == AddressingType::Regdir)
    {
      InstructionOperator::tranInstr(instr, instruction, "regind", 0, regS, regD);
      break;
    }
    if (strcmp(inputBuff, "+"))
    {
      end();
      break;
    }

    afterReg2 = flexer->yylex();
    switch (afterReg2)
    {
    case (Operand::Literal):
    {
      InstructionOperator::tranInstr(instr, instruction, "regindpom", literalToUint_16(inputBuff), regS, regD);
      break;
    }
    case (Operand::Symbol):
    {
      if (!symbolTable->isDefined(inputBuff))
      {
        bool found = false;
        for (int i = 0; i < relocationTable.size(); i++)
        {
          if (relocationTable.at(i).getSection() == SectionTable::currSec)
          {

            bool need = symbolTable->relocNeeded(inputBuff, locationCounter, SectionTable::currSec, RelocationTable::RelocType::absolute);
            if (need)
            {
              relocationTable.at(i).insertReloc(inputBuff, RelocationTable::RelocType::absolute, 0, locationCounter + 3);
            }
            found = true;
            break;
          }
        }
        if (!found)
        {
          RelocationTable reloc;
          reloc.setSection(SectionTable::currSec);
          bool need = symbolTable->relocNeeded(inputBuff, locationCounter, SectionTable::currSec, RelocationTable::RelocType::absolute);
          if (need)
          {
            reloc.insertReloc(inputBuff, RelocationTable::RelocType::absolute, 0, locationCounter + 3);
          }
          relocationTable.push_back(reloc);
        }
      }
      InstructionOperator::tranInstr(instr, instruction, "regindpom", 0, regS, regD);

      break;
    }
    default:
      end();
      break;
    }
    if (flexer->yylex() != AddressingType::Regdir)
    {
      end();
      return;
    }
    break;
  }
  case (Operand::Register):
  {
    InstructionOperator::tranInstr(instr, instruction, "regdir", 0, literalToUint_16(inputBuff), regD);
    break;
  }

  case (Operand::Literal):
  {

    InstructionOperator::tranInstr(instr, instruction, "mem", literalToUint_16(inputBuff),0b1111, regD);
    break;
  }
  case (Operand::Symbol):
  {

    if (!symbolTable->isDefined(inputBuff))
    {
      bool found = false;

      for (int i = 0; i < relocationTable.size(); i++)
      {
        if (relocationTable.at(i).getSection() == SectionTable::currSec)
        {

          bool need = symbolTable->relocNeeded(inputBuff, locationCounter, SectionTable::currSec, RelocationTable::RelocType::absolute);
          if (need)
          {
            relocationTable.at(i).insertReloc(inputBuff, RelocationTable::RelocType::absolute, 0, locationCounter + 3);
          }

          found = true;
          break;
        }
      }
      if (!found)
      {
        RelocationTable reloc;
        reloc.setSection(SectionTable::currSec);
        bool need = symbolTable->relocNeeded(inputBuff, locationCounter, SectionTable::currSec, RelocationTable::RelocType::absolute);
        if (need)
        {
          reloc.insertReloc(inputBuff, RelocationTable::RelocType::absolute, 0, locationCounter + 3);
        }
        relocationTable.push_back(reloc);
      }
    }
    InstructionOperator::tranInstr(instr, instruction, "mem", 0, 0b1111, regD);
    break;
  }
  case (AddressingType::SymbRelative):
  {
    int next = flexer->yylex();
    if (next != Operand::Symbol)
    {
      end();
      break;
    }
    int offset = 0;
    if (symbolTable->isDefinedAndInSection(inputBuff, SectionTable::currSec))
    {
      offset = symbolTable->findEntry(inputBuff)->symbol->value - locationCounter;
    }
    else
    {
      bool found = false;
      for (int i = 0; i < relocationTable.size(); i++)
      {
        if (relocationTable.at(i).getSection() == SectionTable::currSec)
        {
          relocationTable.at(i).insertReloc(inputBuff, RelocationTable::RelocType::relative, offset, locationCounter + 3);
          found = true;
        }
      }
      if (!found)
      {
        RelocationTable reloc;
        reloc.setSection(SectionTable::currSec);
        reloc.insertReloc(inputBuff, RelocationTable::RelocType::relative, offset, locationCounter + 3);
      }
    }
    int pc = 7;
    InstructionOperator::tranInstr(instr, instruction, "regindpom", offset, regD, pc);
  }

  default:
    end();
    break;
  }
  insertInstruction(instr);
  locationCounter += instr.size();
  sectionTable->incSizeOfSection(SectionTable::currSec, instr.size());
}

void Assembler::handleTwoRegister()
{
  vector<uint8_t> instr;
  if(!strcmp(inputBuff,"add")){
    instr.push_back(InstructionOperator::TypeOfInstruction::add);
  }else if(!strcmp(inputBuff,"sub")){
     instr.push_back(InstructionOperator::TypeOfInstruction::sub);
  }else if(!strcmp(inputBuff,"mul")){
     instr.push_back(InstructionOperator::TypeOfInstruction::mul);
  }else if(!strcmp(inputBuff,"div")){
     instr.push_back(InstructionOperator::TypeOfInstruction::div);
  }else if(!strcmp(inputBuff,"cmp")){
     instr.push_back(InstructionOperator::TypeOfInstruction::cmp);
  }else if(!strcmp(inputBuff,"and")){
     instr.push_back(InstructionOperator::TypeOfInstruction::andd);
  }else if(!strcmp(inputBuff,"or")){
     instr.push_back(InstructionOperator::TypeOfInstruction::orr);
  }else if(!strcmp(inputBuff,"xor")){
     instr.push_back(InstructionOperator::TypeOfInstruction::xorr);
  }else if(!strcmp(inputBuff,"test")){
     instr.push_back(InstructionOperator::TypeOfInstruction::test);
  }else if(!strcmp(inputBuff,"shl")){
     instr.push_back(InstructionOperator::TypeOfInstruction::shl);
  }else if(!strcmp(inputBuff,"shr")){
     instr.push_back(InstructionOperator::TypeOfInstruction::shr);
  }else if(!strcmp(inputBuff,"xchg")){
     instr.push_back(InstructionOperator::TypeOfInstruction::xchg);
  }
  //vector<uint8_t> instr = {InstructionOperator::instruction[inputBuff]};
  string instruction = inputBuff;
  int reg1, reg2, comma;
  reg1 = flexer->yylex();
  int regD=literalToUint_16(inputBuff);
  comma = flexer->yylex();
  reg2 = flexer->yylex();
  int regS=literalToUint_16(inputBuff);
  if (reg1 != Operand::Register || reg2 != Operand::Register || comma != Operand::Comma)
  {
    end();
    return;
  }
  InstructionOperator::tranInstr(instr, instruction, "unknown", 0, regS, regD, 0);
    insertInstruction(instr);
    locationCounter += instr.size();
    sectionTable->incSizeOfSection(SectionTable::currSec, instr.size());
  
}

void Assembler::handleZeroArg()
{
   vector<uint8_t> instr;
  if(!strcmp(inputBuff,"iret")){
    instr.push_back(InstructionOperator::TypeOfInstruction::iret);
  }else if(!strcmp(inputBuff,"ret")){
      instr.push_back(InstructionOperator::TypeOfInstruction::ret);
  }else if(!strcmp(inputBuff,"halt")){
    instr.push_back(InstructionOperator::TypeOfInstruction::halt);
  }
  
  bool found = false;
  insertInstruction(instr);
  locationCounter += instr.size();
  sectionTable->incSizeOfSection(SectionTable::currSec, instr.size());
}

void Assembler::handleJumps()
{

   vector<uint8_t> instr;
  if(!strcmp(inputBuff,"call")){
    instr.push_back(InstructionOperator::TypeOfInstruction::call);
  }else if(!strcmp(inputBuff,"jmp")){
      instr.push_back(InstructionOperator::TypeOfInstruction::jmp);
  }else if(!strcmp(inputBuff,"jeq")){
    instr.push_back(InstructionOperator::TypeOfInstruction::jeq);
  }else if(!strcmp(inputBuff,"jne")){
      instr.push_back(InstructionOperator::TypeOfInstruction::jne);
  }else if(!strcmp(inputBuff,"jgt")){
    instr.push_back(InstructionOperator::TypeOfInstruction::jgt);
  }

  cout<<hex<<unsigned(instr.at(0));
  string instruction = inputBuff;
  int addr;

  addr = flexer->yylex();
  switch (addr)
  {
  case AddressingType::SymbRelative:
  {
    if ((addr = flexer->yylex()) != Operand::Symbol)
    {
      end();
      return;
    }
    int offset = 0;
    if (symbolTable->isDefinedAndInSection(inputBuff, SectionTable::currSec))
    {
      offset = symbolTable->findEntry(inputBuff)->symbol->value - locationCounter;
    }
    else
    {
      bool found = false;
      for (int i = 0; i < relocationTable.size(); i++)
      {
        if (relocationTable.at(i).getSection() == SectionTable::currSec)
        {
          relocationTable.at(i).insertReloc(inputBuff, RelocationTable::RelocType::relative, offset, locationCounter + 3);
          found = true;
        }
      }
      if (!found)
      {
        RelocationTable reloc;
        reloc.setSection(SectionTable::currSec);
        reloc.insertReloc(inputBuff, RelocationTable::RelocType::relative, offset, locationCounter + 3);
      }
    }
    int pc = 7;
    InstructionOperator::tranInstr(instr, instruction, "regdirpom", offset, pc);

    break;
  }
  case AddressingType::Indirect:
  {
    addr = flexer->yylex();
    switch (addr)
    {
    case (AddressingType::RegRel):
    {
      int operand = flexer->yylex();
      int operInt=literalToUint_16(inputBuff);

      int symb = flexer->yylex();

      if (symb == AddressingType::Regdir && operand == Operand::Register)
      {
        InstructionOperator::tranInstr(instr, instruction, "regind", 0,operInt);
        break;
      }
      else
      {
        if (strcmp(inputBuff, "+"))
        {

          end();
          break;
        }
      }

      int add = flexer->yylex();
      switch (add)
      {
      case (Operand::Literal):
      {
        InstructionOperator::tranInstr(instr, instruction, "regindpom", literalToUint_16(inputBuff), operInt);
        break;
      }
      case (Operand::Symbol):
      {
        if (!symbolTable->isDefined(inputBuff))
        {
          bool found = false;
          for (int i = 0; i < relocationTable.size(); i++)
          {
            if (relocationTable.at(i).getSection() == SectionTable::currSec)
            {

              bool need = symbolTable->relocNeeded(inputBuff, locationCounter, SectionTable::currSec, RelocationTable::RelocType::absolute);
              if (need)
              {
                relocationTable.at(i).insertReloc(inputBuff, RelocationTable::RelocType::absolute, 0, locationCounter + 3);
              }
              found = true;
              break;
            }
          }
          if (!found)
          {
            RelocationTable reloc;
            reloc.setSection(SectionTable::currSec);
            bool need = symbolTable->relocNeeded(inputBuff, locationCounter, SectionTable::currSec, RelocationTable::RelocType::absolute);
            if (need)
            {
              reloc.insertReloc(inputBuff, RelocationTable::RelocType::absolute, 0, locationCounter + 3);
            }
            relocationTable.push_back(reloc);
          }
        }
        InstructionOperator::tranInstr(instr, instruction, "regindpom", literalToUint_16(inputBuff),operInt);

        
        break;
      }

      default:
        end();
        break;
      }
      if (flexer->yylex() != AddressingType::Regdir)
      {
        end();
        break;
      }
      break;
    }
    case Operand::Register:
    {
      InstructionOperator::tranInstr(instr, instruction, "regdir", 0, (uint8_t)literalToUint_16(inputBuff));
    break;
    }
    case (Operand::Symbol):
    {

      if (!symbolTable->isDefined(inputBuff))
      {

        bool found = false;
        for (int i = 0; i < relocationTable.size(); i++)
        {
          if (relocationTable.at(i).getSection() == SectionTable::currSec)
          {

            bool need = symbolTable->relocNeeded(inputBuff, locationCounter, SectionTable::currSec, RelocationTable::RelocType::absolute);
            if (need)
            {
              relocationTable.at(i).insertReloc(inputBuff, RelocationTable::RelocType::absolute, 0, locationCounter + 3);
            }
            found = true;
            break;
          }
        }
        if (!found)
        {
          RelocationTable reloc;
          reloc.setSection(SectionTable::currSec);
          bool need = symbolTable->relocNeeded(inputBuff, locationCounter, SectionTable::currSec, RelocationTable::RelocType::absolute);
          if (need)
          {
            reloc.insertReloc(inputBuff, RelocationTable::RelocType::absolute, 0, locationCounter + 3);
          }
          relocationTable.push_back(reloc);
        }
      }

      InstructionOperator::tranInstr(instr, instruction, "mem", 0);

      break;
    }
    case (Operand::Literal):
    {
      InstructionOperator::tranInstr(instr, instruction, "mem", literalToUint_16(inputBuff));
      break;
    }
    }
    /*if(flexer->yylex()!=AddressingType::Regdir){
      end();
      break;
    }*/

    break;
  }

  case Operand::Literal:
  {
    InstructionOperator::tranInstr(instr, instruction, "immed", literalToUint_16(inputBuff));
    break;
  }

  case Operand::Symbol:
  {

    if (!symbolTable->isDefined(inputBuff))
    {
      bool found = false;
      for (int i = 0; i < relocationTable.size(); i++)
      {
        if (relocationTable.at(i).getSection() == SectionTable::currSec)
        {

          bool need = symbolTable->relocNeeded(inputBuff, locationCounter, SectionTable::currSec, RelocationTable::RelocType::absolute);
          if (need)
          {
            relocationTable.at(i).insertReloc(inputBuff, RelocationTable::RelocType::absolute, 0, locationCounter + 3);
          }
          found = true;
          break;
        }
      }
      if (!found)
      {
        RelocationTable reloc;
        reloc.setSection(SectionTable::currSec);
        bool need = symbolTable->relocNeeded(inputBuff, locationCounter, SectionTable::currSec, RelocationTable::RelocType::absolute);
        if (need)
        {
          reloc.insertReloc(inputBuff, RelocationTable::RelocType::absolute, 0, locationCounter + 3);
        }
        relocationTable.push_back(reloc);
      }
    }
    cout<<"CALL"<<endl;
    InstructionOperator::tranInstr(instr, instruction, "immed", 0);
    break;
  }

  case StringType::Comment:
  {

    handleComments();
    break;
  }
  default:

    end();
    break;
  }
  insertInstruction(instr);
  locationCounter += instr.size();
  sectionTable->incSizeOfSection(SectionTable::currSec, instr.size());
}

void Assembler::cleanUpReloc()
{
  for (int i = 0; i < relocationTable.size(); i++)
  {
    for (int j = 0; j < relocationTable.at(i).sizeOfTable(); j++)
    {
      string name = relocationTable.at(i).getTabl(j)->info->SymbolName;
      if (symbolTable->isLocal(name))
      {
        relocationTable.at(i).setAddendSymbol(relocationTable.at(i).getTabl(j)->info->offset, symbolTable->findEntry(name)->symbol->value);
        relocationTable.at(i).setSymbolSection(name, symbolTable->findEntry(name)->symbol->SectionName);
      }
    }
  }
}

void Assembler::insertInstruction(vector<uint8_t> info){
   bool found = false;
  for (int i = 0; i < relocationTable.size(); i++)
    {
    if (relocationTable.at(i).getSection() == SectionTable::currSec)
      {
        relocationTable.at(i).insertTable(info);
        found = true;
      }
    }
    if (!found)
    {
      RelocationTable reloc;
      reloc.setSection(SectionTable::currSec);
      reloc.insertTable(info);
      relocationTable.push_back(reloc);
    }
}
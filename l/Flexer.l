%{
#include "Assembler.hpp"
%}

%option c++
%option noyywrap

global ".global"
extern ".extern"
wordAlloc ".word"
skip ".skip "{literal}
end ".end"
label [a-zA-Z_0-9]+:
string "\""+[-~\\^\"]+"\""
section ".section "[\.]*{symbol}
blank " "

hexNumber 0x[0-9A-Fa-f]{4}:
register (("r"{1}[0-7]{1})|"sp"|"psw"|"pc")
literal (([-]*"0x"{1}[0-9A-Fa-f]{1,4})|([-]*[0-9]+))
symbol [a-zA-Z_][_a-zA-Z0-9]*


%%
{hexNumber} { }
("halt"|"iret"|"ret") {Assembler::inputInstr(yytext); return Assembler::Instruction::zeroArguments;}
("call"|"jmp"|"jeq"|"jne"|"jgt") {Assembler::inputInstr(yytext); return Assembler::Instruction::oneOperand;}
("push"|"pop"|"not"|"int") {Assembler::inputInstr(yytext); return Assembler::Instruction::oneRegister;}
("xchg"|"add"|"sub"|"mul"|"div"|"cmp"|"and"|"or"|"xor"|"test"|"shl"|"shr") {Assembler::inputInstr(yytext); return Assembler::Instruction::TwoRegisters;}
("ldr"|"str") {Assembler::inputInstr(yytext); return Assembler::Instruction::OneRegandOneOperand;}
("$") {Assembler::inputInstr(yytext); return Assembler::AddressingType::Val;}
("*") {Assembler::inputInstr(yytext); return Assembler::AddressingType::Indirect;}
("%") {Assembler::inputInstr(yytext); return Assembler::AddressingType::SymbRelative;}
("\[") {Assembler::inputInstr(yytext); return Assembler::AddressingType::RegRel;}
("\]") {Assembler::inputInstr(yytext); return Assembler::AddressingType::Regdir;}
{label} {
  yytext[strlen(yytext)-1]='\0';
  Assembler::inputInstr(yytext);
  return Assembler::StringType::Label;
}

{register} {

  if(!strcmp(yytext,"pc"))
  {
    Assembler::inputInstr("7");
  }
  else if(!strcmp(yytext,"sp")){
    Assembler::inputInstr("6");
  }
  else if (!strcmp(yytext,"psw")){
    Assembler::inputInstr("8");}
  else{
    Assembler::inputInstr(yytext+1);}

  return Assembler::Operand::Register;}

{symbol} {Assembler::inputInstr(yytext); return Assembler::Operand::Symbol;}
{literal} {Assembler::inputInstr(yytext); return Assembler::Operand::Literal;}
{global} {Assembler::inputInstr(yytext); return Assembler::Instruction::Global;}
{extern} {Assembler::inputInstr(yytext); return Assembler::Instruction::Extern;}
{section} {Assembler::inputInstr(yytext+9); return Assembler::Instruction::Section;}
{wordAlloc} {Assembler::inputInstr(yytext); return Assembler::Instruction::WordAlloc;}
{skip} {Assembler::inputInstr(yytext+6); return Assembler::Instruction::Skip;}
{string} {Assembler::inputInstr(yytext); return Assembler::Operand::String;}
{blank} { }

"#" {Assembler::inputInstr(yytext); return Assembler::StringType::Comment;}
\n {Assembler::endOfLine(); return Assembler::Instruction::Eol;}
".end" {return 0;}
("+"|"-"|"("|")") {Assembler::inputInstr(yytext); return Assembler::Instruction::Operator;}
"," {Assembler::inputInstr(yytext); return Assembler::Operand::Comma;}
[."] { }
<<EOF>> {return 0;}
. {Assembler::inputInstr(yytext); return 11111;}

%%


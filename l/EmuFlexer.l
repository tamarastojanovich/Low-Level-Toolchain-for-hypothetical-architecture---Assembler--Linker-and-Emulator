%{
#include "../h/Emulator.hpp"
%}

%option c++
%option noyywrap

%%
(0x[a-fA-F0-9]{4}) {
  yytext[strlen(yytext)-1]='\0';
  Emulator::inputInstr(yytext);
  return Emulator::ReadingType::address;
}

([a-fA-F0-9]{2}) {

  Emulator::inputInstr(yytext);
  return Emulator::ReadingType::byte;
}

('\n') {
  return Emulator::ReadingType::eol;
}

" " { }
<<EOF>> {return 0;}

%%
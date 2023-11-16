#include "../h/SymbolTable.hpp"
#include "../h/RelocationTable.hpp"
#include "string.h"
#include <iostream>
using namespace std;

int SymbolTable::id=0;



SymTable* SymbolTable::findEntry(string name){
  for(SymTable* curr=symbols;curr!=nullptr;curr=curr->next){
    const char* needed=name.c_str();
    const char* have=curr->symbol->SymbolName.c_str();
    if(!strcmp(needed,have)){
      
      return curr;
  }}
  return nullptr;
  
}

SymTable* SymbolTable::getSym(int i){
  int cnt=0;
  SymTable* curr=symbols;
  while(cnt!=i){
    curr=curr->next;
    cnt++;
  }
  return curr;
}

void SymbolTable::insertSymbol(string name,int loc,string section){

 if(!symbols){
   
    symbols=new SymTable(new Sym(id++,name,loc,section),nullptr);
    tail=symbols;
    /*symbols->symbol->SymbolName=name;
    symbols->symbol->value=loc;
    symbols->symbol->SectionName=section;
   
    symbols->symbol->id_symbol=id++;
    tail=symbols;*/
  }else{
    
    /*if(glob){
      sym->symbol->global=true;
       sym->symbol->defined=false;
    }else{
      sym->symbol->local=true;
      sym->symbol->defined=true;
    }*/
   
   //sym->symbol->id_symbol=id++;
    if(!tail){
      tail=new SymTable(new Sym(id++,name,loc,section),nullptr);
    }else{

    SymTable* sym=new SymTable(new Sym(id++,name,loc,section),nullptr);    
    tail->next=sym;
    tail=sym;
    }
  }
}

void SymbolTable::setValue(string name,int value){
  cout<<"VAL"<<value<<endl;
  for(SymTable* curr=symbols;curr;curr=curr->next){
    if(!strcmp(curr->symbol->SymbolName.c_str(),name.c_str())) curr->symbol->value+=value;
  }
}

void SymbolTable::globalize(string name){
 /* if(contains(name)){
    for(SymTable* curr=symbols;curr;curr=curr->next){
      if(curr->symbol->SymbolName==name)curr->symbol->global=true;
    }
  }else{
    insertSymbol(name);

  }*/
  if(!contains(name)){
  insertSymbol(name);
  }
  for(SymTable* curr=symbols;curr;curr=curr->next){
      if(curr->symbol->SymbolName==name)curr->symbol->global=true;
    }

}

void SymbolTable::externSym(string name){

  if(!contains(name)){
   
   insertSymbol(name);
  }
  
  for(SymTable* curr=symbols;curr;curr=curr->next){
    if(curr->symbol->SymbolName==name)curr->symbol->is_extern=true;
  }
}

bool SymbolTable::contains(string name){
  for(SymTable* curr=symbols;curr;curr=curr->next){
    if(curr->symbol->SymbolName==name)return true;
  }
  return false;
}

bool SymbolTable::isDefined(string name){
  
  for(SymTable* curr=symbols;curr;curr=curr->next){
    if(!strcmp(curr->symbol->SymbolName.c_str(),name.c_str())){
      
      if(strcmp(curr->symbol->SectionName.c_str(),"undefined")&& curr->symbol->value>=0) return true;
      
    }
    
  }
  return false;
}

void SymbolTable::defineSymbol(string name,int loc,string sect){
  for(SymTable* curr=symbols;curr;curr=curr->next){
    if(curr->symbol->SymbolName==name){
      if(curr->symbol->SectionName!="undefined" || curr->symbol->is_extern || curr->symbol->value>0){
        cout<<"Multiple definitions of symbol";
        return;
      }else{
        curr->symbol->value=loc;
        curr->symbol->SectionName=sect;
        return;
      }
    }
  }
  insertSymbol(name,loc,sect);

}

bool SymbolTable::relocNeeded(string name,int loc,string sect,int reloc){
  if(!contains(name)){
    insertSymbol(name);
  }

  for(SymTable* curr=symbols;curr!=nullptr;curr=curr->next){
    if(curr->symbol->SymbolName==name){
      if(curr->symbol->SectionName=="undefined"){
        return true;
      }
    }
  }
  Flink fl; //sta cemo sve ukljuciti ovde?razmidlii
  fl.location=loc;
  fl.section=sect;
  if(reloc==RelocationTable::absolute)fl.reloc_type="absolute";
  else fl.reloc_type="relative";

  for(SymTable* curr=symbols;curr!=nullptr;curr=curr->next){
    if(curr->symbol->SymbolName==name){
      curr->symbol->flinkList.push_back(fl);
    }
  }
  //flink
  return false;
}

bool SymbolTable::isDefinedAndInSection(string name,string sect){
  for(SymTable* curr=symbols;curr;curr=curr->next){
    if(curr->symbol->SymbolName==name){
      if(curr->symbol->SectionName==sect) return true;
      
    }
  
  }
  return false;
}

SymTable* SymbolTable::operator[](int i){
  int curr=0;
  SymTable* ret;
  ret=symbols;
  while(curr<=i){
    ret=ret->next;
  }
  return ret;
}

int SymbolTable::sizeOfTable(){
  int cnt=0;
  for(SymTable* curr=symbols;curr;curr=curr->next){
    cnt++;
  }
  return cnt;
}

bool SymbolTable::isLocal(string name){
  for(SymTable* curr=symbols;curr;curr=curr->next){
    if(curr->symbol->SymbolName==name){
      if(curr->symbol->global||curr->symbol->is_extern)return false;
    }
  }
  return true;
}
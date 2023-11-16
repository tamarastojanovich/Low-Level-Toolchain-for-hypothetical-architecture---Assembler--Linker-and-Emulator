#include <iostream>
#include <fstream>
#include <iomanip>
#include "../h/SymbolTable.hpp"
#include "../h/SectionTable.hpp"
#include "../h/RelocationTable.hpp"
using namespace std;
void printTable(SymbolTable* symb,SectionTable* sect,vector<RelocationTable> rel,string output){
  ofstream out(output,ios::out);
  int j=0;
  out<<"----"<<"Symbol Table"<<"----"<<endl;

  out<<left<<setw(8)<<"Index";
  out<<left<<setw(12)<<"SymbolValue";
  out<<left<<setw(12)<<"Bind";
  out<<left<<setw(12)<<"Section";
  out<<left<<setw(12)<<"Size";
  out<<left<<setw(12)<<"Name ";
  out<<endl;//imena svih kolona za tabelu simbola

  for(int i=0;i<symb->sizeOfTable();i++){
   
    
  out<<left<<setw(8)<<j++;
 
  out<<left<<setw(12)<<setfill(' ')<<hex<<symb->getSym(i)->symbol->value;
  

  
  if(symb->getSym(i)->symbol->global || symb->getSym(i)->symbol->is_extern){
    out<<setfill(' ')<<left<<setw(12)<<"GLOBAL";
  }else{
    out<<setfill(' ')<<left<<setw(12)<<"LOCAL";
  }
  out<<setfill(' ')<<left<<setw(12)<<symb->getSym(i)->symbol->SectionName;
  out<<setfill(' ')<<left<<setw(12)<<0;
  out<<setfill(' ')<<left<<setw(12)<<symb->getSym(i)->symbol->SymbolName;
  out<<endl;
  }
  
  out<<setw(30)<<"           ----Section Table----          "<<endl;
  out<<left<<setw(10)<<"Index";
  out<<left<<setw(10)<<"Value";
  out<<left<<setw(10)<<"Size";
  out<<left<<setw(10)<<"Type";
  out<<left<<setw(10)<<"Bind";
  out<<left<<setw(10)<<"Name"<<endl;
  j=0;
  for(int i=0;i<sect->sizeOfTable();i++){
    out<<left<<setw(10)<<j++;
    out<<left<<setw(10)<<setfill(' ')<<0;
    out<<left<<setw(10)<<hex<<sect->getSect(i)->section->size;
    out<<left<<setw(10)<<"SECTION";
    out<<left<<setw(10)<<"LOCAL";
    out<<left<<setw(10)<<sect->getSect(i)->section->SectionName;
    out<<endl;
  }

  out<<"----"<<"RELOCATION TABLES"<<"----"<<endl;
  for(int i=0;i<rel.size();i++){
    out<<"rel."<<rel[i].getSection()<<endl;
    out<<left<<setw(10)<<"Offset";
    out<<left<<setw(10)<<"Type";
    out<<left<<setw(10)<<"Symbol";
    out<<left<<setw(8)<<"Addend";
    out<<endl;
   // vector<uint8_t> cont=rel[i].getContent();

    for(int j=0;j<rel.at(i).sizeOfTable();j++){
      out<<setfill(' ')<<right<<setw(8)<<hex<<rel.at(i).getTabl(j)->info->offset;
      out<<left<<setfill(' ')<<setw(10)<<(rel[i].getTabl(j)->info->type_of_rel==RelocationTable::RelocType::absolute?"  ABS":"  PC_REL");
      out<<left<<setw(10)<<rel[i].getTabl(j)->info->SymbolName;
      out<<left<<setw(8)<<rel[i].getTabl(j)->info->addend;
      out<<endl;

    }
  }
  out<<"---MACHINE CODE---"<<endl;
  for(int i=0;i<rel.size();i++){//getting machine code
    out<<"For Section "<<rel.at(i).getSection()<<endl;
    vector<uint8_t> content=rel.at(i).getContent();
    int k=0;
    for(int j=0;j<content.size();j++,k++){
      out<<hex<<setw(2)<<setfill('0')<<right<<unsigned(content.at(j))<<"  ";
      if(k==3) out<<"  ";
      if(k==7 || (j+1)==content.size()) {
        k=-1;
        out<<endl;}
    }

  }
}

uint16_t offset=0;
int numofBytes=0;

void printLinker(string output,vector<vector<uint8_t>>content){
 
  ofstream out(output,ios::out);
//da odstampamo linkere

for(int i=0;i<content.size();i++){
  for(int j=0;j<content.at(i).size();j++){
  if(numofBytes==0){
  out<<"0x"<<hex<<setw(4)<<setfill('0')<<right<<offset<<": ";
  }
 // cout<<content.at(i);
  cout<<std::hex<<setfill('0')<<setw(2)<<right<<(unsigned)content.at(i).at(j)<<" ";
  //cout<<"COKO";
  out<<std::hex<<setfill('0')<<setw(2)<<right<<(unsigned)content.at(i).at(j)<<"  ";
  //out<<"COKO";
  
  numofBytes++;
  if(numofBytes==8 ||(j+1)==content.at(i).size()){out<<endl;
  offset+=numofBytes;
  cout<<endl;
  numofBytes=0;
  }
  }
}

}


#include "../h/Linker.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
using namespace std;

void printLinker(string output,vector<vector<uint8_t>>content);

vector<string> splitString(string str){
  vector<string> words;
  string word="";
  bool started=false;
  for(auto x:str){
    if(x==' '){
      if(started){
        words.push_back(word);
        started=false;
        word="";
      }
    }else{
      if(!started){
        started=true;  
      }
      word=word+x;
    }
  }
  if(strcmp(word.c_str(),"")){
    words.push_back(word);
  }
  return words;
}

Linker::Linker(vector<string> files,string output,bool hex){
  symbols=new SymbolTable();
  sections=new SectionTable();
  inputfile=files;
  outputFile=output;

  readFile();
  if(terminate){
    cout<<"Program terminated";
    return;
  }

  link();

  if(terminate){
    cout<<"Program terminated";
    return;
  }
  vector<vector<uint8_t>> write;
  for(int i=0;i<relocatinTables.size();i++){
    
  write.push_back(relocatinTables.at(i).getContent());
    
}
  printLinker(output,write);
}
void Linker::readFile(){
  for(int i=0;i<inputfile.size();i++){
  ifstream ObjectFile(inputfile.at(i),std::ios::in|std::ios::binary);
  if(!ObjectFile.good()){
    terminate=true;
    return;
    }
    string line;
    bool symbol=true;
    bool section=false;
    bool reloc=false;
    map<string,int> sectionsBefore;
    int sizeOfpreviousSection=0;
    while(getline(ObjectFile,line)){
    if(line.find("Symbol Table")!=std::string::npos){
      symbol=true;
      section=false;
      continue;
      }
      while(symbol){
      getline(ObjectFile,line);
      if(line.find("Section Table")!=std::string::npos){
      section=true;
      symbol=false;
      reloc=false;
      getline(ObjectFile,line);
      break;
        }
      vector<string> info=splitString(line);
      int value,size,index;
      string name,section,bind;
      char* p;
      index=strtoul(info.at(0).c_str(),&p,16);
      value=strtoul(info.at(1).c_str(),&p,16);
      bind=info.at(2);
      section=info.at(3);
      size=strtoul(info.at(4).c_str(),&p,16);
      name=info.at(5);
      if(!symbols->contains(name) && bind=="GLOBAL"){
      symbols->insertSymbol(name,value,section);
      symbols->globalize(name);
      }else{
      if(symbols->isDefined(name)){
      if (bind=="GLOBAL"&&section!="undefined"){
      cout<<"Multiple definition of symbol "<<name<<endl;
      }
      }else{
      symbols->defineSymbol(name,value+sections->getSizeOfSection(section),section);
            }
      }
     
    }
    while(section){
      getline(ObjectFile,line);
      if(line.find("RELOCATION TABLES")!=std::string::npos){
        reloc=true;
        section=false;
        continue;
      }
     vector<string> info=splitString(line);
     
      string token;
      int index,value,size;
      string name,bind,type;
      char* p;
      index=strtoul(info.at(0).c_str(),&p,16);
      value=strtoul(info.at(1).c_str(),&p,16);
      size=strtoul(info.at(2).c_str(),&p,16);
      type=info.at(3);
      bind=info.at(4);
      name=info.at(5);
      if(sections->findSection(name)){
        sectionsBefore.insert({name,sections->getSizeOfSection(name)});
        sections->incSizeOfSection(name,size);
      }else{
        sections->insertSection(name,size);
        sections->setSizeOfSection(name,size);
      }
    }
    while(reloc){
    getline(ObjectFile,line);
    if(line.find("MACHINE CODE")!=std::string::npos){
      reloc=false;
      getline(ObjectFile,line);
      continue;
    }
    string sec;
    if(line.find("rel.")!=std::string::npos){
      sec=line.substr(4,line.size()-4);
      currRelocSec=sec;
      bool has=false;
      for(int i=0;i<relocatinTables.size();i++){
        if(relocatinTables.at(i).getSection()==sec){
        indexOfCurrSec=i;
        has=true;
        }
      }
      if(!has){RelocationTable reloc;
      reloc.setSection(sec);
      indexOfCurrSec=relocatinTables.size();
      relocatinTables.push_back(reloc);}
      getline(ObjectFile,line);
      continue;
    }
    int offset,addend;
    char *p;
    int rel_type;
    string type,symbolName;
    stringstream ss(line);
    vector<string> info=splitString(line);
    offset=strtoul(info.at(0).c_str(),&p,16);
    type=info.at(1);
    if(type=="ABS"){
      rel_type=1;
    }else{
      rel_type=0;
    }
    if(sectionsBefore.find(currRelocSec)!=sectionsBefore.end()){
      offset+=sectionsBefore.at(currRelocSec);//ja ovde zapravo dodajem prethodnu velicinu segmenta
    }
    symbolName=info.at(2);
    addend=strtoul(info.at(3).c_str(),&p,16);
    if(!rel_type) {
      addend=addend+sizeof(uint16_t);
      cout<<"ADDEND"<<addend;
    }
    relocatinTables.at(indexOfCurrSec).insertReloc(symbolName,rel_type,addend,offset);
    }
    stringstream ss(line);
    vector<string> info=splitString(line);
    if(line.find("For Section")!=std::string::npos){
    machineCodeSec=info.at(2);
    continue;
     }
    vector<uint8_t> code;
    string s="";
    for(int i=0;i<info.size();i++){
    char* p;
    unsigned int x=strtoul(info.at(i).c_str(),&p,16);
    uint8_t xx;
    xx=x&0xFF;
    code.push_back(xx);
  }
  insertInstruction(code);
    }
}
}

void Linker::link(){
  for(int i=0;i<symbols->sizeOfTable();i++){
    string sec=symbols->getSym(i)->symbol->SectionName;
    const char* c=sec.c_str();
    if(!strcmp(c,"undefined")){
      terminate=true;
      return;
    }
  }
  for(int i=0;i<symbols->sizeOfTable();i++){//sredjivanje simbola
    int sz=sections->getOffsetFromStart(symbols->getSym(i)->symbol->SectionName);
    symbols->setValue(symbols->getSym(i)->symbol->SymbolName,sz);
  }

  int S,A,P;
  uint16_t content;
  for(int i=0;i<relocatinTables.size();i++){
  
    for(int j=0;j<relocatinTables.at(i).sizeOfTable();j++){
      RelocTable* r=relocatinTables.at(i).getTabl(j);
      if(r->info->addend<=0){
      if(r->info->type_of_rel==RelocationTable::RelocType::absolute){
        A=r->info->addend;
        S=symbols->findEntry(r->info->SymbolName)->symbol->value;
        content=A+S;

        relocatinTables.at(i).updateMachineCode(content,r->info->offset);
        relocatinTables.at(i).updateOffset(r->info->offset,sections->getOffsetFromStart(relocatinTables.at(i).getSection())+r->info->offset);

      }else{
        A=r->info->addend;
        S=symbols->findEntry(r->info->SymbolName)->symbol->value;
        P=r->info->offset+sections->getOffsetFromStart(relocatinTables.at(i).getSection());
        content=S+A-P;
        
        relocatinTables.at(i).updateMachineCode(content,r->info->offset);
        relocatinTables.at(i).updateOffset(r->info->offset,sections->getOffsetFromStart(relocatinTables.at(i).getSection())+r->info->offset);
 
      }
      }else{
        relocatinTables.at(i).updateMachineCode(r->info->addend+sections->getOffsetFromStart(r->info->SymbolName),r->info->offset);

      }
    }
  }  
  
}
void Linker::insertInstruction(vector<uint8_t> info){
   bool found = false;
   for (int i = 0; i < relocatinTables.size(); i++)
    {
    if (relocatinTables.at(i).getSection() == machineCodeSec)
      {
        relocatinTables.at(i).insertTable(info);
        found = true;
      }
    }
    if (!found)
    {
      RelocationTable reloc;
      reloc.setSection(machineCodeSec);
      reloc.insertTable(info);
      relocatinTables.push_back(reloc);
    }
}



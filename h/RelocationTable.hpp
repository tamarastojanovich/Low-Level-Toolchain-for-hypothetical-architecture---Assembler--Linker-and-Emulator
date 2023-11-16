#include <vector>
#include <iostream>
#include <string>
#include <map>

using namespace std;


struct Entry{
  //check if i need anything else later on
  int offset;
  //string SectionName;
  string SymbolName;
  int type_of_rel;
  int addend;
  Entry(int o,string s,int r,int a):offset(o),SymbolName(s),type_of_rel(r),addend(a){}

};

struct RelocTable{
  Entry* info;
  RelocTable* next;
  RelocTable(Entry* e,RelocTable* n=nullptr):info(e),next(n){}
};

class RelocationTable{




RelocTable* head,*tail;
string section;
int offset=0;
vector<uint8_t> contentTable;

//tabela sadrzaja

public:
  
enum RelocType{
  relative,
  absolute
};
RelocationTable(string sec="undefined"):section(sec){
  tail=nullptr;
  head=nullptr;
}
void insertReloc(string sym,int rel_type,int ad,int off,string word=" ",bool literal=false);
void insertTable(vector<uint8_t> array);//sta mi treba od parametra
void setSection(string name){section=name;}
uint16_t literalToUint_16(string &l);
void printContent();
vector<uint8_t> getContent(){return contentTable;}
string getSection(){return section;}
int sizeOfTable();
void setSymbolSection(string name,string sect);
void setAddendSymbol(int offset,int addend);
void updateMachineCode(uint16_t update,int offset);
void updateOffset(int before,int after);
RelocTable* operator[](int i);
RelocTable* getTabl(int i);

void resolve();
};


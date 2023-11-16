#include <vector>
#include <iostream>
#include <string>
using namespace std;

struct Flink{
    string section;
    int location;
    string reloc_type;
  };

  struct Sym{
  int id_symbol;
  int value;
  bool global;
  bool local;
  bool is_extern;
  bool defined;
  string SymbolName;
  string SectionName;
  vector<Flink> flinkList;
  Sym(int id,string sym,int v=-1,string sec="undefined",bool g=false,bool l=false,bool e=false,bool d=false):id_symbol(id),value(v),global(g),local(l),is_extern(e),
  defined(d),SymbolName(sym),SectionName(sec){}
  };

struct SymTable{
  Sym* symbol;
  SymTable* next;
  SymTable(Sym* s,SymTable* n=nullptr):symbol(s),next(n){}
};

class SymbolTable{
private:

  
SymTable* symbols;
SymTable* tail;
static int id;
  
  
public:

  SymbolTable(){symbols=nullptr;}
  void insertSymbol(string name,int loc=-1,string section="undefined");
  void defineSymbol(string name,int loc,string sect);
  void setValue(string name,int value);
  SymTable* findEntry(string name);//ovo mogu da koristim kad proveravam da li je simbol vec def
  void globalize(string name);
  bool isDefined(string name);
  bool isDefinedAndInSection(string name,string sect);
  bool  relocNeeded(string name,int loc,string sect,int reloc);
  int SymbolOffset(string name);//
  bool contains(string name);
  void externSym(string name);
  SymTable* getSym(int i);
  bool isLocal(string name);
  int sizeOfTable();
  //preklopiti operatore za dohvatanje elemente niza
  SymTable* operator[](int i);

};
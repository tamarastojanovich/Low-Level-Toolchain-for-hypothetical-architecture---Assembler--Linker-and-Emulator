#include <vector>
#include <iostream>
#include <string>
using namespace std;

struct Section{
  int id_section;
  string SectionName;
  int size;
  vector<int> offsets;
  Section(int id,string s,int sz=0):id_section(id),SectionName(s),size(sz){}
  };
  
struct SectionTabl{
    Section* section;
    SectionTabl* next;
    SectionTabl(Section* s,SectionTabl* n=nullptr):section(s),next(n){}
  };
class SectionTable{
  

  

  SectionTabl* head,*tail;

  static int id;
  public:
  static string currSec;

  void insertSection(
    string name,int sz=0
  );

  void setSizeOfSection(string name,int sz);
  bool findSection(string name);
  void incSizeOfSection(string name,int sz);
  int getSizeOfSection(string name);
  void incSizeOfSectionsAfter(string name,int sz);
  int sizeOfTable();
  int getOffsetFromStart(string sec);
  SectionTabl* getSect(int i);
  SectionTabl* operator[](int i);

};

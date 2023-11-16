#include "../h/SectionTable.hpp"
#include "string.h"

int SectionTable::id=0;
string SectionTable::currSec;
void SectionTable::insertSection(string name,int sz){
  if(!head){
    //Section* sec=new Section(id++,name);
    head=new SectionTabl(new Section(id++,name));
    tail=head;
  }else{
  //  Section* sec=new Section(id++,name);
    SectionTabl* sect=new SectionTabl(new Section(id++,name));
    
    //sec->section->size=sz;
    tail->next=sect;
    tail=tail->next;
  }
}

void SectionTable::setSizeOfSection(string name,int sz){
    for(SectionTabl* curr=head;curr!=nullptr;curr=curr->next){
      const char* needed=name.c_str();
      const char* have=curr->section->SectionName.c_str();
      if(!strcmp(needed,have)){
        curr->section->size=sz;
      }
    }
  }

bool SectionTable::findSection(string name){
  for(SectionTabl* curr=head;curr!=nullptr;curr=curr->next){
    const char* needed=name.c_str();
      const char* have=curr->section->SectionName.c_str();
      if(!strcmp(needed,have))return true;
  }
  return false;
}

void SectionTable::incSizeOfSection(string name,int sz){
  for(SectionTabl* curr=head;curr!=nullptr;curr=curr->next){
    const char* needed=name.c_str();
      const char* have=curr->section->SectionName.c_str();
      if(!strcmp(needed,have))curr->section->size+=sz;
  }
}

void SectionTable::incSizeOfSectionsAfter(string name,int sz){
  for(SectionTabl* curr=head;curr!=nullptr;curr=curr->next){
    const char* needed=name.c_str();
      const char* have=curr->section->SectionName.c_str();
      if(!strcmp(needed,have)){
        for(SectionTabl* cur=curr->next;cur;cur=cur->next){
          cur->section->size+=sz;
        }
      }
  }
}

int SectionTable::getSizeOfSection(string name){
  for(SectionTabl* curr=head;curr!=nullptr;curr=curr->next){
   const char* needed=name.c_str();
      const char* have=curr->section->SectionName.c_str();
      if(!strcmp(needed,have)) return curr->section->size;
  }
  return 0;
}

SectionTabl* SectionTable::getSect(int i){
  int cnt=0;
  SectionTabl* curr=head;
  while(cnt!=i){
    curr=curr->next;
    cnt++;
  }
  return curr;
}

SectionTabl* SectionTable::operator[](int i){
  int curr=0;
  SectionTabl* ret;
  ret=head;
  while(curr<=i){
    ret=ret->next;
  }
  return ret;
}

int SectionTable::sizeOfTable(){
  int cnt=0;
  for(SectionTabl* curr=head;curr;curr=curr->next){
    cnt++;
  }
  return cnt;
}

int SectionTable::getOffsetFromStart(string sec){
  int off=0;
  for(SectionTabl* curr=head;curr;curr=curr->next){
    cout<<"SECTION NAME"<<curr->section->SectionName<<curr->section->size<<endl;
    const char* needed=sec.c_str();
      const char* have=curr->section->SectionName.c_str();
      if(!strcmp(needed,have)) break;
      else off+=curr->section->size;
  }
  return off;
}
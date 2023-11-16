#include "../h/RelocationTable.hpp"
#include <sstream>

void RelocationTable::insertReloc(string sym,int rel_type,int ad,int off,string word,bool literal){
  uint16_t w=0;
  
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
  if(!literal){
  if(!head){//treba da ubacimo u tabelu
    int addend=ad+(rel_type==RelocType::absolute?0:-1*sizeof(uint16_t));

    Entry* e=new Entry(off,sym,rel_type,addend);
    head=new RelocTable(e);
    /*head->info->addend=ad;
   // head->info->offset=off;
    head->info->SymbolName=sym;
    head->info->type_of_rel=rel_type;
    head->next=nullptr;*/
   // tail=head;
    
  }else{
    
    int addend=ad+(rel_type==RelocType::absolute?0:-1*sizeof(uint16_t));

    Entry* e=new Entry(off,sym,rel_type,addend);
   // RelocTable* rel=new RelocTable(e);
   /* rel->info->addend=ad;
  //  rel->info->offset=off;
    rel->info->SymbolName=sym;
    rel->info->type_of_rel=rel_type;
    rel->info->addend=ad+(rel_type==RelocType::absolute?0:-1*sizeof(uint16_t));
*/

  if(tail==nullptr){
    tail=new RelocTable(e);
     if(head->next==nullptr)head->next=tail;
  }else{
    tail->next=new RelocTable(e);
    
    tail=tail->next;
  }
  
  }
    offset+=2;

  }
}


uint16_t RelocationTable::literalToUint_16(string &l){
  stringstream liter;
  liter<<((l.size()>1 && l.at(1)=='x')||(l.size()>2 && l.at(2)=='x')?hex:dec)<<l;
  int ret;
  liter>>ret;
  return ret;

}

RelocTable* RelocationTable::getTabl(int i){
  int cnt=0;
  RelocTable* curr=head;
  while(cnt!=i){
    curr=curr->next;
    cnt++;
  }
  
  return curr;
}

RelocTable* RelocationTable::operator[](int i){
  int curr=0;
  RelocTable* ret;
  ret=head;
  while(curr<=i){
    ret=ret->next;
  }
  return ret;
}


int RelocationTable::sizeOfTable(){
 
  int cnt=0;
  for(RelocTable* curr=head;curr!=nullptr;curr=curr->next){
    cnt++;
  }
  return cnt;
}

void RelocationTable::insertTable(vector<uint8_t> array){
  for(int i=0;i<array.size();i++) {
  //  cout<<hex<<"HEXASE"<<unsigned(array.at(i))<<endl;
    contentTable.push_back(array.at(i));
  }
}

void RelocationTable::setSymbolSection(string name,string sect){
  for(RelocTable* curr=head;curr!=nullptr;curr=curr->next){
    if(curr->info->SymbolName==name){

     curr->info->SymbolName=sect;
     break;
    }
  }
}

void RelocationTable::setAddendSymbol(int offset,int ad){
  for(RelocTable* curr=head;curr!=nullptr;curr=curr->next){
    if(curr->info->offset==offset) {
      curr->info->addend=ad;
      break;
  }
}
}

void RelocationTable::printContent(){
  for(int i=0;i<contentTable.size();i++){
    cout<<contentTable.at(i);
    cout<<endl;
  }
}

void RelocationTable::updateMachineCode(uint16_t code,int offset){
 /* for(int i=0;i<code.size();i++){
    
    contentTable.at(offset)=code.at(i);
    offset++;
  }*/
  contentTable.at(offset)=(uint8_t)(code&0xFF);
  contentTable.at(offset+1)=(uint8_t)((code>>8)&0xFF);
}
void RelocationTable::updateOffset(int before,int after){
  for(RelocTable* curr=head;curr!=nullptr;curr=curr->next){
    if(curr->info->offset==before) {
      curr->info->offset=after;
      break;
  }
}
}
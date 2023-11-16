
#include <iostream>
#include <vector>
#include <fstream>
#include <string.h>
using namespace std;

class FlexLexer;

class Emulator{
  private:
  //mapa za memoriju mozda ili neki niz
  //svi registri da bih mogla da ih menjam
  //trebaju nam i statusni registri kada proveravamo uslove za jumpove i tako to
  static FlexLexer* flexer;
  ifstream input_stream;
  static vector<uint8_t> MemTable;//memorija
  uint8_t memMap[0xFFFF];
  bool terminate=false;
  bool finished=false;
  int16_t registers[8];
  int16_t psw=0;
  //vector<uint16_t> registers;
  string inpuHex;
  string outputFile;
  static const char* inputBuff;
  int analyzingByte=0;
  const int pc=7;
  const int sp=6;
  const uint16_t zero=0b1;
  const uint16_t overflow=0b10;
  const uint16_t carry=0b100;
  const uint16_t negative=0b1000;


  enum Instruction{
    ldr=0xa0,
    str=0xb0,
    shr=0x91,
    shl=0x90,
    test=0x84,
    xorr=0x83,
    orr=0x82,
    andd=0x81,
    nott=0x80,
    cmp=0x74,
    div=0x73,
    mul=0x72,
    sub=0x71,
    add=0x70,
    xchg=0x60,
    jgt=0x53,
    jne=0x52,
    jeq=0x51,
    jmp=0x50,
    ret=0x40,
    call=0x30,
    iret=0x20,
    intt=0x10,
    halt=0x00


  };

  enum typeOfAddresing{
    immed=0x0,
    regdir=0x1,
    regdirpom=0x5,
    regind=0x2,
    regindpom=0x3,
    mem=0x4
  };

  void handleAdd();
  void handleSub();
  void handleMul();
  void handleDiv();
  void handleCmp();

  void handleNot();
  void handleXor();
  void handleOr();
  void handleAnd();
  void handleTest();

  void handleJmp();
  void handleJeq();
  void handleJne();
  void handleJgt();

  void handleShl();
  void handleShr();

  void handleLdr();
  void handleStr();

  void handleInt();
  void handleRet();
  void handleIRet();
  void handleCall();

  void handleXChg();
  void handleHalt();

  uint16_t gettingAddress();
  int16_t upModeBeforeOper(uint8_t up);
  int16_t upModeAfterOper(uint8_t up);
  void push(int16_t val);
  int16_t pop();
  public:


  enum ReadingType{
    address=1,
    byte=2,
    eol=3
  };


  Emulator(string input);
  void analyzeHexFile();
  void compileHexFile();
  static void inputInstr(const char *c);

};
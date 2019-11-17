#ifndef _ENGINEMSMT_H
#define _ENGINEMSMT_H
#include <arduino.h>
#include <avr/pgmspace.h>


#define FOREACH_EngineError(EngineError) \
        EngineError(OK)   \
        EngineError(CRC_NOT_VALID)  \
     

#define GENERATE_ENUM(ENUM) ENUM,

enum EngineError {
	FOREACH_EngineError(GENERATE_ENUM)
};


enum EGT_Status {
	MAX31855OK=0,
	OC=1,
	SCG=2,
	SCV=4

};

typedef struct {
	unsigned int St_ACD : 3;
	unsigned int F_Anz_ACD : 1;
	unsigned int ACD_Text : 3;
	unsigned int Anz_ACD_ein : 1;

}ACD_1;

typedef struct {
	unsigned int B_ll : 1;
	unsigned int E_fpp : 1;
	unsigned int B_dza : 1;
	unsigned int S_ITLK : 1;
	unsigned int E_canasr : 1;
	unsigned int B_noasr : 1;
	unsigned int E_cge : 1;
	unsigned int E_lm : 1;
	unsigned char miist;
	unsigned short nmot;
	unsigned char mifab;
	unsigned char Wped_fgr;
	unsigned char mdverl;
	unsigned char mrfa;
} MOTOR_1;

typedef struct {
	unsigned RES_MOTOR_GET_CODE_MONORM :6;
	unsigned int MUL_CODE :2;
	unsigned char Tmot;
	unsigned int E_tm : 1;
	unsigned int B_bremse : 1;
	unsigned int E_bremse : 1;
	unsigned int dummy : 3;
	unsigned int B_ffz : 1;
	unsigned int stat_fgr : 1;
	unsigned char mibas;
	unsigned char nstat;
	unsigned char mizwmn;
	unsigned char mimax;
	unsigned char misol;
} MOTOR_2;

typedef struct  {
 int status:8;			// 0
 int EGT_Status_left:4;		// 1
 int EGT_Status_right:4;
 word egtl;					// 2
 word egtr;					// 4
 word iatl;					// 6
 word iatr;					// 8
 byte lambdaplus100;		// 10
 byte llambdaplus100;		// 11
 word iatbeforeIC;			// 12
 unsigned int map:15;		// 14
 unsigned int gearboxoilpump : 1;
 } EngineMsmt ;

typedef union {
  EngineMsmt sensor;
  char b[sizeof(EngineMsmt)];
} EngineMsmtU;

typedef struct {
  int rpm:13;
  int waterinjection:1;
  int oilpump:1;
  int vacuumpump : 1;
  
} Headunit;

typedef union {
  Headunit settings;
  char b[sizeof(Headunit)];
} HeadU;

#ifdef __AVR__
#ifndef __AVR_ATmega2560__
#define NO_PRINTLNDATASERIAL
#endif
#endif


#ifndef NO_PRINTLNDATASERIAL
void PrintlnDataSerial(EngineMsmt &engine, MOTOR_1 &can242,MOTOR_2 & can245);
#endif
extern EngineMsmtU Engine;
extern HeadU Head;
extern PGM_P const EngineError_STRING[] PROGMEM;
void HeadU_Zero();
void EngineMsmtU_Zero();
float EngOilTemp(MOTOR_2 & can245);
#endif

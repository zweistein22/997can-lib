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



#ifndef NO_CAN_242_245_441

typedef struct {
    unsigned int St_Luefter_Li_A : 2;
	unsigned int St_Luefter_Re_A : 2;
	unsigned int S_DH_offen_A : 1;
	unsigned int Sportmodus_BSG_A : 1;
	unsigned int Sportmodus_LSS_A : 1;
	unsigned int Funkschl_Nr_A : 4;
	unsigned int dummy0 : 2 ;
	unsigned int Einst_ACD_A : 2;
	unsigned int Einst_ACD_sp_A : 1;
	unsigned char Helligkeit_A;
	unsigned char dummy1[4];
	unsigned int F_Bug_2_A_1 : 1 ;
	unsigned int F_Heck_1_A_1 : 1;
	unsigned int F_BSG_1_A : 1;
	unsigned int dummy2 : 5;
	

} GW_A_1;


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

typedef struct {
	unsigned int L_checkE :2;
	unsigned int B_tankd :1;
	unsigned int B_redmd:1;
	unsigned int E_mspg : 1;
	unsigned int E_Toel : 1;
	unsigned int S_Oeldr : 1;
	unsigned int KL_Gen : 1;
	unsigned int KuehlerL_Stg : 7;
	unsigned int Motorlauf : 1;
	unsigned int kva:16;
	unsigned char P_Lade;
	unsigned char Toel;
	unsigned char Oeldruck;
	unsigned int S_KuehlWS:1;
	unsigned int dummy:7;
} MOTOR_4;

#endif
float EngOilTemp(byte& Tmot);

typedef struct  {
 unsigned int egtl : 12;
 int EGT_Status_left:4;		
 unsigned int egtr : 12;	
 int EGT_Status_right:4;
 
 byte status;			
 byte nmot100;
 byte lambdaplus100;		
 byte llambdaplus100;		
 
 int iatl:12;					// 6
 int iatr:12;					// 8
 byte Tmot;
 
 int iatbeforeIC:12;			// 12
 int oilpressuredrop:1;
 int dummy1:2;
 int wi:1;
 unsigned int map:15;		// 14
 unsigned int gearboxoilpump : 1;
 } EngineMsmt ;

typedef union {
  EngineMsmt sensor;
  char b[sizeof(EngineMsmt)];
} EngineMsmtU;

typedef struct {
  int waterinjection:1;
  int oilpump:1;
  int vacuumpump : 1;
  int dummy : 5;
  
} Headunit;

typedef union {
  Headunit settings;
  char b[sizeof(Headunit)];
} HeadU;



#ifndef NO_PRINTLNDATASERIAL
void PrintlnDataSerial(EngineMsmt &engine, MOTOR_1 &can242,MOTOR_2 & can245,MOTOR_4 & can441);
#endif
extern EngineMsmtU Engine;
extern HeadU Head;
extern PGM_P const EngineError_STRING[] PROGMEM;
void HeadU_Zero();
void EngineMsmtU_Zero();

#endif

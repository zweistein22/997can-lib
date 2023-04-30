#ifndef __CAN_ACD_H
#define __CAN_ACD_H


#include <mcp_can.h>


#define MCP_STDERR Serial

#define CAN_ANTRIEB_ACD_1 0x442
#define CAN_ANTRIEB_ACD_EB1_TX 0x7FA
#define CAN_ANTRIEB_ACD_V 0x71B
#define CAN_ANTRIEB_D_RS_ACD  0x6F4
#define CAN_ANTRIEB_BREMSE_1 0x14A
#define CAN_ANTRIEB_LWS_1 0xC2
#define CAN_ANTRIEB_KOMBI_7_A 0x62A
#define CAN_ANTRIEB_D_RQ_ACD 0x5F4
#define CAN_ANTRIEB_D_RQ_ALL_A 0x5D6
#define CAN3_CS 7	
#define CAN3_INT 2 // Set INT to pin 6
#define CAN3_RESET A3


typedef struct {
	unsigned int St_ACD : 3;
	unsigned int F_Anz_ACD : 1;
	unsigned int ACD_Text : 3;
	unsigned int Anz_ACD_ein : 1;

}ACD_1;

typedef struct {
	unsigned char AC_SW_Vers;
	unsigned char AC_CAN_STAND;
	unsigned int AC_SW_Tag:5;
	unsigned int dummy0 : 3;
	unsigned int AC_SW_Monat : 4;
	unsigned int AC_KD_F : 1;
	unsigned int AC_F_BusOff : 1;
	unsigned int AC_F_CAN_Time : 1;
	unsigned int AC_F_CAN_Sig : 1;
	unsigned char AC_SW_Jahr;
	unsigned char AC_F_BusOff_Anz;
	unsigned char AC_TEC;
	unsigned char AC_REC;


}ACD_V;




typedef struct {
	unsigned char Fzg_ID_4_A;
	unsigned char Fzg_ID_5_A;
	unsigned char Fzg_ID_8_A;
	unsigned char Fzg_ID_10_A;
	unsigned char Fzg_ID_11_A;
	unsigned int Fzg_ID_12_A : 4;
	unsigned int Fzg_ID_13_A : 4;
	unsigned int Fzg_ID_14_A : 4;
	unsigned int Fzg_ID_15_A : 4;
	unsigned int Fzg_ID_16_A: 4;
	unsigned int Fzg_ID_17_A:4;
	

}KOMBI_7A;


typedef struct {
	unsigned int LWS_LRW:15;
	unsigned int LWS_LRW_Sign:1;
	unsigned int LWS_Gesch:15;
	unsigned int LWS_Gesch_Sign:1;
	unsigned char LWS_ID;
	unsigned int dummy : 1;
	unsigned int St_LWS : 2;
	unsigned int St_LWS_KL30 : 1;
	unsigned int LWS_BZ : 4;
	unsigned char LWS_Kodier;
	unsigned char LWS_CRC;


}LWS_1;


typedef struct {
	unsigned char data[8];
}D_RQ_ACD;

typedef struct {
	unsigned char TP_TA;
	unsigned char TP_PCI;
	unsigned char TP_Data0;
	unsigned char TP_Data1;
	unsigned char TP_Data2;
	unsigned char TP_Data3;
	unsigned char TP_Data4;
	unsigned char TP_Data5;

}D_RQ_ALL_A;


INT8U CAN3_ACD_Begin();
extern MCP_CAN CAN3;


#endif


#ifndef __CAN997_H
#define __CAN997_H
#include <EngineMsmt.h>
#include <mcp_can.h>
#if defined __AVR_ATmega2560__
#define CAN0_INT 21                             // Set INT to pin 21
#define CAN_CS 5	
#else
#if defined __AVR__
#define CAN_CS 7	
#define CAN0_INT 2 // Set INT to pin 2
#define DISPLAY_CAN_SLAVE
#else
#define CAN_CS 7	
#define CAN0_INT 6 // Set INT to pin 2
#define DISPLAY_CAN_SLAVE
#endif
#endif

INT8U CAN_BeginMaster();

void CAN_Spi_UnSelect();

#ifdef DISPLAY_CAN_SLAVE
INT8U CAN_BeginSlave();
INT8U  getCANDataPrivate(long duration, EngineMsmtU& Engine);
#else 
INT8U sendBothPrivateCan(EngineMsmtU& _Engine);
INT8U send442();
INT8U  getCan242(long duration, MOTOR_1& can242);
INT8U  getCan245(long duration, MOTOR_2& can245);
#endif


#ifdef FUNCS_CAN_REMOTEREQUEST

INT8U  getRemoteRequest(long duration, int txid);
INT8U SendRemoteRequest(int txid);
#endif
extern MCP_CAN CAN0;

#define CAN_EXTENDED 0x80000000
#define CAN_REMOTEREQUEST  0x40000000
#define CAN_PRIVATE1 (0x17fc|CAN_EXTENDED)   // MUST BE EVEN!!
#define CAN_PRIVATE2 (CAN_PRIVATE1|1)  
#define CAN_ANTRIEB242 0x242
#define CAN_ANTRIEB245 0x245
#define CAN_ANTRIEB442 0x442
#endif



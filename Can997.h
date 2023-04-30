#ifndef __CAN997_H
#define __CAN997_H
#if defined __AVR_ATmega2560__
#define CAN0_INT 21                             // Set INT to pin 21
#define CAN0_CS 5	
#define CAN0_RESET A10
#define CAN1_INT 20
#define CAN1_CS 41
#define CAN1_RESET A11
#define MCP_STDERR Serial
#define NO_CAN_ACD
#else 
#if defined __AVR__
#ifdef __AVR_ATmega328P__
#include "DisplayStderr.h"
#define MCP_STDERR STDERR
#else
#define MCP_STDERR Serial
#endif
#define CAN0_CS 7	
#define CAN0_INT 2 // Set INT to pin 2
#define CAN0_RESET A3
#define NO_CAN_242_245
#else
#define CAN0_CS 7	
#define CAN0_INT 6 // Set INT to pin 6
#define CAN0_RESET A3

#define CAN1_CS 5	
#define CAN1_INT 4 // Set INT to pin 6
#define CAN1_RESET A4

#define MCP_STDERR Serial
#endif
#endif
#include <EngineMsmt.h>
#include <mcp_can.h>

INT8U CAN0_BeginSlave();
INT8U  CAN0_getbothPrivate(long duration, EngineMsmtU& Engine);
INT8U CAN0_BeginMaster();
#if defined __AVR_ATmega2560__
extern MCP_CAN CAN1;
INT8U CAN1_BeginMaster();
INT8U CAN1_sendbothPrivate(EngineMsmtU& _Engine);
#endif
#ifndef NO_CAN_242_245
#define CAN_ANTRIEB242 0x242
#define CAN_ANTRIEB245 0x245
INT8U  CAN0_get242(long duration, MOTOR_1& can242);
INT8U  CAN0_get245(long duration, MOTOR_2& can245);
#endif

extern MCP_CAN CAN0;

#define CAN_EXTENDED 0x80000000
#define CAN_REMOTEREQUEST  0x40000000
#define CAN_PRIVATE1 (0x7FD00)   
#define CAN_PRIVATE2 (0x7FE00)   


#endif



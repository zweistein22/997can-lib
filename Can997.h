#ifndef __CAN997_H
#define __CAN997_H
#if defined __AVR_ATmega2560__
#define CAN0_INT 21                             // Set INT to pin 21
#define CAN_CS 5	
#define MCP_STDERR Serial
#else
#if defined __AVR__
#ifdef __AVR_ATmega328P__
#include "DisplayStderr.h"
#define MCP_STDERR STDERR
#else
#define MCP_STDERR Serial
#endif
#define CAN_CS 7	
#define CAN0_INT 2 // Set INT to pin 2
#else
#define CAN_CS 7	
#define CAN0_INT 6 // Set INT to pin 6
#define MCP_STDERR Serial
#endif
#endif
#include <EngineMsmt.h>
#include <mcp_can.h>

void CAN_Spi_UnSelect();
INT8U CAN_BeginSlave();
INT8U  getCANDataPrivate(long duration, EngineMsmtU& Engine);
INT8U CAN_BeginMaster();
INT8U sendBothPrivateCan(EngineMsmtU& _Engine);
#ifndef NO_CAN_242_245_442
INT8U send442();
INT8U  getCan242(long duration, MOTOR_1& can242);
INT8U  getCan245(long duration, MOTOR_2& can245);
#endif
extern MCP_CAN CAN0;

#define CAN_EXTENDED 0x80000000
#define CAN_REMOTEREQUEST  0x40000000
#define CAN_PRIVATE1 (0x7FD)   
#define CAN_PRIVATE2 (0x7FE)   

#define CAN_ANTRIEB242 0x242
#define CAN_ANTRIEB245 0x245
#define CAN_ANTRIEB442 0x442
#endif



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
#define NO_CAN_SEND
#else
#define CAN_CS 7	
#define CAN0_INT 6 // Set INT to pin 2
#endif
#endif

INT8U CAN_Begin();
void CAN_Spi_UnSelect();
#ifndef NO_CAN_SEND
INT8U sendCan();
#endif
INT8U  getCan242(long duration, MOTOR_1 &can242);
INT8U  getCan245(long duration, MOTOR_2 &can245);
INT8U  getCANDataPrivate(long duration, EngineMsmtU &Engine);
extern MCP_CAN CAN0;

#endif



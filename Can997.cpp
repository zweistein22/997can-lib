#include <mcp_can_dfs.h>
#include <mcp_can.h>
#include <EngineMsmt.h>
#include "Can997.h"
//#define DEBUGSERIAL Serial

#ifdef __AVR__
#ifndef __AVR_ATmega2560__
#include "DisplayStderr.h"
#define MCP_STDERR STDERR
#else
#define MCP_STDERR Serial
#endif
#else 
#define MCP_STDERR Serial
#endif

unsigned char flagRecv = 0;
unsigned char len = 0;
unsigned char rxBuf[8];
long unsigned int rxId;

#ifdef DEBUGSERIAL
char msgString[] = "                            ";
#endif

#define CAN_EXTENDED 0x80000000
#define CAN_PRIVATE1 0x7fc    //|CAN_EXTENDED // MUST BE EVEN!!
#define CAN_PRIVATE2 (CAN_PRIVATE1|1) //|CAN_EXTENDED) 
#define CAN_ANTRIEB242 0x242
#define CAN_ANTRIEB245 0x245
#define CAN_ANTRIEB442 0x442
MCP_CAN CAN0(CAN_CS);                               // Set CS to pin 5
//assert(sizeof(EngineMsmtU) >= 16)
void CAN_Spi_UnSelect() {
	digitalWrite(CAN_CS, HIGH);
}


INT8U CAN_Begin() {
	INT8U rv = MCP2515_OK;
	if (CAN0.begin(MCP_STDEXT, CAN_500KBPS, MCP_8MHZ) == CAN_OK) {
		rv = MCP2515_OK;
#ifdef MCP_STDERR
		MCP_STDERR.println("");
#endif
	}
	else {
		rv = MCP2515_FAIL;
#ifdef MCP_STDERR
		MCP_STDERR.println("MCP2515_FAIL");
#endif
		return rv;
	}

	rv=CAN0.setMode(MCP_LISTENONLY);
	if (rv != CAN_OK) {
#ifdef MCP_STDERR
		MCP_STDERR.print("CAN0.setMode(MCP_LISTENONLY) error:");
		MCP_STDERR.println(rv);
#endif
		return rv;
	}
	pinMode(CAN0_INT, INPUT);								// Configuring pin for /INT input
	rv=CAN0.init_Mask(0, 0, ((long)CAN_ANTRIEB245&CAN_PRIVATE2) <<16);      // there are 2 mask in mcp2515, you need to set both of them
	if (rv != CAN_OK) {
#ifdef MCP_STDERR
		MCP_STDERR.print("CAN0.init_Mask(0, 0, ((long)CAN_ANTRIEB245&CAN_PRIVATE2) <<16) error:");
		MCP_STDERR.println(rv);
#endif
		return rv;
	}

	CAN0.init_Filt(0, 0, ((long)CAN_ANTRIEB245)<<16);		// there are 6 filter in mcp2515
	CAN0.init_Filt(1, 0, ((long)CAN_PRIVATE2) <<16);      // there are 6 filter in mcp2515

	CAN0.init_Mask(1, 0, ((long)CAN_PRIVATE1&CAN_ANTRIEB242&CAN_ANTRIEB442) << 16); // only address containing 0x40 are allowed, 0x640, 0x440, 0x240, 0x241,0x24F,...
	CAN0.init_Filt(2, 0, ((long)CAN_PRIVATE1)<<16);                          // there are 6 filter in mcp2515
	CAN0.init_Filt(3, 0, ((long)CAN_ANTRIEB242)<<16);
	CAN0.init_Filt(4, 0, ((long)CAN_ANTRIEB442)<< 16);
 //   CAN0.init_Filt(5, 0, ((long)CAN_ANTRIEB442 << 16));
//	CAN0.init_Filt(6, 0, ((long)CAN_ANTRIEB442 << 16));
	// there are 6 filter in mcp2515
	return rv;
}


#ifndef NO_CAN_SEND
byte data[8] = { 0x00 ,0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 };
INT8U sendCan() {
	byte sndStat = CAN_FAIL;
	sndStat=CAN0.setMode(MCP_NORMAL);
	if (sndStat != CAN_OK) {
#ifdef MCP_STDERR
		MCP_STDERR.print("Error CAN0.setMode(MCP_NORMAL)");
		MCP_STDERR.print(" code=");
		MCP_STDERR.println(sndStat);
#endif
		return sndStat;
	}
	data[0] = 0x02;// ACD_1  2 = Normal_Prog_aktiv
	//sndStat= CAN0.sendMsgBuf(CAN_ANTRIEB442, 1, data);
	if (sndStat != CAN_OK) {
#ifdef MCP_STDERR
	//	MCP_STDERR.print("Error Sending Message CAN_ANTRIEB442 Id:0x");
	//	MCP_STDERR.print(CAN_ANTRIEB442);
	//	MCP_STDERR.print(" code=");
	//	MCP_STDERR.println(sndStat);
#endif
		
	}
	else {
		data[0] = Engine.b[0];
		data[1] = Engine.b[1];
		data[2] = Engine.b[2];
		data[3] = Engine.b[3];
		data[4] = Engine.b[4];
		data[5] = Engine.b[5];
		data[6] = Engine.b[6];
		data[7] = Engine.b[7];
		// use address CAN_PRIVATE1, must not interfere
		sndStat = CAN0.sendMsgBuf(CAN_PRIVATE1, 8, data);
		if (sndStat != CAN_OK) {
#ifdef MCP_STDERR
			MCP_STDERR.print("Error Sending Message CAN_PRIVATE1 Id:0x");
			MCP_STDERR.print(CAN_PRIVATE1);
			MCP_STDERR.print(" code=");
			MCP_STDERR.println(sndStat);
#endif
		}
		else {
			data[0] = Engine.b[8];
			data[1] = Engine.b[9];
			data[2] = Engine.b[10];
			data[3] = Engine.b[11];
			data[4] = Engine.b[12];
			data[5] = Engine.b[13];
			data[6] = Engine.b[14];
			data[7] = Engine.b[15];
			sndStat = CAN0.sendMsgBuf(CAN_PRIVATE2, 8, data);
			if (sndStat != CAN_OK) {
#ifdef MCP_STDERR
				MCP_STDERR.print("Error Sending Message CAN_PRIVATE2 Id:0x");
				MCP_STDERR.println(CAN_PRIVATE2);
				MCP_STDERR.print(" code=");
				MCP_STDERR.println(sndStat);
#endif
			}
			else {}
		}
		
	}
	byte sndStat2 = CAN0.setMode(MCP_LISTENONLY);
	if (sndStat2 != CAN_OK) {
#ifdef MCP_STDERR
		MCP_STDERR.println("Error setMode(MCP_LISTENONLY)");
		MCP_STDERR.print(" code=");
		MCP_STDERR.println(sndStat);
#endif
		if (sndStat == CAN_OK) sndStat = sndStat2;
	}
	return sndStat;
}
#endif


INT8U  getCANDataPrivate(long duration, EngineMsmtU &Engine) {
	INT8U sndStat = CAN_OK;
	sndStat=CAN0.setMode(MCP_NORMAL);
	if (sndStat != CAN_OK) return sndStat;
	long start = millis();
	int bothreached = 0;
	do {
	
		if (!digitalRead(CAN0_INT))                         // If CAN0_INT pin is low, read receive buffer
		{
			sndStat = CAN0.readMsgBuf(&rxId, &len, rxBuf);      // Read data: len = data length, buf = data byte(s)
			if (sndStat != CAN_OK) continue;
			if (bothreached == 0) {	if (rxId != CAN_PRIVATE1) continue;	}
#ifdef DEBUGSERIAL
			if (len > 0 && len <= 8) {
				DEBUGSERIAL.print("  rxId=:0x");
				DEBUGSERIAL.print(rxId, HEX);
				DEBUGSERIAL.println("(");
				for (byte i = 0; i < len; i++) {
					sprintf(msgString, " 0x%.2X", rxBuf[i]);
					DEBUGSERIAL.print(msgString);
				}
				DEBUGSERIAL.println(")");
			}
#endif
			if (rxId == CAN_PRIVATE1) {
#ifdef DEBUGSERIAL
				DEBUGSERIAL.print("CAN_PRIVATE1:0x");
				DEBUGSERIAL.print(rxId, HEX);
				DEBUGSERIAL.print(" (");
				DEBUGSERIAL.print("len=");
				DEBUGSERIAL.print(len);
				DEBUGSERIAL.println(")");
#endif
				if (len < 8) continue;
				Engine.b[0] = rxBuf[0];
				Engine.b[1] = rxBuf[1];
				Engine.b[2] = rxBuf[2];
				Engine.b[3] = rxBuf[3];
				Engine.b[4] = rxBuf[4];
				Engine.b[5] = rxBuf[5];
				Engine.b[6] = rxBuf[6];
				Engine.b[7] = rxBuf[7];
				bothreached |= 1;
				if (bothreached == 3) break;
				else continue;
			}
			if (rxId == CAN_PRIVATE2) {
#ifdef DEBUGSERIAL
				DEBUGSERIAL.print("CAN_PRIVATE2:0x");
				DEBUGSERIAL.print(rxId, HEX);
				DEBUGSERIAL.print(" (");
				DEBUGSERIAL.print("len=");
				DEBUGSERIAL.print(len);
				DEBUGSERIAL.println(")");
#endif
				if (len < 8) continue;
				Engine.b[8] = rxBuf[0];
				Engine.b[9] = rxBuf[1];
				Engine.b[10] = rxBuf[2];
				Engine.b[11] = rxBuf[3];
				Engine.b[12] = rxBuf[4];
				Engine.b[13] = rxBuf[5];
				Engine.b[14] = rxBuf[6];
				Engine.b[15] = rxBuf[7];
				bothreached |= 2;
				if (bothreached == 3) break;
				else continue;
			}
		}
	} while (millis() - start < duration);
	if (bothreached == 3)	return sndStat;
	else {
#ifdef	MCP_STDERR
		Serial.println("Timout reading both CAN_PRIVATE1 and CAN_PRIVATE2");
#endif
	}
	return sndStat;
}


uint16_t lastnmot = 0;
int nsame = 0;

INT8U getCan242(long duration, MOTOR_1 &can242){
	INT8U sndStat = CAN_OK;
	sndStat = CAN0.setMode(MCP_LISTENONLY);
	if (sndStat!=CAN_OK) return sndStat;
	long start = millis();
	bool receivedintime = false;
	do {
		if (!digitalRead(CAN0_INT))                         // If CAN0_INT pin is low, read receive buffer
		{
			sndStat = CAN0.readMsgBuf(&rxId, &len, rxBuf);
			if (sndStat != CAN_OK) continue;
			if (rxId == 0x242) {
#ifdef DEBUGSERIAL
				DEBUGSERIAL.print("Antrieb(0x242):");
				if (sizeof(MOTOR_1) != 8)DEBUGSERIAL.println("FATAL: MOTOR_1 NOT sizeof(8)");
#endif
				if (len < 8) continue;
				memcpy(&can242, rxBuf, 8);
#ifdef DEBUGSERIAL
				DEBUGSERIAL.print("miist=");
				DEBUGSERIAL.print(can242.miist*100.0 / 255.0);
				DEBUGSERIAL.print(",nmot=");
				DEBUGSERIAL.print(can242.nmot*0.25);
				DEBUGSERIAL.print(",Wped_fgr=");
				DEBUGSERIAL.print(can242.Wped_fgr*100.0 / 254.0);
				if(can242.E_lm) DEBUGSERIAL.print("E_lm: HFM Error");
				DEBUGSERIAL.println("");
#endif
				receivedintime = true;
				break;
			}
		}
	} while (millis() - start < duration);
	if (can242.nmot == lastnmot) nsame++;
	else nsame = 0;
	lastnmot = can242.nmot;

	//Serial.println(nsame);
	if (nsame > 7) {
		can242.nmot = 0;
		can242.Wped_fgr = 0;
		can242.miist = 0;
	}
	if (!receivedintime) {
#ifdef	MCP_STDERR
		MCP_STDERR.println("Timout reading CAN_ANTRIEB242");
#endif
	}
	return sndStat;
}

INT8U getCan245(long duration, MOTOR_2 &can245) {
	INT8U sndStat = CAN_OK;
	sndStat=CAN0.setMode(MCP_LISTENONLY);
	if (sndStat != CAN_OK) return sndStat;
	long start = millis();
	bool receivedintime = false;
	do {
		if (!digitalRead(CAN0_INT))                         // If CAN0_INT pin is low, read receive buffer
		{
			sndStat=CAN0.readMsgBuf(&rxId, &len, rxBuf);
			if (sndStat != CAN_OK) continue;
			if (rxId == 0x245) {
#ifdef DEBUGSERIAL
				DEBUGSERIAL.print("Antrieb(0x245):");
				if (sizeof(MOTOR_2) != 8)DEBUGSERIAL.println("FATAL: MOTOR_1 NOT sizeof(8)");
#endif
				if (len < 8) continue;
				memcpy(&can245, rxBuf, 8);
				
#ifdef DEBUGSERIAL
				DEBUGSERIAL.print("Tmot=");
				DEBUGSERIAL.print(EngOilTemp(can245));
				DEBUGSERIAL.print(",E_tm=");
				DEBUGSERIAL.print(can245.E_tm);
				DEBUGSERIAL.print(",B_bremse=");
				DEBUGSERIAL.print(can245.B_bremse);
				DEBUGSERIAL.print(",E_bremse=");
				DEBUGSERIAL.print(can245.E_bremse);
				DEBUGSERIAL.print(",B_ffz=");
				DEBUGSERIAL.print(can245.B_ffz);
				DEBUGSERIAL.print(",stat_fgr=");
				DEBUGSERIAL.print(can245.stat_fgr);
				DEBUGSERIAL.print(",mibas=");
				DEBUGSERIAL.print(can245.mibas);
				DEBUGSERIAL.print(",nstat=");
				DEBUGSERIAL.print(can245.nstat);
				DEBUGSERIAL.print(",mizwmn=");
				DEBUGSERIAL.print(can245.mizwmn);
				DEBUGSERIAL.print(",mimax=");
				DEBUGSERIAL.print(can245.mimax);
				DEBUGSERIAL.print(",misol=");
				DEBUGSERIAL.print(can245.misol);
				DEBUGSERIAL.println("");
#endif
				receivedintime = true;
				break;
			}
		}
	} while (millis() - start < duration);
	if(!receivedintime){
#ifdef	MCP_STDERR
	MCP_STDERR.println("Timout reading CAN_ANTRIEB245");
#endif
	}
	return sndStat;
}
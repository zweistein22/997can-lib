#include <arduino.h>
#include <mcp_can_dfs.h>
#include <mcp_can.h>
#include <EngineMsmt.h>
#include "Can997.h"

unsigned char flagRecv = 0;
unsigned char len = 0;
long unsigned int rxId;
byte canbuf[8];


#ifdef DEBUGSERIAL
char msgString[] = "                            ";
#endif

MCP_CAN CAN0(CAN0_CS);                               // Set CS to pin 7

INT8U CAN0_SetMode(int mode) {
	byte sndStat = CAN_FAIL;
	sndStat = CAN0.setMode(mode);
#ifdef MCP_STDERR
	if (sndStat != CAN_OK) {
		MCP_STDERR.print("Error CAN0.setMode(");
		if (mode == MCP_NORMAL) MCP_STDERR.print("MCP_NORMAL ");
		else if (mode == MCP_LISTENONLY) MCP_STDERR.print("MCP_LISTENONLY ");
		MCP_STDERR.print(") code=");
		MCP_STDERR.println(sndStat);
	}
#endif
	return sndStat;
}


INT8U CAN0_BeginSlave() {
	
	pinMode(CAN0_RESET, OUTPUT);
	digitalWrite(CAN0_RESET, LOW);
	delay(10);
	digitalWrite(CAN0_RESET, HIGH);
	pinMode(CAN0_RESET, INPUT_PULLUP);
	int retries = 1;
	INT8U rv = MCP2515_FAIL;
	for (int r = 0; r < retries; r++) {
		if (CAN0.begin(MCP_ANY, CAN_500KBPS, MCP_8MHZ) == CAN_OK) {
			rv = MCP2515_OK;
			break;
		}
		else delay(100);
	}
	if (rv == MCP2515_FAIL) {
#ifdef MCP_STDERR
		MCP_STDERR.println("MCP2515_FAIL");
#endif
#ifdef DEBUGSERIAL
		DEBUGSERIAL.println("MCP2515_FAIL");
#endif
		return rv;
	}
	pinMode(CAN0_INT, INPUT);								// Configuring pin for /INT input
	long mask = ((long)0x7FF) << 16;
	INT8U ext = 0;
	// here we allow EXTENDED IDs CAN_PRIVATE1 and CAN_PRIVATE2 only
	if (CAN_PRIVATE1 > 0x7FF) {
		mask = 0x1FFFFFFF;
		ext = 1;
	}
	
	// we assume CAN_PRIVATE2 same behaviour, so both STD or both EXT
	rv = CAN0.init_Mask(0, ext, mask);      // there are 2 mask in mcp2515, you need to set both of them
	if (rv != CAN_OK) {
#ifdef MCP_STDERR
		MCP_STDERR.print("CAN0.init_Mask(0, 0, ((long)0xCAN_PRIVATE1&CAN_ALL_STD) error:");
		MCP_STDERR.println(rv);
#endif
		return rv;
	}
	CAN0.init_Filt(0, ext ,ext?((long)CAN_PRIVATE1):((long)CAN_PRIVATE1)<<16);
	CAN0.init_Filt(1, ext ,ext?((long)CAN_PRIVATE1) : ((long)CAN_PRIVATE1) << 16);

	CAN0.init_Mask(1, ext, mask);
	CAN0.init_Filt(2,  ext, ext?((long)CAN_PRIVATE2) : ((long)CAN_PRIVATE2) << 16);
	CAN0.init_Filt(3, ext, ext?((long)CAN_PRIVATE2) : ((long)CAN_PRIVATE2) << 16);
	CAN0.init_Filt(4, ext, ext ? ((long)CAN_PRIVATE2) : ((long)CAN_PRIVATE2) << 16);
	CAN0.init_Filt(5, ext, ext ? ((long)CAN_PRIVATE2) : ((long)CAN_PRIVATE2) << 16);
	// there are 6 filter in mcp2515
	return  CAN0_SetMode(MCP_NORMAL);
}



INT8U  CAN0_getbothPrivate(long duration, EngineMsmtU& Engine) {
	INT8U sndStat = CAN_FAIL;
	int bothreached = 0;
	unsigned long privateids[] = { CAN_PRIVATE1 ,CAN_PRIVATE2 };
	if (privateids[0] > 0x7FF) privateids[0] |= CAN_EXTENDED;
	if (privateids[1] > 0x7FF) privateids[1] |= CAN_EXTENDED;
	long start = millis();
#ifdef DEBUGSERIAL
	DEBUGSERIAL.println("CAN0_getbothPrivate");
#endif

	do {
		if (!digitalRead(CAN0_INT))                         // If CAN0_INT pin is low, read receive buffer
		{
			sndStat = CAN0.readMsgBuf(&rxId, &len, canbuf);      // Read data: len = data length, buf = data byte(s)
			if (sndStat != CAN_OK) continue;
#ifdef DEBUGSERIAL
			if (len > 0 && len <= 8) {
				DEBUGSERIAL.print("  rxId=:0x");
				DEBUGSERIAL.print(rxId, HEX);
				DEBUGSERIAL.println("(");
				for (byte i = 0; i < len; i++) {
					sprintf(msgString, " 0x%.2X", canbuf[i]);
					DEBUGSERIAL.print(msgString);
				}
				DEBUGSERIAL.println(")");
			}
#endif
			if (rxId == privateids[0] || rxId == privateids[1]) {
#ifdef DEBUGSERIAL
				if (rxId == privateids[0]) DEBUGSERIAL.print("CAN_PRIVATE1:");
				if (rxId == privateids[1]) DEBUGSERIAL.print("CAN_PRIVATE2:");
				DEBUGSERIAL.print(rxId, HEX);
				DEBUGSERIAL.print(" (");
				DEBUGSERIAL.print("len=");
				DEBUGSERIAL.print(len);
				DEBUGSERIAL.println(")");
#endif
				if (len < 8) continue;
				if (rxId == privateids[0]) {
					Engine.b[0] = canbuf[0];
					Engine.b[1] = canbuf[1];
					Engine.b[2] = canbuf[2];
					Engine.b[3] = canbuf[3];
					Engine.b[4] = canbuf[4];
					Engine.b[5] = canbuf[5];
					Engine.b[6] = canbuf[6];
					Engine.b[7] = canbuf[7];
					bothreached |= 1;
				}
				if (rxId == privateids[1]) {
					Engine.b[8] = canbuf[0];
					Engine.b[9] = canbuf[1];
					Engine.b[10] = canbuf[2];
					Engine.b[11] = canbuf[3];
					Engine.b[12] = canbuf[4];
					Engine.b[13] = canbuf[5];
					Engine.b[14] = canbuf[6];
					Engine.b[15] = canbuf[7];
					bothreached |= 2;
				}
			}
		}
		if (bothreached == 3) 	break;
	} while (millis() - start < duration);
	return sndStat;
}







#ifndef NO_CAN_242_245_442
INT8U CAN0_BeginMaster() {
	pinMode(CAN0_RESET, OUTPUT);
	digitalWrite(CAN0_RESET, LOW);
	delay(10);
	digitalWrite(CAN0_RESET, HIGH);
	pinMode(CAN0_RESET, INPUT_PULLUP);

	int retries = 1;
	INT8U rv = MCP2515_FAIL;
	for (int r = 0; r < retries; r++) {
		if (CAN0.begin(MCP_ANY, CAN_500KBPS, MCP_8MHZ) == CAN_OK) {
			rv = MCP2515_OK;
			break;
		}
		
	}
	if (rv == MCP2515_FAIL) {
#ifdef MCP_STDERR
		MCP_STDERR.println("CAN0 MCP2515_FAIL");
#endif
#ifdef DEBUGSERIAL
		DEBUGSERIAL.println("CAN0 MCP2515_FAIL");
#endif
	
		return rv;
	}


	pinMode(CAN0_INT, INPUT);								// Configuring pin for /INT input
	long mask = ((long)0x7FF) << 16;

	rv = CAN0.init_Mask(0, 0, mask);      // there are 2 mask in mcp2515, you need to set both of them
	if (rv != CAN_OK) {
#ifdef MCP_STDERR
		MCP_STDERR.print("CAN0.init_Mask(0, 0, ((long)0x7ff) error:");
		MCP_STDERR.println(rv);
#endif
		
		return rv;
	}
	CAN0.init_Filt(0, 0, ((long)CAN_ANTRIEB242) << 16);
	CAN0.init_Filt(1, 0, ((long)CAN_ANTRIEB245) << 16);

	CAN0.init_Mask(1, 0, mask); // only address containing 0x40 are allowed, 0x640, 0x440, 0x240, 0x241,0x24F,...
	CAN0.init_Filt(2, 0, ((long)CAN_ANTRIEB242) << 16);
	CAN0.init_Filt(3, 0, ((long)CAN_ANTRIEB245) << 16);
	CAN0.init_Filt(4, 0, ((long)CAN_ANTRIEB442) << 16);
	CAN0.init_Filt(5, 0, ((long)CAN_ANTRIEB442) << 16);
	// there are 6 filter in mcp2515
	return  CAN0_SetMode(MCP_LISTENONLY);
}
#endif

#if defined __AVR_ATmega2560__
MCP_CAN CAN1(CAN1_CS);
INT8U CAN1_SetMode(int mode) {
	byte sndStat = CAN_FAIL;
	sndStat = CAN1.setMode(mode);
#ifdef MCP_STDERR
	if (sndStat != CAN_OK) {
		MCP_STDERR.print("Error CAN1.setMode(");
		if (mode == MCP_NORMAL) MCP_STDERR.print("MCP_NORMAL ");
		else if (mode == MCP_LISTENONLY) MCP_STDERR.print("MCP_LISTENONLY ");
		MCP_STDERR.print(") code=");
		MCP_STDERR.println(sndStat);
	}
#endif
	return sndStat;
}
INT8U CAN1_BeginMaster() {
	Serial.print("CAN1_CS=");
	Serial.print(CAN1_CS);

	Serial.print(" CAN1_RESET=");
	Serial.print(CAN1_RESET);
	Serial.print(" CAN1_INT=");
	Serial.print(CAN1_INT);
	Serial.println(" CAN1_BeginMaster");
	pinMode(CAN1_RESET, OUTPUT);
	digitalWrite(CAN1_RESET, LOW);
	delay(10);
	digitalWrite(CAN1_RESET, HIGH);
	pinMode(CAN1_RESET, INPUT_PULLUP);
	int retries = 1;
	INT8U rv = MCP2515_FAIL;
	for (int r = 0; r < retries; r++) {
		if (CAN1.begin(MCP_ANY, CAN_500KBPS, MCP_8MHZ) == CAN_OK) {
			rv = MCP2515_OK;
			break;
		}
		else delay(100);
	}
	if (rv == MCP2515_FAIL) {
#ifdef MCP_STDERR
		MCP_STDERR.println("MCP2515_FAIL");
#endif
#ifdef DEBUGSERIAL
		DEBUGSERIAL.println("MCP2515_FAIL");
#endif
		return rv;
	}
	pinMode(CAN1_INT, INPUT);								// Configuring pin for /INT input
	long mask = ((long)0x7FF) << 16;

	INT8U ext = 0;
	// here we allow EXTENDED IDs CAN_PRIVATE1 and CAN_PRIVATE2 only
	if (CAN_PRIVATE1 > 0x7FF) {
		mask = 0x1FFFFFFF;
		ext = 1;
	}
	rv = CAN1.init_Mask(0, ext, mask);      // there are 2 mask in mcp2515, you need to set both of them
	if (rv != CAN_OK) {
#ifdef MCP_STDERR
		MCP_STDERR.print("CAN1.init_Mask(0, 1, ((long)0x7ff) error:");
		MCP_STDERR.println(rv);
#endif
		return rv;
	}
	CAN1.init_Filt(0, ext, ext ? ((long)CAN_PRIVATE1) : ((long)CAN_PRIVATE1) << 16);
	CAN1.init_Filt(1, ext, ext ? ((long)CAN_PRIVATE2) : ((long)CAN_PRIVATE2) << 16);
	
	CAN1.init_Mask(1, 0, ((long)0x7FF) << 16);
	CAN1.init_Filt(2, 0, ((long)CAN_ANTRIEB442) << 16);
	CAN1.init_Filt(3, 0, ((long)CAN_ANTRIEB442) << 16);
	CAN1.init_Filt(4, 0, ((long)CAN_ANTRIEB442) << 16);
	CAN1.init_Filt(5, 0, ((long)CAN_ANTRIEB442) << 16);
	
	// there are 6 filter in mcp2515
	return CAN1_SetMode(MCP_NORMAL);
}
INT8U CAN1_SendMsg(INT32U id, INT8U ext, INT8U len, INT8U* buf) {

	INT8U sndStat = CAN_FAIL;
	sndStat = CAN1.sendMsgBuf(id, ext, len, buf);
#ifdef DEBUGSERIAL
	if (sndStat != CAN_OK) {
		DEBUGSERIAL.print("ERROR: CAN1.sendMsgBuf(");
		DEBUGSERIAL.print(id, HEX);
		if (id & CAN_REMOTEREQUEST) DEBUGSERIAL.print("  CAN_REMOTEREQUEST");
		if (ext) DEBUGSERIAL.print("  CAN_EXTENDED");
		DEBUGSERIAL.print(")=");
		if (sndStat == CAN_GETTXBFTIMEOUT) DEBUGSERIAL.println("CAN_GETTXBFTIMEOUT");
		else if (sndStat == CAN_SENDMSGTIMEOUT) DEBUGSERIAL.println("CAN_SENDMSGTIMEOUT");
		else DEBUGSERIAL.println(sndStat);

	}
#endif
	return sndStat;
}
INT8U CAN1_SendMsg(INT32U id, INT8U len, INT8U* buf) {

	INT8U sndStat = CAN_FAIL;
	sndStat = CAN1.sendMsgBuf(id, len, buf);
#ifdef DEBUGSERIAL
	if (sndStat != CAN_OK) {
		DEBUGSERIAL.print("ERROR: CAN1.sendMsgBuf(");
		DEBUGSERIAL.print(id, HEX);
		if (id & CAN_REMOTEREQUEST) DEBUGSERIAL.print("  CAN_REMOTEREQUEST");
		if (id & CAN_EXTENDED) DEBUGSERIAL.print("  CAN_EXTENDED");
		DEBUGSERIAL.print(")=");
		if (sndStat == CAN_GETTXBFTIMEOUT) DEBUGSERIAL.println("CAN_GETTXBFTIMEOUT");
		else if (sndStat == CAN_SENDMSGTIMEOUT) DEBUGSERIAL.println("CAN_SENDMSGTIMEOUT");
		
		
		else DEBUGSERIAL.println(sndStat);

	}
#endif
	return sndStat;
}
INT8U CAN1_sendbothPrivate(EngineMsmtU& _Engine) {
	byte sndStat = CAN_FAIL;
	INT8U ext = 0;
	if (CAN_PRIVATE1 > 0x7FF) ext = 1;

	sndStat = CAN1_SendMsg(CAN_PRIVATE1,ext, 8, (byte*)&_Engine.b[0]);
	if (sndStat != CAN_OK) return sndStat;
	ext = 0;
	if (CAN_PRIVATE2 > 0x7FF) ext = 1;
	sndStat = CAN1_SendMsg(CAN_PRIVATE2,ext, 8, (byte*)&_Engine.b[8]);
	return sndStat;
}
#endif
#ifndef NO_CAN_242_245_442
INT8U CAN0_send442() {
	byte sndStat = CAN_FAIL;
	sndStat = CAN0_SetMode(MCP_NORMAL);
	canbuf[0] = 0x02;// ACD_1  2 = Normal_Prog_aktiv
	sndStat = CAN0.sendMsgBuf(CAN_ANTRIEB442, 1, canbuf);
	byte sndStat2 = CAN0_SetMode(MCP_LISTENONLY);
	return sndStat; // never sends
}
INT8U CAN0_get245(long duration, MOTOR_2& can245) {
	INT8U sndStat = CAN_FAIL;
	//sndStat = CAN0.setMode(MCP_LISTENONLY);
	//if (sndStat != CAN_OK) return sndStat;
	long start = millis();
	bool receivedintime = false;
	do {
		if (!digitalRead(CAN0_INT))                         // If CAN0_INT pin is low, read receive buffer
		{
			sndStat = CAN0.readMsgBuf(&rxId, &len, canbuf);
			if (sndStat != CAN_OK) continue;
			if (rxId == 0x245) {
#ifdef DEBUGSERIAL
				DEBUGSERIAL.print("Antrieb(0x245):");
				if (sizeof(MOTOR_2) != 8)DEBUGSERIAL.println("FATAL: MOTOR_1 NOT sizeof(8)");
#endif
				if (len < 8) continue;
				memcpy(&can245, canbuf, 8);

#ifdef DEBUGSERIAL
				DEBUGSERIAL.print("Tmot=");
				DEBUGSERIAL.print(EngOilTemp(can245.Tmot));
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
	//sndStat = CAN0.setMode(MCP_NORMAL);
	
	if (!receivedintime) {
#ifdef	MCP_STDERR
		MCP_STDERR.println("Timout reading CAN_ANTRIEB245");
#endif
	}
	return sndStat;
}
uint16_t lastnmot = 0;
int nsame = 0;
INT8U CAN0_get242(long duration, MOTOR_1& can242) {
	INT8U sndStat = CAN_OK;
	//sndStat = CAN0.setMode(MCP_LISTENONLY);
	//if (sndStat != CAN_OK) return sndStat;
	long start = millis();
	bool receivedintime = false;
	do {
		if (!digitalRead(CAN0_INT))                         // If CAN0_INT pin is low, read receive buffer
		{
			sndStat = CAN0.readMsgBuf(&rxId, &len, canbuf);
			if (sndStat != CAN_OK) continue;
			if (rxId == 0x242) {
#ifdef DEBUGSERIAL
				DEBUGSERIAL.print("Antrieb(0x242):");
				if (sizeof(MOTOR_1) != 8)DEBUGSERIAL.println("FATAL: MOTOR_1 NOT sizeof(8)");
#endif
				if (len < 8) continue;
				memcpy(&can242, canbuf, 8);
#ifdef DEBUGSERIAL
				DEBUGSERIAL.print("miist=");
				DEBUGSERIAL.print(can242.miist * 100.0 / 255.0);
				DEBUGSERIAL.print(",nmot=");
				DEBUGSERIAL.print(can242.nmot * 0.25);
				DEBUGSERIAL.print(",Wped_fgr=");
				DEBUGSERIAL.print(can242.Wped_fgr * 100.0 / 254.0);
				if (can242.E_lm) DEBUGSERIAL.print("E_lm: HFM Error");
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
	//sndStat = CAN0.setMode(MCP_NORMAL);
	
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
#endif
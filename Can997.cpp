#include <arduino.h>
#include <mcp_can_dfs.h>
#include <mcp_can.h>
#include <EngineMsmt.h>
#include "Can997.h"
//#define DEBUGSERIAL Serial

//#pragma message("please set TIMEOUTVALUE to 250 in mcp_can_dfs.h")
#ifdef __AVR_ATmega328P__
#include "DisplayStderr.h"
#define MCP_STDERR STDERR
#define DISPLAY_CAN_SLAVE

#else
#define MCP_STDERR Serial
#endif




unsigned char flagRecv = 0;
unsigned char len = 0;
long unsigned int rxId;
byte canbuf[8];



#ifdef DEBUGSERIAL
char msgString[] = "                            ";
#endif

MCP_CAN CAN0(CAN_CS);                               // Set CS to pin 7
void CAN_Spi_UnSelect() {
	digitalWrite(CAN_CS, HIGH);
}

INT8U CAN_SendMsg(INT32U id, INT8U len, INT8U* buf) {

	INT8U sndStat = CAN_FAIL;
	sndStat = CAN0.sendMsgBuf(id, len, buf);
#ifdef DEBUGSERIAL
	if (sndStat != CAN_OK) {
		DEBUGSERIAL.print("ERROR: CAN0.sendMsgBuf(");
		DEBUGSERIAL.print(id, HEX);
		if (id & CAN_REMOTEREQUEST) DEBUGSERIAL.print("  CAN_REMOTEREQUEST");
		if (id & CAN_EXTENDED) DEBUGSERIAL.print("  CAN_EXTENDED");
		DEBUGSERIAL.print(")=");
		if (sndStat == CAN_GETTXBFTIMEOUT) DEBUGSERIAL.println("CAN_GETTXBFTIMEOUT");
		else if(sndStat == CAN_SENDMSGTIMEOUT) DEBUGSERIAL.println("CAN_SENDMSGTIMEOUT");
		else DEBUGSERIAL.println(sndStat);
		
	}
#endif
	return sndStat;
}
INT8U CAN_SetMode(int mode) {
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

#ifdef DISPLAY_CAN_SLAVE


INT8U CAN_BeginSlave() {
	int retries = 1;
	INT8U rv = MCP2515_FAIL;
	for (int r = 0; r < retries; r++) {
		if (CAN0.begin(MCP_STDEXT, CAN_500KBPS, MCP_8MHZ) == CAN_OK) {
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
	rv = CAN_SetMode(MCP_NORMAL);

	if (rv != CAN_OK)return rv;
	pinMode(CAN0_INT, INPUT);								// Configuring pin for /INT input
	long mask = ((long)0x7FF) << 16;
	// here we allow EXTENDED IDs CAN_PRIVATE1 and CAN_PRIVATE2 only
	if (CAN_PRIVATE1 & CAN_EXTENDED) { mask = 0x1FFFFFFF; }
	
	rv = CAN0.init_Mask(0, (CAN_PRIVATE1 & CAN_EXTENDED) ? 1 : 0, mask);      // there are 2 mask in mcp2515, you need to set both of them
	if (rv != CAN_OK) {
#ifdef MCP_STDERR
		MCP_STDERR.print("CAN0.init_Mask(0, 0, ((long)0xCAN_PRIVATE1&CAN_ALL_STD) error:");
		MCP_STDERR.println(rv);
#endif
		return rv;
	}
	CAN0.init_Filt(0, (CAN_PRIVATE1 & CAN_EXTENDED) ? 1 : 0, (CAN_PRIVATE1 & CAN_EXTENDED) ? (long)CAN_PRIVATE1 : (long)CAN_PRIVATE1 << 16);
	CAN0.init_Filt(1, (CAN_PRIVATE2 & CAN_EXTENDED) ? 1 : 0, (CAN_PRIVATE2 & CAN_EXTENDED) ? (long)CAN_PRIVATE2 : (long)CAN_PRIVATE2 << 16);

	CAN0.init_Mask(1, (CAN_PRIVATE1 & CAN_EXTENDED) ? 1 : 0, mask);
	CAN0.init_Filt(2, (CAN_PRIVATE1 & CAN_EXTENDED) ? 1 : 0, (CAN_PRIVATE1 & CAN_EXTENDED) ? (long)CAN_PRIVATE1 : (long)CAN_PRIVATE1 << 16);
	CAN0.init_Filt(3, (CAN_PRIVATE2 & CAN_EXTENDED) ? 1 : 0, (CAN_PRIVATE2 & CAN_EXTENDED) ? (long)CAN_PRIVATE2 : (long)CAN_PRIVATE2 << 16);
	CAN0.init_Filt(4, (CAN_PRIVATE2 & CAN_EXTENDED) ? 1 : 0, (CAN_PRIVATE2 & CAN_EXTENDED) ? (long)CAN_PRIVATE2 : (long)CAN_PRIVATE2 << 16);
	CAN0.init_Filt(5, (CAN_PRIVATE2 & CAN_EXTENDED) ? 1 : 0, (CAN_PRIVATE2 & CAN_EXTENDED) ? (long)CAN_PRIVATE2 : (long)CAN_PRIVATE2 << 16);
	CAN0.init_Filt(6, (CAN_PRIVATE2 & CAN_EXTENDED) ? 1 : 0, (CAN_PRIVATE2 & CAN_EXTENDED) ? (long)CAN_PRIVATE2 : (long)CAN_PRIVATE2 << 16);
	// there are 6 filter in mcp2515

	return rv;
}


INT8U  getCANDataPrivate(long duration, EngineMsmtU& Engine) {
	INT8U sndStat = CAN_FAIL;
	int bothreached = 0;
	unsigned long privateids[] = { CAN_PRIVATE1 ,CAN_PRIVATE2 };
	long start = millis();
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
				if (rxId == CAN_PRIVATE1) DEBUGSERIAL.print("CAN_PRIVATE1");
				else if (rxId == CAN_PRIVATE2) DEBUGSERIAL.print("CAN_PRIVATE2");
				else DEBUGSERIAL.print(rxId, HEX);
				DEBUGSERIAL.print(" (");
				DEBUGSERIAL.print("len=");
				DEBUGSERIAL.print(len);
				DEBUGSERIAL.println(")");
#endif
				if (len < 8) continue;
				if (rxId == CAN_PRIVATE1) {
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
				if (rxId == CAN_PRIVATE2) {
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

#else
INT8U CAN_BeginMaster() {
	int retries = 1;
	INT8U rv = MCP2515_FAIL;
	for (int r = 0; r < retries; r++) {
		if (CAN0.begin(MCP_STDEXT, CAN_500KBPS, MCP_8MHZ) == CAN_OK) {
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
	
	rv = CAN_SetMode(MCP_NORMAL);
	// here we allow only standard Ids CAN_ANTRIEB242 CAN_ANTRIEB245 CAN_ANTRIEB442
	if (rv != CAN_OK)return rv;
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

	CAN0.init_Mask(1, 0, ((long)0x7FF) << 16); // only address containing 0x40 are allowed, 0x640, 0x440, 0x240, 0x241,0x24F,...
	CAN0.init_Filt(2, 0, ((long)CAN_ANTRIEB242) << 16);
	CAN0.init_Filt(3, 0, ((long)CAN_ANTRIEB245) << 16);
	CAN0.init_Filt(4, 0, ((long)CAN_ANTRIEB442) << 16);
	CAN0.init_Filt(5, 0, ((long)CAN_ANTRIEB442) << 16);
	CAN0.init_Filt(6, 0, ((long)CAN_ANTRIEB442) << 16);
	// there are 6 filter in mcp2515
	
	return rv;
}
INT8U send442() {
	byte sndStat = CAN_FAIL;
	//sndStat = CAN_SetMode(MCP_NORMAL);
	canbuf[0] = 0x02;// ACD_1  2 = Normal_Prog_aktiv
	sndStat = CAN0.sendMsgBuf(CAN_ANTRIEB442, 1, canbuf);
	//byte sndStat2 = CAN_SetMode(MCP_LISTENONLY);
	return sndStat; // never sends
}

INT8U sendBothPrivateCan(EngineMsmtU& _Engine) {
	byte sndStat = CAN_FAIL;
	//sndStat = CAN_SetMode(MCP_NORMAL);

	//if (sndStat != CAN_OK) return sndStat;
	// use address CAN_PRIVATE1, must not interfere
	sndStat = CAN_SendMsg(CAN_PRIVATE1, 8, (byte*)&_Engine.b[0]);
	if (sndStat != CAN_OK) return sndStat;
	sndStat = CAN_SendMsg(CAN_PRIVATE2, 8, (byte*)&_Engine.b[8]);
	if (sndStat != CAN_OK) return sndStat;
	//byte sndStat2 = CAN_SetMode(MCP_LISTENONLY);
	return sndStat;
}
uint16_t lastnmot = 0;
int nsame = 0;

INT8U getCan242(long duration, MOTOR_1& can242) {
	INT8U sndStat = CAN_OK;
	sndStat = CAN0.setMode(MCP_LISTENONLY);
	if (sndStat != CAN_OK) return sndStat;
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
	sndStat = CAN0.setMode(MCP_NORMAL);
	//CAN_Spi_UnSelect();
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

INT8U getCan245(long duration, MOTOR_2& can245) {
	INT8U sndStat = CAN_FAIL;
	sndStat = CAN0.setMode(MCP_LISTENONLY);
	if (sndStat != CAN_OK) return sndStat;
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
	sndStat = CAN0.setMode(MCP_NORMAL);
	//CAN_Spi_UnSelect();
	if (!receivedintime) {
#ifdef	MCP_STDERR
		MCP_STDERR.println("Timout reading CAN_ANTRIEB245");
#endif
	}
	return sndStat;
}

#endif







#ifdef FUNCS_CAN_REMOTEREQUEST
INT8U SendRemoteRequest(int txid) {
	INT8U sndStat = CAN_FAIL;
	sndStat = CAN_SetMode(MCP_NORMAL);
	if (sndStat != CAN_OK) return sndStat;
	sndStat = CAN_SendMsg(txid | CAN_REMOTEREQUEST, 8, canbuf);
	return sndStat;
}

INT8U getRemoteRequest(long duration, int txId) {
	INT8U sndStat = CAN_FAIL;
	sndStat = CAN_SetMode(MCP_LISTENONLY);
	if (sndStat != CAN_OK) return sndStat;
	txId |= CAN_REMOTEREQUEST;
	long start = millis();
	bool receivedintime = false;
	do {
		if (!digitalRead(CAN0_INT))                         // If CAN0_INT pin is low, read receive buffer
		{
			sndStat = CAN0.readMsgBuf(&rxId, &len, canbuf);
			if (sndStat != CAN_OK) continue;
			if (rxId == txId ) {
#ifdef DEBUGSERIAL
				DEBUGSERIAL.print("RECEIVED: CAN_REMOTEREQUEST(0x");
				int realid = rxId ^ CAN_REMOTEREQUEST;
				if(realid==CAN_PRIVATE1)DEBUGSERIAL.print("CAN_PRIVATE1");
				else if(realid == CAN_PRIVATE2) DEBUGSERIAL.print("CAN_PRIVATE1");
				else DEBUGSERIAL.print(realid, HEX);
				DEBUGSERIAL.println(")");
#endif
				receivedintime = true;
				break;
			}
		}
	} while (millis() - start < duration);
	CAN_Spi_UnSelect();

	if (!receivedintime) {
#ifdef	MCP_STDERR
		MCP_STDERR.print("TIMEOUT WAITING FOR REMOTEREQUEST ");
		int realid = rxId ^ CAN_REMOTEREQUEST;
		if (realid == CAN_PRIVATE1)MCP_STDERR.print("CAN_PRIVATE1");
		else if (realid == CAN_PRIVATE2) MCP_STDERR.print("CAN_PRIVATE1");
		else MCP_STDERR.print(realid);
		MCP_STDERR.println(")");
#endif
	}
	return sndStat;
}
#endif



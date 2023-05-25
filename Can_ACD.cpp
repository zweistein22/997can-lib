#include <can997.h>
#include "CAN_ACD.h"


MCP_CAN CAN3(CAN3_CS);

#define DEBUGSERIAL Serial





INT8U CAN3_ACD_Begin(int mode) {
	pinMode(CAN3_RESET, OUTPUT);
	digitalWrite(CAN3_RESET, LOW);
	delay(10);
	digitalWrite(CAN3_RESET, HIGH);
	pinMode(CAN3_RESET, INPUT_PULLUP);
	
	SPI.begin();
	
	int retries = 1;
	INT8U rv = MCP2515_FAIL;
	for (int r = 0; r < retries; r++) {
		if (CAN3.begin(MCP_ANY, CAN_500KBPS, MCP_8MHZ) == CAN_OK) {
			rv = MCP2515_OK;
			break;
		}

	}
	if (rv == MCP2515_FAIL) {

		MCP_STDERR(println("CAN3_ACD_Begin() FAIL"));

		return rv;
	}


	pinMode(CAN3_INT, INPUT);								// Configuring pin for /INT input
	long mask = ((long)0x7FF) << 16;

	rv = CAN3.init_Mask(0, 0, mask);      // there are 2 mask in mcp2515, you need to set both of them
	if (rv != CAN_OK) {
		MCP_STDERR(print("CAN3.init_Mask(0, 0, ((long)0x7ff) error:"));
		MCP_STDERR(println(rv));

		return rv;
	}
	CAN3.init_Filt(0, 0, ((long)CAN_ANTRIEB_ACD_1) << 16);
	CAN3.init_Filt(1, 0, ((long)CAN_ANTRIEB_ACD_EB1_TX) << 16);

	CAN3.init_Mask(1, 0, mask); // only address containing 0x40 are allowed, 0x640, 0x440, 0x240, 0x241,0x24F,...
	CAN3.init_Filt(2, 0, ((long)CAN_ANTRIEB_ACD_V) << 16);
	CAN3.init_Filt(3, 0, ((long)CAN_ANTRIEB_D_RS_ACD) << 16);
	CAN3.init_Filt(4, 0, ((long)CAN_ANTRIEB_D_RS_ACD) << 16);
	CAN3.init_Filt(5, 0, ((long)CAN_ANTRIEB_D_RS_ACD) << 16);
	// there are 6 filter in mcp2515

	byte sndStat = CAN_FAIL;
	sndStat = CAN3.setMode(mode);
	if (sndStat != CAN_OK) {
		MCP_STDERR(print("Error CAN3.setMode("));
		MCP_STDERR(print(mode));
		MCP_STDERR(print(") code="));
		MCP_STDERR(println(sndStat));
	}
	return sndStat;

}

INT8U CAN3_get442(long duration, ACD_1& can442) {
	INT8U sndStat = CAN_FAIL;
	long start = millis();
	bool receivedintime = false;
	do {
		if (!digitalRead(CAN3_INT))                         // If CAN0_INT pin is low, read receive buffer
		{
			sndStat = CAN3.readMsgBuf(&rxId, &len, canbuf);
			if (sndStat != CAN_OK) continue;
			if (rxId == 0x442) {
#ifdef DEBUGSERIAL
				DEBUGSERIAL.print("Antrieb(0x442):");
				if (sizeof(ACD_1) != 8)DEBUGSERIAL.println("FATAL: ACD_1 NOT sizeof(8)");
#endif
				if (len < 8) continue;
				memcpy(&can442, canbuf, 8);

#ifdef DEBUGSERIAL
				DEBUGSERIAL.print("St_ACD=");
				DEBUGSERIAL.print(can442.St_ACD);
				DEBUGSERIAL.print(",F_Anz_ACD=");
				DEBUGSERIAL.print(can442.F_Anz_ACD);
				DEBUGSERIAL.print(",ACD_Text=");
				DEBUGSERIAL.print(can442.ACD_Text);
				DEBUGSERIAL.print(",Anz_ACD_ein=");
				DEBUGSERIAL.print(can442.Anz_ACD_ein);
				DEBUGSERIAL.println("");
#endif
				receivedintime = true;
				break;
			}
		}
	} while (millis() - start < duration);
	//sndStat = CAN0.setMode(MCP_NORMAL);

	if (!receivedintime) {
		MCP_STDERR(println("Timout reading CAN_ANTRIEB_ACD_1"));
	}
	return sndStat;
}


INT8U CAN3_get71b(long duration, ACD_V& can71b) {
	INT8U sndStat = CAN_FAIL;
	long start = millis();
	bool receivedintime = false;
	do {
		if (!digitalRead(CAN3_INT))                         // If CAN0_INT pin is low, read receive buffer
		{
			sndStat = CAN3.readMsgBuf(&rxId, &len, canbuf);
			if (sndStat != CAN_OK) continue;
			if (rxId == 0x71b) {
#ifdef DEBUGSERIAL
				DEBUGSERIAL.print("Antrieb(0x442):");
				if (sizeof(ACD_V) != 8)DEBUGSERIAL.println("FATAL: ACD_V NOT sizeof(8)");
#endif
				if (len < 8) continue;
				memcpy(&can71b, canbuf, 8);

#ifdef DEBUGSERIAL
				DEBUGSERIAL.print("AC_SW_Vers=");
				DEBUGSERIAL.print(can71b.AC_SW_Vers);
				DEBUGSERIAL.print(" (");
				DEBUGSERIAL.print(can71b.AC_SW_Jahr);
				DEBUGSERIAL.print(".");
				DEBUGSERIAL.print(can71b.AC_SW_Monat);
				DEBUGSERIAL.print(".");
				DEBUGSERIAL.print(can71b.AC_SW_Tag);
				DEBUGSERIAL.print(")");

				DEBUGSERIAL.print(",AC_CAN_STAND=");
				DEBUGSERIAL.print(can71b.AC_CAN_STAND);


				DEBUGSERIAL.print(",AC_F_BusOff=");
				DEBUGSERIAL.print(can71b.AC_F_BusOff);
				DEBUGSERIAL.print(",AC_F_BusOff_Anz=");
				DEBUGSERIAL.print(can71b.AC_F_BusOff_Anz);

				DEBUGSERIAL.print(",AC_F_CAN_Sig=");
				DEBUGSERIAL.print(can71b.AC_F_CAN_Sig);
				DEBUGSERIAL.print(",AC_F_CAN_Time=");
				DEBUGSERIAL.print(can71b.AC_F_CAN_Time);
				DEBUGSERIAL.print(",AC_KD_F=");
				DEBUGSERIAL.print(can71b.AC_KD_F);
				DEBUGSERIAL.print(",AC_REC=");
				DEBUGSERIAL.print(can71b.AC_REC);
				
				DEBUGSERIAL.print(",AC_TEC=");
				DEBUGSERIAL.print(can71b.AC_TEC);




				DEBUGSERIAL.println("");
#endif
				receivedintime = true;
				break;
			}
		}
	} while (millis() - start < duration);
	//sndStat = CAN0.setMode(MCP_NORMAL);

	if (!receivedintime) {
		MCP_STDERR(println("Timout reading CAN_ANTRIEB_ACD_V"));
	}
	return sndStat;
}
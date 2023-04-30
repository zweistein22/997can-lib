#include "CAN_ACD.h"


MCP_CAN CAN3(CAN3_CS);







INT8U CAN3_ACD_Begin() {
	pinMode(CAN3_RESET, OUTPUT);
	digitalWrite(CAN3_RESET, LOW);
	delay(10);
	digitalWrite(CAN3_RESET, HIGH);
	pinMode(CAN3_RESET, INPUT_PULLUP);

	int retries = 1;
	INT8U rv = MCP2515_FAIL;
	for (int r = 0; r < retries; r++) {
		if (CAN3.begin(MCP_ANY, CAN_500KBPS, MCP_8MHZ) == CAN_OK) {
			rv = MCP2515_OK;
			break;
		}

	}
	if (rv == MCP2515_FAIL) {
#ifdef MCP_STDERR
		MCP_STDERR.println("CAN3_ACD_Begin() FAIL");
#endif
		return rv;
	}


	pinMode(CAN3_INT, INPUT);								// Configuring pin for /INT input
	long mask = ((long)0x7FF) << 16;

	rv = CAN3.init_Mask(0, 0, mask);      // there are 2 mask in mcp2515, you need to set both of them
	if (rv != CAN_OK) {
#ifdef MCP_STDERR
		MCP_STDERR.print("CAN3.init_Mask(0, 0, ((long)0x7ff) error:");
		MCP_STDERR.println(rv);
#endif

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
	sndStat = CAN3.setMode(MCP_NORMAL);
#ifdef MCP_STDERR
	if (sndStat != CAN_OK) {
		MCP_STDERR.print("Error CAN3.setMode(MCP_NORMAL)");
		MCP_STDERR.print(") code=");
		MCP_STDERR.println(sndStat);
	}
#endif
	return sndStat;

}



#include <CAN_ACD.h>


byte can3buf[8];

byte rv = CAN_FAIL;


void setup() {
  // put your setup code here, to run once:
  delay(500);
  MCP_STDERR(begin(115200));

   do {
    rv = CAN3_ACD_Begin(MCP_LISTENONLY);
   }while(rv==CAN_FAIL);
}

ACD_1 acd_1;

ACD_V acd_V;


int iloop=0;
void loop() {
  // put your main code here, to run repeatedly:

	if (true) {
		

	}
	else {
		setMode(CAN3, MCP_NORMAL);
		rv = CAN_FAIL;
		acd_1.Anz_ACD_ein = 1;
		acd_1.ACD_Text = 0x02;

		//can3buf[0] = 0x02;// ACD_1  2 = Normal_Prog_aktiv
		rv = CAN3.sendMsgBuf(CAN_ANTRIEB_ACD_1, 1, (byte*)&acd_1);
		delay(100);
		if (!iloop % 10) {
			// so every 1000 ms
			acd_V.AC_CAN_STAND =
				acd_V.AC_SW_Jahr = 2010;
			acd_V.AC_SW_Monat = 3;
			acd_V.AC_SW_Tag = 20;
			rv = CAN3.sendMsgBuf(CAN_ANTRIEB_ACD_V, 8, (byte*)&acd_V);

		}
		setMode(CAN3, MCP_LISTENONLY);
	}
  iloop++;


}

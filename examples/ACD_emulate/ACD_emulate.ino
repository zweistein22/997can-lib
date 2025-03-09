#include <Arduino.h>



#include "PushButton.h"
#include <CAN_ACD.h>

#define PASM_A2  3  // Pasm on switch sets pin to ground with a 120Ohm resistance
#define PASM_A5  4  // status led, set pin to ground to illuminate led, if pin is high impedance offgh, then PASM should go on.
byte rv = CAN_FAIL;


Pushbutton pasmbutton(PASM_A2);


void PASM_LED_ON() {
    pinMode(PASM_A5, OUTPUT);
    digitalWrite(PASM_A5, LOW);
}
void PASM_LED_OFF() {
    pinMode(PASM_A5, INPUT);
}

bool pasmActive = false; 

ACD_1 acd_1;

ACD_V acd_V;


uint8_t known_rq_rs[22][8] = {
    {0x02,0x10,0x89,0x00,0x00,0x00,0x00,0x00},
    {0x02,0x50,0x89,0x00,0x00,0x00,0x00,0x00},
    {0x04,0x18,0x00,0xff,0x00,0x00,0x00,0x00},
    {0x10,0x1d,0x58,0x09,0x00,0x0b,0x20,0x00},
    {0x30,0x00,0x01,0x55,0x55,0x55,0x55,0x55},
    {0x21,0x0d,0x20,0x00,0x0e,0x20,0x00,0x0f},
    {0x22,0x20,0x00,0x18,0x60,0xc1,0x11,0x20},
    {0x23,0xc2,0x03,0x60,0xc2,0x02,0x20,0xc1},
    {0x24,0x50,0x20,0x00,0x0d,0x20,0x00,0x0e},
    {0x03,0x14,0xff,0x00,0x00,0x00,0x00,0x00},
    {0x03,0x54,0xff,0x00,0x0d,0x20,0x00,0x0e},
    {0x04,0x18,0x00,0xff,0x00,0x00,0x00,0x00},
    {0x05,0x58,0x01,0xc2,0x03,0x60,0x00,0x0e},
    {0x02,0x10,0x89,0x00,0x00,0x00,0x00,0x00},
    {0x02,0x50,0x89,0xc2,0x03,0x60,0x00,0x0e},
    {0x02,0x1a,0x91,0x00,0x00,0x00,0x00,0x00},
    {0x10,0x0d,0x5a,0x91,0x39,0x38,0x37,0x36},
    {0x30,0x00,0x01,0x55,0x55,0x55,0x55,0x55},
    {0x21,0x31,0x38,0x31,0x30,0x35,0x30,0x39},
    {0x02,0x10,0x89,0x00,0x00,0x00,0x00,0x00},
    {0x02,0x50,0x89,0x31,0x30,0x35,0x30,0x39},
    {0x02,0x1a,0x90,0x00,0x00,0x00,0x00,0x00}
};

void setup() {
  // put your setup code here, to run once:
  acd_1.Anz_ACD_ein = 1;
  acd_1.ACD_Text = 0x02; // normal
  acd_V.AC_CAN_STAND = 1;
  acd_V.AC_SW_Jahr = 2010;
  acd_V.AC_SW_Monat = 3;
  acd_V.AC_SW_Tag = 20;

  MCP_STDERR(begin(115200));
 
  delay(2000);
    MCP_STDERR(println("setup start."));
  do {
    rv = CAN3_ACD_Begin(MCP_NORMAL);
   }while(rv==CAN_FAIL);
    
    MCP_STDERR(println("setup end."));
  
    pasmActive ? PASM_LED_ON() : PASM_LED_OFF();
}





D_RQ_ACD d_rq_acd;

D_RQ_ALL_A d_rq_all_a;

int last_good_acd_status=0;


int iloop=0;

long millisV=0;
long millisI=0;
long AnzeigeOffAfter = 10000;
long AnzeigeOnStarted = millis();

void loop() {
    // put your main code here, to run repeatedly:
    rv = CAN3_get5f4_5d6(30, d_rq_acd, d_rq_all_a);

    if (rv == CAN_OK) {
        if (rxId == 0x5d6) {

        }
        if (rxId == 0x5f4) {
            int index = 1;
            rv = CAN3.sendMsgBuf(CAN_ANTRIEB_D_RS_ACD, 8, (byte*)known_rq_rs[index]);

        }

    }
    if (pasmbutton.getSingleDebouncedPress()) {
        pasmActive = !pasmActive;
        pasmActive ? PASM_LED_ON() : PASM_LED_OFF();
        if (pasmActive) {
            if (acd_1.ACD_Text != 0x03) {
                acd_1.Anz_ACD_ein = 1;
                MCP_STDERR(println("acd_1.Anz_ACD_ein, ACD_Text = 0x03"));
                AnzeigeOnStarted = millis();
                acd_1.ACD_Text = 0x03;
            }
        }
        else {
            if (acd_1.ACD_Text != 0x02) {
                acd_1.Anz_ACD_ein = 1;
                MCP_STDERR(println("acd_1.Anz_ACD_ein, ACD_Text = 0x02"));
                AnzeigeOnStarted = millis();
                acd_1.ACD_Text = 0x02;
            }
        }
    }
    if (acd_1.Anz_ACD_ein) {
            if (millis() > AnzeigeOnStarted + AnzeigeOffAfter) {
                MCP_STDERR(println("acd_1.Anz_ACD aus"));
                acd_1.Anz_ACD_ein = 0;
                // turn off after a time    
            }
    }
    if (millis() - millisI > 100) {
            rv = CAN3.sendMsgBuf(CAN_ANTRIEB_ACD_1, 4, (byte*)&acd_1);
            millisI = millis();
    }
    // CAN3_get5f4(50,d_rq_acd);
    //CAN3_get5d6(100, d_rq_all_a);
    if (millis() - millisV > 1000) {
        rv = CAN3.sendMsgBuf(CAN_ANTRIEB_ACD_V, 8, (byte*)&acd_V);
        millisV = millis();
    }
}


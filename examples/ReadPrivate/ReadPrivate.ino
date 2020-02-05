#define DISPLAY_CAN_SLAVE
#define  NO_CAN_242_245_442
#define NO_PRINTLNDATASERIAL
#include <Can997.h>
#include <EngineMsmt.h>

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  while (!Serial);
  Serial.flush();
  Serial.print("sizeof(EngineMsmt)=");
  Serial.println(sizeof(EngineMsmt));
  Serial.flush();
}
byte can_result = CAN_FAIL;

void loop() {
  // put your main code here, to run repeatedly:

   
   if (can_result != CAN_OK) {
       can_result = CAN_BeginSlave();
       delay(500);
       //if (MCP2515_FAIL == can_result) resetarduino();
   }
  //getCan242(50, can242);
  //getCan245(50, can245);
   if (getCANDataPrivate(200, Engine) == CAN_OK) {
       Serial.print("EGT(left)=");
       Serial.println((int)Engine.sensor.egtl);
   }
  // else Serial.println("ERROR: getCANDataPrivate(400, Engine) ");
  
}

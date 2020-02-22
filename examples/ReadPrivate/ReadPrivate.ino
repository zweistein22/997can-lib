//#define  NO_CAN_242_245_442
//#define NO_PRINTLNDATASERIAL
#define DEBUGSERIAL Serial
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
   if (can_result== CAN_OK && getCANDataPrivate(300, Engine) == CAN_OK) {
       Serial.print("EGT(left)=");
       Serial.print((int)Engine.sensor.egtl);
       Serial.print(", iatbeforeIC=");
       Serial.println((int)Engine.sensor.iatbeforeIC);

   }
  // else Serial.println("ERROR: getCANDataPrivate(400, Engine) ");
   delay(100);
  
}

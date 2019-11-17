#include <Can997.h>
#include <EngineMsmt.h>

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:

   byte can_result = CAN_OK;
  //getCan242(50, can242);
  //getCan245(50, can245);
  can_result=getCANDataPrivate(200,Engine);
  if (can_result != CAN_OK) {
    can_result=CAN_Begin();
    delay(500);
    return;
  }

  Serial.print("EGT(left)=");
  Serial.println(Engine.sensor.egtl);
}

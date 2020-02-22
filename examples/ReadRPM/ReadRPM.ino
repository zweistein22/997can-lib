#include <Can997.h>
#include <EngineMsmt.h>

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
}

MOTOR_1 can242;

byte can_result = CAN_FAIL;
void loop() {
  // put your main code here, to run repeatedly:

	
	if (can_result != CAN_OK) {
		can_result = CAN_BeginMaster();
		delay(500);
	}
	if(can_result==CAN_OK)	can_result = getCan242(100, can242);
	if (can_result == CAN_OK) {
		Serial.print("RPM=");
		Serial.println((float)can242.nmot / 4);
	}
	
	delay(100);

}

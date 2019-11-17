#include <Can997.h>
#include <EngineMsmt.h>

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
}

MOTOR_1 can242;
void loop() {
  // put your main code here, to run repeatedly:

	byte can_result = getCan242(100, can242);
	if (can_result != CAN_OK) {
		can_result = CAN_Begin();
		delay(500);
		return;
	}
	Serial.print("RPM=");
	Serial.println((float)can242.nmot/4);

}

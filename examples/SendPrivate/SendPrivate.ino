
#include <Can997.h>
#include <EngineMsmt.h>


void setup()
{
  delay(2000);
  Serial.begin(115200);
  EngineMsmtU_Zero();
  CAN_Begin();
  }
int iloop = 0;
void loop()
{
	Engine.sensor.egtl = iloop++ % 1050;
	Engine.sensor.egtr = Engine.sensor.egtl;
	Engine.sensor.lambdaplus100 = (byte)((int)random(-9 + 100,-9+179));
	Serial.print("EGT =");
	Serial.println(Engine.sensor.egtl);
	if (sendCan()!=CAN_OK) {
		//Serial.println("SendCan FAIL");
		CAN_Begin();
		delay(1000);
	}
	delay(100);
}

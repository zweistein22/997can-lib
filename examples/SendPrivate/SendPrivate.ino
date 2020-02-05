
#include <Can997.h>
#include <EngineMsmt.h>


void setup()
{
  Serial.begin(115200);
  while (!Serial);
  EngineMsmtU_Zero();
  
  }
int iloop = 0;
byte can_result = CAN_FAIL;
unsigned long lastmillis = millis();
long minlooptime = 100;
void loop()
{
	if (can_result != CAN_OK) {
		can_result = CAN_BeginMaster();
		delay(20);
		//if (MCP2515_FAIL == can_result) resetarduino();
	}
	Engine.sensor.egtl =( iloop++ % 1050);
	Engine.sensor.egtr = Engine.sensor.egtl;
	Engine.sensor.lambdaplus100 = (byte)((int)random(-9 + 100,-9+179));
	

	if(sendBothPrivateCan(Engine)==CAN_OK){
	
		Serial.print("EGT =");
		Serial.println(Engine.sensor.egtl);
	}
	//else Serial.println("ERROR sendBothPrivateCan(Engine)");

	long to_wait= minlooptime- (millis() - lastmillis);
	if (to_wait > 0) delay(to_wait);
	lastmillis = millis();
	
	
}

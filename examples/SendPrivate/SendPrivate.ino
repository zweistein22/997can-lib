
#include <Can997.h>

byte can_result = CAN_FAIL;
void setup()
{
  Serial.begin(115200);
  //while (!Serial);
  EngineMsmtU_Zero();
  can_result = CAN1_BeginMaster();
  
  }
int iloop = 0;

unsigned long lastmillis = millis();
long minlooptime = 10;
void loop()
{
	if (can_result != CAN_OK) {
		can_result = CAN1_BeginMaster();
		delay(200);
		//if (MCP2515_FAIL == can_result) resetarduino();
	}
	Engine.sensor.egtl =(int)( iloop++ % 1050);
	Engine.sensor.egtr = Engine.sensor.egtl;
	Engine.sensor.lambdaplus100 = (byte)((int)random(87 + 100,115 +100));
	Engine.sensor.llambdaplus100 = Engine.sensor.lambdaplus100;
	Engine.sensor.iatbeforeIC = Engine.sensor.iatl = Engine.sensor.iatr = 0;
	Engine.sensor.gearboxoilpump = 0;
	Engine.sensor.Tmot = 50;
	
	Engine.sensor.map = (int)random(950, 1800);

	can_result = CAN1_sendbothPrivate(Engine);
	if(can_result == CAN_OK){
	
		Serial.print("EGT =");
		Serial.print(Engine.sensor.egtl);
		Serial.print(", iatl=");
		Serial.println(Engine.sensor.iatl);
	}
	//else Serial.println("ERROR sendBothPrivateCan(Engine)");

	long to_wait= minlooptime- (millis() - lastmillis);
	if (to_wait > 0) delay(to_wait);
	lastmillis = millis();
	
	
}

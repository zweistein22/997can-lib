#include <BreitBandLambda.h>
#include "EngineMsmt.h"
#include <PString.h>

//#define PRN_MOTOR1
//#define PRN_MOTOR2
#define PRN_MOTOR4


EngineMsmtU Engine;
HeadU Head;


#define GENERATE_PROGMEMSTRING(STRING) const char str_##STRING[] PROGMEM ={#STRING};
#define GENERATE_STRINGNAME(STRING)  str_##STRING,
	FOREACH_EngineError(GENERATE_PROGMEMSTRING);


PGM_P const EngineError_STRING[] PROGMEM = {
	FOREACH_EngineError(GENERATE_STRINGNAME)
};


float EngOilTemp(byte & Tmot) {
	return 0.75*Tmot - 48;
}


void HeadU_Zero() {

	Head.settings.oilpump = 0;
	Head.settings.vacuumpump = 0;
	Head.settings.waterinjection = 0;
}

void EngineMsmtU_Zero() {

	Engine.sensor.egtl = 0;
	Engine.sensor.egtr = 0;
	Engine.sensor.gearboxoilpump = 0;
	Engine.sensor.iatbeforeIC = 0;
	Engine.sensor.iatl = 0;
	Engine.sensor.iatr = 0;
	Engine.sensor.lambdaplus100 = 170;
	Engine.sensor.llambdaplus100 =170;
	Engine.sensor.map = 1013;

}

#ifndef NO_PRINTLNDATASERIAL

char _buffer1[196];
PString line1(_buffer1, sizeof(_buffer1));

//#define PRN_MOTOR1
//#define PRN_MOTOR2
//#define PRN_MOTOR4

void PrintlnDataSerial(EngineMsmt &engine, MOTOR_1 &can242, MOTOR_2 & can245, MOTOR_4 & can441) {
	line1.begin();
	if (engine.status != OK) {
		char buf[20];
		strcpy_P(buf, (char*)pgm_read_word(&(EngineError_STRING[engine.status])));
		line1.print(buf);
		//line.print(vvv++);
		line1.print("|");
	}

	line1.print("map:");
	line1.print(engine.map);
	line1.print("|");
	line1.print("air:");
	line1.print(engine.iatbeforeIC);
	line1.print("|");
	line1.print("iatl:");
	line1.print(engine.iatl);
	line1.print("|");
	line1.print("iatr:");
	line1.print(engine.iatr);
	line1.print("|");
	line1.print("egtl:");
	if (engine.EGT_Status_left != MAX31855OK) {
		if (engine.EGT_Status_left == OC) line1.print("OC");
		if (engine.EGT_Status_left == SCV) line1.print("SCV");
		if (engine.EGT_Status_left == SCG) line1.print("SCG");

	}
	else   line1.print(engine.egtl);
	line1.print("|");

	line1.print("egtr:");
	if (engine.EGT_Status_right != MAX31855OK) {
		if (engine.EGT_Status_right == OC) line1.print("OC");
		if (engine.EGT_Status_right == SCV) line1.print("SCV");
		if (engine.EGT_Status_right == SCG) line1.print("SCG");
	}
	else line1.print(engine.egtr);
	line1.print("|");
	line1.print("llamb:");
	if (engine.llambdaplus100 < 100) {
		line1.print("(");
		line1.print(pstr_lambdaErrors(engine.llambdaplus100 - 100));
		line1.print(")");
	}
	else line1.print((float)(engine.llambdaplus100 - 100) / 100.0);
	line1.print("|");
	
	line1.print("lambda:");
	if (engine.lambdaplus100 < 100) {
		line1.print("(");
		line1.print(pstr_lambdaErrors(engine.lambdaplus100 - 100));
		line1.print(")");
	}
	else line1.print((float)(engine.lambdaplus100 - 100) / 100.0);
#ifdef PRN_MOTOR1
	
	line1.print("|");
	line1.print("nmot:");
	line1.print(can242.nmot / 4);

	line1.print("|");
	line1.print("Tmot:");
	line1.print(EngOilTemp(can245.Tmot));

	line1.print("|");
	line1.print("miist:");
	line1.print(can242.miist*100.0 / 255.0);

	line1.print("|");
	line1.print("Wped_fgr:");
	line1.print(can242.Wped_fgr*100.0 / 254.0);
	line1.print("|");
#endif
#ifdef PRN_MOTOR_2
	line1.print("E_tm:");
	line1.print(can245.E_tm);
	line1.print("|");

	line1.print("B_bremse:");
    line1.print(can245.B_bremse);
	line1.print("|");

	
	//line1.print(",E_bremse=");
	//line1.print(can245.E_bremse);
	//line1.print(",B_ffz=");
	//line1.print(can245.B_ffz);

	line1.print("stat_fgr:");
	line1.print(can245.stat_fgr);
	line1.print("|"); 
	line1.print("mibas:");
	line1.print(can245.mibas);
	line1.print("|");
	line1.print("nstat:");
	line1.print(can245.nstat);
	line1.print("|");
	line1.print("mizwmn:");
	line1.print(can245.mizwmn);
	line1.print("|");
	line1.print("mimax:");
	line1.print(can245.mimax);
	line1.print("|");
	line1.print("misol:");
	line1.print(can245.misol);
#endif

#ifdef PRN_MOTOR4
	line1.print("|");
	line1.print("Toel:");
	line1.print(EngOilTemp(can441.Toel));
	line1.print("|");
	line1.print("Oeldruck:");
	line1.print(can441.Oeldruck*0.04);
#endif




	Serial.println(line1);

}
#endif

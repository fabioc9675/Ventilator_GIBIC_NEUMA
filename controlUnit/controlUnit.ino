/*
 Name:		controlUnit.ino
 Created:	4/3/2020 17:28:49
 Author:	Helber Carvajal
*/
#include <math.h>
#include <EEPROM.h>

// Definiciones para controlar el shiel DFRobot quad motor driver
// Definiciones para controlar el shiel DFRobot quad motor driver
#define EV_INSPIRA   25  // equivale al pin 3 del shield, velocidad motor 1
#define EV_ESC_CAM   27  // equivale al pin 6 del shield, velocidad motor 4
#define EV_ESPIRA    23  // equivale al pin 11 del shield, velocidad motor 2

#define EV_IN_CAM    26  // equivale al pin 5 del shield, velocidad motor 3
#define EV_IN_FLU    14  // equivale al pin 5 del shield, velocidad motor 3

// Definiciones para el manejo del ADC
#define ADC_PRESS_1     39  // pin ADC para presion 1
#define ADC_PRESS_2     36  // pin ADC para presion 2
#define ADC_PRESS_3     34  // pin ADC para presion 3

// Definiciones para controlar el sensor de flujo
#define FLANCO          18  // pin digital numero 2 para deteccion de flujo IRQ

#define STANDBY         20
#define BATTALARM       19

// Calibracion de los sensores de presion
#define AMP1       0.0232
#define OFFS1      -22.6363
#define AMP2       0.0289
#define OFFS2      -26.995
#define AMP3       0.0296
#define OFFS3      -27.006

// Variables de control del protocolo
#define RXD2 16
#define TXD2 17

// variables y parametros de impresion
String SERIALEQU;
String patientPress;
String circuitPress;
String patientFlow;
String circuitFlow;
String inspTime;
String expiTime;
String frequency;
String volume;

// variables para el envio de las alarmas
int alerPresionPIP = 0;
int alerDesconexion = 0;
int alerGeneral = 0;
int alerBateria = 0;

String RaspberryChain = "";

// estructura para el conteo de los pulsos
struct Fan {
	const uint8_t PIN;
	uint32_t numberOfPulses;
	bool aspa;
};

Fan fan1 = { FLANCO, 0, false };

// Definiciones del ADC
volatile int contADC = 0;

int ADC1_Value = 0;     //
int ADC2_Value = 0;
int ADC3_Value = 0;
float Pressure1 = 0;
float Pressure2 = 0;
float Pressure3 = 0;
int numeroPulsos = 0;

// variables para la atencion de interrupciones
bool flagTimerInterrupt = false;
volatile bool flagSwInterrupt = false;
volatile bool flagEncoderInterrupt_A = false;
volatile bool flagEncoderInterrupt_B = false;
volatile bool flagDettachInterrupt_A = false;
volatile bool flagDettachInterrupt_B = false;
volatile bool flagDetach = false;
bool flagAlarmPpico = false;
bool flagAlarmPatientDesconnection = false;
bool flagAlarmGeneral = false;
volatile unsigned int contDetach = 0;
unsigned int contCiclos = 0;
unsigned int contEscrituraEEPROM = 0;
unsigned int contUpdateData = 0;
bool flagStandbyInterrupt = false;
unsigned int contStandby = 0;

//***********************************
// datos para prueba de transmision
int pruebaDato = 0;
int second = 0;
int milisecond = 0;
// *********************************

// definiciones del ADC
float Pin[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
float Pout[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
float Ppac[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; //Vector de acumulacion de presiones en el paciente
float SPin = 0; //Senal filtrada de presion de entada a la camara
float SPout = 0; //Senal filtrada de presion de salida de la camara
float SPpac = 0; //Senal filtrada de presion del paciente
float Peep = 0;
float Ppico = 0;
float UmbralPeep = 100;
float UmbralPpico = -100;

float Pin_max = 0;
float Pout_max = 0;
float Pin_min = 0;
float Pout_min = 0;
float DifP = 0;
float VT = 0;
float V_Ppico[5] = { 0, 0, 0, 0, 0 };
float V_Ppeep[5] = { 0, 0, 0, 0, 0 };
float SPpico = 0;
float SPpeep = 0;

unsigned char BandGeneral = 0;
unsigned char ContGeneral = 0;

// Some global variables available anywhere in the program
volatile float inspirationTime = 1.666;
volatile float Time2 = 1.666;
volatile float expirationTime = 3.333;

// definiciones del timer
volatile int contCycling = 0;

bool flagInicio = true;

int frecRespiratoria = 12;
int I = 1;
int E = 41;
int maxPresion = 30;
// inicializacion del contador del timer
hw_timer_t* timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

void IRAM_ATTR onTimer();  // funcion de interrupcion
void receiveData();
void sendSerialData();
void alarmsDetection();


int eeprom_wr_int(int dataIn = 0, char process = 'r') {
	if (process == 'w') {
		byte r1 = (dataIn & 0xff);
		EEPROM.write(0, r1);
		r1 = (dataIn & 0xff00) >> 8;
		EEPROM.write(1, r1);
		r1 = (dataIn & 0xff0000) >> 16;
		EEPROM.write(2, r1);
		r1 = (dataIn & 0xff000000) >> 24;
		EEPROM.write(3, r1);
		EEPROM.commit();
		return 0;
	}
	else if (process == 'r') {
		int dataRead = 0;
		dataRead = EEPROM.read(0);
		dataRead = (EEPROM.read(1) << 8) + dataRead;
		dataRead = (EEPROM.read(2) << 16) + dataRead;
		dataRead = (EEPROM.read(3) << 24) + dataRead;
		return dataRead;
	}
}


// the setup function runs once when you press reset or power the board
void setup() {
	// Configuracion del timer a 1 kHz
	timer = timerBegin(0, 80, true);               // Frecuencia de reloj 80 MHz, prescaler de 80, frec 1 MHz
	timerAttachInterrupt(timer, &onTimer, true);
	timerAlarmWrite(timer, 1000, true);             // Interrupcion cada 1000 conteos del timer, es decir 100 Hz
	timerAlarmEnable(timer);                        // Habilita interrupcion por timer

	// configuracion del pin de interrupcion
	//pinMode(fan1.PIN, INPUT_PULLUP);
	//attachInterrupt(fan1.PIN, isr, RISING);

	// inicializacion de los pines controladores de las EV como salidas
	pinMode(EV_INSPIRA, OUTPUT);	// PIN 3   velocidad
	pinMode(EV_ESC_CAM, OUTPUT);	// PIN 6   velocidad
	pinMode(EV_ESPIRA, OUTPUT);		// PIN 12  velocidad
	pinMode(EV_IN_CAM, OUTPUT);		// PIN 5  velocidad
	pinMode(EV_IN_FLU, OUTPUT);		// PIN 5  velocidad

	pinMode(BATTALARM, INPUT);

	Serial.begin(115200);
	Serial2.begin(115200); // , SERIAL_8N1, RXD2, TXD2);

	EEPROM.begin(4);
	contCiclos = eeprom_wr_int();

	// inicializacion de los strings
	SERIALEQU = String("1234567890ABCD");
	patientPress = String("");
	circuitPress = String("");
	patientFlow = String("234.67");
	circuitFlow = String("245.67");
	inspTime = String("2.1");
	expiTime = String("3.5");
	frequency = String("20");
	volume = String("240");
	RaspberryChain = String("");
}

// the loop function runs over and over again until power down or reset
void loop() {

	// *************************************************
	// **** Atencion a rutina de interrupcion por timer
	// *************************************************
	if (flagTimerInterrupt) {
		portENTER_CRITICAL(&timerMux);
		flagTimerInterrupt = false;
		portEXIT_CRITICAL(&timerMux);

		cycling();
		receiveData();

		//Update data on LCD each 200ms
		contUpdateData++;
		if (contUpdateData >= 200) {
			contUpdateData = 0;
			sendSerialData();
		}

		//Write the EEPROM each 2 minuts
		contEscrituraEEPROM++;
		if (contEscrituraEEPROM > 3600000) {
			contEscrituraEEPROM = 0;
			eeprom_wr_int(contCiclos, 'w');
		}

		// *************************************************
		// **** Atencion a rutina de adquisicion ADC
		// *************************************************
		// Read the ADC BATTALARM and  50ms
		contADC++;
		if (contADC == 50) {
			contADC = 0;

			alerBateria = digitalRead(BATTALARM);

			ADC1_Value = analogRead(ADC_PRESS_1);
			ADC2_Value = analogRead(ADC_PRESS_2);
			ADC3_Value = analogRead(ADC_PRESS_3);
			// falta realizatr la conversion de adc a presion
			Pressure1 = AMP1 * float(ADC1_Value) + OFFS1;
			Pressure2 = AMP2 * float(ADC2_Value) + OFFS2;
			Pressure3 = AMP3 * float(ADC3_Value) + OFFS3;

			//Filtrado media movil
			//-Almacenamiento de valores
			Ppac[9] = Pressure1;
			Pin[9] = Pressure3;
			Pout[9] = Pressure2;
			//-Corrimiento inicial
			for (char i = 9; i >= 1; i--) {
				Pin[9 - i] = Pin[9 - i + 1];
				Pout[9 - i] = Pout[9 - i + 1];
				Ppac[9 - i] = Ppac[9 - i + 1];
			}
			//-Calculo promedio
			SPin = 0;
			SPout = 0;
			SPpac = 0;
			for (char i = 0; i <= 9; i++) {
				SPin = SPin + Pin[i];
				SPout = SPout + Pout[i];
				SPpac = SPpac + Ppac[i];
			}
			SPin = SPin / 10;
			SPout = SPout / 10;
			SPpac = SPpac / 10;
			//-Calculo Peep
			if (UmbralPeep > SPpac) {
				UmbralPeep = SPpac;
			}
			//-Calculo Ppico
			if (UmbralPpico < SPpac) {
				UmbralPpico = SPpac;
			}
			// conteo de los pulsos
			numeroPulsos = fan1.numberOfPulses;  //Numero de pulsos en 50ms
			fan1.numberOfPulses = 0;
			numeroPulsos = (numeroPulsos * 2000 / 75); //(#pulsos/1 s)=7.5*Q [L/min] (considera un decimal)

			patientPress = String(Pressure1);
			circuitPress = String(Pressure2);

			if (pruebaDato == 20) {
				pruebaDato = 0;
			}
			frequency = String(pruebaDato);
			volume = String(second);
			expiTime = String(inspirationTime * 1000);
			inspTime = String(expirationTime * 1000);

			pruebaDato++;

			// composicion de cadena
			RaspberryChain = SERIALEQU + ',' + patientPress + ',' + circuitPress + ',' + patientFlow + ',' + circuitFlow + ',' + inspTime + ',' + expiTime + ',' + frequency + ',' + volume;

			// Envio de la cadena de datos
	//		      Serial.print("SPin = ");
	//          Serial.print(SPin);
	//          Serial.print(", SPin_max = ");
	//          Serial.print(SPin_max);
	//          Serial.print(", SPin_min = ");
	//          Serial.print(SPin_min);          
	//          Serial.print(", SPout = ");
	//          Serial.print(SPout);  
	//          Serial.print(", SPout_max = ");
	//          Serial.print(SPout_max);
	//          Serial.print(", SPout_min = ");
	//          Serial.print(SPout_min);
			Serial.print(", SPpac = ");
			Serial.println(SPpac);
			//          Serial.print(", Peep = ");
			//          Serial.print(Peep);
			//          Serial.print(", Ppico = ");
			//          Serial.println(Ppico);
		}
	}
}

void IRAM_ATTR onTimer() {
	portENTER_CRITICAL_ISR(&timerMux);
	flagTimerInterrupt = true;
	portEXIT_CRITICAL_ISR(&timerMux);
}

// Cycling of the Mechanical Ventilator
void cycling() {
	contCycling++;
	if (contCycling == 1) {        // Inicia el ciclado abriendo electrovalvula de entrada y cerrando electrovalvula de salida
		digitalWrite(EV_INSPIRA, HIGH);   // turn the LED on (HIGH is the voltage level)
		digitalWrite(EV_ESC_CAM, LOW);   // turn the LED on (HIGH is the voltage level)
		digitalWrite(EV_ESPIRA, HIGH);   // turn the LED on (HIGH is the voltage level)
		digitalWrite(EV_IN_CAM, HIGH);   // turn the LED on (HIGH is the voltage level)
		//digitalWrite(EV_IN_FLU, LOW);   // turn the LED on (HIGH is the voltage level)
	}
	else if (contCycling == int(inspirationTime * 1000)) { // espera 1 segundo y cierra electrovalvula de entrada y abre electrovalvula de salida
	  //Calculos
		Pin_max = SPin;
		Pout_max = SPout;
		//
		digitalWrite(EV_INSPIRA, LOW);   // turn the LED on (HIGH is the voltage level)
		digitalWrite(EV_ESC_CAM, HIGH);    // turn the LED off by making the voltage LOW
		digitalWrite(EV_ESPIRA, LOW);    // turn the LED off by making the voltage LOW
		digitalWrite(EV_IN_CAM, LOW);   // turn the LED on (HIGH is the voltage level)
		//digitalWrite(EV_IN_FLU, HIGH);   // turn the LED on (HIGH is the voltage level)
	}
	else if (contCycling >= int(((inspirationTime + expirationTime) * 1000))) {
		contCycling = 0;
		//Calculos
		Pin_min = SPin;
		Pout_min = SPout;
		//
		if (BandGeneral == 1) {
			ContGeneral++;
		}
		//
		if (UmbralPeep > 4) {
			Peep = UmbralPeep + 5;
		}
		else {
			Peep = UmbralPeep + 2.5;
		}
		UmbralPeep = 100;

		if (UmbralPpico > 8) {
			Ppico = UmbralPpico + 5;
		}
		else {
			Ppico = UmbralPpico;
		}
		UmbralPpico = -100;
		//Calculo VT
		DifP = (Ppico - Peep);
		VT = DifP * 26.325 - 98.897;

		//-Almacenamiento de valores
		V_Ppico[4] = Ppico;
		V_Ppeep[4] = Peep;
		//-Corrimiento inicial
		for (char i = 4; i >= 1; i--) {
			V_Ppico[4 - i] = V_Ppico[4 - i + 1];
			V_Ppeep[4 - i] = V_Ppeep[4 - i + 1];
		}
		//-Calculo promedio
		SPpico = 0;
		SPpeep = 0;
		for (char i = 0; i <= 4; i++) {
			SPpico = SPpico + V_Ppico[i];
			SPpeep = SPpeep + V_Ppeep[i];
		}
		SPpico = SPpico / 5;
		SPpeep = SPpeep / 5;

		contCiclos++;
		//lcd_show();

		if (Ppico > 2 && Peep > 2) {
			flagInicio = false;
		}
		alarmsDetection();
	}

	milisecond++;
	if (milisecond == 1000) {
		milisecond = 0;
		second++;
		if (second == 60) {
			second = 0;
		}
	}
} // end cycling()

// Function to receive data from serial communication
void receiveData() {
	if (Serial2.available() > 5) {
		String dataIn = Serial2.readStringUntil(';');
		int contComas = 0;
		for (int i = 0; i < dataIn.length(); i++) {
			if (dataIn[i] == ',') {
				contComas++;
			}
		}
		String dataIn2[40];
		for (int i = 0; i < contComas + 1; i++) {
			dataIn2[i] = dataIn.substring(0, dataIn.indexOf(','));
			dataIn = dataIn.substring(dataIn.indexOf(',') + 1);
		}

		frecRespiratoria = dataIn2[0].toInt();
		I = dataIn2[1].toInt();
		E = dataIn2[2].toInt();
		maxPresion = dataIn2[3].toInt();
		Serial2.flush();

		Serial.println(String(frecRespiratoria) + ',' + String(I) + ',' + String(E) + ',' + String(maxPresion));
		/*for (int i = 0; i < contComas + 1; i++) {
			Serial.println(dataIn2[i]);
		}*/

		// Calculo del tiempo I:E
		if (I == 1) {
			inspirationTime = (60 / frecRespiratoria) / (1 + (float)(E / 10));
			expirationTime = (float)(E / 10) * inspirationTime;
		}
		else {
			expirationTime = (60 / frecRespiratoria) / (1 + (float)(I / 10));
			inspirationTime = (float)(I / 10) * expirationTime;
		}
		//Serial.println("I = " + String(inspirationTime) + " E = " + String(expirationTime));
	}
}

void sendSerialData() {
	String dataToSend = String(Ppico) + ',' + String(Peep) + ',' + "0" + ',' +
		String(alerPresionPIP) + ',' + String(alerDesconexion) + ',' +
		String(alerGeneral) + ',' + String(alerBateria) + ';';
	Serial2.print(dataToSend);
}

void alarmsDetection() {
	// Ppico Alarm
	if (flagInicio == false) {
		if (Ppico > maxPresion) {
			flagAlarmPpico = true;
			alerPresionPIP = 1;
		}
		else {
			flagAlarmPpico = false;
			alerPresionPIP = 0;

		}
		// Patient desconnection Alarm
		if (Ppico < 3 && Peep < 2) {
			flagAlarmPatientDesconnection = true;
			alerDesconexion = 1;
		}
		else {
			flagAlarmPatientDesconnection = false;
			alerDesconexion = 0;
		}
		// General Alarm (Obstrucci`n)
		if (Peep < 3) {
			BandGeneral = 1;
			if (ContGeneral >= 2) {
				flagAlarmGeneral = true;
				alerGeneral = 1;
				BandGeneral = 0;
				ContGeneral = 0;
			}
		}
		else {
			flagAlarmGeneral = false;
			alerGeneral = 0;
			ContGeneral = 0;
			BandGeneral = 0;
		}
	}
}

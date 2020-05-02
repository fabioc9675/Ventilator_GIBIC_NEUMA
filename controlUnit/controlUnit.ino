/*
 Name:		controlUnit.ino
 Created:	4/3/2020 17:28:49
 Author:	Helber Carvajal
*/
#include <math.h>
#include <EEPROM.h>

// Definiciones para controlar el shiel DFRobot quad motor driver
// Definiciones para controlar el shiel DFRobot quad motor driver
#define EV_INSPIRA   13  // Valvula 3/2 de control de la via inspiratoria (pin 3 del shield, velocidad motor 1)
#define EV_ESC_CAM   14  // Valvula 3/2 de activación de la camara (pin 6 del shield, velocidad motor 4)
#define EV_ESPIRA    12  // Valvula 3/2 de control de presiones PCON y PEEP (pin 11 del shield, velocidad motor 2)

// Definiciones para el manejo del ADC
#define ADC_PRESS_1     26  // Sensor de presión xx (pin ADC para presion 1)
#define ADC_PRESS_2     32  // Sensor de presión xx (pin ADC para presion 2)
#define ADC_PRESS_3     35  // Sensor de presión via aerea del paciente (pin ADC para presion 3)
#define ADC_FLOW_1      36  // Sensor de flujo linea xx (pin ADC para presion 2)
#define ADC_FLOW_2      39  // Sensor de flujo linea xx (pin ADC para presion 3)

// Calibracion de los sensores de presion - coeficientes regresion lineal
#define AMP1       0.0262
#define OFFS1      -15.092
#define AMP2       0.0293
#define OFFS2      -20.598
#define AMP3       0.0292
#define OFFS3      -21.429

// Variables de control del protocolo
#define RXD2 16
#define TXD2 17

// Variables y parametros de impresion en raspberry
String SERIALEQU;
String patientPress;
String patientFlow;
String patientVolume;
String pressPIP;
String pressPEEP;
String frequency;
String rInspir;
String rEspir;
String volumeT;
String alertPip;
String alertPeep;
String alertDiffPress;
String alertConnPat;
String alertLeak;
String alertConnEquipment;
String alertValve1Fail;
String alertValve2Fail;
String alertValve3Fail;
String alertValve4Fail;
String alertValve5Fail;
String alertValve6Fail;
String alertValve7Fail;
String alertValve8Fail;
String valve1Temp;
String valve2Temp;
String valve3Temp;
String valve4Temp;
String valve5Temp;
String valve6Temp;
String valve7Temp;
String valve8Temp;
String cameraPress;
String bagPress;
String inspFlow;
String inspExp;

//Variables para el envio de las alarmas
int alerPresionPIP = 0;
int alerDesconexion = 0;
int alerGeneral = 0;
int alerPeep = 0;
int alerBateria = 0;
int estabilidad = 0;
int PeepEstable = 0;

String RaspberryChain = "";

// Variables de manejo de ADC
volatile int contADC = 0;
volatile int contADCfast = 0;
bool fl_ADC = false;
int ADC1_Value = 0;
int ADC2_Value = 0;
int ADC3_Value = 0;
int ADC4_Value = 0;
int ADC5_Value = 0;
float Pressure1 = 0;
float Pressure2 = 0;
float Pressure3 = 0;
float flow1 = 0;
float flow2 = 0;


// Variables para la atencion de interrupciones
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

// State machine
#define CHECK_STATE		0
#define STANDBY_STATE	1
#define CYCLING_STATE	2
#define FAILURE_STATE	3
int currentStateMachine = STANDBY_STATE;
int newStateMachine = STANDBY_STATE;
int currentVentilationMode = 0;
int newVentilationMode = 0;


#define STOP_CYCLING			0
#define START_CYCLING			1
#define INSPIRATION_CYCLING		2
#define EXPIRATION_CYCLING		3
unsigned int currentStateMachineCycling = START_CYCLING;
//***********************************
// datos para prueba de transmision
int pruebaDato = 0;
int second = 0;
int milisecond = 0;
// *********************************

// Variables de calculo
//- Mediciones
float Peep = 0;
float Ppico = 0;
float Vtidal = 0;
float VT = 0;
volatile float inspirationTime = 1.666;
volatile float expirationTime = 3.333;
float SUMVin_Ins = 0;
float SUMVout_Ins = 0;
float SUMVin_Esp = 0;
float SUMVout_Esp = 0;
float Vin_Ins = 0;
float Vout_Ins = 0;
float Vin_Esp = 0;
float Vout_Esp = 0;

//- Señales
float SFin = 0; //Señal de flujo inspiratorio
float SFout = 0; //Señal de flujo espiratorio
float SPpac = 0; //Señal de presión en la via aerea del paciente
float SPin = 0; //Señal filtrada de presion en la camara
float SPout = 0; //Señal filtrada de presion en la bolsa

//- Filtrado
float Pin[40] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
float Pout[40] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
float Ppac[40] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
float Fin[40] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
float Fout[40] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

//- Mediciones derivadas
float UmbralPpmin = 100;
float UmbralPpico = -100;
float SFinMax = 50;
float SFoutMax = -50;
float SFinMaxInsp = 50;
float SFtotalMax = -50;
float Pin_max = 0;
float Pout_max = 0;
float Pin_min = 0;
float Pout_min = 0;
float pmin = 0;
float pmax = 0;
unsigned char BandInsp = 0;

unsigned char BandGeneral = 0;
unsigned char ContGeneral = 0;

// definiciones del timer
volatile int contCycling = 0;

bool flagInicio = true;

int currentFrecRespiratoria = 12;
int newFrecRespiratoria = currentFrecRespiratoria;
int currentI = 1;
int currentE = 20;
int newI = currentI;
int newE = currentE;
float relI = 0;
float relE = 0;
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

	// Configuración de los pines de conexion con del driver para manejo de electrovalvulas
	pinMode(2, OUTPUT);		// PIN 3   velocidad
	pinMode(4, OUTPUT);		// PIN 6   velocidad
	pinMode(5, OUTPUT);		// PIN 12  velocidad
	pinMode(12, OUTPUT);	// PIN 3   velocidad
	pinMode(13, OUTPUT);	// PIN 6   velocidad
	pinMode(14, OUTPUT);	// PIN 12  velocidad
	pinMode(15, OUTPUT);	// PIN 3   velocidad
	pinMode(18, OUTPUT);	// PIN 6   velocidad

	// inicializacion de los pines controladores de las EV como salidas
	pinMode(EV_INSPIRA, OUTPUT);	// PIN 3   velocidad
	pinMode(EV_ESC_CAM, OUTPUT);	// PIN 6   velocidad
	pinMode(EV_ESPIRA, OUTPUT);		// PIN 12  velocidad

	Serial.begin(115200);
	Serial2.begin(115200); // , SERIAL_8N1, RXD2, TXD2);
	Serial2.setTimeout(10);

	adcAttachPin(ADC_PRESS_1);
	adcAttachPin(ADC_PRESS_2);
	adcAttachPin(ADC_PRESS_3);
	adcAttachPin(ADC_FLOW_1);
	adcAttachPin(ADC_FLOW_2);

	EEPROM.begin(4);
	contCiclos = eeprom_wr_int();

	//Inicializacion de los strings comunicacion con la Raspberry
	SERIALEQU = String("1A");
	patientPress = String("");
	patientFlow = String("");
	patientVolume = String("");
	pressPIP = String("");
	pressPEEP = String("");
	frequency = String("");
	rInspir = String("");
	rEspir = String("");
	volumeT = String("");
	alertPip = String("");
	alertPeep = String("");
	alertDiffPress = String("");
	alertConnPat = String("");
	alertLeak = String("");
	alertConnEquipment = String("");
	alertValve1Fail = String("");
	alertValve2Fail = String("");
	alertValve3Fail = String("");
	alertValve4Fail = String("");
	alertValve5Fail = String("");
	alertValve6Fail = String("");
	alertValve7Fail = String("");
	alertValve8Fail = String("");
	valve1Temp = String("");
	valve2Temp = String("");
	valve3Temp = String("");
	valve4Temp = String("");
	valve5Temp = String("");
	valve6Temp = String("");
	valve7Temp = String("");
	valve8Temp = String("");
	cameraPress = String("");
	bagPress = String("");
	inspFlow = String("");
	inspExp = String("");

	RaspberryChain = String("");

	digitalWrite(2, LOW);	// PIN 3   velocidad
	digitalWrite(4, LOW);	// PIN 6   velocidad
	digitalWrite(5, LOW);   // PIN 12  velocidad
	digitalWrite(12, LOW);  // PIN 3   velocidad
	digitalWrite(13, LOW);  // PIN 6   velocidad
	digitalWrite(14, LOW);  // PIN 12  velocidad
	digitalWrite(15, LOW);  // PIN 3   velocidad
	digitalWrite(18, LOW);  // PIN 6   velocidad

	// Clean Serial buffers
	delay(1000);
	Serial.flush();
	Serial2.flush();
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

		receiveData();

		switch (currentStateMachine) {
		case CHECK_STATE:
			break;
		case STANDBY_STATE:
			adcReading();
			standbyRoutine();
			//Update data on LCD each 200ms
			contUpdateData++;
			if (contUpdateData >= 200) {
				contUpdateData = 0;
				sendSerialData();
			}
			//Serial.println("Standby state on control Unit");
			break;
		case CYCLING_STATE:
			//Serial.println("I am on CYCLING_STATE");
			cycling();
			adcReading();
			//Update data on LCD each 200ms
			contUpdateData++;
			if (contUpdateData >= 200) {
				contUpdateData = 0;
				sendSerialData();
			}
			//Write the EEPROM each 10 minuts
			contEscrituraEEPROM++;
			if (contEscrituraEEPROM > 3600000) {
				contEscrituraEEPROM = 0;
				eeprom_wr_int(contCiclos, 'w');
			}
			break;
		case FAILURE_STATE:
			break;
		default:
			break;
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

	if ((newFrecRespiratoria != currentFrecRespiratoria) ||
		(newI != currentI) || (newE != !currentE)) {
		currentFrecRespiratoria = newFrecRespiratoria;
		currentI = newI;
		currentE = newE;
		// Calculo del tiempo I:E
		if (currentI == 1) {
			inspirationTime = (float)(60.0 / currentFrecRespiratoria) / (1 + (float)(currentE / 10));
			expirationTime = (float)(currentE / 10) * inspirationTime;
		}
		else {
			expirationTime = (float)(60.0 / currentFrecRespiratoria) / (1 + (float)(currentI / 10));
			inspirationTime = (float)(currentI / 10) * expirationTime;
		}
	}

	switch (currentStateMachineCycling) {
	case STOP_CYCLING:
		break;
	case START_CYCLING:
		if (contCycling >= 1) {        // Inicia el ciclado abriendo electrovalvula de entrada y cerrando electrovalvula de salida
			BandInsp = 1;// Activa bandera que indica que empezo la inspiración
			digitalWrite(EV_INSPIRA, LOW);//Piloto conectado a ambiente -> Desbloquea valvula piloteada y permite el paso de aire
			digitalWrite(EV_ESPIRA, HIGH);//Piloto conectado a PIP -> Limita la presión de la via aerea a la PIP configurada
			digitalWrite(EV_ESC_CAM, HIGH);//Piloto conectado a Presión de activación -> Presiona la camara  
			currentStateMachineCycling = INSPIRATION_CYCLING;
		}//- Mitad de la inspiración
		else if (contCycling == int(((inspirationTime) * 1000) / 2)) {
			SFinMax = SFin;//
			SFoutMax = SFout;//
		}
		break;
	case INSPIRATION_CYCLING:
		if (contCycling >= int(inspirationTime * 1000)) {
			//Calculo PIP
			//Ppico = SPpac;//Detección de Ppico como la presión al final de la inspiración
			//Ppico = -0.0079 * (Ppico * Ppico) + 1.6493 * Ppico - 33.664;//Ajuste de Ppico
			if (Ppico < 0) {// Si el valor de Ppico es negativo
				Ppico = 0;// Lo limita a 0
			}
			Ppico = int(Ppico);

			//Mediciones de presion del sistema
			Pin_max = SPin;//Presión maxima de la camara
			Pout_max = SPout;//Presión maxima de la bolsa

			//Medicion de Volumen circulante
			VT = Vtidal;

			//Mediciones de flujo
			SFinMaxInsp = SFin;
			SFtotalMax = SFin - SFout;

			//Rutina de ciclado
			BandInsp = 0;	// Desactiva la bandera, indicando que empezo la espiración
			digitalWrite(EV_INSPIRA, HIGH);//Piloto conectado a presión de bloqueo -> Bloquea valvula piloteada y restringe el paso de aire
			digitalWrite(EV_ESC_CAM, LOW);//Piloto conectado a PEEP -> Limita la presión de la via aerea a la PEEP configurada
			digitalWrite(EV_ESPIRA, LOW);//Piloto conectado a ambiente -> Despresuriza la camara y permite el llenado de la bolsa
			currentStateMachineCycling = EXPIRATION_CYCLING;
		}
		break;
	case EXPIRATION_CYCLING:
		//Add para el modo A/C
		if (contCycling >= int(((inspirationTime + expirationTime) * 1000))) {
			contCycling = 0;
			//Calculo de Peep
			Peep = SPpac;// Peep como la presion en la via aerea al final de la espiración

			if (Peep < 0) {// Si el valor de Peep es negativo
				Peep = 0;// Lo limita a 0
			}
			Peep = int(Peep);
			if (estabilidad) {
				PeepEstable = Peep;
				estabilidad = 0;
			}
			else {
				if (Peep <= PeepEstable - 1.5) {
					alerPeep = 1;
				}
				else {
					alerPeep = 0;
				}
			}
			//Ajuste del valor de volumen
			Vtidal = 0;

			//Calculos de volumenes
			//- Asignación
			Vin_Ins = SUMVin_Ins / 1000;
			Vout_Ins = SUMVout_Ins / 1000;
			Vin_Esp = SUMVin_Esp / 1000;
			Vout_Esp = SUMVout_Esp / 1000;
			//- Reinio de acumuladores
			SUMVin_Ins = 0;
			SUMVout_Ins = 0;
			SUMVin_Esp = 0;
			SUMVout_Esp = 0;

			//Mediciones de presion del sistema
			Pin_min = SPin;//Presión minima de la camara
			Pout_min = SPout;//Presión minima de la bolsa

			//Asignación de valores maximos y minimos de presión      
			pmin = UmbralPpmin;//asigna la presion minima encontrada en todo el periodo      
			pmax = UmbralPpico;//asigna la presion maxima encontrada en todo el periodo
			Ppico = pmax;
			UmbralPpmin = 100;//Reinicia el umbral minimo de presion del paciente
			UmbralPpico = -100;//Reinicia el umbral maximo de presion del paciente

			//Metodo de exclusión de alarmas
			if (Ppico > 2 && Peep > 2) {
				flagInicio = false;
			}

			alarmsDetection();
			currentStateMachineCycling = START_CYCLING;

			if (newStateMachine != currentStateMachine) {
				currentStateMachine = newStateMachine;
			}
		}
		break;
	default:
		break;
	}

	//Calculo de Volumenes en tiempo inspiratorio y espiratorio 
	if (BandInsp == 1) {//Durante el tiempo inspiratorio    
		SUMVin_Ins = SUMVin_Ins + SFin;
		SUMVout_Ins = SUMVout_Ins + SFout;
	}
	else {//Durante el tiempo espiratorio
		SUMVin_Esp = SUMVin_Esp + SFin;
		SUMVout_Esp = SUMVin_Esp + SFout;
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
		//Serial.println(dataIn);
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
		//cargue los datos aqui
		//para entero
		//contCiclos =dataIn2[0].toInt();
		//para float
		newFrecRespiratoria = dataIn2[0].toInt();
		newI = dataIn2[1].toInt();
		newE = dataIn2[2].toInt();
		maxPresion = dataIn2[3].toInt();
		alerBateria = dataIn2[4].toInt();
		estabilidad = dataIn2[5].toInt();
		newStateMachine = dataIn2[6].toInt();
		newVentilationMode = dataIn2[7].toInt();

		Serial2.flush();

		/*Serial.println("State = " + String(currentStateMachine));
		Serial.println(String(newFrecRespiratoria) + ',' + String(newI) + ',' +
			String(newE) + ',' + String(maxPresion) + ',' +
			String(alerBateria) + ',' + String(estabilidad) + ',' +
			String(newStateMachine) + ',' + String(newVentilationMode));*/
		//Serial.println(String(alerPeep));
		/*for (int i = 0; i < contComas + 1; i++) {
			Serial.println(dataIn2[i]);
		  }*/

		  // Calculo del tiempo I:E
		if (newI == 1) {
			inspirationTime = (float)(60.0 / (float)newFrecRespiratoria) / (1 + (float)((float)newE / 10.0));
			expirationTime = (float)((float)newE / 10.0) * (float)inspirationTime;
			//Serial.println("IC=I = " + String(I) + ":" + String(E) + "-" + String(inspirationTime) + " E = " + String(expirationTime));
		}
		else {
			expirationTime = (float)(60.0 / (float)newFrecRespiratoria) / (1 + (float)((float)newI / 10.0));
			inspirationTime = (float)((float)newI / 10.0) * (float)expirationTime;
			//Serial.println("EC=I = " + String(I) + ":" + String(E) + "-" + String(inspirationTime) + " E = " + String(expirationTime));
		}
	}
}

// Read ADC BATTALARM (each 50ms)
void adcReading() {
	contADC++;
	contADCfast++;
	if (contADCfast == 3) {
		contADCfast = 0;

		// Lectura de valores ADC
		ADC4_Value = analogRead(ADC_FLOW_1);
		ADC5_Value = analogRead(ADC_FLOW_2);
		ADC1_Value = analogRead(ADC_PRESS_1);
		ADC2_Value = analogRead(ADC_PRESS_2);
		ADC3_Value = analogRead(ADC_PRESS_3);// ADC presión de la via aerea

		// Procesamiento señales
	//- Almacenamiento
		Fin[39] = ADC4_Value;
		Fout[39] = ADC5_Value;
		Ppac[39] = ADC3_Value;
		Pin[39] = ADC1_Value;
		Pout[39] = ADC2_Value;

		//- Corrimiento inicial
		for (int i = 39; i >= 1; i--) {
			Fin[39 - i] = Fin[39 - i + 1];
			Fout[39 - i] = Fout[39 - i + 1];
			Pin[39 - i] = Pin[39 - i + 1];
			Pout[39 - i] = Pout[39 - i + 1];
			Ppac[39 - i] = Ppac[39 - i + 1];
		}

		//- Inicialización
		SFin = 0;
		SFout = 0;
		SPin = 0;
		SPout = 0;
		SPpac = 0;

		//- Actualización
		for (int i = 0; i <= 39; i++) {
			SFin = SFin + Fin[i];
			SFout = SFout + Fout[i];
			SPin = SPin + Pin[i];
			SPout = SPout + Pout[i];
			SPpac = SPpac + Ppac[i];
		}

		//- Calculo promedio
		SFin = SFin / 40;
		SFout = SFout / 40;
		SPin = SPin / 40;
		SPout = SPout / 40;
		SPpac = SPpac / 40;

		//- Conversión ADC-Presión
		SPin = AMP1 * float(SPin) + OFFS1;
		SPout = AMP2 * float(SPout) + OFFS2;
		SPpac = AMP3 * float(SPpac) + OFFS3;// Presión de la via aerea

		// machetazo flujo
		SFin = float(SFin - 1695) / 10;
		SFout = float(SFout - 1768) / 10;
		//- machetazo flujo
		SFin = (SFin - 10);
		SFout = (SFout + 12);
		SFout = SFout * 2 + 3.5;

	}


	if (contADC == 50) {
		contADC = 0;

		// Calculo Presiones maximas y minimas en la via aerea
		if (UmbralPpmin > SPpac) {
			UmbralPpmin = SPpac;
		}
		if (UmbralPpico < SPpac) {
			UmbralPpico = SPpac;
		}

		// Calculo de relaciones I:E
		if (currentI != 1) {
			relI = (float)(currentI / 10.0);
		}
		else {
			relI = (float)(currentI);
		}
		if (currentE != 1) {
			relE = (float)(currentE / 10.0);
		}
		else {
			relE = (float)(currentE);
		}

		// Calculo de volumen circulante	
		Vtidal = Vtidal + (SFin - SFout - 10) * .5;

		// Transmicón serial
		//- Asignación de variables
		//if (alerDesconexion == 1) {
		//	patientPress = String(0);
		//}
		//else {
			patientPress = String(SPpac);
		//}
		patientFlow = String(SFin - SFout - 10);
		patientVolume = String(Vtidal);
		pressPIP = String(Ppico, 0);
		pressPEEP = String(Peep, 0);
		frequency = String(currentFrecRespiratoria);
		if (currentI == 1) {
			rInspir = String(relI, 0);
		}
		else {
			rInspir = String(relI, 1);
		}
		if (currentE == 1) {
			rEspir = String(relE, 0);
		}
		else {
			rEspir = String(relE, 1);
		}

		volumeT = String(VT, 0);
		alertPip = String(alerPresionPIP);

		alertPeep = String(alerGeneral);

		alertDiffPress = String(0);
		alertConnPat = String(alerDesconexion);
		alertLeak = String(0);
		alertConnEquipment = String(alerBateria);
		//alertConnEquipment = String(0);
		alertValve1Fail = String(0);
		alertValve2Fail = String(0);
		alertValve3Fail = String(0);
		alertValve4Fail = String(0);
		alertValve5Fail = String(0);
		alertValve6Fail = String(0);
		alertValve7Fail = String(0);
		alertValve8Fail = String(0);
		valve1Temp = String(25.1);
		valve2Temp = String(25.2);
		valve3Temp = String(25.3);
		valve4Temp = String(25.4);
		valve5Temp = String(24.1);
		valve6Temp = String(24.2);
		valve7Temp = String(24.3);
		valve8Temp = String(24.4);
		cameraPress = String(SPin);
		bagPress = String(SPout);
		inspFlow = String(SFin);
		inspExp = String(SFout);

		//- Composicion de cadena
		RaspberryChain = SERIALEQU + ',' + patientPress + ',' + patientFlow + ',' + patientVolume + ',' +
			pressPIP + ',' + pressPEEP + ',' + frequency + ',' + rInspir + ',' + rEspir + ',' + volumeT + ',' +
			alertPip + ',' + alertPeep + ',' + alertDiffPress + ',' + alertConnPat + ',' + alertLeak + ',' +
			alertConnEquipment + ',' + alertValve1Fail + ',' + alertValve2Fail + ',' + alertValve3Fail + ',' +
			alertValve4Fail + ',' + alertValve5Fail + ',' + alertValve6Fail + ',' + alertValve7Fail + ',' +
			alertValve8Fail + ',' + valve1Temp + ',' + valve2Temp + ',' + valve3Temp + ',' + valve4Temp + ',' +
			valve5Temp + ',' + valve6Temp + ',' + valve7Temp + ',' + valve8Temp + ',' + cameraPress + ',' + bagPress + ',' +
			inspFlow + ',' + inspExp;

		// Envio de la cadena de datos (visualizacion Raspberry)
		Serial.println(RaspberryChain);

		// Envio de la cadena de datos
	//    Serial.print(", Ppac = ");
	//    Serial.print(SPpac);
	//    Serial.print(", SUMVin_Ins = ");
	//    Serial.print(SUMVin_Ins);
	//    Serial.print(", SUMVout_Ins = ");
	//    Serial.print(SUMVout_Ins);
	//    Serial.print(", Vin_Ins = ");
	//    Serial.print(Vin_Ins);
	//    Serial.print(", Vout_Ins = ");
	//    Serial.print(Vout_Ins);
	//    Serial.print(", Vin_Esp = ");
	//    Serial.print(Vin_Esp);
	//    Serial.print(", Vout_Esp = ");
	//    Serial.println(Vout_Esp);
		//        Serial.print("Fin = ");
		//    Serial.print(SFin);
		//    Serial.print(", Fout = ");
		//    Serial.print(SFout);
		//    Serial.print("F = ");
		//    Serial.print(SFin-SFout-10);
		//    Serial.print(", V = ");
		//    Serial.println(Vtidal);
	}
}

void sendSerialData() {
	String dataToSend = String(Ppico) + ',' + String(Peep) + ',' + String(VT) + ',' +
		String(alerPresionPIP) + ',' + String(alerDesconexion) + ',' +
		String(alerGeneral) + ',' + String(estabilidad)+ ';';
	Serial2.print(dataToSend);
}

void alarmsDetection() {
	// Ppico Alarm
	if (flagInicio == false) {//Si hay habilitación de alarmas
	// Alarma por Ppico elevada
		if (Ppico > maxPresion) {
			flagAlarmPpico = true;
			alerPresionPIP = 1;
		}
		else {
			flagAlarmPpico = false;
			alerPresionPIP = 0;
		}

		// Alarma por desconexión del paciente
		if ((SFinMax - SFtotalMax) <= 10) {
			flagAlarmPatientDesconnection = true;
			alerDesconexion = 1;
		}
		else {
			flagAlarmPatientDesconnection = false;
			alerDesconexion = 0;
		}

		// Alarma por obstrucción
		if ((SFinMax - SFoutMax) <= 5) {
			flagAlarmGeneral = true;
			alerGeneral = 1;
		}
		else {
			flagAlarmGeneral = false;
			alerGeneral = 0;
		}

		SFinMax = SFin;
		SFoutMax = SFout;
	}
}

void standbyRoutine() {
	if (newStateMachine != currentStateMachine) {
		currentStateMachine = newStateMachine;
	}
	contCycling = 0;
	digitalWrite(EV_INSPIRA, LOW);//Piloto conectado a presión de bloqueo -> Bloquea valvula piloteada y restringe el paso de aire
	digitalWrite(EV_ESC_CAM, LOW);//Piloto conectado a PEEP -> Limita la presión de la via aerea a la PEEP configurada
	digitalWrite(EV_ESPIRA, LOW);//Piloto conectado a ambiente -> Despresuriza la camara y permite el llenado de la bolsa
}
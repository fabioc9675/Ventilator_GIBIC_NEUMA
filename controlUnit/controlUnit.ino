/*
  Name:     controlUnit.ino
  Created:  6/5/2020 11:40:24
  Author:   Helber Carvajal
*/

#include <Arduino.h>
#include <Esp.h>
#include <math.h>
#include <EEPROM.h>
#include <nvs_flash.h>
#include <stdio.h>

//********DEFINICIONES CONDICIONES******
#define TRUE          1
#define FALSE         0

//********DEFINICION DE VERSION*********
#define VERSION_1_0       TRUE

// #define SERIAL_DEVICE     "9GF100007LJD00004"
// #define SERIAL_DEVICE     "9GF100007LJD00005"

#define SERIAL_DEVICE     "1NEUMA0005"

//********COMPILACION CONDICIONAL*******
#ifdef VERSION_1_0

// Definiciones para controlar el shiel DFRobot quad motor driver
// Definiciones para controlar el shiel DFRobot quad motor driver
#define EV_INSPIRA      13  // out 7 // Valvula 3/2 de control de la via inspiratoria (pin 3 del shield, velocidad motor 1)
#define EV_ESC_CAM      14  // out 6 // Valvula 3/2 de activacion de la camara (pin 6 del shield, velocidad motor 4)
#define EV_ESPIRA       12  // out 8 // Valvula 3/2 de control de presiones PCON y PEEP (pin 11 del shield, velocidad motor 2)

// Definiciones para el manejo del ADC
#define ADC_PRESS_1     26  // ADC 7 // Sensor de presion xx (pin ADC para presion 1)
#define ADC_PRESS_2     35  // ADC 4 // Sensor de presion xx (pin ADC para presion 2)
#define ADC_PRESS_3     33  // ADC 6 // Sensor de presion via aerea del paciente (pin ADC para presion 3)
#define ADC_FLOW_1      39  // ADC 1 // Sensor de flujo linea xx (pin ADC para presion 2)
#define ADC_FLOW_2      36  // ADC 2 // Sensor de flujo linea xx (pin ADC para presion 3)

#elif VERSION_1_1

// Definiciones para controlar el shiel DFRobot quad motor driver
// Definiciones para controlar el shiel DFRobot quad motor driver
#define EV_INSPIRA      5   // out 3 // Valvula 3/2 de control de la via inspiratoria (pin 3 del shield, velocidad motor 1)
#define EV_ESPIRA       4  // out 2 // Valvula 3/2 de control de presiones PCON y PEEP (pin 11 del shield, velocidad motor 2)
#define EV_ESC_CAM      18   // out 1 // Valvula 3/2 de activaci�n de la camara (pin 6 del shield, velocidad motor 4)

// Definiciones para el manejo del ADC
#define ADC_PRESS_1     33  // ADC 6 // Sensor de presion xx (pin ADC para presion 1)
#define ADC_PRESS_2     32  // ADC 5 // Sensor de presion xx (pin ADC para presion 2)
#define ADC_PRESS_3     34  // ADC 4 // Sensor de presion via aerea del paciente (pin ADC para presion 3)
#define ADC_FLOW_1      36  // ADC 1 // Sensor de flujo linea xx (pin ADC para presion 2)
#define ADC_FLOW_2      39  // ADC 2 // Sensor de flujo linea xx (pin ADC para presion 3)

#endif
// Calibracion de los sensores de presion - coeficientes regresion lineal
#define AMP1          0.027692
#define OFFS1         -22.4863
#define AMP2          0.027692
#define OFFS2         -22.4863
#define AMP3          0.028396
#define OFFS3         -20.0500


// Calibracion de los sensores de flujo - coeficientes regresion lineal
// Sensor de flujo Inspiratorio
#define AMP_FI_1      0.141500         
#define OFFS_FI_1     -244.049400         
#define LIM_FI_1      1619         
#define AMP_FI_2      0.622700         
#define OFFS_FI_2     -1023.119600         
#define LIM_FI_2      1667         
#define AMP_FI_3      0.141500         
#define OFFS_FI_3     -220.865500         

// Sensor de flujo Espiratorio
#define AMP_FE_1      0.117800         
#define OFFS_FE_1     -214.996900         
#define LIM_FE_1      1698         
#define AMP_FE_2      0.858600         
#define OFFS_FE_2     -1473.279900         
#define LIM_FE_2      1733         
#define AMP_FE_3      0.117800         
#define OFFS_FE_3     -189.111300     


// variable para ajustar el nivel cero de flujo y calcular el volumen
#define FLOWUP_LIM        3
#define FLOWLO_LIM        -3
#define FLOW_CONV         16.666666    // conversion de L/min a mL/second
#define DELTA_T           0.003 // 0.05         // delta de tiempo para realizar la integra
#define VOL_SCALE         1.00         // Factor de escala para ajustar el volumen


#define ADC_FAST          3  // muestreo cada 3 ms
#define ADC_SLOW          50  // muestreo cada 50 ms

// **********************************************************
// Calibracion sensores Sitio
// Calibracion de los sensores de presion - coeficientes regresion lineal
#define AMP1_SITE          1.00
#define OFFS1_SITE         0.00
#define AMP2_SITE          1.00
#define OFFS2_SITE         0.00
#define AMP3_SITE          1.00
#define OFFS3_SITE         0.00

// Calibracion de los sensores de flujo - coeficientes regresion lineal
// Sensor de flujo Inspiratorio
#define AMP_FI_1_SITE      1.00         
#define OFFS_FI_1_SITE     0.00         
#define LIM_FI_1_SITE      1702         
#define AMP_FI_2_SITE      1.00         
#define OFFS_FI_2_SITE     0.00         
#define LIM_FI_2_SITE      1764         
#define AMP_FI_3_SITE      1.00         
#define OFFS_FI_3_SITE     0.00         

// Sensor de flujo Espiratorio
#define AMP_FE_1_SITE      1.00         
#define OFFS_FE_1_SITE     0.00         
#define LIM_FE_1_SITE      1706         
#define AMP_FE_2_SITE      1.00         
#define OFFS_FE_2_SITE     0.00         
#define LIM_FE_2_SITE      1749         
#define AMP_FE_3_SITE      1.00        
#define OFFS_FE_3_SITE     0.00    

// Variables de control del protocolo
#define RXD2 16
#define TXD2 17

// Definiciones para State machine
#define CHECK_STATE         0
#define STANDBY_STATE       1
#define PCMV_STATE          2
#define AC_STATE            3
#define CPAP_STATE          4
#define FAILURE_STATE       5

// Definiciones para ciclado en mode C-PMV
#define STOP_CYCLING          0
#define START_CYCLING         1
#define INSPIRATION_CYCLING   2
#define EXPIRATION_CYCLING    3

// Definiciones para ciclado en mode CPAP
#define COMP_FLOW_MAX_CPAP             2.5  // cambiado desde 3, variable para comparacion de flujo y entrar en modo Inspiratorio en CPAP
#define COMP_FLOW_MIN_CPAP            -2.5  // cambiado desde 3, variable para comparacion de flujo y entrar en modo Inspiratorio en CPAP
#define COMP_DEL_F_MAX_CPAP            1.5  // cambiado desde 2, variable para comparacion de flujo y entrar en modo Inspiratorio en CPAP
#define COMP_DEL_F_MIN_CPAP           -1.5  // cambiado desde 2, variable para comparacion de flujo y entrar en modo Inspiratorio en CPAP
#define CPAP_NONE                      0  // Estado de inicializacion
#define CPAP_INIT                      1  // Estado de inicio de CPAP
#define CPAP_INSPIRATION               2  // Entra en modo inspiratorio
#define CPAP_ESPIRATION                3  // Entra en modo espiratorio

#define DERIVATE_UP_THRESHOLD          0.2   // umbral para definir el valor maximo en estabilidad
#define DERIVATE_DO_THRESHOLD          -0.2  // umbral para definir el valor minimo en estabilidad
#define DERIVATE_LO_THRESHOLD          -0.3    // nivel para deteccion de cambio rapido



//creo el manejador para el semaforo como variable global
SemaphoreHandle_t xSemaphoreTimer = NULL;
SemaphoreHandle_t xSemaphoreAdc = NULL;
SemaphoreHandle_t xSemaphoreRaspberry = NULL;
//xQueueHandle timer_queue = NULL;

// definicion de los core para ejecucion
static uint8_t taskCoreZero = 0;
static uint8_t taskCoreOne = 1;

// Variables y parametros de impresion en raspberry
String idEqupiment;
String patientPress;
String patientFlow;
String patientVolume;
String pressPIP;
String pressPEEP;
String frequency;
String xSpeed;
String rInspir;
String rEspir;
String volumeT;
String alertPip;
String alertPeep;
String alertObstruction;
String alertConnPat;
String alertGeneralFailure;
String alertConnEquipment;
String alertFrequency;
String alertMinuteVentilation;
String alertValve1Fail;
String alertValve2Fail;
String alertValve3Fail;
String valve1Temp;
String valve2Temp;
String valve3Temp;
String valve1Current;
String valve2Current;
String valve3Current;
String source5v0Voltage;
String source5v0Current;
String source5v0SWVoltage;
String source5v0SWCurrent;
String cameraPress;
String bagPress;
String inspFlow;
String EspFlow;
String lPresSup;
String lPresInf;
String lFlowSup;
String lFlowInf;
String lVoluSup;
String lVoluInf;


String alertValve4Fail;
String alertValve5Fail;
String alertValve6Fail;
String alertValve7Fail;
String alertValve8Fail;
String valve4Temp;
String valve5Temp;
String valve6Temp;
String valve7Temp;
String valve8Temp;

// Cadena de impresion en raspberry
String RaspberryChain = "";
unsigned int contSendData = 0;

// contadores de tiempo
int second = 0;
int milisecond = 0;

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
float flowZero = 0;
float flowTotalV = 0;
float flowTotalC = 0;

// Limites de presion, flujo y volumen para las graficas
float lMaxPres;
float lMinPres;
float lMaxFlow;
float lMinFlow;
float lMaxVolu;
float lMinVolu;

// Variables de maquinas de estado
unsigned int currentStateMachineCycling = START_CYCLING;
int currentStateMachine = STANDBY_STATE;
int newStateMachine = STANDBY_STATE;
int currentVentilationMode = 0;
int newVentilationMode = 0;
int newTrigger = 2;
int newPeepMax = 5;
int maxFR = 30;
int maxVE = 30;
int apneaTime = 20;
int minFR = 4;
byte AC_stateMachine = 0;

// variables para calibracion de sensores
float CalFin = 0;	// almacena valor ADC para calibracion
float CalFout = 0;	// almacena valor ADC para calibracion
float CalPpac = 0;	// almacena valor ADC para calibracion
float CalPin = 0;	// almacena valor ADC para calibracion
float CalPout = 0;	// almacena valor ADC para calibracion

//- Senales
float SFin = 0;		//Senal de flujo inspiratorio
float SFout = 0;	//Senal de flujo espiratorio

float SPpac = 0;	// Senal de presion en la via aerea del paciente
float SPpac0 = 0;
float SPpac1 = 0;
float SPpac2 = 0;
float SPpac3 = 0;
float SPpac4 = 0;
float dPpac = 0;	// Derivada de SPpac
float SFpac = 0;	// Senal de flujo del paciente
float SPin = 0;		// Senal filtrada de presion en la camara
float SPout = 0;	// Senal filtrada de presion en la bolsa
float SVtidal = 0;	// informacion de promedio para Vtidal
float Sfrec = 0;	// informacion de promedio para frecuencia

float SFinADC = 0; //Senal de flujo inspiratorio
float SFoutADC = 0; //Senal de flujo espiratorio
float SPpacADC = 0; //Senal de presion en la via aerea del paciente
float SPinADC = 0; //Senal filtrada de presion en la camara
float SPoutADC = 0; //Senal filtrada de presion en la bolsa
float SFinFACTORY = 0; //Senal de flujo inspiratorio
float SFoutFACTORY = 0; //Senal de flujo espiratorio
float SPpacFACTORY = 0; //Senal de presion en la via aerea del paciente
float SPinFACTORY = 0; //Senal filtrada de presion en la camara
float SPoutFACTORY = 0; //Senal filtrada de presion en la bolsa

//- Filtrado
float PinADC[40] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
float PoutADC[40] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
float PpacADC[40] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
float FinADC[40] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
float FoutADC[40] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
float VTidProm[3] = { 0, 0, 0 };
float FreqProm[3] = { 0, 0, 0 };
float FPacProm[40];

#define L_F_PROM      40

// Variables de ventilacion umbrales
int currentFrecRespiratoria = 12;
int currentI = 1;
int currentE = 20;
float relI = 0;
float relE = 0;
volatile float inspirationTime = 1.666;
volatile float expirationTime = 3.333;
int frecRespiratoriaCalculada = 0;
int calculatedE = currentE;
int calculatedI = currentI;
int currentVE = 0;
int maxPresion = 30;
//- Mediciones derivadas
float UmbralPpmin = 100;
float UmbralPpico = -100;
float UmbralPpicoDistal = -100;
float UmbralFmin = 100;
float UmbralFmax = -100;
float UmbralVmin = 100;
float UmbralVmax = -100;
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
float flmin = 0;
float flmax = 0;
float vmin = 0;
float vmax = 0;
unsigned char BandInsp = 0;
unsigned char BandGeneral = 0;
unsigned char ContGeneral = 0;

// Variables recibidas en el Serial
int newFrecRespiratoria = currentFrecRespiratoria;
int newI = currentI;
int newE = currentE;

// mediciones
float Peep = 0;
float Peep_AC = 0;
float Ppico = 0;
float PeepProximal = 0;   // medicion realizada con sensor distal a paciente
float PpicoProximal = 0;  // medicion realizada con sensor distal a paciente
float PeepDistal = 0;   // medicion realizada con sensor distal a paciente
float PpicoDistal = 0;  // medicion realizada con sensor distal a paciente
float VtidalV = 0;
float VtidalC = 0;
float SFpacV = 0;       // senal de flujo para visualizacion
float VT = 0;
float SUMVin_Ins = 0;
float SUMVout_Ins = 0;
float SUMVin_Esp = 0;
float SUMVout_Esp = 0;
float Vin_Ins = 0;
float Vout_Ins = 0;
float Vin_Esp = 0;
float Vout_Esp = 0;

// variables para calculo de frecuencia y relacion IE en CPAP
float SFant = 0;
float dFlow = 0;
int stateFrecCPAP = 0;
int contFrecCPAP = 0;
int contEspCPAP = 0;
int contInsCPAP = 0;

// Variables para el envio y recepcion de alarmas
int alerPresionPIP = 0;
int alerDesconexion = 0;
int alerObstruccion = 0;
int alerPeep = 0;
int alerBateria = 0;
int alerGeneral = 0;
int alerFR_Alta = 0;
int alerVE_Alto = 0;
int estabilidad = 0;
int PeepEstable = 0;

// variables de atencion a interrupcion
volatile uint8_t flagInicio = true;
volatile uint8_t flagTimerInterrupt = false;
volatile uint8_t flagAdcInterrupt = false;
volatile uint8_t flagAlarmPpico = false;
uint8_t flagMaxPresion = false;
volatile uint8_t flagAlarmGeneral = false;
volatile uint8_t flagAlarmPatientDesconnection = false;
volatile uint8_t flagAlarmObstruccion = false;
volatile uint8_t flagAlarmFR_Alta = false;
volatile uint8_t flagAlarmVE_Alto = false;
volatile uint8_t flagStandbyInterrupt = false;
uint8_t flagAC = false;

// variables contadores
volatile unsigned int contDetach = 0;
unsigned int contCiclos = 0;
unsigned int contEscrituraEEPROM = 0;
unsigned int contUpdateData = 0;
unsigned int contStandby = 0;
volatile int contCycling = 0;

// inicializacion del contador del timer
hw_timer_t* timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

// definicion de interrupciones
void IRAM_ATTR onTimer(void);  // funcion de interrupcion

/* *********************************************************************
 * **** FUNCIONES DE INICIALIZACION MEMORIA ****************************
 * *********************************************************************/
int eeprom_wr_int(int dataIn = 0, char process = 'r') {
	int dataRead = 0;
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
		return dataRead;
	}
	else if (process == 'r') {
		dataRead = EEPROM.read(0);
		dataRead = (EEPROM.read(1) << 8) + dataRead;
		dataRead = (EEPROM.read(2) << 16) + dataRead;
		dataRead = (EEPROM.read(3) << 24) + dataRead;
		return dataRead;
	}
	return dataRead;
}

/************************************************************
 ********** FUNCIONES DE INICIALIZACION *********************
 ***********************************************************/
void init_GPIO() {
	// Configuracion de los pines de conexion con del driver para manejo de electrovalvulas
	pinMode(2, OUTPUT);   // PIN 3   velocidad
	pinMode(4, OUTPUT);   // PIN 6   velocidad
	pinMode(5, OUTPUT);   // PIN 12  velocidad
	pinMode(12, OUTPUT);  // PIN 3   velocidad
	pinMode(13, OUTPUT);  // PIN 6   velocidad
	pinMode(14, OUTPUT);  // PIN 12  velocidad
	pinMode(15, OUTPUT);  // PIN 3   velocidad
	pinMode(18, OUTPUT);  // PIN 6   velocidad

	// inicializacion de los pines controladores de las EV como salidas
	pinMode(EV_INSPIRA, OUTPUT);  // PIN 3   velocidad
	pinMode(EV_ESC_CAM, OUTPUT);  // PIN 6   velocidad
	pinMode(EV_ESPIRA, OUTPUT);   // PIN 12  velocidad

	// Inicializacion de los pines de ADC para conversion Analogo-digitalPinToInterrupt
	adcAttachPin(ADC_PRESS_1);
	adcAttachPin(ADC_PRESS_2);
	adcAttachPin(ADC_PRESS_3);
	adcAttachPin(ADC_FLOW_1);
	adcAttachPin(ADC_FLOW_2);

	// desactiva todas las salidas de electrovalvulas
	digitalWrite(2, LOW); // PIN 3   velocidad
	digitalWrite(4, LOW); // PIN 6   velocidad
	digitalWrite(5, LOW);   // PIN 12  velocidad
	digitalWrite(12, LOW);  // PIN 3   velocidad
	digitalWrite(13, LOW);  // PIN 6   velocidad
	digitalWrite(14, LOW);  // PIN 12  velocidad
	digitalWrite(15, LOW);  // PIN 3   velocidad
	digitalWrite(18, LOW);  // PIN 6   velocidad

}

void init_TIMER() {
	// Configuracion del timer a 1 kHz
	timer = timerBegin(0, 80, true);                // Frecuencia de reloj 80 MHz, prescaler de 80, frec 1 MHz
	timerAttachInterrupt(timer, &onTimer, true);    // Attach onTimer function to our timer
	timerAlarmWrite(timer, 1000, true);             // Interrupcion cada 1000 conteos del timer, es decir 100 Hz
	timerAlarmEnable(timer);                        // Habilita interrupcion por timer
}

void init_MEMORY() {
	// Inicializacion y consulta de la memoria EEPROM
	EEPROM.begin(512);
	contCiclos = eeprom_wr_int();

	//Inicializacion de los strings comunicacion con la Raspberry
	idEqupiment = String(SERIAL_DEVICE);
	patientPress = String("");
	patientFlow = String("");
	patientVolume = String("");
	pressPIP = String("");
	pressPEEP = String("");
	frequency = String("");
	xSpeed = String("");
	rInspir = String("");
	rEspir = String("");
	volumeT = String("");
	alertPip = String("");
	alertPeep = String("");
	alertObstruction = String("");
	alertConnPat = String("");
	alertGeneralFailure = String("");
	alertConnEquipment = String("");
	alertFrequency = String("");
	alertMinuteVentilation = String("");
	alertValve1Fail = String("");
	alertValve2Fail = String("");
	alertValve3Fail = String("");
	valve1Temp = String("");
	valve2Temp = String("");
	valve3Temp = String("");
	valve1Current = String("");
	valve2Current = String("");
	valve3Current = String("");
	source5v0Voltage = String("");
	source5v0Current = String("");
	source5v0SWVoltage = String("");
	source5v0SWCurrent = String("");
	cameraPress = String("");
	bagPress = String("");
	inspFlow = String("");
	EspFlow = String("");
	lPresSup = String("");
	lPresInf = String("");
	lFlowSup = String("");
	lFlowInf = String("");
	lVoluSup = String("");
	lVoluInf = String("");

	RaspberryChain = String("");

}

/* *********************************************************************
 * **** FUNCIONES DE ATENCION A INTERRUPCION ***************************
 * *********************************************************************/
 // Interrupcion por timer
void IRAM_ATTR onTimer(void) {
	portENTER_CRITICAL_ISR(&timerMux);
	flagTimerInterrupt = true;   // asignacion de banderas para atencion de interrupcion
	flagAdcInterrupt = true;
	xSemaphoreGiveFromISR(xSemaphoreTimer, NULL);  // asignacion y liberacion de semaforos
	xSemaphoreGiveFromISR(xSemaphoreAdc, NULL);
	portEXIT_CRITICAL_ISR(&timerMux);
}

/************************************************************
 ***** FUNCIONES DE ATENCION A INTERRUPCION TAREA TIMER *****
 ************************************************************/
void task_Timer(void* arg) {

	while (1) {
		// Se atiende la interrpcion del timer
		if (xSemaphoreTake(xSemaphoreTimer, portMAX_DELAY) == pdTRUE) {
			if (flagTimerInterrupt == true) {
				portENTER_CRITICAL(&timerMux);
				flagTimerInterrupt = false;
				portEXIT_CRITICAL(&timerMux);

				/* *************************************************************
				 * **** SECUENCIA DE FUNCIONAMIENTO, ESTADOS DEL VENTILADOR ****
				 * *************************************************************/
				switch (currentStateMachine) {
				case CHECK_STATE:		// Estado de checkeo
					break;
				case STANDBY_STATE:		// Modo StandBy
					standbyRoutine();
					//Serial.println("Standby state on control Unit");
					break;
				case PCMV_STATE:		// Modo Controlado por presion
					cycling();
					
					// Write the EEPROM each 10 minutes
					contEscrituraEEPROM++;
					if (contEscrituraEEPROM > 3600000) {
						contEscrituraEEPROM = 0;
						eeprom_wr_int(contCiclos, 'w');
					}
					//Serial.println("I am on PCMV_STATE");
					break;
				case AC_STATE:  // Modo asistido controlado por presion
					cycling();
					// Write the EEPROM each 10 minutes
					contEscrituraEEPROM++;
					if (contEscrituraEEPROM > 3600000) {
						contEscrituraEEPROM = 0;
						eeprom_wr_int(contCiclos, 'w');
					}
					//Serial.println("I am on AC_STATE");
					break;
				case CPAP_STATE:  // Modo CPAP
					cpapRoutine();
					//Serial.println("I am on CPAP_STATE");
					break;
				case FAILURE_STATE:
					currentStateMachine = STANDBY_STATE;
					//Serial.println("I am on FAILURE_STATE");
					break;
				default:
					currentStateMachine = STANDBY_STATE;
					//Serial.println("I am on DEFAULT_STATE");
					break;
				}
			}
		}
	}
	vTaskDelete(NULL);
}

/************************************************************
 ***** FUNCIONES DE ATENCION A INTERRUPCION TAREA ADC *******
 ************************************************************/
void task_Adc(void* arg) {
	while (1) {
		// Se atiende la interrpcion del timer
		if (xSemaphoreTake(xSemaphoreAdc, portMAX_DELAY) == pdTRUE) {
			if (flagAdcInterrupt == true) {
				portENTER_CRITICAL(&timerMux);
				flagAdcInterrupt = false;
				portEXIT_CRITICAL(&timerMux);

				/* *************************************************************
				* **** SECUENCIA DE MEDICION, ADQUISICION DE VARIABLES *********
				* *************************************************************/
				contADC++;
				contADCfast++;
				// muestreo rapido de ADC
				if (contADCfast == ADC_FAST) {
					contADCfast = 0;

					// Lectura de valores ADC
					ADC4_Value = analogRead(ADC_FLOW_1);  // ADC flujo de entrada
					ADC5_Value = analogRead(ADC_FLOW_2);  // ADC flujo de salida
					ADC1_Value = analogRead(ADC_PRESS_1);  // ADC presion camara
					ADC2_Value = analogRead(ADC_PRESS_2);  // ADC presion equipo
					ADC3_Value = analogRead(ADC_PRESS_3);// ADC presion de la via aerea

					// Procesamiento senales
					//- Almacenamiento
					FinADC[39] = ADC4_Value;
					FoutADC[39] = ADC5_Value;
					PpacADC[39] = ADC3_Value;
					PinADC[39] = ADC1_Value;
					PoutADC[39] = ADC2_Value;

					//- Corrimiento inicial
					for (int i = 39; i >= 1; i--) {
						FinADC[39 - i] = FinADC[39 - i + 1];
						FoutADC[39 - i] = FoutADC[39 - i + 1];
						PinADC[39 - i] = PinADC[39 - i + 1];
						PoutADC[39 - i] = PoutADC[39 - i + 1];
						PpacADC[39 - i] = PpacADC[39 - i + 1];
					}

					//- Inicializacion
					SFinADC = 0;
					SFoutADC = 0;
					SPinADC = 0;
					SPoutADC = 0;
					SPpacADC = 0;

					//- Actualizacion
					for (int i = 0; i <= 39; i++) {
						SFinADC = SFinADC + FinADC[i];
						SFoutADC = SFoutADC + FoutADC[i];
						SPinADC = SPinADC + PinADC[i];
						SPoutADC = SPoutADC + PoutADC[i];
						SPpacADC = SPpacADC + PpacADC[i];
					}

					//- Calculo promedio
					SFinADC = SFinADC / 40;
					SFoutADC = SFoutADC / 40;
					SPinADC = SPinADC / 40;
					SPoutADC = SPoutADC / 40;
					SPpacADC = SPpacADC / 40;

					// Actualizacion de valores para realizar calibracion
					CalFin = SFinADC;
					CalFout = SFoutADC;
					CalPpac = SPpacADC;
					CalPin = SPinADC;
					CalPout = SPoutADC;

					//- Conversion ADC-Presion de fabrica
					SPinFACTORY = AMP1 * float(SPinADC) + OFFS1;
					SPoutFACTORY = AMP2 * float(SPoutADC) + OFFS2;
					SPpacFACTORY = AMP3 * float(SPpacADC) + OFFS3;// Presion de la via aerea

					// Conversion ADC Flujo Inspiratorio de fabrica, ajuste por tramos para linealizacion
					if (SFinADC <= LIM_FI_1) {
						SFinFACTORY = AMP_FI_1 * float(SFinADC) + OFFS_FI_1;
					}
					else if (SFinADC <= LIM_FI_2) {
						SFinFACTORY = AMP_FI_2 * float(SFinADC) + OFFS_FI_2;
					}
					else {
						SFinFACTORY = AMP_FI_3 * float(SFinADC) + OFFS_FI_3;
					}

					// Conversion ADC Flujo Espiratorio de fabrica, ajuste por tramos para linealizacion
					if (SFoutADC <= LIM_FE_1) {
						SFoutFACTORY = AMP_FE_1 * float(SFoutADC) + OFFS_FE_1;
					}
					else if (SFoutADC <= LIM_FE_2) {
						SFoutFACTORY = AMP_FE_2 * float(SFoutADC) + OFFS_FE_2;
					}
					else {
						SFoutFACTORY = AMP_FE_3 * float(SFoutADC) + OFFS_FE_3;
					}

					// *********************************************************
					// Conversion de valores de fabrica a valores de sitio 
					//- Conversion ADC-Presion de fabrica
					SPin = AMP1_SITE * float(SPinFACTORY) + OFFS1_SITE;
					SPout = AMP2_SITE * float(SPoutFACTORY) + OFFS2_SITE;
					SPpac = AMP3_SITE * float(SPpacFACTORY) + OFFS3_SITE;// Presion de la via aerea

					// Conversion ADC Flujo Inspiratorio de fabrica, ajuste por tramos para linealizacion
					if (SFinADC <= LIM_FI_1_SITE) {
						SFin = AMP_FI_1_SITE * float(SFinFACTORY) + OFFS_FI_1_SITE;
					}
					else if (SFinADC <= LIM_FI_2_SITE) {
						SFin = AMP_FI_2_SITE * float(SFinFACTORY) + OFFS_FI_2_SITE;
					}
					else {
						SFin = AMP_FI_3_SITE * float(SFinFACTORY) + OFFS_FI_3_SITE;
					}

					// Conversion ADC Flujo Espiratorio de fabrica, ajuste por tramos para linealizacion
					if (SFoutADC <= LIM_FE_1_SITE) {
						SFout = AMP_FE_1_SITE * float(SFoutFACTORY) + OFFS_FE_1_SITE;
					}
					else if (SFoutADC <= LIM_FE_2_SITE) {
						SFout = AMP_FE_2_SITE * float(SFoutFACTORY) + OFFS_FE_2_SITE;
					}
					else {
						SFout = AMP_FE_3_SITE * float(SFoutFACTORY) + OFFS_FE_3_SITE;
					}

					SFpac = SFin - SFout;  // flujo del paciente

					// filtro moving average para la senal de flujo
					FPacProm[L_F_PROM-1] = SFpac;
                    //- Corrimiento inicial
					for (int i = L_F_PROM-1; i >= 1; i--) {
						FPacProm[L_F_PROM-1 - i] = FPacProm[L_F_PROM-1 - i + 1];						
					}
					//- Inicializacion
					SFpacV = 0;
					//- Actualizacion
					for (int i = 0; i <= L_F_PROM-1; i++) {
						SFpacV = SFpacV + FPacProm[i];
					}
					//- Calculo promedio
					SFpacV = SFpacV / L_F_PROM;


					// Calculo de volumen circulante
					flowTotalV = SFpacV - flowZero;
					flowTotalC = SFpac;
					//SFant = SFpac;

					// Calculo de volumen
					if (alerGeneral == 0) {
						if ((flowTotalC <= FLOWLO_LIM) || (flowTotalC >= FLOWUP_LIM)) {
							VtidalC = VtidalC + (flowTotalC * DELTA_T * FLOW_CONV * VOL_SCALE);

							if (VtidalC < 0) {
								VtidalC = 0;
							}

							if (VtidalC > 3000) {
								VtidalC = 3000;
							}

						}
						if ((flowTotalV <= FLOWLO_LIM) || (flowTotalV >= FLOWUP_LIM)) {
							VtidalV = VtidalV + (flowTotalV * DELTA_T * FLOW_CONV * VOL_SCALE);

							if (VtidalV < 0) {
								VtidalV = 0;
							}

							if (VtidalV > 3000) {
								VtidalV = 3000;
							}
						}
					}
					else {
						VtidalC = 0;
						VtidalV = 0;
					}

				}

				// muestreo lento de ADC
				if (contADC == ADC_SLOW) {
					contADC = 0;
					// Derivada SPpac
					SPpac0 = SPpac1;
					SPpac1 = SPpac2;
					SPpac2 = SPpac3;
					SPpac3 = SPpac4;
					SPpac4 = SPpac;
					dPpac = SPpac4 - SPpac0;
					//Serial.println(String(SPpac) + ';' + String(10*dPpac));
					if (currentStateMachineCycling == INSPIRATION_CYCLING) {
						if (SPpac > maxPresion && flagAlarmPpico == false) {
							flagAlarmPpico = true;
							flagMaxPresion = true;
							alerPresionPIP = 1;
						}
					}
					else if ((newVentilationMode == 1) && (currentStateMachineCycling == EXPIRATION_CYCLING) &&
						(contCycling >= int(inspirationTime * 1000 + expirationTime * 100))) {
						switch (AC_stateMachine) {
						case 0:
							//Serial.println("Estado 0 AC");
							if (dPpac > DERIVATE_DO_THRESHOLD && dPpac < DERIVATE_UP_THRESHOLD) {	// dP/dt
								AC_stateMachine = 1;
							}
							break;
						case 1:
							//Calculo de Peep
							if (dPpac > DERIVATE_DO_THRESHOLD && dPpac < DERIVATE_UP_THRESHOLD) {	// dP/dt
								Peep_AC = SPpac1;
								if (Peep_AC < 0) {	// Si el valor de Peep es negativo
									Peep_AC = 0;	// Lo limita a 0
								}
							}
							if (dPpac < DERIVATE_LO_THRESHOLD) {
								AC_stateMachine = 2;
							}
							break;
						case 2:
							//Serial.println("Estado 2 AC");
							if (SPpac4 < Peep_AC - newTrigger) {
								flagAC = true;
								AC_stateMachine = 0;
							}
							else {
								AC_stateMachine = 1;
							}
						}
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

					// // Calculo de volumen circulante
					// flowTotalV = SFin - SFout - flowZero;
					// flowTotalC = SFin - SFout;
					dFlow = SFpac - SFant;
					SFant = SFpac;
					// SFpac = SFin - SFout;  // flujo del paciente
					// if (alerGeneral == 0) {
					// 	if ((flowTotalC <= FLOWLO_LIM) || (flowTotalC >= FLOWUP_LIM)) {
					// 		VtidalC = VtidalC + (flowTotalC * DELTA_T * FLOW_CONV * VOL_SCALE);

					// 		if (VtidalC < 0) {
					// 			VtidalC = 0;
					// 		}

					// 		if (VtidalC > 3000) {
					// 			VtidalC = 3000;
					// 		}

					// 	}
					// 	if ((flowTotalV <= FLOWLO_LIM) || (flowTotalV >= FLOWUP_LIM)) {
					// 		VtidalV = VtidalV + (flowTotalV * DELTA_T * FLOW_CONV * VOL_SCALE);

					// 		if (VtidalV < 0) {
					// 			VtidalV = 0;
					// 		}

					// 		if (VtidalV > 3000) {
					// 			VtidalV = 3000;
					// 		}
					// 	}
					// }
					// else {
					// 	VtidalC = 0;
					// 	VtidalV = 0;
					// }

					// Calculo Presiones maximas y minimas en la via aerea
					if (UmbralPpmin > SPpac) {
						UmbralPpmin = SPpac;
					}
					if (UmbralPpico < SPpac) {
						UmbralPpico = SPpac;
					}
					if (UmbralPpicoDistal < SPout) {
						UmbralPpicoDistal = SPout;
					}

					// Calculo Flujos maximos y minimos en la via aerea
					if (UmbralFmin > SFpac) {
						UmbralFmin = SFpac;
					}
					if (UmbralFmax < SFpac) {
						UmbralFmax = SFpac;
					}

					// Calculo Volumenes maximos y minimos en la via aerea
					if (UmbralVmin > VtidalV) {
						UmbralVmin = VtidalV;
					}
					if (UmbralVmax < VtidalV) {
						UmbralVmax = VtidalV;
					}

					// Transmicon serial
					//- Asignacion de variables
					//if (alerDesconexion == 1) {
					//  patientPress = String(0);
					//}
					//else {

					// evaluacion de condicion de desconexion de paciente
					if (flagAlarmPatientDesconnection == true) {
						SPpac = 0;
						SFpac = 0;
						VtidalV = 0;
						VtidalC = 0;
						Ppico = 0;
						Peep = 0;
						PpicoProximal = 0;
						PeepProximal = 0;
						PpicoDistal = 0;
						PeepDistal = 0;
						currentFrecRespiratoria = 0;
						relI = 0;
						relE = 0;
						VT = 0;
					}

					if (currentStateMachine == STANDBY_STATE) {
						SPpac = 0;
						SFpac = 0;
						VtidalV = 0;
						VtidalC = 0;
						//Ppico = 0;
						//Peep = 0;
						//PpicoProximal = 0;
						//PeepProximal = 0;
						//PpicoDistal = 0;
						//PeepDistal = 0;
						//currentFrecRespiratoria = 0;
						//relI = 0;
						//relE = 0;
						VT = 0;
					}

					// activacion de secuencia para el envío por Raspberry
					xSemaphoreGive(xSemaphoreRaspberry);  // asignacion y liberacion de semaforos

				}
			}
		}
	}
	vTaskDelete(NULL);
}

/************************************************************
 ***** ENVÍO DE TRAMA DE DATOS HACIA LA RASPBERRY ***********
 ************************************************************/
void task_Raspberry(void* arg) {

	while (1) {
		// Se atiende solicitud de envio a traves de serial 1 para raspberry
		if (xSemaphoreTake(xSemaphoreRaspberry, portMAX_DELAY) == pdTRUE) {
			// Almacenamiento de los datos para envio a la raspberry
			patientPress = String(SPpac, 1);
			//}
			patientFlow = String(SFpacV, 1);
			patientVolume = String(VtidalV, 1);
			pressPIP = String(int(Ppico));
			pressPEEP = String(int(Peep));
			// Frequency = String(currentFrecRespiratoria);
			frequency = String(int(frecRespiratoriaCalculada));
			// Seleccion de la velocidad de graficacion
			if (frecRespiratoriaCalculada < 6) {
				// xSpeed = String(int(6));
				xSpeed = String(int(12));
			}
			else if (frecRespiratoriaCalculada < 20) {
				// xSpeed = String(int(12));
				xSpeed = String(int(12));
			}
			else {
				// xSpeed = String(int(20));
				xSpeed = String(int(12));
			}
			// Envio de relacion I:E
			if (currentI == 1) {
				rInspir = String(int(relI));
			}
			else {
				rInspir = String(relI, 1);
			}
			if (currentE == 1) {
				rEspir = String(int(calculatedE/10.0));
			}
			else {
				rEspir = String(calculatedE/10.0, 1);
			}
			volumeT = String(int(VT));
			alertPip = String(alerPresionPIP);
			alertPeep = String(alerPeep);
			alertObstruction = String(alerObstruccion);
			alertConnPat = String(alerDesconexion);
			alertGeneralFailure = String(alerGeneral);
			alertConnEquipment = String(alerBateria);
			alertFrequency = String(int(alerFR_Alta));
			alertMinuteVentilation = String(int(alerVE_Alto));
			alertValve1Fail = String(0);
			alertValve2Fail = String(0);
			alertValve3Fail = String(0);
			valve1Temp = String(int(32));
			valve2Temp = String(int(33));
			valve3Temp = String(int(34));
			valve1Current = String(int(500));
			valve2Current = String(int(400));
			valve3Current = String(int(450));
			source5v0Voltage = String(5.0);
			source5v0Current = String(int(1500));
			source5v0SWVoltage = String(5.1);
			source5v0SWCurrent = String(int(800));
			cameraPress = String(SPin, 1);
			bagPress = String(SPout, 1);
			inspFlow = String(SFin, 1);
			EspFlow = String(SFout, 1);

			if (pmax < 10) {
				lPresSup = String(int(10));
			}
			else if (pmax < 25) {
				lPresSup = String(int(25));
			}
			else if (pmax < 35) {
				lPresSup = String(int(35));
			}
			else if (pmax < 45) {
				lPresSup = String(int(45));
			}
			else if (pmax < 60) {
				lPresSup = String(int(60));
			}
			else {
				lPresSup = String(int(100));
			}
			lPresInf = String(int(-5));
			// lPresInf = String(int(pmin));

			if (flmax < 15) {
				lFlowSup = String(int(15));
				lFlowInf = String(int(-15));
			}
			else if (flmax < 30) {
				lFlowSup = String(int(30));
				lFlowInf = String(int(-30));
			}
			else if (flmax < 45) {
				lFlowSup = String(int(45));
				lFlowInf = String(int(-45));
			}
			else if (flmax < 60) {
				lFlowSup = String(int(60));
				lFlowInf = String(int(-60));
			}
			else {
				lFlowSup = String(int(100));
				lFlowInf = String(int(-100));
			}

			// lFlowSup = String(int(flmax));
			// lFlowInf = String(int(flmin));

			if (vmax < 400) {
				lVoluSup = String(int(400));
			}
			else if (vmax < 600) {
				lVoluSup = String(int(600));
			}
			else if (vmax < 800) {
				lVoluSup = String(int(800));
			}
			else if (vmax < 1000) {
				lVoluSup = String(int(1000));
			}
			else if (vmax < 1200) {
				lVoluSup = String(int(1200));
			}
			else {
				lVoluSup = String(int(1600));
			}
			lVoluInf = String(int(-50));

			// lVoluSup = String(int(vmax));
			// lVoluInf = String(int(vmin));

			alertValve4Fail = String(0);
			alertValve5Fail = String(0);
			alertValve6Fail = String(0);
			alertValve7Fail = String(0);
			alertValve8Fail = String(0);
			valve4Temp = String(int(32));
			valve5Temp = String(int(32));
			valve6Temp = String(int(32));
			valve7Temp = String(int(32));
			valve8Temp = String(int(32));

			//- Composicion de cadena
			/* RaspberryChain = idEqupiment + ',' + patientPress + ',' + patientFlow + ',' + patientVolume + ',' +
							  pressPIP + ',' + pressPEEP + ',' + frequency + ',' + rInspir + ',' + rEspir + ',' + volumeT + ',' +
							  alertPip + ',' + alertPeep + ',' + alertObstruction + ',' + alertConnPat + ',' + alertGeneralFailure + ',' +
							  alertConnEquipment + ',' + alertValve1Fail + ',' + alertValve2Fail + ',' + alertValve3Fail + ',' +
							  alertValve4Fail + ',' + alertValve5Fail + ',' + alertValve6Fail + ',' + alertValve7Fail + ',' +
							  alertValve8Fail + ',' + valve1Temp + ',' + valve2Temp + ',' + valve3Temp + ',' + valve4Temp + ',' +
							  valve5Temp + ',' + valve6Temp + ',' + valve7Temp + ',' + valve8Temp + ',' + cameraPress + ',' + bagPress + ',' +
							  inspFlow + ',' + EspFlow; */
			RaspberryChain = idEqupiment + ',' + patientPress + ',' + patientFlow + ',' + patientVolume + ',' + pressPIP + ',' +
				pressPEEP + ',' + frequency + ',' + xSpeed + ',' + rInspir + ',' + rEspir + ',' + volumeT + ',' +
				alertPip + ',' + alertPeep + ',' + alertObstruction + ',' + alertConnPat + ',' + alertGeneralFailure + ',' +
				alertConnEquipment + ',' + alertFrequency + ',' + alertMinuteVentilation + ',' + alertValve1Fail + ',' + alertValve2Fail + ',' +
				alertValve3Fail + ',' + valve1Temp + ',' + valve2Temp + ',' + valve3Temp + ',' + valve1Current + ',' + valve2Current + ',' +
				valve3Current + ',' + source5v0Voltage + ',' + source5v0Current + ',' + source5v0SWVoltage + ',' + source5v0SWCurrent + ',' +
				cameraPress + ',' + bagPress + ',' + inspFlow + ',' + EspFlow + ',' + lPresSup + ',' + lPresInf + ',' + lFlowSup + ',' +
				lFlowInf + ',' + lVoluSup + ',' + lVoluInf;

			// Envio de la cadena de datos (visualizacion Raspberry)
			contSendData++;
			if (contSendData == 1) {
				contSendData = 0;
				Serial.println(RaspberryChain);
			}

			// Serial.print(dPpac);
			// Serial.print(',');
			// Serial.print(SPpac);
			// Serial.print(',');
			// Serial.print(Peep_AC);
			// Serial.print(',');
			// Serial.println(Peep);

			/* ********************************************************************
			  * **** ENVIO DE VARIABLES PARA CALIBRACION ***************************
			  * ********************************************************************/
			  //  Serial.print(CalFin);
			  //  Serial.print(",");
			  //  Serial.println(CalFout);  // informacion para calibracion de flujo
			  //  Serial.println(CalPpac);
			  //  Serial.println(CalPin);
			  //  Serial.println(CalPout); // informacion para calibracion de presion
		}
		vTaskDelay(20 / portTICK_PERIOD_MS);
	}
	vTaskDelete(NULL);
}

/* ***************************************************************************
 * **** Rutinas de ciclado definidas para el ventilador **********************
 * ***************************************************************************/
 // Rutina de StandBy
void standbyRoutine() {
	if (newStateMachine != currentStateMachine) {	// si hay un cambio de estado de la maquina
		currentStateMachine = newStateMachine;		// actualiza el estado de funcionamiento de la maquina a StandBy
		PeepEstable = 0;
		contCycling = 0;							// detiene el contador de ciclado
	}

	digitalWrite(EV_INSPIRA, LOW);  //Piloto conectado a presion de bloqueo -> Libera valvula piloteada y permite el paso de aire
	digitalWrite(EV_ESC_CAM, LOW);  //Piloto conectado a Camara -> Despresuriza la camara y permite el llenado de la bolsa
	digitalWrite(EV_ESPIRA, LOW);   //Piloto conectado a PEEP -> Limita la presion de la via aerea a la PEEP configurada
}

// Cycling of the Mechanical Ventilator
void cycling() {
	
	contCycling++;  // contador que incrementa cada ms en la funcion de ciclado
	// Maquina de estados del ciclado
	switch (currentStateMachineCycling) {
	case STOP_CYCLING:
		break;
	case START_CYCLING:
		if (contCycling >= 1) {				// Inicia el ciclado abriendo electrovalvula de entrada y cerrando electrovalvula de salida
			BandInsp = 1;					// Activa bandera que indica que empezo la inspiracion
			digitalWrite(EV_INSPIRA, LOW);	// Piloto conectado a ambiente -> Desbloquea valvula piloteada y permite el paso de aire
			digitalWrite(EV_ESPIRA, HIGH);	//Piloto conectado a PIP -> Limita la presion de la via aerea a la PIP configurada
			digitalWrite(EV_ESC_CAM, HIGH);	//Piloto conectado a Presion de activacion -> Presiona la camara
			currentStateMachineCycling = INSPIRATION_CYCLING;
		}
		break;
	case INSPIRATION_CYCLING:
		//if (flagAlarmPpico == true) {
		//	flagAlarmPpico == false;
		//	Serial.println("Entre");
		//	//	alerPresionPIP = 0;
		//		digitalWrite(EV_INSPIRA, HIGH);//Piloto conectado a presion de bloqueo -> Bloquea valvula piloteada y restringe el paso de aire
		//		digitalWrite(EV_ESC_CAM, LOW);//Piloto conectado a PEEP -> Limita la presion de la via aerea a la PEEP configurada
		//		digitalWrite(EV_ESPIRA, LOW);//Piloto conectado a ambiente -> Despresuriza la camara y permite el llenado de la bolsa
		//	currentStateMachineCycling = EXPIRATION_CYCLING;
		//	//contCycling = int(inspirationTime * 1000);
		//}
		if (contCycling >= int(inspirationTime * 1000) || flagMaxPresion == true) {
			flagMaxPresion = false;
			//Calculo PIP
			if (Ppico < 0) {// Si el valor de Ppico es negativo
				Ppico = 0;// Lo limita a 0
			}
			Ppico = int(round(Ppico));

			//Mediciones de presion del sistema
			Pin_max = SPin;//Presion maxima de la camara
			Pout_max = SPout;//Presion maxima de la bolsa

			//Medicion de Volumen circulante
			if (VtidalC >= 0) {
				VTidProm[2] = VtidalC;
			}
			else {
				VTidProm[2] = 0;
			}
			// promediado del Vtidal
			for (int i = 2; i >= 1; i--) {
				VTidProm[2 - i] = VTidProm[2 - i + 1];
			}
			//- Inicializacion
			SVtidal = 0;
			//- Actualizacion
			for (int i = 0; i <= 2; i++) {
				SVtidal = SVtidal + VTidProm[i];
			}
			//- Calculo promedio
			VT = SVtidal / 3;

			//Mediciones de flujo cero
			// flowZero = SFin - SFout; // nivel cero de flujo para calculo de volumen
			flowZero = SFpacV;
			//Rutina de ciclado
			BandInsp = 0; // Desactiva la bandera, indicando que empezo la espiracion
			digitalWrite(EV_INSPIRA, HIGH);//Piloto conectado a presion de bloqueo -> Bloquea valvula piloteada y restringe el paso de aire
			digitalWrite(EV_ESC_CAM, LOW);//Piloto conectado a PEEP -> Limita la presion de la via aerea a la PEEP configurada
			digitalWrite(EV_ESPIRA, LOW);//Piloto conectado a ambiente -> Despresuriza la camara y permite el llenado de la bolsa
			currentStateMachineCycling = EXPIRATION_CYCLING;
		}
		break;
	case EXPIRATION_CYCLING:
		//Add para el modo A/C
		if (flagAC == true) {
			flagAC = false;
			frecRespiratoriaCalculada = 60.0 / ((float)contCycling / 1000.0);
			calculatedE = (int)((((60.0 / (float)frecRespiratoriaCalculada) / (float)inspirationTime) - 1) * currentI * 10);
			contCycling = 0;

			//Calculo de Peep
			PeepProximal = SPpac + newTrigger;// Peep como la presion en la via aerea al final de la espiracion
			PeepDistal = SPout;

			Peep = Peep_AC;

			if (Peep < 0) {// Si el valor de Peep es negativo
				Peep = 0;// Lo limita a 0
			}
			Peep = int(round(Peep_AC));

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
			VtidalV = 0;
			VtidalC = 0;
			// flowZero = SFin - SFout; // nivel cero de flujo para calculo de volumen
			flowZero = SFpacV;

			//Calculos de volumenes
			//- Asignacion
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
			Pin_min = SPin;  //Presion minima de la camara
			Pout_min = SPout;  //Presion minima de la bolsa

		  //Asignacion de valores maximos y minimos de presion
			pmin = UmbralPpmin;  //asigna la presion minima encontrada en todo el periodo
			pmax = UmbralPpico;  //asigna la presion maxima encontrada en todo el periodo
			flmin = UmbralFmin;  //asigna el flujo minimo encontrada en todo el periodo
			flmax = UmbralFmax;  //asigna el flujo maximo encontrada en todo el periodo
			vmin = UmbralVmin;  //asigna el volumen minimo encontrada en todo el periodo
			vmax = UmbralVmax;  //asigna el volumen maximo encontrada en todo el periodo
			Ppico = pmax;
			UmbralPpmin = 100;  //Reinicia el umbral minimo de presion del paciente
			UmbralPpico = -100;  //Reinicia el umbral maximo de presion del paciente
			UmbralFmin = 100;  //Reinicia el umbral minimo de flujo del paciente
			UmbralFmax = -100;  //Reinicia el umbral maximo de flujo del paciente
			UmbralVmin = 100;  //Reinicia el umbral minimo de volumen del paciente
			UmbralVmax = -100;  //Reinicia el umbral maximo de volumen del paciente

			//Calculo de PIP
			PpicoProximal = pmax;
			PpicoDistal = UmbralPpicoDistal;
			/* *******************************************************************
			  * *** Aqui se debe verificar cual es el valor de Ppico a utlizar *****
			  * *******************************************************************/
			Ppico = PpicoProximal;// PIP como la presion en la via aerea al final de la espiracion

			//Metodo de exclusion de alarmas
			if (Ppico > 3 && Peep > 1) {
				flagInicio = false;
			}

			currentVE = (int)((VT * frecRespiratoriaCalculada) / 100.0);  // calculo de la ventilacion minuto

			if (newStateMachine != currentStateMachine) {
				currentStateMachine = newStateMachine;
				PeepEstable = 0;
			}

			alarmsDetection();  // se ejecuta la rutina de deteccion de alarmas
			flagAlarmPpico = false;
			alerPresionPIP = 0;

			currentStateMachineCycling = START_CYCLING;
			AC_stateMachine = 0;
		}

		// Add to C-PMV mode
		if ((contCycling >= int(((inspirationTime + expirationTime) * 1000)))) {
			frecRespiratoriaCalculada = 60.0 / ((float)contCycling / 1000.0);
			calculatedE = (int)((((60.0 / (float)frecRespiratoriaCalculada) / (float)inspirationTime) - 1) * currentI * 10);
			contCycling = 0;
			//Calculo de Peep
			PeepProximal = SPpac;
			PeepDistal = SPout;
			/* *******************************************************************
			 * *** Aqui se debe verificar cual es el valor de Peep a utlizar *****
			 * *******************************************************************/
			Peep = PeepProximal;// Peep como la presion en la via aerea al final de la espiracion

			if (Peep < 0) {// Si el valor de Peep es negativo
				Peep = 0;// Lo limita a 0
			}
			Peep = int(round(Peep));
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
			VtidalV = 0;
			VtidalC = 0;
			// flowZero = SFin - SFout; // nivel cero de flujo para calculo de volumen
			flowZero = SFpacV;

			//Calculos de volumenes
			//- Asignacion
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
			Pin_min = SPin;//Presion minima de la camara
			Pout_min = SPout;//Presion minima de la bolsa

			//Asignacion de valores maximos y minimos de presion
			pmin = UmbralPpmin;  //asigna la presion minima encontrada en todo el periodo
			pmax = UmbralPpico;  //asigna la presion maxima encontrada en todo el periodo
			flmin = UmbralFmin;  //asigna el flujo minimo encontrada en todo el periodo
			flmax = UmbralFmax;  //asigna el flujo maximo encontrada en todo el periodo
			vmin = UmbralVmin;  //asigna el volumen minimo encontrada en todo el periodo
			vmax = UmbralVmax;  //asigna el volumen maximo encontrada en todo el periodo
			UmbralPpmin = 100;  //Reinicia el umbral minimo de presion del paciente
			UmbralPpico = -100;  //Reinicia el umbral maximo de presion del paciente
			UmbralPpicoDistal = -100;  //Reinicia el umbral maximo de presion del paciente
			UmbralFmin = 100;  //Reinicia el umbral minimo de flujo del paciente
			UmbralFmax = -100;  //Reinicia el umbral maximo de flujo del paciente
			UmbralVmin = 100;  //Reinicia el umbral minimo de volumen del paciente
			UmbralVmax = -100;  //Reinicia el umbral maximo de volumen del paciente

			//Calculo de PIP
			PpicoProximal = pmax;
			PpicoDistal = UmbralPpicoDistal;
			/* *******************************************************************
			  * *** Aqui se debe verificar cual es el valor de Ppico a utlizar *****
			  * *******************************************************************/
			Ppico = PpicoProximal;// PIP como la presion en la via aerea al final de la espiracion

			//Metodo de exclusion de alarmas
			if (Ppico > 2 && Peep > 2) {
				flagInicio = false;
			}

			currentVE = (int)((VT * frecRespiratoriaCalculada) / 100.0);  // calculo de la ventilacion minuto

			alarmsDetection();  // se ejecuta la rutina de deteccion de alarmas
			flagAlarmPpico = false;
			alerPresionPIP = 0;
			currentStateMachineCycling = START_CYCLING;

			if (newStateMachine != currentStateMachine) {
				currentStateMachine = newStateMachine;
				contCycling = 0;
			}

			if ((newFrecRespiratoria != currentFrecRespiratoria) ||
				(newI != currentI) || (newE != !currentE)) {  // condicion implementada para terminar un ciclado normal cada que se cambie de modo o de parametros ventilatorios
				currentFrecRespiratoria = newFrecRespiratoria;
				currentI = newI;
				currentE = newE;
				// Calculo del tiempo I:E
				if (currentI == 1) {  // calculo de la relacion IE en el modo controlado
					inspirationTime = (float)(60.0 / (float)currentFrecRespiratoria) / (1 + (float)((float)currentE / 10.0));
					expirationTime = (float)((float)currentE / 10.0) * (float)inspirationTime;
				}
				else {
					expirationTime = (float)(60.0 / (float)currentFrecRespiratoria) / (1 + (float)((float)currentI / 10.0));
					inspirationTime = (float)((float)currentI / 10.0) * (float)expirationTime;
				}
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

// CPAP of the Mechanical Ventilator
void cpapRoutine() {
	float frecCalcCPAP = 0;
	// esta funcion se ejecuta cada milisegundo
	if (newStateMachine != currentStateMachine) {
		currentStateMachine = newStateMachine;
		PeepEstable = 0;
		stateFrecCPAP = CPAP_INIT;
		alerFR_Alta = 0;
	}
	contCycling = 0;
	digitalWrite(EV_INSPIRA, LOW);  //Piloto conectado a presion de bloqueo -> Bloquea valvula piloteada y restringe el paso de aire
	digitalWrite(EV_ESC_CAM, LOW);  //Piloto conectado a PEEP -> Limita la presion de la via aerea a la PEEP configurada
	digitalWrite(EV_ESPIRA, LOW);   //Piloto conectado a ambiente -> Despresuriza la camara y permite el llenado de la bolsa

	if ((SFpac > COMP_FLOW_MAX_CPAP) && ((dFlow) > COMP_DEL_F_MAX_CPAP) && (stateFrecCPAP != CPAP_INSPIRATION))
	{	// inicio de la inspiracion
		// Inicializa Maquina de estados para que inicie en CPAP
		stateFrecCPAP = CPAP_INSPIRATION;

		// Calculo de la frecuecnia respiratoria en CPAP
		frecCalcCPAP = 60.0 / ((float)contFrecCPAP / 1000.0);
		frecRespiratoriaCalculada = (int)frecCalcCPAP;

		// Calculo de la relacion IE en CPAP
		if (contInsCPAP < contEspCPAP) {
			calculatedI = 1;
			calculatedE = int(10 * ((float)contEspCPAP / 1000.0) / ((60.0 / (float)frecCalcCPAP) - ((float)contEspCPAP) / 1000.0));
		}
		else if (contEspCPAP < contInsCPAP) {
			calculatedE = 1;
			calculatedI = int(10 * ((float)contInsCPAP / 1000.0) / ((60.0 / (float)frecCalcCPAP) - ((float)contInsCPAP) / 1000.0));
		}

		// limita el valor maximo de frecuencia a 35
		if (frecRespiratoriaCalculada > 35) {
			frecRespiratoriaCalculada = 35;
		}

		//Calculo de Peep
		PeepProximal = SPpac;
		PeepDistal = SPout;
		/* *******************************************************************
		  * *** Aqui se debe verificar cual es el valor de Peep a utlizar *****
		  * *******************************************************************/
		Peep = PeepProximal;// Peep como la presion en la via aerea al final de la espiracion
		if (Peep < newPeepMax) {
			alerPeep = 1;
		}
		else {
			alerPeep = 0;
		}

		//Ajuste del valor de volumen
		VtidalV = 0;
		VtidalC = 0;
		// flowZero = SFin - SFout; // nivel cero de flujo para calculo de volumen
		contFrecCPAP = 0;
		contEspCPAP = 0;
		contInsCPAP = 0;

		//Asignacion de valores maximos y minimos de presion
		pmin = UmbralPpmin;  //asigna la presion minima encontrada en todo el periodo
		pmax = UmbralPpico;  //asigna la presion maxima encontrada en todo el periodo
		flmin = UmbralFmin;  //asigna el flujo minimo encontrada en todo el periodo
		flmax = UmbralFmax;  //asigna el flujo maximo encontrada en todo el periodo
		vmin = UmbralVmin;  //asigna el volumen minimo encontrada en todo el periodo
		vmax = UmbralVmax;  //asigna el volumen maximo encontrada en todo el periodo
		UmbralPpmin = 100;  //Reinicia el umbral minimo de presion del paciente
		UmbralPpico = -100;  //Reinicia el umbral maximo de presion del paciente
		UmbralPpicoDistal = -100;  //Reinicia el umbral maximo de presion del paciente
		UmbralFmin = 100;  //Reinicia el umbral minimo de flujo del paciente
		UmbralFmax = -100;  //Reinicia el umbral maximo de flujo del paciente
		UmbralVmin = 100;  //Reinicia el umbral minimo de volumen del paciente
		UmbralVmax = -100;  //Reinicia el umbral maximo de volumen del paciente

		if (frecRespiratoriaCalculada > maxFR)
		{
			flagAlarmFR_Alta = true;
			alerFR_Alta = 1;
		}
		else if (frecRespiratoriaCalculada < minFR)
		{
			flagAlarmFR_Alta = true;
			alerFR_Alta = 2;
		}
		else
		{
			flagAlarmFR_Alta = false;
			alerFR_Alta = 0;
		}
	}
	if ((SFpac < COMP_FLOW_MIN_CPAP) && ((dFlow) < COMP_DEL_F_MIN_CPAP) && (stateFrecCPAP != CPAP_ESPIRATION))
	{ // si inicia la espiracion
		stateFrecCPAP = CPAP_ESPIRATION;

		//Calculo de PIP
		PpicoProximal = SPpac;
		PpicoDistal = SPout;
		/* *******************************************************************
		  * *** Aqui se debe verificar cual es el valor de Ppico a utlizar *****
		  * *******************************************************************/
		Ppico = PpicoProximal;// PIP como la presion en la via aerea al final de la espiracion

		//Medicion de Volumen circulante
		if (VtidalC >= 0) {
			VTidProm[2] = VtidalC;
		}
		else {
			VTidProm[2] = 0;
		}
		// promediado del Vtidal
		for (int i = 2; i >= 1; i--) {
			VTidProm[2 - i] = VTidProm[2 - i + 1];
		}
		//- Inicializacion
		SVtidal = 0;
		//- Actualizacion
		for (int i = 0; i <= 2; i++) {
			SVtidal = SVtidal + VTidProm[i];
			// frecRespiratoriaCalculada = frecRespiratoriaCalculada + FreqProm[i];
		}
		//- Calculo promedio
		VT = SVtidal / 3;
		// frecRespiratoriaCalculada = (int) (frecRespiratoriaCalculada / 3);
	}

	// Maquina de estados para identificar la Inspiracion y la espiracion
	switch (stateFrecCPAP) {
		// Inicio de CPAP
	case CPAP_INIT:
		frecRespiratoriaCalculada = 0;
		VT = 0;
		VtidalV = 0;
		VtidalC = 0;
		calculatedE = 0;
		calculatedI = 0;
		
		PeepProximal = SPpac;
		PeepDistal = SPout;
		/* *******************************************************************
		  * *** Aqui se debe verificar cual es el valor de Peep a utlizar *****
		  * *******************************************************************/
		Peep = PeepProximal;// Peep como la presion en la via aerea al final de la espiracion
		break;
		// Ciclo Inspiratorio
	case CPAP_INSPIRATION:
		contFrecCPAP++;  // Se incrementan los contadore para el calculo de frecuencia y relacion IE
		contInsCPAP++;
		break;
		// Ciclo Espiratorio
	case CPAP_ESPIRATION:
		contFrecCPAP++;
		contEspCPAP++;
		break;
	default:
		break;
	}

	if ((contFrecCPAP > ((apneaTime * 1000) - 5000)) && (stateFrecCPAP != CPAP_INIT)) {
		flagAlarmFR_Alta = true;
		alerFR_Alta = 2;
		frecRespiratoriaCalculada = 0;
	}
	if ((contFrecCPAP > apneaTime * 1000) && (stateFrecCPAP != CPAP_INIT)) {
		newStateMachine = AC_STATE;
		newVentilationMode = 1; // A/C Ventilation Mode
	}
}

/* ***************************************************************************
 * **** Ejecucion de la rutina de comunicacion por serial ********************
 * ***************************************************************************/
 // Function to receive data from serial communication
void task_Receive(void* pvParameters) {

	// Clean Serial buffers
	vTaskDelay(1000 / portTICK_PERIOD_MS);
	Serial.flush();
	Serial2.flush();

	while (1) {
		if (Serial2.available() > 5) {
			// if (Serial.available() > 5) {  // solo para pruebas
			String dataIn = Serial2.readStringUntil(';');
			// String dataIn = Serial.readStringUntil(';');  // solo para pruebas
			// Serial.println(dataIn);
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
			newTrigger = dataIn2[8].toInt();
			newPeepMax = dataIn2[9].toInt();
			maxFR = dataIn2[10].toInt();
			maxVE = dataIn2[11].toInt();
			apneaTime = dataIn2[12].toInt();
			Serial2.flush();
			// Serial.flush();  // solo para pruebas
			/*Serial.println("State = " + String(currentStateMachine));
			  Serial.println(String(newFrecRespiratoria) + ',' + String(newI) + ',' +
			  String(newE) + ',' + String(maxPresion) + ',' +
			  String(alerBateria) + ',' + String(estabilidad) + ',' +
			  String(newStateMachine) + ',' + String(newVentilationMode));*/

		}
		vTaskDelay(10 / portTICK_PERIOD_MS);
	}
	vTaskDelete(NULL);
}

// Function to rupdate LCD each 200 ms
void task_sendSerialData(void* arg) {
	while (1) {
		// evaluacion de condicion de desconexion de paciente
		if (flagAlarmPatientDesconnection == true) {
			SPpac = 0;
			SFpac = 0;
			VtidalV = 0;
			VtidalC = 0;
			Ppico = 0;
			Peep = 0;
			PpicoProximal = 0;
			PeepProximal = 0;
			PpicoDistal = 0;
			PeepDistal = 0;
			frecRespiratoriaCalculada = 0;
			calculatedE = 0;
			currentVE = 0;
			VT = 0;
		}
		String dataToSend = String(Ppico) + ',' + String(Peep) + ',' + String(VT) + ',' +
			String(alerPresionPIP) + ',' + String(alerDesconexion) + ',' +
			String(alerObstruccion) + ',' + String(alerPeep) + ',' + String(alerGeneral) + ',' +
			String(int(frecRespiratoriaCalculada)) + ',' + String(int(calculatedE)) + ',' +
			String(int(alerFR_Alta)) + ',' + String(int(alerVE_Alto)) + ',' + String(currentVE) + ';';

		// Serial.print(dataToSend);  // solo para pruebas
		Serial2.print(dataToSend);

		vTaskDelay(200 / portTICK_PERIOD_MS); // update each 200 ms
	}
	vTaskDelete(NULL);
}

/* ***************************************************************************
 * **** Rutina de deteccion de alarmas ***************************************
 * ***************************************************************************/
void alarmsDetection() {
	// Ppico Alarm
	if (flagInicio == false) { //Si hay habilitacion de alarmas
	  // Alarma por Ppico elevada
		/*if (Ppico > maxPresion) {
			flagAlarmPpico = true;
			alerPresionPIP = 1;
		}
		else {*/
		/*if (Ppico < maxPresion){
			flagAlarmPpico = false;
			alerPresionPIP = 0;
		}*/
		// Fallo general
		if ((Pin_max) < 6) {
			flagAlarmGeneral = true;
			alerGeneral = 1;
			newStateMachine = 1;
		}
		else {
			flagAlarmGeneral = false;
			alerGeneral = 0;
		}

		// Alarma por desconexion del paciente
		if ((Ppico) < 4) {
			flagAlarmPatientDesconnection = true;
			alerDesconexion = 1;
		}
		else {
			flagAlarmPatientDesconnection = false;
			alerDesconexion = 0;
		}

		// Alarma por obstruccion
		if ((Vout_Ins >= 0.5 * Vin_Ins) && (Peep < 3)) {
			flagAlarmObstruccion = true;
			alerObstruccion = 1;
		}
		else {
			flagAlarmObstruccion = false;
			alerObstruccion = 0;
		}

		SFinMax = SFin;
		SFoutMax = SFout;

		if (frecRespiratoriaCalculada > maxFR) {
			flagAlarmFR_Alta = true;
			alerFR_Alta = 1;
		}
		else if (frecRespiratoriaCalculada < minFR) {
			flagAlarmFR_Alta = true;
			alerFR_Alta = 2;
		}
		else {
			flagAlarmFR_Alta = false;
			alerFR_Alta = 0;
		}

		if (currentVE > maxVE*10) {
			flagAlarmVE_Alto = true;
			alerVE_Alto = 1;
		}
		else {
			flagAlarmVE_Alto = false;
			alerVE_Alto = 0;

		}
	}
}

/* ***************************************************************************
 * **** CONFIGURACION ********************************************************
 * ***************************************************************************/
void setup()
{
	init_MEMORY();
	init_GPIO();
	init_TIMER();
	Serial.begin(115200);
	Serial2.begin(115200); // , SERIAL_8N1, RXD2, TXD2);
	Serial2.setTimeout(10);

	// nvs_flash_init();

	// se crea el semaforo binario
	// xSemaphoreEncoder = xSemaphoreCreateBinary();
	xSemaphoreTimer = xSemaphoreCreateBinary();
	xSemaphoreAdc = xSemaphoreCreateBinary();
	xSemaphoreRaspberry = xSemaphoreCreateBinary();

	// // creo la tarea task_pulsador
	// xTaskCreatePinnedToCore(task_Encoder, "task_Encoder", 2048, NULL, 4, NULL, taskCoreOne);
	// // xTaskCreatePinnedToCore(task_Encoder_B, "task_Encoder_B", 10000, NULL, 1, NULL, taskCoreZero);

	xTaskCreatePinnedToCore(task_Timer, "task_Timer", 4096, NULL, 7, NULL, taskCoreOne);
	xTaskCreatePinnedToCore(task_Adc, "task_Adc", 4096, NULL, 4, NULL, taskCoreOne);
	// xTaskCreatePinnedToCore(task_Display, "task_Display", 2048, NULL, 3, NULL, taskCoreOne);  // se puede colocar en el core cero
	xTaskCreatePinnedToCore(task_Receive, "task_Receive", 4096, NULL, 1, NULL, taskCoreOne);
	xTaskCreatePinnedToCore(task_sendSerialData, "task_sendSerialData", 4096, NULL, 1, NULL, taskCoreOne);
	xTaskCreatePinnedToCore(task_Raspberry, "task_Raspberry", 4096, NULL, 4, NULL, taskCoreOne);

	// Clean Serial buffers
	vTaskDelay(1000 / portTICK_PERIOD_MS);
	// delay(1000);
	// Serial.flush();
	// Serial2.flush();

}

/* ***************************************************************************
 * **** LOOP MAIN_MENU *******************************************************
 * ***************************************************************************/
void loop() {

}

/* ***************************************************************************
 * **** FIN DEL PROGRAMA *****************************************************
 * ***************************************************************************/

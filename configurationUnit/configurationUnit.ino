/*
  Name:		configurationUnit.ino
  Created:	4/3/2020 11:40:24
  Author:	Helber Carvajal
*/

#include <nvs_flash.h>
#include <stdio.h>
#include <Wire.h>
#include "LiquidCrystal_I2C.h"

//LiquidCrystal_I2C lcd(0x27, 20, 4);
LiquidCrystal_I2C lcd(0x3F, 20, 4);

//********DEFINICIONES CONDICIONES******
#define TRUE          1
#define FALSE         0

//********DEFINICION DE PINES***********
#define A           18     //variable A a pin digital 2 (DT en modulo)
#define B           19     //variable B a pin digital 4 (CLK en modulo)
#define SW          5      //sw a pin digital 3 (SW en modulo)  

#define BUZZER_BTN          12
#define SILENCE_BTN         26  // Silenciar alarma cambiar
#define SILENCE_LED			27  // Led Boton silencio
#define STABILITY_BTN       25
#define STABILITY_LED		34
#define STANDBY				32  // Stabdby button
#define STANDBY_LED			33  // Stabdby button

#define LUMING      13  // Alarma luminosa
#define LUMINR      14
#define LUMINB      15
#define BATTALARM   4

#define LED         2

#define ESP_INTR_FLAG_DEFAULT 0

#define DEBOUNCE_ENC            50  // tiempo para realizar antirrebote
#define DEBOUNCE_ENC_2          400  // tiempo para realizar antirrebote
#define DEBOUNCE_ENC_OUT        300  // tiempo para realizar antirrebote
#define DEBOUNCE_ENC_OUT_2      800  // tiempo para realizar antirrebote
#define DEBOUNCE_ENC_SW         400  // tiempo para realizar antirrebote
#define LOW_ATT_INT             50  // Interrupcion cada 10 ms

#define ENCOD_INCREASE          1  // movimiento a la derecha, aumento
#define ENCOD_DECREASE          2  // movimiento a la derecha, aumento
#define ENCOD_COUNT             3  // cantidad de interrupciones antes de reconocer el conteo 

//**********VALORES MAXIMOS**********
#define MENU_QUANTITY       4
#define MAX_FREC            30
#define MIN_FREC            6
#define MAX_PEEP			15
#define MIN_PEEP			1
#define MAX_RIE             40
#define MIN_RIE				10  // se cambio de 20 a 10 para evitar relaciones negativas
#define MAX_PRESION         40
#define MAX_MAX_FR			60
#define MIN_MAX_FR			5
#define MAX_MAX_VE			50
#define MIN_MAX_VE			1    // l/min
#define MIN_PRESION			10
#define MAX_TRIGGER			10
#define MIN_TRIGGER			1
#define MAX_FLUJO           40
#define SILENCE_BTN_TIME        2*60*1000/LOW_ATT_INT  // tiempo, 2 minutos a 20 Hz
#define SILENCE_BTN_BATTERY     30*60*1000/LOW_ATT_INT
#define ALARM_QUANTITY		9

// Definitions for menu operation
#define MAIN_MENU           0	// Menu principal
#define CONFIG_MENU         1	// Configuracion de frecuencias
#define CONFIG_ALARM        2	// Configuracion Alarma
#define VENT_MENU		    3	// Ventilation menu selection
#define SERVICE_MENU		4   


#define ALE_PRES_DES        5	// desconexion del paciente
#define ALE_OBSTRUCCION     6	// fallo OBSTRUCCION
#define BATTERY             7	// Bateria
#define CHECK_MENU          8	// Show in check state
#define CONFIRM_MENU	    9
#define CPAP_MENU           10
#define ALE_PRES_PEEP       11	// Perdida de Peep
#define ALE_BATTERY_10MIN	12
#define ALE_BATTERY_5MIN	13
#define ALE_GENERAL			14 
#define ALE_FR_ALTA			15
#define ALE_VE_ALTO			16
#define ALE_PRES_PIP        17	// presion pico
#define ALE_APNEA			18

#define MODE_CHANGE         19 // definicion para obligar al cambio entre StandBy y modo normal en el LCD

volatile bool flagDetachInterrupt_A = false;
volatile bool flagDetachInterrupt_B = false;
volatile bool flagDetachInterrupt_A_B = false;
volatile bool flagDetachInterrupt_B_A = false;
volatile bool flagDetachInterrupt_S = false;
volatile bool flagDetach = false;
unsigned int contDetachA = 0;
unsigned int contDetachB = 0;
unsigned int contDetachS = 0;
unsigned int contAlarm = 0;
unsigned int contRecogIntA = 0;   // contador para reconocimiento del conteo del encoder
unsigned int contRecogIntB = 0;   // contador para reconocimiento del conteo del encoder
unsigned int contLowAtten = 0;

unsigned int contSilence = 0;
unsigned int contSilenceBattery = 0;
unsigned int contBattery = 0;
unsigned int contStability = 0;
unsigned int contBattery5min = 0;
unsigned int fl_StateEncoder = 0;

bool flagStandbyInterrupt = true;
unsigned int contStandby = 0;

// variables de menu
volatile signed int menu = MAIN_MENU;
volatile unsigned int menuImprimir = MAIN_MENU;
int menuAlerta[ALARM_QUANTITY + 1] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
volatile unsigned int lineaAlerta = MAIN_MENU;
volatile uint8_t flagAlreadyPrint = false;

// Variables para menu anterior
volatile unsigned int menuAnterior = CONFIRM_MENU;  // valor de menu anterior
volatile unsigned int lineaAnterior = CONFIRM_MENU;  // valor de menu anterior
byte IAnte = 1;
byte EAnte = 1;
float PpicoAnte = 1;
float PeepAnte = 0;
float PconAnte = 0;
unsigned int VTAnte = 1;
byte maxPresionAnte = 0;
byte frecRespiratoriaAnte = 0;
byte minFR_Ante = 0;
byte minVE_Ante = 0;
byte maxFR = 30;
byte maxVE = 30;
byte apneaTime = 20;

bool flagAlerta = false;
bool flagBatteryAlert = false;
unsigned int contAlertas = 0;

// Global al ser usada en loop e ISR (encoder)
int currentRelacionIE = 20;
int newRelacionIE = 20;
String relacion_IE = "1:2.0";
byte currentFrecRespiratoria = 12;
byte newFrecRespiratoria = 12;
byte frecRespiratoriaCalculada = 0;
byte I = 1;
byte E = 20;
int calculatedE = E;
byte maxPresion = 30;
byte optionVentMenu = 0;
byte optionConfigMenu = 0;
byte trigger = 2;
float maxFlujo = 4;

// Banderas utilizadas en las interrupciones
bool insideMenuFlag = false;
bool flagPresion = false;
bool flagFlujo = false;
bool flagFrecuencia = false;
bool flagTrigger = false;
bool flagIE = false;
bool flagPeepMax = false;
bool flagSensibilityCPAP = false;
bool flagMode = false;
bool flagConfirm = false;
bool flagMinFR = false;
bool flagVE = false;
int flagEncoder = 0;

byte currentVentilationMode = 0;
byte newVentilationMode = 0;

byte currentVE = 0;
byte newVE = 0;

float Peep = 0;
byte PeepMax = 10;
float Ppico = 0;
float Pcon = 0;
unsigned int VT = 0;
byte sensibilityCPAP = 0;

int alerPresionPIP = 0;
int alerDesconexion = 0;
int alerObstruccion = 0;
int alerGeneral = 0;
int alerPresionPeep = 0;
int alerFR_Alta = 0;
int alerVE_Alto = 0;
int currentBatteryAlert = 1;        // When is working with energy supply
int newBatteryAlert = 1;

// State Machine
#define CHECK_STATE		0
#define STANDBY_STATE	1
#define PCMV_STATE		2
#define AC_STATE		3
#define CPAP_STATE		4
#define FAILURE_STATE	5
byte stateMachine = STANDBY_STATE;

#define BATTERY_NO_ALARM      0
#define batteryAlarm        1
#define batteryAlarm10min   2
#define batteryAlarm5min    3
byte batteryAlert = BATTERY_NO_ALARM;

//creo el manejador para el sem�foro como variable global
SemaphoreHandle_t xSemaphoreEncoder = NULL;
SemaphoreHandle_t xSemaphoreTimer = NULL;
//xQueueHandle timer_queue = NULL;

volatile uint8_t flagAEncoder = false;
volatile uint8_t flagBEncoder = false;
volatile uint8_t flagSEncoder = false;
volatile uint8_t flagTimerInterrupt = false;

volatile uint8_t flagSilenceInterrupt = false;
volatile uint8_t flagStabilityInterrupt = false;
volatile uint8_t flagBatterySilence = false;

unsigned int temp = 0;
bool flagFirst = false;
bool flagEntre = false;

// inicializacion del contador del timer
hw_timer_t* timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

// definicion de interrupciones
void IRAM_ATTR onTimer(void);  // funcion de interrupcion
void IRAM_ATTR swInterrupt(void);
void IRAM_ATTR encoderInterrupt_A(void);
void IRAM_ATTR encoderInterrupt_B(void);
void IRAM_ATTR standbyButtonInterrupt(void);
void IRAM_ATTR silenceButtonInterrupt(void);
void IRAM_ATTR stabilityButtonInterrupt(void);

// definicion de los core para ejecucion
static uint8_t taskCoreZero = 0;
static uint8_t taskCoreOne = 1;

/************************************************************
 ********** FUNCIONES DE INICIALIZACION *********************
 ***********************************************************/
void init_GPIO() {
	//Encoder setup
	pinMode(A, INPUT_PULLUP);    // A como entrada
	pinMode(B, INPUT_PULLUP);    // B como entrada
	pinMode(SW, INPUT_PULLUP);   // SW como entrada
	pinMode(SILENCE_BTN, INPUT_PULLUP); // switch para el manejo de silencio
	pinMode(STANDBY, INPUT_PULLUP);
	pinMode(STABILITY_BTN, INPUT_PULLUP);
	pinMode(BUZZER_BTN, OUTPUT);
	pinMode(LUMINR, OUTPUT);
	pinMode(LUMING, OUTPUT);
	pinMode(LUMINB, OUTPUT);
	pinMode(SILENCE_LED, OUTPUT);
	pinMode(STANDBY_LED, OUTPUT);
	pinMode(STABILITY_LED, OUTPUT);

	pinMode(BATTALARM, INPUT);

	attachInterrupt(digitalPinToInterrupt(A), encoderInterrupt_A, FALLING);
	attachInterrupt(digitalPinToInterrupt(B), encoderInterrupt_B, FALLING);
	attachInterrupt(digitalPinToInterrupt(SW), swInterrupt, RISING);
	attachInterrupt(digitalPinToInterrupt(STANDBY), standbyButtonInterrupt, FALLING);
	attachInterrupt(digitalPinToInterrupt(SILENCE_BTN), silenceButtonInterrupt, FALLING);
	//attachInterrupt(digitalPinToInterrupt(STABILITY_BTN), stabilityButtonInterrupt, FALLING);

	digitalWrite(STANDBY_LED, HIGH);
	digitalWrite(STABILITY_LED, HIGH);
	digitalWrite(LUMING, LOW);
	pinMode(LED, OUTPUT);
}

void init_TIMER() {
	// Configuracion del timer a 1 kHz
	timer = timerBegin(0, 80, true);                // Frecuencia de reloj 80 MHz, prescaler de 80, frec 1 MHz
	timerAttachInterrupt(timer, &onTimer, true);    // Attach onTimer function to our timer
	timerAlarmWrite(timer, 1000, true);             // Interrupcion cada 1000 conteos del timer, es decir 100 Hz
	timerAlarmEnable(timer);                        // Habilita interrupcion por timer
}

/* *********************************************************************
 * **** FUNCIONES DE ATENCION A INTERRUPCION ***************************
 * *********************************************************************/
 // Interrupcion por presion del switch
void IRAM_ATTR swInterrupt(void) {
	// da el semaforo para que quede libre para la tarea pulsador
	portENTER_CRITICAL_ISR(&mux);
	flagSEncoder = true;
	xSemaphoreGiveFromISR(xSemaphoreEncoder, NULL);
	portEXIT_CRITICAL_ISR(&mux);
}

// Interrupcion por encoder A
void IRAM_ATTR encoderInterrupt_A(void) {
	portENTER_CRITICAL_ISR(&mux);
	flagAEncoder = true;
	xSemaphoreGiveFromISR(xSemaphoreEncoder, NULL);
	portEXIT_CRITICAL_ISR(&mux);
}

// Interrupcion por encoder B
void IRAM_ATTR encoderInterrupt_B(void) {
	portENTER_CRITICAL_ISR(&mux);
	flagBEncoder = true;
	xSemaphoreGiveFromISR(xSemaphoreEncoder, NULL);
	portEXIT_CRITICAL_ISR(&mux);
}

// Interrupcion por timer
void IRAM_ATTR onTimer(void) {
	portENTER_CRITICAL_ISR(&timerMux);
	flagTimerInterrupt = true;
	xSemaphoreGiveFromISR(xSemaphoreTimer, NULL);
	portEXIT_CRITICAL_ISR(&timerMux);
}

void IRAM_ATTR standbyButtonInterrupt() {
	portENTER_CRITICAL_ISR(&mux);
	detachInterrupt(digitalPinToInterrupt(STANDBY));
	flagStandbyInterrupt = true;
	portEXIT_CRITICAL_ISR(&mux);
}

// Interrupcion por button silence
void IRAM_ATTR silenceButtonInterrupt(void) {
	if (flagBatteryAlert == true && flagBatterySilence == false) {
		portENTER_CRITICAL_ISR(&mux);
		flagBatterySilence = true;
		portEXIT_CRITICAL_ISR(&mux);
	}
	if (flagAlerta == true && flagSilenceInterrupt == false) {
		portENTER_CRITICAL_ISR(&mux);
		flagSilenceInterrupt = true;
		portEXIT_CRITICAL_ISR(&mux);
	}
}

void IRAM_ATTR stabilityButtonInterrupt(void) {
	portENTER_CRITICAL_ISR(&mux);
	flagStabilityInterrupt = true;
	detachInterrupt(digitalPinToInterrupt(STABILITY_BTN));
	portEXIT_CRITICAL_ISR(&mux);
}

/************************************************************
 ***** FUNCIONES DE ATENCION A INTERRUPCION TAREA TIMER *****
 ************************************************************/
void task_timer(void* arg) {
	int contms = 0;
	uint8_t debounceENC = 0;
	uint8_t debounceENC_2 = 0;

	while (1) {
		// Se atiende la interrupcion del timer
		if (xSemaphoreTake(xSemaphoreTimer, portMAX_DELAY) == pdTRUE) {
			if (flagTimerInterrupt == true) {
				portENTER_CRITICAL(&timerMux);
				flagTimerInterrupt = false;
				portEXIT_CRITICAL(&timerMux);

				switch (stateMachine) {
				case CHECK_STATE:
					//alertMonitoring();
					//checkRoutine();
					//hardwareInterruptAttention();
					break;
				case STANDBY_STATE:
					//alertMonitoring();
					standbyInterruptAttention();
					break;
				case PCMV_STATE:
					//alertMonitoring();
					standbyInterruptAttention();
					break;
				case AC_STATE:
					standbyInterruptAttention();
					break;
				case CPAP_STATE:
					standbyInterruptAttention();
					break;
				case FAILURE_STATE:
					break;
				default:
					break;
				}
				contms++;
				if (contms % 25 == 0) {
					digitalWrite(LED, !digitalRead(LED));
				}
				if (contms == 1000) {
					contms = 0;
					// digitalWrite(LED, !digitalRead(LED));
					// Serial.print("LED_SEGUNDO ");
					// Serial.print("Core ");
					// Serial.println(xPortGetCoreID());
				}
				/****************************************************************************
				****  En esta seccion del codigo se agregan de nuevo las interrupciones  ****
				****************************************************************************/
				if (insideMenuFlag) { // si esta configurando un parametro
					debounceENC = DEBOUNCE_ENC;
					debounceENC_2 = DEBOUNCE_ENC_2;
				}
				else {
					debounceENC = DEBOUNCE_ENC_OUT;
					debounceENC_2 = DEBOUNCE_ENC_OUT_2;
				}

				// Agregar interrupcion A
				if ((flagDetachInterrupt_A == true) || (flagDetachInterrupt_B_A == true)) {
					contDetachA++;
					if ((contDetachA >= debounceENC) && (flagDetachInterrupt_A == true)) {
						contDetachA = 0;
						flagDetachInterrupt_A = false;
						attachInterrupt(digitalPinToInterrupt(A), encoderInterrupt_A, FALLING);
					}
					if ((contDetachA >= debounceENC_2) && (flagDetachInterrupt_B_A == true)) {
						contDetachB = 0;
						flagDetachInterrupt_B_A = false;
						attachInterrupt(digitalPinToInterrupt(B), encoderInterrupt_B, FALLING);
					}
				}
				// Agregar interrupcion B
				if ((flagDetachInterrupt_B == true) || (flagDetachInterrupt_A_B == true)) {
					contDetachB++;
					if ((contDetachB >= debounceENC) && (flagDetachInterrupt_B == true)) {
						contDetachB = 0;
						flagDetachInterrupt_B = false;
						attachInterrupt(digitalPinToInterrupt(B), encoderInterrupt_B, FALLING);
					}
					if ((contDetachB >= debounceENC_2) && (flagDetachInterrupt_A_B == true)) {
						contDetachB = 0;
						flagDetachInterrupt_A_B = false;
						attachInterrupt(digitalPinToInterrupt(A), encoderInterrupt_A, FALLING);
					}
				}
				// Agregar interrupcion S
				if (flagDetachInterrupt_S == true) {
					contDetachS++;
					if (contDetachS >= DEBOUNCE_ENC_SW) {
						contDetachS = 0;
						flagDetachInterrupt_S = false;
						attachInterrupt(digitalPinToInterrupt(SW), swInterrupt, FALLING);
					}
				} // Finaliza agregar interrupciones

				if (flagStabilityInterrupt == true) {
					contStability++;
					if (contStability > 400) {
						//Serial.println("Estabilidad");
						contStability = 0;
						sendSerialData();
						portENTER_CRITICAL(&timerMux);
						flagStabilityInterrupt = false;
						attachInterrupt(digitalPinToInterrupt(STABILITY_BTN), stabilityButtonInterrupt, FALLING);
						portEXIT_CRITICAL(&timerMux);
						digitalWrite(STABILITY_LED, HIGH);
						digitalWrite(LUMING, HIGH);
					}
				}
				alarmMonitoring();
			}
		}
	}
}

/****************************************************************************
 ***** Atencion a interrupcion por encoder **********************************
 ****************************************************************************/
void task_Encoder(void* arg) {
	while (1) {
		// Espero por la notificacion de la ISR por A
		if (xSemaphoreTake(xSemaphoreEncoder, portMAX_DELAY) == pdTRUE) {
			if (flagAEncoder == true) {

				portENTER_CRITICAL(&mux);
				flagAEncoder = false;
				portEXIT_CRITICAL(&mux);

				if ((digitalRead(B) == HIGH) && (digitalRead(A) == LOW)) {
					detachInterrupt(digitalPinToInterrupt(A));
					detachInterrupt(digitalPinToInterrupt(B));
					flagDetachInterrupt_A = true;
					flagDetachInterrupt_B_A = true;
					contDetachA = 0;
					fl_StateEncoder = ENCOD_INCREASE;
					// ejecucion de la tarea de incremento, esta funcion se coloca por sensibildad del encoder,
					// si se cambia por menor sensibilidad, solo necesitara ejecutar la funcion  encoderRoutine();
					contRecogIntA++;
					if (contRecogIntA == ENCOD_COUNT) {
						contRecogIntA = 0;
						encoderRoutine();
					}
				}
			}

			if (flagBEncoder == true) {

				portENTER_CRITICAL(&mux);
				flagBEncoder = false;
				portEXIT_CRITICAL(&mux);

				if ((digitalRead(A) == HIGH) && (digitalRead(B) == LOW)) {
					detachInterrupt(digitalPinToInterrupt(B));
					detachInterrupt(digitalPinToInterrupt(A));
					flagDetachInterrupt_B = true;
					flagDetachInterrupt_A_B = true;
					contDetachB = 0;
					fl_StateEncoder = ENCOD_DECREASE;
					// ejecucion de la tarea de decremento, esta funcion se coloca por sensibildad del encoder,
					// si se cambia por menor sensibilidad, solo necesitara ejecutar la funcion  encoderRoutine();
					contRecogIntB++;
					if (contRecogIntB == ENCOD_COUNT) {
						contRecogIntB = 0;
						encoderRoutine();
					}
				}
			}

			if (flagSEncoder == true) {

				portENTER_CRITICAL(&mux);
				flagSEncoder = false;
				portEXIT_CRITICAL(&mux);

				detachInterrupt(digitalPinToInterrupt(SW));
				flagDetachInterrupt_S = true;
				contDetachS = 0;
				// ejecucion de la tarea de Switch
				contRecogIntA = 0;
				contRecogIntB = 0;
				switchRoutine();
			}
		}
	}
}

/* ***************************************************************************
 * **** Ejecucion de incremento o decremento del encoder *********************
 * ***************************************************************************/
void encoderRoutine() {

	switch (fl_StateEncoder) {
		// Incremento
	case ENCOD_INCREASE:
		if (insideMenuFlag == false) {
			menu++;
			if (menu < 0 || menu > MENU_QUANTITY - 1)
				menu = 0;
			//Serial.println("menu = " + String(menu));
		}
		else {
			switch (menu) {
			case CONFIG_MENU:
				if (flagFrecuencia) {
					newFrecRespiratoria++;
					if (newFrecRespiratoria > MAX_FREC) {
						newFrecRespiratoria = MAX_FREC;
					}
				}
				else if (flagIE) {
					newRelacionIE++;
					if (newRelacionIE >= MAX_RIE) {
						newRelacionIE = MAX_RIE;
					}
					if (newRelacionIE > -10 && newRelacionIE < 0) {
						newRelacionIE = 10;
					}
				}
				else if (flagTrigger == true) {
					trigger++;
					if (trigger > MAX_TRIGGER) {
						trigger = MAX_TRIGGER;
					}
				}
				else if (flagPeepMax == true) {
					PeepMax++;
					if (PeepMax > MAX_PEEP) {
						PeepMax = MAX_PEEP;
					}
				} 
				else if (insideMenuFlag == true) {
					optionConfigMenu++;
					if (currentVentilationMode == 0) {
						if (optionConfigMenu > 2) {
							optionConfigMenu = 0;
						}
					}
					else if (currentVentilationMode == 1) {
						if (optionConfigMenu > 3) {
							optionConfigMenu = 0;
						}
					}
					else {
						if (optionConfigMenu > 1) {
							optionConfigMenu = 0;
						}	
					}
				}	
				break;
			case CONFIG_ALARM:
				if (flagPresion == true) {
					maxPresion++;
					if (maxPresion > MAX_PRESION) {
						maxPresion = MAX_PRESION;
					}
				}
				else if (flagMinFR == true) {
					maxFR++;
					if (maxFR > MAX_MAX_FR) {
						maxFR = MAX_MAX_FR;
					}	
				}
				else if (flagVE == true) {
					maxVE++;
					if (maxVE > MAX_MAX_VE) {
						maxVE = MAX_MAX_VE;
					}
				}
				else if (insideMenuFlag == true) {
					optionConfigMenu++;
					if (optionConfigMenu > 3) {
						optionConfigMenu = 0;
					}
				}
				break;
			case VENT_MENU:
				optionVentMenu++;
				if (optionVentMenu > 3) {
					optionVentMenu = 0;
				}
				break;
			case CONFIRM_MENU:
				flagConfirm = !flagConfirm;
				break;
			}
		}
		fl_StateEncoder = 0;
		break;
		// Decremento
	case ENCOD_DECREASE:
		if (insideMenuFlag == false) {
			menu--;
			if (menu < 0 || menu > MENU_QUANTITY - 1)
				menu = MENU_QUANTITY - 1;
		}
		else {
			switch (menu) {
			case CONFIG_MENU:
				if (flagFrecuencia) {
					newFrecRespiratoria--;
					if (newFrecRespiratoria < MIN_FREC) {
						newFrecRespiratoria = MIN_FREC;
					}
				}
				else if (flagIE) {
					newRelacionIE --;
					if (newRelacionIE <= -MIN_RIE) {
						newRelacionIE = -MIN_RIE;
					}
					if (newRelacionIE < 10 && newRelacionIE > 0) {
						newRelacionIE = -10;
					}
				}
				else if (flagTrigger == true) {
					trigger--;
					if (trigger < MIN_TRIGGER) {
						trigger = MIN_TRIGGER;
					}
				}
				else if (flagPeepMax == true) {
					PeepMax--;
					if (PeepMax < MIN_PEEP) {
						PeepMax = MIN_PEEP;
					}
				}
				else if (insideMenuFlag == true) {
					optionConfigMenu--;
					if (currentVentilationMode == 0) {
						if (optionConfigMenu > 2) {
							optionConfigMenu = 2;
						}
					}
					else if (currentVentilationMode == 1) {
						if (optionConfigMenu > 3) {
							optionConfigMenu = 3;
						}
					}
					else {
						if (optionConfigMenu > 1) {
							optionConfigMenu = 1;
						}
					}
					
				}
				break;
			case CONFIG_ALARM:
				if (flagPresion == true) {
					maxPresion--;
					if (maxPresion < MIN_PRESION) {
						maxPresion = MIN_PRESION;
					}
				}
				else if (flagMinFR == true) {
					maxFR--;
					if (maxFR > MAX_MAX_FR) {
						maxFR = MIN_MAX_FR;
					}
				}
				else if (flagVE == true) {
					maxVE--;
					if (maxVE > MAX_MAX_VE) {
						maxVE = MIN_MAX_VE;
					}
				}
				else if (insideMenuFlag == true) {
					optionConfigMenu--;
					if (optionConfigMenu > 3) {
						optionConfigMenu = 3;
					}
				}
				break;
			case VENT_MENU:
				optionVentMenu--;
				if (optionVentMenu > 3) {
					optionVentMenu = 3;
				}
				break;
			case CONFIRM_MENU:
				flagConfirm = !flagConfirm;
				break;
			}
		}
		fl_StateEncoder = 0;
		break;
	default:
		fl_StateEncoder = 0;
		break;
	}

	menuImprimir = menu;
	lineaAlerta = menu;
	flagAlreadyPrint = false;

}

/* ***************************************************************************
 * **** Ejecucion de tarea de Switch *****************************************
 * ***************************************************************************/
void switchRoutine() {
	// Serial.println("SW MENU");
	switch (menu) {
	case CONFIG_MENU:
		if (optionConfigMenu == 0) {
			if (insideMenuFlag == false) {
				insideMenuFlag = true;
			}
			else {
				insideMenuFlag = false;
				menu = MAIN_MENU;
				sendSerialData();
			}
		}
		else if (currentVentilationMode == 2) {
			if (optionConfigMenu == 1) {
				flagPeepMax = !flagPeepMax;
			}
		}
		else if(currentVentilationMode == 1){
			if (optionConfigMenu == 1) {
				flagTrigger = !flagTrigger;
			}
			if (optionConfigMenu == 2) {
				flagFrecuencia = !flagFrecuencia;
				currentFrecRespiratoria = newFrecRespiratoria;
			}
			if (optionConfigMenu == 3) {
				flagIE = !flagIE;
				currentRelacionIE = newRelacionIE;
			}
		}
		else {
			if (optionConfigMenu == 1) {
				flagFrecuencia = !flagFrecuencia;
				currentFrecRespiratoria = newFrecRespiratoria;
			}
			if (optionConfigMenu == 2) {
				flagIE = !flagIE;
				currentRelacionIE = newRelacionIE;
			}
		}
		break;
	case VENT_MENU:
		if (optionVentMenu == 0) {
			insideMenuFlag = !insideMenuFlag;
		}
		else if (optionVentMenu == 1) {
			menu = CONFIG_MENU;
			if (stateMachine != STANDBY_STATE) {
				stateMachine = PCMV_STATE;
			}
			currentVentilationMode = 0;
			optionVentMenu = 0;
		}
		else if (optionVentMenu == 2) {
			menu = CONFIG_MENU;
			if (stateMachine != STANDBY_STATE) {
				stateMachine = AC_STATE;
			}
			currentVentilationMode = 1;
			optionVentMenu = 0;
		}
		else if (optionVentMenu == 3) {
			menu = CONFIG_MENU;
			if (stateMachine != STANDBY_STATE) {
				stateMachine = CPAP_STATE;
			}
			currentVentilationMode = 2;
			optionVentMenu = 0;
		}
		break;
	case CPAP_MENU:
		menu = CONFIRM_MENU;
		break;
	case CONFIRM_MENU:
		insideMenuFlag = !insideMenuFlag;
		menu = CONFIG_MENU;
		if (flagConfirm == true) {
			flagConfirm = false;
			currentFrecRespiratoria = newFrecRespiratoria;
			currentRelacionIE = newRelacionIE;
			currentVentilationMode = newVentilationMode;
			sendSerialData();
		}
		else {
			newFrecRespiratoria = currentFrecRespiratoria;
			newRelacionIE = currentRelacionIE;
			newVentilationMode = currentVentilationMode;
		}
		break;
	case CONFIG_ALARM:
		if (optionConfigMenu == 0) {
			if (insideMenuFlag == false) {
				insideMenuFlag = true;
			}
			else {
				insideMenuFlag = false;
				sendSerialData();
			}
		}
		else if(optionConfigMenu == 1) {
			flagPresion = !flagPresion;
		}
		else if (optionConfigMenu == 2) {
			flagMinFR = !flagMinFR;
		}
		else if (optionConfigMenu == 3) {
			flagVE = !flagVE;
		}
		break;
	}
	menuImprimir = menu;
	lineaAlerta = menu;
	flagAlreadyPrint = false;
}

/* ***************************************************************************
 * **** Atencion a interrupcion por buttons **********************************
 * ***************************************************************************/
void silenceInterruptAttention() {
	if (flagSilenceInterrupt == true) {
		//digitalWrite(BUZZER_BTN, LOW);
		contSilence++;
		if (contSilence > SILENCE_BTN_TIME) {
			portENTER_CRITICAL(&mux);
			flagSilenceInterrupt = false;
			portEXIT_CRITICAL(&mux);
			contSilence = 0;
		}
	}

	if (flagBatterySilence == true) {
		//digitalWrite(BUZZER_BTN, LOW);
		contSilenceBattery++;
		if (contSilenceBattery > SILENCE_BTN_BATTERY) {
			portENTER_CRITICAL(&mux);
			flagBatterySilence = false;
			portEXIT_CRITICAL(&mux);
			contSilenceBattery = 0;
		}
	}
}

void standbyInterruptAttention() {
	if (flagStandbyInterrupt) {
		contStandby++;
		Serial.println(digitalRead(STANDBY));
		if (stateMachine == STANDBY_STATE && contStandby > 500 && digitalRead(STANDBY) == 0) {
			portENTER_CRITICAL(&mux);
			attachInterrupt(digitalPinToInterrupt(STANDBY), standbyButtonInterrupt, FALLING);
			flagStandbyInterrupt = false;
			portEXIT_CRITICAL(&mux);
			contStandby = 0;

			switch (currentVentilationMode) {
			case 0:
				stateMachine = PCMV_STATE;
				portENTER_CRITICAL_ISR(&mux);
				attachInterrupt(digitalPinToInterrupt(STABILITY_BTN), stabilityButtonInterrupt, FALLING);
				portEXIT_CRITICAL_ISR(&mux);
				break;
			case 1:
				stateMachine = AC_STATE;
				portENTER_CRITICAL_ISR(&mux);
				attachInterrupt(digitalPinToInterrupt(STABILITY_BTN), stabilityButtonInterrupt, FALLING);
				portEXIT_CRITICAL_ISR(&mux);
				break;
			case 2:
				stateMachine = CPAP_STATE;
				digitalWrite(STABILITY_LED, LOW);
				digitalWrite(LUMING, LOW);
				portENTER_CRITICAL_ISR(&mux);
				detachInterrupt(digitalPinToInterrupt(STABILITY_BTN));
				portEXIT_CRITICAL_ISR(&mux);
				break;
			default:
				stateMachine = STANDBY_STATE;
				digitalWrite(STANDBY_LED, HIGH);
				digitalWrite(STABILITY_LED, LOW);
				digitalWrite(LUMING, LOW);
				portENTER_CRITICAL_ISR(&mux);
				detachInterrupt(digitalPinToInterrupt(STABILITY_BTN));
				portEXIT_CRITICAL_ISR(&mux);
				break;
			}
			//Serial.println("I am on Cycling state");
			digitalWrite(STANDBY_LED, LOW);
			lineaAnterior = MODE_CHANGE;
			sendSerialData();

		} else if (stateMachine != STANDBY_STATE){
			Serial.println(digitalRead(STANDBY));
			if (contStandby < 3000 && digitalRead(STANDBY) == 1) {
				Serial.println("Standby Interrupt");
				contStandby = 0;
				portENTER_CRITICAL_ISR(&mux);
				attachInterrupt(digitalPinToInterrupt(STANDBY), standbyButtonInterrupt, FALLING);
				flagStandbyInterrupt = false;
				portEXIT_CRITICAL_ISR(&mux);
			}
			else if (contStandby > 3000) {
				contStandby = 0;
				stateMachine = STANDBY_STATE;
				digitalWrite(STANDBY_LED, HIGH);
				digitalWrite(STABILITY_LED, LOW);
				digitalWrite(LUMING, LOW);
				portENTER_CRITICAL_ISR(&mux);
				attachInterrupt(digitalPinToInterrupt(STANDBY), standbyButtonInterrupt, FALLING);
				flagStandbyInterrupt = false;
				detachInterrupt(digitalPinToInterrupt(STABILITY_BTN));
				portEXIT_CRITICAL_ISR(&mux);
				lineaAnterior = MODE_CHANGE;
				sendSerialData();
				//Serial.println("I am on Standby state");
			}
		}
			
	}
}

/************************************************************
 ***** Ejecucion de la rutina de refrescamiento del LCD *****
 ***********************************************************/
void lcd_show_comp() {

	menuAnterior = menuImprimir;
	lineaAnterior = lineaAlerta;
	flagAlreadyPrint = true;
	String calculatedRelacion_IE;
	if (currentRelacionIE > 0) {
		//relacion_IE = "1:" + String((float)currentRelacionIE / 10, 1);
		relacion_IE = "1:" + String((float)currentRelacionIE / 10, 1);
		calculatedRelacion_IE = "1:" + String((float)calculatedE / 10.0, 1);
		I = 1;
		E = (char)currentRelacionIE;
	}
	else {
		relacion_IE = String(-(float)currentRelacionIE / 10, 1) + ":1";
		I = (char)(-currentRelacionIE);
		E = 1;
	}

	// Print the first line
	lcdPrintFirstLine();

	switch (menuImprimir) {
	case MAIN_MENU:
		// lcd.home();
		lcd.setCursor(0, 1);
		lcd.print("FR        PIP     ");
		lcd.setCursor(4, 1);
		lcd.print(frecRespiratoriaCalculada);
		//lcd.print(currentFrecRespiratoria);
		lcd.setCursor(15, 1);
		lcd.print(String(Ppico, 0));
		lcd.setCursor(0, 2);
		if (currentVentilationMode == 0) {
			lcd.print("I:E       PCON      ");
			lcd.setCursor(15, 2);
			lcd.print(String(int(Pcon)));
		}
		else {
			lcd.print("I:E       VE        ");
			lcd.setCursor(15, 2);
			lcd.print(String(int(currentVE)));
		}
		lcd.setCursor(4, 2);
		lcd.print(calculatedRelacion_IE + " ");
		lcd.setCursor(0, 3);
		lcd.print("VT        PEEP      ");
		lcd.setCursor(4, 3);
		lcd.print(String(VT));
		lcd.setCursor(15, 3);
		lcd.print(String(int(Peep)));

		frecRespiratoriaAnte = frecRespiratoriaCalculada;
		PpicoAnte = Ppico;
		IAnte = I;
		EAnte = E;
		PeepAnte = Peep;
		VTAnte = VT;
		PconAnte = Pcon;
		break;
	case CONFIG_MENU:
		lcd.setCursor(0, 1);
		lcd.print("                    ");
		if (currentVentilationMode == 1) { // A/C Mode
			lcd.setCursor(0, 2);
			lcd.print(" Trig | Frec | I:E  ");
			lcd.setCursor(0, 3);
			lcd.print("      |      |      ");
			lcd.setCursor(2, 3);
			lcd.print(trigger);
			lcd.setCursor(9, 3);
			lcd.print(currentFrecRespiratoria);
			lcd.setCursor(14, 3);
			lcd.print(relacion_IE);
			frecRespiratoriaAnte = currentFrecRespiratoria;
			IAnte = I;
			EAnte = E;
		}
		else if (currentVentilationMode == 2) { // CPAP Mode
			lcd.setCursor(0, 2);
			lcd.print("  PEEP-CPAP:        ");
			lcd.setCursor(13, 2);
			lcd.print(int(Peep));
			lcd.setCursor(0, 3);
			lcd.print("  PEEP MAX:         ");
			lcd.setCursor(13, 3);
			lcd.print(PeepMax);
		}
		else { // P-CMV
			lcd.setCursor(0, 2);
			lcd.print("    Frec  |  I:E    ");
			lcd.setCursor(0, 3);
			lcd.print("          |         ");
			lcd.setCursor(5, 3);
			lcd.print(currentFrecRespiratoria);
			lcd.setCursor(12, 3);
			lcd.print(relacion_IE);
			frecRespiratoriaAnte = currentFrecRespiratoria;
			IAnte = I;
			EAnte = E;
		}
		break;
	case VENT_MENU:
		lcd.setCursor(0, 1);
		lcd.print("                    ");
		lcd.setCursor(0, 2);
		lcd.print(" P-CMV   A/C   CPAP ");
		if (currentVentilationMode == 0) {
			lcd.setCursor(6, 2);
			lcd.print('*');
		}
		else if (currentVentilationMode == 1) {
			lcd.setCursor(12, 2);
			lcd.print('*');
		}
		else {
			lcd.setCursor(19, 2);
			lcd.print('*');
		}
		lcd.setCursor(0, 3);
		lcd.print("                    ");
		break;
	case CONFIG_ALARM:
		lcd.setCursor(0, 1);
		lcd.print("                    ");
		lcd.setCursor(0, 2);
		lcd.print("  PIP |  FR  |  VE  ");
		lcd.setCursor(0, 3);
		lcd.print("      |      |      ");
		lcd.setCursor(2, 3);
		lcd.print(maxPresion);
		lcd.setCursor(9, 3);
		lcd.print(maxFR);
		lcd.setCursor(16, 3);
		lcd.print(maxVE);
		maxPresionAnte = maxPresion;
		minFR_Ante = maxFR;
		minVE_Ante = maxVE;
		break;
	case CHECK_MENU:
		lcd.setCursor(0, 1);
		lcd.print("                    ");
		lcd.setCursor(0, 2);
		lcd.print("Valvulas            ");
		lcd.setCursor(0, 3);
		lcd.print("Sensores            ");
		break;
	case CONFIRM_MENU:
		lcd.setCursor(0, 1);
		lcd.print("                    ");
		lcd.setCursor(0, 2);
		if (flagConfirm == true) {
			lcd.print("   ");
			lcd.write(126);
			lcd.print("Si        No    ");
		}
		else {
			lcd.print("    Si       ");
			lcd.write(126);
			lcd.print("No    ");
		}
		lcd.setCursor(0, 3);
		lcd.print("                    ");
		break;
	case CPAP_MENU:
		lcd.setCursor(0, 1);
		lcd.print("                    ");
		lcd.setCursor(0, 2);
		lcd.print("   PEEP CPAP =      ");
		lcd.setCursor(14, 2);
		lcd.print(int(Peep));
		break;
	case SERVICE_MENU:
		lcd.setCursor(0, 1);
		lcd.print("                    ");
		lcd.setCursor(0, 2);
		lcd.print("   PEEP CPAP =      ");
		lcd.setCursor(14, 2);
		break;
	default:
		lcd.setCursor(0, 0);
		lcd.print("                    ");
		lcd.setCursor(0, 1);
		lcd.print("                    ");
		lcd.setCursor(0, 2);
		lcd.print("Default: ");
		lcd.print(12);                  // Escribimos el numero de segundos trascurridos
		lcd.print("           ");
		lcd.setCursor(0, 3);
		lcd.print("                    ");
		break;
	}
	//Serial.println("I am in lcd_show()");
}

/* ***************************************************************************
 * **** Ejecucion de la rutina de refrescamiento del LCD *********************
 * ***************************************************************************/
void lcd_show_part() {
	menuAnterior = menuImprimir;
	String newRelacion_IE;
	String calculatedRelacion_IE;
	if (newRelacionIE > 0) {
		newRelacion_IE = "1:" + String((float)newRelacionIE / 10, 1);
		calculatedRelacion_IE = "1:" + String((float)calculatedE / 10.0, 1);
	}
	else {
		newRelacion_IE = String(-(float)newRelacionIE / 10, 1) + ":1";
	}

	// Print the first line
	if (lineaAnterior != lineaAlerta) {
		lineaAnterior = lineaAlerta;
		lcdPrintFirstLine();
	}

	switch (menuImprimir) {
	case MAIN_MENU:
		//if (currentFrecRespiratoria != frecRespiratoriaAnte) {
		if (frecRespiratoriaCalculada != frecRespiratoriaAnte) {
			lcd.setCursor(4, 1);
			lcd.print(frecRespiratoriaCalculada);
			if (frecRespiratoriaCalculada < 10) {
				lcd.print(" ");
			}
			frecRespiratoriaAnte = frecRespiratoriaCalculada;
			// Serial.println("Changed freq");
		}
	
		
		if (Ppico != PpicoAnte) {
			lcd.setCursor(15, 1);
			lcd.print(String(int(Ppico)));
			if (Ppico < 10) {
				lcd.print(" ");
			}
			PpicoAnte = Ppico;
			// Serial.println("Changed Ppico");
		}
		
		if ((I != IAnte) || (calculatedE != EAnte)) {
			lcd.setCursor(4, 2);
			lcd.print(calculatedRelacion_IE + " ");
			IAnte = I;
			EAnte = calculatedE;
			// Serial.println("Changed IE");
		}

		if (currentVentilationMode == 0) {
			if (Pcon != PconAnte) {
				lcd.setCursor(15, 2);
				lcd.print(String(int(Pcon)));
				if (Pcon < 10)
				{
					lcd.print(" ");
				}
				PconAnte = Pcon;
			}
		}
		else {
			if (newVE != currentVE) {
				lcd.setCursor(15, 2);
				lcd.print(String(int(newVE)));
				if (newVE < 10) {
					lcd.print(" ");
				}
				currentVE = newVE;
				// Serial.println("Changed Ppico");
			}
		}
		if (Peep != PeepAnte) {
			lcd.setCursor(15, 3);
			lcd.print(String(int(Peep)));
			if (Peep < 10) {
				lcd.print(" ");
			}
			PeepAnte = Peep;
			// Serial.println("Changed Peep");
		}
		if (VT != VTAnte) {
			lcd.setCursor(4, 3);
			lcd.print(String(VT));
			if (VT < 10) {
				lcd.print("    ");
			}
			else if (VT < 100) {
				lcd.print("   ");
			}
			else if (VT < 9999) {
				lcd.print("  ");
			}
			VTAnte = VT;
			// Serial.println("Changed VT");
		}
		break;
	case CONFIG_MENU:
		lcd.setCursor(0, 0);
		if (optionConfigMenu == 0 && insideMenuFlag == true) {
			lcd.write(126);
		}
		else {
			lcd.print(' ');
		}
		if (currentVentilationMode == 1) {  // A/C Mode
			lcd.setCursor(0, 2);
			if (optionConfigMenu == 1) {
				lcd.write(126);
				lcd.setCursor(5, 3);
				if (flagTrigger == true) {
					lcd.write(60);
					lcd.setCursor(2, 3);
					lcd.print(trigger);
					if (trigger < 10) {
						lcd.print(' ');
					}
				}
				else {
					lcd.print(' ');
				}
			}
			else {
				lcd.print(' ');
			}
			lcd.setCursor(7, 2);
			if (optionConfigMenu == 2) { 
				lcd.write(126);
				lcd.setCursor(12, 3);
				if (flagFrecuencia == true) {
					lcd.write(60);
					if (newFrecRespiratoria != currentFrecRespiratoria) {
						lcd.setCursor(9, 3);
						lcd.print(newFrecRespiratoria);
						if (newFrecRespiratoria < 10) {
							lcd.print(" ");
						}
						frecRespiratoriaAnte = newFrecRespiratoria;
					}
				}
				else {
					lcd.print(' ');
				}
			}
			else {
				lcd.print(' ');
			}
			lcd.setCursor(14, 2);
			if (optionConfigMenu == 3) {
				lcd.write(126);
				lcd.setCursor(19, 3);
				if (flagIE == true) {
					lcd.write(60);
					if (newRelacionIE != currentRelacionIE) {
						lcd.setCursor(14, 3);
						lcd.print(newRelacion_IE);
						IAnte = I;
						EAnte = E;
					}
				}
				else {
					lcd.print(' ');
				}
			}
			else {
				lcd.print(' ');
			}
		}
		else if (currentVentilationMode == 2) { // CPAP Mode
			lcd.setCursor(13, 2);
			lcd.print(int(Peep));
			lcd.setCursor(1, 3);
			if (optionConfigMenu == 1) {
				lcd.write(126);
			}
			else {
				lcd.print(' ');
			}
			lcd.setCursor(13, 3);
			lcd.print(PeepMax);
			if (PeepMax < 10) {
				lcd.print(' ');
			}
			lcd.setCursor(16, 3);
			if (flagPeepMax == true) {
				lcd.write(60);
			}
			else {
				lcd.print(' ');
			}
		}
		else { // P-CMV
			lcd.setCursor(3, 2);
			if (optionConfigMenu == 1) {
				lcd.write(126);
				lcd.setCursor(8, 3);
				if (flagFrecuencia == true) {
					lcd.write(60);
					if (newFrecRespiratoria != currentFrecRespiratoria) {
						lcd.setCursor(5, 3);
						lcd.print(newFrecRespiratoria);
						if (newFrecRespiratoria < 10) {
							lcd.print(" ");
						}
						frecRespiratoriaAnte = newFrecRespiratoria;
					}
				}
				else {
					lcd.print(' ');
				}
			}
			else {
				lcd.print(' ');
			}
			lcd.setCursor(12, 2);
			if (optionConfigMenu == 2) {
				lcd.write(126);
				lcd.setCursor(18, 3);
				if (flagIE == true) {
					lcd.write(60);
					if (newRelacionIE != currentRelacionIE) {
						lcd.setCursor(12, 3);
						lcd.print(newRelacion_IE);
						IAnte = I;
						EAnte = E;
					}
				}
				else {
					lcd.print(' ');
				}
			}
			else {
				lcd.print(' ');
			}
		}
		break;
	case VENT_MENU:
		lcd.setCursor(0, 0);
		if (optionVentMenu == 0 && insideMenuFlag == true) {
			lcd.write(126);
		}
		else {
			lcd.print(' ');
		}
		lcd.setCursor(0, 2);
		if (optionVentMenu == 1) {
			lcd.write(126);
		}
		else {
			lcd.print(' ');
		}
		lcd.setCursor(8, 2);
		if (optionVentMenu == 2) {
			lcd.write(126);
		}
		else {
			lcd.print(' ');
		}
		lcd.setCursor(14, 2);
		if (optionVentMenu == 3) {
			lcd.write(126);
		}
		else {
			lcd.print(' ');
		}
		break;
	case CONFIG_ALARM:
		lcd.setCursor(0, 0);
		if (optionConfigMenu == 0 && insideMenuFlag == true) {
			lcd.write(126);
		}
		else
		{
			lcd.print(' ');
		}
		lcd.setCursor(0, 2);
		if (optionConfigMenu == 1) {
			lcd.write(126);
			lcd.setCursor(5, 3);
			if (flagPresion == true) {
				lcd.write(60);
				if (maxPresion != maxPresionAnte) {
					lcd.setCursor(2, 3);
					lcd.print(maxPresion);
					if (maxPresion < 10) {
						lcd.print(" ");
					}
					maxPresionAnte = maxPresion;
				}
			}
			else {
				lcd.print(' ');
			}
		}
		else {
			lcd.print(' ');
		}
		lcd.setCursor(7, 2);
		if (optionConfigMenu == 2) {
			lcd.write(126);
			lcd.setCursor(12, 3);
			if (flagMinFR == true) {
				lcd.write(60);
				if (maxFR != minFR_Ante) {
					lcd.setCursor(9, 3);
					lcd.print(maxFR);
					if (maxFR < 10) {
						lcd.print(" ");
					}
					minFR_Ante = maxFR;
				}
			}
			else {
				lcd.print(' ');
			}
		}
		else {
			lcd.print(' ');
		}
		lcd.setCursor(14, 2);
		if (optionConfigMenu == 3) {
			lcd.write(126);
			lcd.setCursor(19, 3);
			if (flagVE == true) {
				lcd.write(60);
				if (maxVE != minVE_Ante) {
					lcd.setCursor(16, 3);
					lcd.print(maxVE);
					if (maxVE < 10) {
						lcd.print(" ");
					}
					minVE_Ante = maxVE;
				}
			}
			else {
				lcd.print(' ');
			}
		}
		else {
			lcd.print(' ');
		}


		break;
	case CONFIRM_MENU:
		if (flagConfirm == true) {
			lcd.setCursor(3, 2);
			lcd.write(126);
			lcd.setCursor(13, 2);
			lcd.print(' ');
		}
		else {
			lcd.setCursor(3, 2);
			lcd.print(' ');
			lcd.setCursor(13, 2);
			lcd.write(126);
		}
		break;
	case CPAP_MENU:
		lcd.setCursor(0, 1);
		lcd.print("                    ");
		lcd.setCursor(0, 2);
		lcd.print("   PEEP CPAP =      ");
		lcd.setCursor(14, 2);
		lcd.print(String(Peep, 0));
		break;
	}
	//Serial.println("I am in lcd_show()");
}

void lcdPrintFirstLine() {
	lcd.setCursor(0, 0);
	switch (lineaAlerta) {
	case MAIN_MENU:
		if (stateMachine == STANDBY_STATE) {
			lcd.print("    Modo Standby    ");
		}
		else {
			lcd.print("  GIBIC Neuma ");
			if (currentVentilationMode == 0) {
				lcd.print("P-CMV ");
			}
			else if (currentVentilationMode == 1) {
				lcd.print("A/C   ");
			}
			else {
				lcd.print("CPAP  ");
			}
		}
		break;
	case CONFIG_MENU:
		if (currentVentilationMode == 0) {
			lcd.print(" Configuracion P-CMV");
		}
		else if (currentVentilationMode == 1) {
			lcd.print("  Configuracion A/C ");
		}
		else {
			lcd.print(" Configuracion CPAP ");
		}
		break;
	case CONFIG_ALARM:
		lcd.print("      Alarmas       ");
		break;
	case VENT_MENU:
		lcd.print(" Modo Ventilatorio ");
		if (insideMenuFlag == true && optionVentMenu == 0) {
			lcd.setCursor(0, 0);
			lcd.write(126);
		}
		break;
	case SERVICE_MENU:
		lcd.print("  Chequeo de fugas  ");
		break;
	case ALE_PRES_PIP:
		lcd.print("Presion PIP elevada ");
		break;
	case ALE_PRES_DES:
		lcd.print("Desconexion Paciente");
		break;
	case ALE_OBSTRUCCION:
		lcd.print("    Obstruccion     ");
		break;
	case ALE_GENERAL:
		lcd.print("   Fallo general   ");
		break;
	case ALE_PRES_PEEP:
		lcd.print("  Perdida de PEEP   ");
		break;
	case BATTERY:
		lcd.print(" Fallo red electrica");
		break;
	case ALE_BATTERY_10MIN:
		lcd.print("Bateria baja 10 Min");
		break;
	case ALE_BATTERY_5MIN:
		lcd.print(" Bateria baja 5 Min ");
		break;
	case ALE_FR_ALTA:
		lcd.print("      FR alta       ");
		break;
	case ALE_VE_ALTO:
		lcd.print("    Vol/min alto    ");
		break;
	case ALE_APNEA:
		lcd.print("       Apnea        ");
		break;
	case CHECK_MENU:
		lcd.print("Comprobacion Inicial");
		break;
	case CONFIRM_MENU:
		lcd.print(" Confirmar cambios  ");
		break;
	case CPAP_MENU:
		lcd.print(" Configuracion CPAP ");
		break;
	default:
		break;
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
			// Serial.println("Inside receiveData");
			String dataIn = Serial2.readStringUntil(';');
			// String dataIn = Serial.readStringUntil(';');  // solo para pruebas
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
			Ppico = dataIn2[0].toFloat();
			Peep = dataIn2[1].toFloat();
			Pcon = Ppico - Peep;
			VT = dataIn2[2].toInt();
			alerPresionPIP = dataIn2[3].toInt();
			alerDesconexion = dataIn2[4].toInt();
			alerObstruccion = dataIn2[5].toInt();
			alerPresionPeep = dataIn2[6].toInt();
			alerGeneral = dataIn2[7].toInt();
			frecRespiratoriaCalculada = dataIn2[8].toInt();
			calculatedE = dataIn2[9].toInt();
			alerFR_Alta = dataIn2[10].toInt();
			alerVE_Alto = dataIn2[11].toInt();
			newVE = dataIn2[12].toInt();
			Serial2.flush();
			//Serial.flush();  // solo para pruebas
			//Serial.println(VT);
		}
		vTaskDelay(10 / portTICK_PERIOD_MS);
	}
	//   vTaskDelete(NULL);
}

// envio de datos por serial para configuracion
void sendSerialData() {
	if (currentRelacionIE > 0) {
		relacion_IE = "1:" + String((float)currentRelacionIE / 10, 1);
		I = 1;
		E = (char)currentRelacionIE;
	}
	else {
		relacion_IE = String(-(float)currentRelacionIE / 10, 1) + ":1";
		I = (char)(-currentRelacionIE);
		E = 1;
	}
	String dataToSend = String(currentFrecRespiratoria) + ',' + String(I) + ',' +
		String(E) + ',' + String(maxPresion) + ',' + String(batteryAlert) + ',' +
		String(flagStabilityInterrupt) + ',' + String(stateMachine) + ',' +
		String(currentVentilationMode) + ',' + String(trigger) + ',' + 
		String(PeepMax) + ',' + String(maxFR) + ',' + String(maxVE) + ',' +
		String(apneaTime) + ';';
	Serial2.print(dataToSend);
	//Serial.println(stateMachine);
}

/* ***************************************************************************
 * **** Ejecucion de la rutina de refrescado de Display ++********************
 * ***************************************************************************/
void task_Display(void* pvParameters) {
	String taskMessage = "LCD Task one running on core ";
	taskMessage = taskMessage + xPortGetCoreID();
	// Serial.println(taskMessage);

	lcd_setup(); // inicializacion de LCD
	lcd.setCursor(0, 0);

	while (true) {
		/* *************************************************
		 * **** Analisis de los estados de alarma **********
		 * ************************************************/
		if (flagAlerta == true || flagBatteryAlert == true) {
			if (flagBatteryAlert == true) {
				contSilenceBattery++;
			}
			menuAlerta[0] = menu;
			contAlertas++;
			
			if (contAlertas == 3) {
				
				/*Serial.println(String(menuAlerta[0]) + ',' + String(menuAlerta[1]) + ',' +
					String(menuAlerta[2]) + ',' + String(menuAlerta[3]) + ',' +
					String(menuAlerta[4]) + ',' + String(menuAlerta[5]));
				Serial.println(temp);*/
				flagFirst = false;
				flagEntre = false;
				for (int i = 0; i < ALARM_QUANTITY + 1; i++) {
					if ((menuAlerta[i] != 0) && (i > temp) && (flagFirst == false)) {
						flagFirst = true;
						flagEntre = true;
						temp = i;
					}
					if (flagEntre == false && i == ALARM_QUANTITY) {
						temp = 0;
					}
				}

				//if (flagSilenceInterrupt == false || flagBatterySilence == false) {
				digitalWrite(BUZZER_BTN, LOW);
				//}
				if (flagAlerta == true) {
					digitalWrite(LUMINR, HIGH);
				}
				//digitalWrite(LUMING, HIGH);
				//digitalWrite(LUMINB, HIGH);
				digitalWrite(SILENCE_LED, HIGH);
			}
			else if (contAlertas == 10) {
				lineaAlerta = menuAlerta[temp];
				flagAlreadyPrint = false;
				if ((!flagSilenceInterrupt && flagAlerta) || (!flagBatterySilence && flagBatteryAlert)) {
					digitalWrite(BUZZER_BTN, HIGH);
				}
				else {
					digitalWrite(BUZZER_BTN, LOW);
				}
				digitalWrite(LUMINR, LOW);
				digitalWrite(SILENCE_LED, LOW);
			}
			else if (contAlertas == 13) {
				digitalWrite(BUZZER_BTN, LOW);
				digitalWrite(SILENCE_LED, HIGH);
				if (flagAlerta == true) {
					digitalWrite(LUMINR, HIGH);
				}
			}
			else if (contAlertas >= 20) {
				lineaAlerta = menu;
				flagAlreadyPrint = false;
				if ((!flagSilenceInterrupt && flagAlerta) || (!flagBatterySilence && flagBatteryAlert)) {
					digitalWrite(BUZZER_BTN, HIGH);
				}
				else {
					digitalWrite(BUZZER_BTN, LOW);
				}
				digitalWrite(LUMINR, LOW);
				digitalWrite(SILENCE_LED, LOW);
				contAlertas = 0;
			}
		}
		else {
			contAlertas++;
			if (contAlertas == 10) {
				lineaAlerta = menu;
				flagAlreadyPrint = false;
				digitalWrite(LUMINR, LOW);
				//digitalWrite(LUMING, LOW);
				digitalWrite(SILENCE_LED, LOW);
				digitalWrite(BUZZER_BTN, LOW);
				contSilence = 0;
				flagSilenceInterrupt = false;
			}
			else if (contAlertas >= 20) {
				lineaAlerta = menu;
				flagAlreadyPrint = false;
				contAlertas = 0;
			}
		}

		/* ****************************************************************
		 * **** Actualizacion de valores en pantalla LCD ******************
		 * ***************************************************************/
		if ((menuAnterior != menuImprimir) && (flagAlreadyPrint == false)) {
			lcd.clear();
			lineaAnterior = MODE_CHANGE;
			lcd_show_comp();
		}
		else {
			lcd_show_part();
		}
		// delay de 100 ms en la escritura del LCD
		vTaskDelay(100 / portTICK_PERIOD_MS);
	}
}

// Configuracion del LCD
void lcd_setup() {
	lcd.begin(21, 22);
	lcd.backlight();
}

/* ***************************************************************************
 * **** CONFIGURACION ********************************************************
 * ***************************************************************************/
void setup()
{
	Serial.begin(115200);
	Serial2.begin(115200);
	Serial2.setTimeout(10);
	init_GPIO();
	init_TIMER();

	// nvs_flash_init();

	// se crea el semaforo binario
	xSemaphoreEncoder = xSemaphoreCreateBinary();
	xSemaphoreTimer = xSemaphoreCreateBinary();

	// creo la tarea task_pulsador
	xTaskCreatePinnedToCore(task_Encoder, "task_Encoder", 2048, NULL, 4, NULL, taskCoreOne);
	// xTaskCreatePinnedToCore(task_Encoder_B, "task_Encoder_B", 10000, NULL, 1, NULL, taskCoreZero);

	xTaskCreatePinnedToCore(task_timer, "task_timer", 2048, NULL, 2, NULL, taskCoreOne);
	xTaskCreatePinnedToCore(task_Display, "task_Display", 2048, NULL, 3, NULL, taskCoreOne);  // se puede colocar en el core cero
	xTaskCreatePinnedToCore(task_Receive, "task_Receive", 2048, NULL, 1, NULL, taskCoreOne);

	// Clean Serial buffers
	vTaskDelay(1000 / portTICK_PERIOD_MS);
}

/* ***************************************************************************
* **** Atencion a las alertas ***********************************************
* ***************************************************************************/
void alarmMonitoring(void) {
	
	contLowAtten++;  // contador para generar atencion a bajas frecuencias
	
	// ejecuta tareas de baja prioridad en tiempo
	if (contLowAtten == LOW_ATT_INT) {
		contLowAtten = 0;

		newBatteryAlert = digitalRead(BATTALARM);
		switch (batteryAlert) {
		case BATTERY_NO_ALARM:
			if (newBatteryAlert == 0) { // Battery Alarm
				portENTER_CRITICAL_ISR(&mux);
				flagBatterySilence = false;
				portEXIT_CRITICAL_ISR(&mux);
				contSilenceBattery = 0;
				currentBatteryAlert = newBatteryAlert;
				batteryAlert = batteryAlarm;
				digitalWrite(LUMINB, HIGH);
				sendSerialData();
			}
			break;
		case batteryAlarm:
			if (newBatteryAlert == 1) {
				contBattery++;
			}
			else if (contBattery > 2 && contBattery < 50) {
				contBattery = 0;
				batteryAlert = batteryAlarm10min;
				sendSerialData();
			}
			if (contBattery > 100) {
				contBattery = 0;
				currentBatteryAlert = newBatteryAlert;
				batteryAlert = BATTERY_NO_ALARM;
				digitalWrite(LUMINB, LOW);
				sendSerialData();
			}
			break;
		case batteryAlarm10min:
			if (newBatteryAlert == 1) {
				contBattery++;
			}
			else {
				contBattery = 0;
			}
			if (contBattery > 100) {
				contBattery = 0;
				contBattery5min = 0;
				currentBatteryAlert = newBatteryAlert;
				batteryAlert = BATTERY_NO_ALARM;
				digitalWrite(LUMINB, LOW);
				sendSerialData();
			}
			contBattery5min++;
			if (contBattery5min > 6000) {
				contBattery = 0;
				contBattery5min = 0;
				batteryAlert = batteryAlarm5min;
			}
			break;
		case batteryAlarm5min:
			if (newBatteryAlert == 1) {
				contBattery++;
			}
			if (contBattery > 100) {
				contBattery = 0;
				currentBatteryAlert = newBatteryAlert;
				batteryAlert = BATTERY_NO_ALARM;
				digitalWrite(LUMINB, LOW);
				sendSerialData();
			}
			break;
		default:
			break;
		}

		// verifica la condicion de silenciar alarmas
		silenceInterruptAttention();
		// activacion alerta de bateria
		if (currentBatteryAlert == 0) {
			menuAlerta[8] = BATTERY;
			if (batteryAlert == 2) {
				menuAlerta[9] = ALE_BATTERY_10MIN;
			}
			else if (batteryAlert == 3) {
				menuAlerta[9] = ALE_BATTERY_5MIN;
				flagAlerta = true;
			}
			flagBatteryAlert = true;
		}
		else {
			menuAlerta[8] = 0;
			menuAlerta[9] = 0;
			flagBatteryAlert = false;
		}

		if ((alerVE_Alto == 1) && (stateMachine == AC_STATE)) {
			menuAlerta[7] = ALE_VE_ALTO;
			flagAlerta = true;
		}
		else {
			menuAlerta[7] = 0;
		}

		if ((alerFR_Alta == 1) && ((stateMachine == AC_STATE) || (stateMachine == CPAP_STATE))) {
			menuAlerta[6] = ALE_FR_ALTA;
			flagAlerta = true;
		}
		else if((alerFR_Alta == 2)){
			menuAlerta[6] = ALE_APNEA;
			if (stateMachine != STANDBY_STATE) {
				stateMachine = AC_STATE;
			}
			currentVentilationMode = 1;
			flagAlerta = true;
		}
		else {
			menuAlerta[6] = 0;
		}

		// activacion alerta general
		if (alerObstruccion == 1) {
			menuAlerta[5] = ALE_OBSTRUCCION;
			flagAlerta = true;
		}
		else {
			menuAlerta[5] = 0;
		}
		// activacion alerta presion alta
		if (alerPresionPIP == 1) {
			menuAlerta[4] = ALE_PRES_PIP;
			flagAlerta = true;
		}
		else {
			menuAlerta[4] = 0;
		}

		if (alerPresionPeep == 1) {
			digitalWrite(LUMING, LOW);
			menuAlerta[3] = ALE_PRES_PEEP;
			flagAlerta = true;
		}
		else {
			menuAlerta[3] = 0;

		}
		// activacion alerta desconexion
		if (alerDesconexion == 1) {
			menuAlerta[2] = ALE_PRES_DES;
			flagAlerta = true;
		}
		else {
			menuAlerta[2] = 0;
		}
		if (alerGeneral == 1) {
			flagAlerta = true;
			menuAlerta[1] = ALE_GENERAL;
			stateMachine = FAILURE_STATE;
			digitalWrite(STANDBY_LED, LOW);
		}
		else {
			menuAlerta[1] = 0;
		}
		// desactivacion alertas
		if ((alerPresionPIP == 0) && (alerDesconexion == 0) && (alerObstruccion == 0) &&
			(alerPresionPeep == 0) && (alerGeneral == 0) && (menuAlerta[9] != ALE_BATTERY_5MIN) && 
			(menuAlerta[7] == 0) && (menuAlerta[6] == 0)) {
			flagAlerta = false;
			for (int i = 1; i < ALARM_QUANTITY - 1; i++) {
				menuAlerta[i] = 0;
			}
			portENTER_CRITICAL_ISR(&mux);
			flagSilenceInterrupt = false;
			portEXIT_CRITICAL_ISR(&mux);
			contSilence = 0;
		}
	}
}

/* ***************************************************************************
 * **** LOOP MAIN_MENU *******************************************************
 * ***************************************************************************/
void loop() {

}

/* ***************************************************************************
 * **** FIN DEL PROGRAMA *****************************************************
 * ***************************************************************************/

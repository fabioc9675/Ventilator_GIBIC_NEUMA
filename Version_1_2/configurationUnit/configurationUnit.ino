/*
 * File:   configurationUnit.h
 * Author: GIBIC UdeA
 *
 * Created on July 4, 2020, 13:41 PM
 */

/** ****************************************************************************
 ** ************ INCLUDES ******************************************************
 ** ****************************************************************************/
#include <Arduino.h>
#include <Esp.h>
#include <nvs_flash.h>
#include <stdio.h>
#include <Wire.h>
#include "LiquidCrystal_I2C.h"

#include "initializer.h"
#include "timer.h"
#include "interruption.h"
#include "encoder.h"
#include "serialCONT.h"



/** ****************************************************************************
 ** ************ DEFINES *******************************************************
 ** ****************************************************************************/

//********DEFINICIONES CONDICIONES******
#define TRUE 1
#define FALSE 0


/** ****************************************************************************
 ** ************ EXTERN VARIABLES **********************************************
 ** ****************************************************************************/
// definiciones para el timer
extern hw_timer_t *timer;
extern portMUX_TYPE timerMux;
extern portMUX_TYPE mux;

// Variable de estado del encoder
extern unsigned int fl_StateEncoder ;

// variables de menu
extern volatile signed int menu;



/** ****************************************************************************
 ** ************ VARIABLES *****************************************************
 ** ****************************************************************************/
//LiquidCrystal_I2C lcd(0x27, 20, 4);
LiquidCrystal_I2C lcd(0x3F, 20, 4);



// definicion de los core para ejecucion
uint8_t taskCoreZero = 0;
uint8_t taskCoreOne = 1;


// manejadores para los semaforos binarios
SemaphoreHandle_t xSemaphoreEncoder = NULL;
SemaphoreHandle_t xSemaphoreTimer = NULL;


// bandera de activacion de timer
volatile uint8_t flagTimerInterrupt = false;

// bandera de activacion de encoder
volatile uint8_t flagAEncoder = false;
volatile uint8_t flagBEncoder = false;
volatile uint8_t flagSEncoder = false;

// banderas de botones de usuario
volatile uint8_t flagStandbyInterrupt = true;   // inicia en modo standby
volatile uint8_t flagSilenceInterrupt = false;
volatile uint8_t flagStabilityInterrupt = false;
volatile uint8_t flagBatterySilence = false;
volatile uint8_t flagAlerta = false;
volatile uint8_t flagBatteryAlert = false;


// banderas de cambio de valores 
volatile uint8_t  flagFrecuencia = false;
volatile uint8_t  flagPresion = false;
volatile uint8_t  flagFlujo = false;
volatile uint8_t  flagTrigger = false;
volatile uint8_t  flagIE = false;
volatile uint8_t  flagPeepMax = false;
volatile uint8_t  flagSensibilityCPAP = false;
volatile uint8_t  flagMode = false;
volatile uint8_t  flagConfirm = false;
volatile uint8_t  flagMinFR = false;
volatile uint8_t  flagVE = false;


// contadores de configuraciones en el menu de usuario
String relacion_IE = "1:2.0";

int newFrecRespiratoria = 12;
int newRelacionIE = 20;
int currentFrecRespiratoria = 12;
int currentRelacionIE = 20;
int trigger = 2;
int PeepMax = 10;
int maxPresion = 30;
int maxFR = 30;
int maxVE = 30;

int batteryAlert = BATTERY_NO_ALARM;

// Variables calculadas
int frecRespiratoriaCalculada = 0;
int I = 1;
int E = 20;
int calculatedE = E;
int apneaTime = 20;


// variables de introduccion a los menus de configuracion
int optionConfigMenu = 0;
int currentVentilationMode = 0;
int optionVentMenu = 0;


// variable de estado de menu
volatile uint8_t  insideMenuFlag = false;


// variables de estado de ventilacion
int stateMachine = STANDBY_STATE;
int newVentilationMode = 0;

















volatile bool flagDetachInterrupt_A = false;
volatile bool flagDetachInterrupt_B = false;
volatile bool flagDetachInterrupt_A_B = false;
volatile bool flagDetachInterrupt_B_A = false;
volatile bool flagDetachInterrupt_S = false;
volatile bool flagDetach = false;
volatile bool flagService = false;
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


unsigned int contStandby = 0;

// variables de menu

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




bool flagChange2AC = false;
unsigned int contAlertas = 0;

// Global al ser usada en loop e ISR (encoder)










float maxFlujo = 4;

// Banderas utilizadas en las interrupciones


int flagEncoder = 0;




byte currentVE = 0;
byte newVE = 0;

float Peep = 0;

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







//creo el manejador para el semaforo como variable global

//xQueueHandle timer_queue = NULL;





unsigned int temp = 0;
bool flagFirst = false;
bool flagEntre = false;


// definicion de interrupciones
void IRAM_ATTR standbyButtonInterrupt(void);
void IRAM_ATTR silenceButtonInterrupt(void);
void IRAM_ATTR stabilityButtonInterrupt(void);



/************************************************************
 ********** FUNCIONES DE INICIALIZACION *********************
 ***********************************************************/




/* *********************************************************************
 * **** FUNCIONES DE ATENCION A INTERRUPCION ***************************
 * *********************************************************************/
 



/************************************************************
 ***** FUNCIONES DE ATENCION A INTERRUPCION TAREA TIMER *****
 ************************************************************/
void task_timer(void* arg) {
	int contms = 0;
	uint16_t debounceENC = 0;
	uint16_t debounceENC_2 = 0;

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
			// Serial.println(digitalRead(STANDBY));
			if (contStandby < 3000 && digitalRead(STANDBY) == 1) {
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
			lcd.print(String(currentVE/10.0, 1));
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
			lcd.print("  CPAP MIN:         ");
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
				lcd.print(String(newVE/10.0, 1));
				if (newVE < 100) {
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
					//if (newFrecRespiratoria != currentFrecRespiratoria) {
						lcd.setCursor(9, 3);
						lcd.print(newFrecRespiratoria);
						if (newFrecRespiratoria < 10) {
							lcd.print(" ");
						}
						frecRespiratoriaAnte = newFrecRespiratoria;
					//}
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
					//if (newRelacionIE != currentRelacionIE) {
						lcd.setCursor(14, 3);
						lcd.print(newRelacion_IE);
						IAnte = I;
						EAnte = E;
					//}
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
					//if (newFrecRespiratoria != currentFrecRespiratoria) {
						lcd.setCursor(5, 3);
						lcd.print(newFrecRespiratoria);
						if (newFrecRespiratoria < 10) {
							lcd.print(" ");
						}
						frecRespiratoriaAnte = newFrecRespiratoria;
					//}
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
					//if (newRelacionIE != currentRelacionIE) {
						lcd.setCursor(12, 3);
						lcd.print(newRelacion_IE);
						IAnte = I;
						EAnte = E;
					//}
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
		lcd.print(" Modo Ventilatorio  ");
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
		}
		vTaskDelay(10 / portTICK_PERIOD_MS);
	}
	//   vTaskDelete(NULL);
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
		if(flagService == false){
			if ((menuAnterior != menuImprimir) && (flagAlreadyPrint == false)) {
				lcd.clear();
				lineaAnterior = MODE_CHANGE;
				lcd_show_comp();
			}
			else {
				lcd_show_part();
			}
		}
		// delay de 100 ms en la escritura del LCD
		vTaskDelay(100 / portTICK_PERIOD_MS);
	}
}

// Configuracion del LCD
void lcd_setup() {
	lcd.begin(21, 22);
	lcd.backlight();

	if (flagService == true){
		lcd.setCursor(0, 0);
		lcd.print("   MODO SERVICIO    ");
		lcd.setCursor(0, 2);
		lcd.print("AL FINALIZAR INICIE ");
		lcd.setCursor(0, 3);
		lcd.print("EL EQUIPO DE NUEVO  ");
	}
}

/* ***************************************************************************
 * **** CONFIGURACION ********************************************************
 * ***************************************************************************/
void setup()
{
	Serial.begin(115200);
	Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
	Serial2.setTimeout(10);
	init_GPIO();
	

	// nvs_flash_init();

	// se crea el semaforo binario
	xSemaphoreEncoder = xSemaphoreCreateBinary();
	xSemaphoreTimer = xSemaphoreCreateBinary();

	// modo servicio de fabrica
	if ((digitalRead(SILENCE_BTN) == LOW) && (digitalRead(STABILITY_BTN) == LOW)){
		while(digitalRead(SILENCE_BTN) == LOW)
		{
			flagService = true;
			Serial2.print('M');
			vTaskDelay(50 / portTICK_PERIOD_MS);
		} 
	} else if (digitalRead(SILENCE_BTN) == LOW){ // Modo servicio de fabrica
		while(digitalRead(SILENCE_BTN) == LOW)
		{
			flagService = true;
			Serial2.print('S');
			vTaskDelay(50 / portTICK_PERIOD_MS);
		} 
	}

	init_Timer();


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
			if (stateMachine != STANDBY_STATE && flagChange2AC == false) {
				flagChange2AC = true;
				stateMachine = AC_STATE;
				insideMenuFlag = false;
				flagPeepMax = false;
				optionConfigMenu = 0;
				menu = VENT_MENU;
				menuImprimir = menu;
				lineaAlerta = menu;
				flagAlreadyPrint = false;
			}
			currentVentilationMode = 1;
			flagAlerta = true;
		}
		else {
			menuAlerta[6] = 0;
			flagChange2AC = false;
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

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
#include "lcdComplete.h"
#include "lcdPartial.h"
#include "lcdConfig.h"

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
extern unsigned int fl_StateEncoder;

// variables de menu
extern volatile signed int menu;

/** ****************************************************************************
 ** ************ VARIABLES *****************************************************
 ** ****************************************************************************/
// LiquidCrystal_I2C lcd(0x27, 20, 4);
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

volatile uint8_t flagDetachInterrupt_A = false;
volatile uint8_t flagDetachInterrupt_B = false;
volatile uint8_t flagDetachInterrupt_A_B = false;
volatile uint8_t flagDetachInterrupt_B_A = false;
volatile uint8_t flagDetachInterrupt_S = false;

unsigned int contDetachA = 0;
unsigned int contDetachB = 0;
unsigned int contDetachS = 0;
unsigned int contStandby = 0;

// banderas de botones de usuario
volatile uint8_t flagStandbyInterrupt = true; // inicia en modo standby
volatile uint8_t flagSilenceInterrupt = false;
volatile uint8_t flagStabilityInterrupt = false;
volatile uint8_t flagBatterySilence = false;
volatile uint8_t flagAlerta = false;
volatile uint8_t flagBatteryAlert = false;

// banderas de cambio de valores
volatile uint8_t flagFrecuencia = false;
volatile uint8_t flagPresion = false;
volatile uint8_t flagFlujo = false;
volatile uint8_t flagTrigger = false;
volatile uint8_t flagIE = false;
volatile uint8_t flagPeepMax = false;
volatile uint8_t flagSensibilityCPAP = false;
volatile uint8_t flagMode = false;
volatile uint8_t flagConfirm = false;
volatile uint8_t flagMinFR = false;
volatile uint8_t flagVE = false;
volatile uint8_t flagLeakage = false;

// contadores de configuraciones en el menu de usuario
String relacion_IE = "1:2.0";

byte newFrecRespiratoria = 12;
int newRelacionIE = 20;
byte currentFrecRespiratoria = 12;
int currentRelacionIE = 20;
byte trigger = 2;
byte PeepMax = 10;
byte maxPresion = 30;
byte maxFR = 30;
byte maxVE = 30;

byte batteryAlert = BATTERY_NO_ALARM;

// Variables calculadas
byte frecRespiratoriaCalculada = 0;
byte I = 1;
byte E = 20;
int calculatedE = E;
byte apneaTime = 20;

// variables de introduccion a los menus de configuracion
byte optionConfigMenu = 0;
byte currentVentilationMode = 0;
byte optionVentMenu = 0;

// variable de estado de menu
volatile uint8_t insideMenuFlag = false;

// variables de estado de ventilacion
byte stateMachine = STANDBY_STATE;
byte newVentilationMode = 0;

// variables de menu
volatile unsigned int menuImprimir = MAIN_MENU;
volatile unsigned int lineaAlerta = MAIN_MENU;
volatile uint8_t flagAlreadyPrint = false;
int menuAlerta[ALARM_QUANTITY + 1] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

// variables contadores de silencio y alarmas
unsigned int contSilence = 0;
unsigned int contSilenceBattery = 0;

// Variables para menu actualizado
float Peep = 0;
float Ppico = 0;
float Pcon = 0;
byte currentVE = 0;
unsigned int VT = 0;
byte newVE = 0;
int presPac = 0;
int flowPac = 0;
int presPacAnte = 0;
int flowPacAnte = 0;

// variables de alerta
int alerPresionPIP = 0;
int alerDesconexion = 0;
int alerObstruccion = 0;
int alerGeneral = 0;
int alerPresionPeep = 0;
int alerFR_Alta = 0;
int alerVE_Alto = 0;
volatile uint8_t flagToACBackUp = 0;

// Variables para menu anterior
volatile unsigned int menuAnterior = CONFIRM_MENU;	// valor de menu anterior
volatile unsigned int lineaAnterior = CONFIRM_MENU; // valor de menu anterior
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

volatile bool flagDetach = false;
volatile bool flagService = false;
unsigned int contAlarm = 0;

float maxFlujo = 4;

int flagEncoder = 0;

byte sensibilityCPAP = 0;

//creo el manejador para el semaforo como variable global

//xQueueHandle timer_queue = NULL;

/** ****************************************************************************
 ** ************ FUNCTIONS *****************************************************
 ** ****************************************************************************/

/* ***************************************************************************
 * **** CONFIGURACION ********************************************************
 * ***************************************************************************/
void setup()
{
	init_GPIO();
	Serial.begin(115200);
	Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
	Serial2.setTimeout(10);
	
	vTaskDelay(100 / portTICK_PERIOD_MS);

	// nvs_flash_init();

	// modo servicio de fabrica
	if ((digitalRead(SILENCE_BTN) == LOW) && (digitalRead(STABILITY_BTN) == LOW))
	{
		while (digitalRead(SILENCE_BTN) == LOW)
		{
			flagService = true;
			Serial2.print('M');
			vTaskDelay(50 / portTICK_PERIOD_MS);
		}
	}
	else if (digitalRead(SILENCE_BTN) == LOW)
	{ // Modo servicio de fabrica
		while (digitalRead(SILENCE_BTN) == LOW)
		{
			flagService = true;
			Serial2.print('S');
			vTaskDelay(50 / portTICK_PERIOD_MS);
		}
	}

	
	// se crea el semaforo binario
	xSemaphoreEncoder = xSemaphoreCreateBinary();
	xSemaphoreTimer = xSemaphoreCreateBinary();
	init_Timer();

	// creo la tarea task_pulsador
	xTaskCreatePinnedToCore(task_Encoder, "task_Encoder", 2048, NULL, 4, NULL, taskCoreOne);
	// xTaskCreatePinnedToCore(task_Encoder_B, "task_Encoder_B", 10000, NULL, 1, NULL, taskCoreZero);

	xTaskCreatePinnedToCore(task_timer, "task_timer", 2048, NULL, 2, NULL, taskCoreOne);
	xTaskCreatePinnedToCore(task_Display, "task_Display", 2048, NULL, 3, NULL, taskCoreOne); // se puede colocar en el core cero
	xTaskCreatePinnedToCore(task_Receive, "task_Receive", 2048, NULL, 1, NULL, taskCoreOne);

	// Clean Serial buffers
	vTaskDelay(1000 / portTICK_PERIOD_MS);
}

/* ***************************************************************************
 * **** LOOP MAIN_MENU *******************************************************
 * ***************************************************************************/
void loop()
{
}

/* ***************************************************************************
 * **** FIN DEL PROGRAMA *****************************************************
 * ***************************************************************************/

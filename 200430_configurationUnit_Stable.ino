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

#define BUZZER          12
#define SILENCE         26  // Silenciar alarma cambiar
#define SILENCE_LED     27  // Led Boton silencio
#define STABILITY       32
#define STABILITY_LED   33
#define STANDBY         34  // Stabdby button
#define STANDBY_LED     35  // Stabdby button

#define LUMINR      13  // Alarma luminosa
#define LUMING      14
#define LUMINB      15
#define BATTALARM   4

#define LED         2

#define ESP_INTR_FLAG_DEFAULT 0

#define DEBOUNCE_ENC            200  // tiempo para realizar antirrebote
#define DEBOUNCE_ENC_2          250  // tiempo para realizar antirrebote
#define DEBOUNCE_ENC_OUT        450  // tiempo para realizar antirrebote
#define DEBOUNCE_ENC_OUT_2      600  // tiempo para realizar antirrebote
#define DEBOUNCE_ENC_SW         400  // tiempo para realizar antirrebote
#define LOW_ATT_INT             50  // Interrupcion cada 10 ms

#define ENCOD_INCREASE          1  // movimiento a la derecha, aumento
#define ENCOD_DECREASE          2  // movimiento a la derecha, aumento

//**********VALORES MAXIMOS**********
#define MENU_QUANTITY       3
#define MAX_FREC            35
#define MIN_FREC            6
#define MAX_RIE             40
#define MAX_PRESION         50
#define MAX_FLUJO           40
#define SILENCE_TIME        2*60*1000/LOW_ATT_INT  // tiempo, 2 minutos a 20 Hz
#define SILENCE_BATTERY     30*60*1000/LOW_ATT_INT

// Casos Menu
#define MAIN_MENU           0  // Menu principal
#define CONFIG_MENU         1  // Configuracion de frecuencias
#define CONFIG_ALARM        2  // Configuracion Alarma
#define ALERT_STANDBY       3
#define ALE_PRES_PIP        4  // presion pico
#define ALE_PRES_DES        5  // desconexion del paciente
#define ALE_GENERAL         6  // fallo general
#define ALE_PRES_PEEP       11 // Perdida de Peep
#define BATTERY             7  // Bateria
#define CHECK_MENU          8  // Show in check state
#define CONFIRM_MENU	    9
#define CPAP_MENU           10

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

unsigned int contSilence = 0;
unsigned int contSilenceBattery = 0;
unsigned int contBattery = 0;
unsigned int contBattery5min = 0;
unsigned int fl_StateEncoder = 0;

bool flagStandbyInterrupt = false;
unsigned int contStandby = 0;

// variables de menu
volatile signed int menu = MAIN_MENU;
volatile unsigned int menuImprimir = MAIN_MENU;
volatile unsigned int menuAlerta = MAIN_MENU;
volatile uint8_t flagAlreadyPrint = false;

// Variables para menu anterior
volatile unsigned int menuAnterior = CONFIRM_MENU;  // valor de menu anterior
byte IAnte = 1;
byte EAnte = 1;
float PpicoAnte = 1;
float PeepAnte = 0;
float PconAnte = 0;
float VTAnte = 1;
byte maxPresionAnte = 0;
byte frecRespiratoriaAnte = 0;

bool flagAlerta = false;
bool flagBatteryAlert = false;
unsigned int contAlertas = 0;

// Global al ser usada en loop e ISR (encoder)
int currentRelacionIE = 20;
int newRelacionIE = 20;
String relacion_IE = "1:2.0";
byte currentFrecRespiratoria = 12;
byte newFrecRespiratoria = 12;
byte I = 1;
byte E = 20;
byte maxPresion = 30;
float maxFlujo = 4;

// Banderas utilizadas en las interrupciones
boolean insideMenuFlag = false;
boolean flagPresion = false;
boolean flagFlujo = false;
boolean flagFrecuencia = false;
boolean flagIE = false;
bool flagMode = false;
bool flagConfirm = false;

int flagEncoder = 0;

byte currentVentilationMode = 0;
byte newVentilationMode = 0;

float Peep = 0;
float Ppico = 0;
float Pcon = 0;
float VT = 0;

int alerPresionPIP = 0;
int alerDesconexion = 0;
int alerGeneral = 0;
int alerPresionPeep = 0;
int currentBatteryAlert = 1;        // When is working with energy supply
int newBatteryAlert = 1;


// State Machine
#define CHECK_STATE		0
#define STANDBY_STATE	1
#define CYCLING_STATE	2
#define FAILURE_STATE	3
unsigned int stateMachine = STANDBY_STATE;

#define batteryNoAlarm      0
#define batteryAlarm        1
#define batteryAlarm10min   2
#define batteryAlarm5min    3
byte batteryAlert = batteryNoAlarm;

//creo el manejador para el semáforo como variable global
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

/* ***********************************************************
 * **** FUNCIONES DE INICIALIZACION **************************
 * ***********************************************************/
void init_GPIO() {
    //Encoder setup
    pinMode(A, INPUT_PULLUP);    // A como entrada
    pinMode(B, INPUT_PULLUP);    // B como entrada
    pinMode(SW, INPUT_PULLUP);   // SW como entrada

    pinMode(SILENCE, INPUT_PULLUP); // switch para el manejo de silencio
    pinMode(STANDBY, INPUT_PULLUP);
    pinMode(STABILITY, INPUT_PULLUP);
    pinMode(BUZZER, OUTPUT);
    pinMode(LUMINR, OUTPUT);
    pinMode(LUMING, OUTPUT);
    pinMode(LUMINB, OUTPUT);
    pinMode(SILENCE_LED, OUTPUT);

    pinMode(BATTALARM, INPUT);

    attachInterrupt(digitalPinToInterrupt(A), encoderInterrupt_A, FALLING);
    attachInterrupt(digitalPinToInterrupt(B), encoderInterrupt_B, FALLING);
    attachInterrupt(digitalPinToInterrupt(SW), swInterrupt, RISING);
    attachInterrupt(digitalPinToInterrupt(STANDBY), standbyButtonInterrupt, FALLING);
    attachInterrupt(digitalPinToInterrupt(SILENCE), silenceButtonInterrupt, FALLING);
    attachInterrupt(digitalPinToInterrupt(STABILITY), stabilityButtonInterrupt, FALLING);

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
    // da el semáforo para que quede libre para la tarea pulsador
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
    if (currentBatteryAlert == 0) {
        portENTER_CRITICAL_ISR(&mux);
        flagBatterySilence = true;
        portEXIT_CRITICAL_ISR(&mux);
    }
    else {
        portENTER_CRITICAL_ISR(&mux);
        flagSilenceInterrupt = true;
        portEXIT_CRITICAL_ISR(&mux);
    }
}

void IRAM_ATTR stabilityButtonInterrupt(void) {
        portENTER_CRITICAL_ISR(&mux);
        flagStabilityInterrupt = true;
        detachInterrupt(digitalPinToInterrupt(STABILITY));
        portEXIT_CRITICAL_ISR(&mux);
}

/************************************************************
 ***** FUNCIONES DE ATENCION A INTERRUPCION TAREA TIMER *****
 ************************************************************/
void task_timer(void* arg) {
    int contms = 0;
    int contLowAtten = 0;
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
                case CYCLING_STATE:
                    //alertMonitoring();
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
                    sendSerialData();

                    portENTER_CRITICAL(&timerMux);
                    flagStabilityInterrupt = false;
                    attachInterrupt(digitalPinToInterrupt(STABILITY), stabilityButtonInterrupt, FALLING);
                    portEXIT_CRITICAL(&timerMux);
                    
                }

                /* ***************************************************************************
                 * **** Atencion a las alertas ***********************************************
                 * ***************************************************************************/
                contLowAtten++; // contador para generar atencion a bajas frecuencias

                // ejecuta tareas de baja prioridad en tiempo
                if (contLowAtten == LOW_ATT_INT) {
                    contLowAtten = 0;
                    
                    newBatteryAlert = digitalRead(BATTALARM);
                    //Serial.println(newBatteryAlert);
                    switch (batteryAlert) {
                    case batteryNoAlarm:
                        if (newBatteryAlert == 0) { // Battery Alarm
                            currentBatteryAlert = newBatteryAlert;
                            batteryAlert = batteryAlarm;
                            sendSerialData();
                            //Serial.println("Battery Alarm");
                        }
                        break;
                    case batteryAlarm:
                        if (newBatteryAlert == 1) {
                            contBattery++;
                        }
                        else {
                            if (contBattery > 2 && contBattery < 50) {
                                currentBatteryAlert = newBatteryAlert;
                                contBattery = 0;
                                batteryAlert = batteryAlarm10min;
                                sendSerialData();
                                //Serial.println("Battery 10 Min");
                            }
                        }
                        if (contBattery > 100) {
                            currentBatteryAlert = newBatteryAlert;
                            contBattery = 0;
                            batteryAlert = batteryNoAlarm;
                            //Serial.println("Battery normal");
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
                            currentBatteryAlert = newBatteryAlert;
                            contBattery = 0;
                            batteryAlert = batteryNoAlarm;
                            //Serial.println("Battery normal");
                            sendSerialData();
                        }
                        contBattery5min++;
                        if (contBattery5min > 6000) {
                            currentBatteryAlert = newBatteryAlert;
                            contBattery = 0;
                            contBattery5min = 0;
                            batteryAlert = batteryAlarm5min; 
                            //Serial.println("Battery 5 Min");
                        }
                        break;
                    case batteryAlarm5min:
                        if (newBatteryAlert == 1) {
                            contBattery++;
                        }
                        if (contBattery > 100) {
                            contBattery = 0;
                            currentBatteryAlert = newBatteryAlert;
                            batteryAlert = batteryNoAlarm;
                            //Serial.println("Battery normal");
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
                        menuAlerta = BATTERY;
                        flagBatteryAlert = true;
                    }
                    else {
                        menuAlerta = 0;
                        flagBatteryAlert = false;
                    }
                    // activacion alerta general
                    if (alerGeneral == 1) {
                        menuAlerta = ALE_GENERAL;
                        flagAlerta = true;
                    }
                    // activacion alerta presion alta
                    if (alerPresionPIP == 1) {
                        menuAlerta = ALE_PRES_PIP;
                        flagAlerta = true;
                    }
                    if (alerPresionPeep == 1) {
                        menuAlerta = ALE_PRES_PEEP;
                        flagAlerta = true;
                    }
                    // activacion alerta desconexion
                    if (alerDesconexion == 1) {
                        menuAlerta = ALE_PRES_DES;
                        flagAlerta = true;
                    }
                    // desactivacion alertas
                    if ((alerPresionPIP == 0) && (alerDesconexion == 0) && 
                        (alerGeneral == 0) && (alerPresionPeep == 0) && (currentBatteryAlert == 1)) {
                        menuAlerta = 0;
                        flagAlerta = false;
                    }
                }
            }
        }
    }
}

 /* ***************************************************************************
  * **** Atencion a interrupcion por encoder **********************************
  * ***************************************************************************/
void task_Encoder(void* arg) {
    while (1) {
        // Espero por la notificación de la ISR por A
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
                    // ejecucion de la tarea de incremento
                    encoderRoutine();
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
                    // ejecucion de la tarea de decremento
                    encoderRoutine();
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
                    newRelacionIE = newRelacionIE + 1;
                    if (newRelacionIE >= MAX_RIE) {
                        newRelacionIE = MAX_RIE;;
                    }
                    if (newRelacionIE > -10 && newRelacionIE < 0) {
                        newRelacionIE = 10;
                    }
                }
                else if (flagMode == true) {
                    newVentilationMode ++;
                    if (newVentilationMode > 2) {
                        newVentilationMode = 0;
                    }
                }
                break;
            case CONFIG_ALARM:
                if (flagPresion) {
                    maxPresion++;
                    if (maxPresion > MAX_PRESION) {
                        maxPresion = MAX_PRESION;
                    }
                }
                else if (flagFlujo) {
                    maxFlujo++;
                    if (maxFlujo > MAX_FLUJO) {
                        maxFlujo = MAX_FLUJO;
                    }
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
                    newRelacionIE = newRelacionIE - 1;
                    if (newRelacionIE <= -MAX_RIE) {
                        newRelacionIE = -MAX_RIE;
                    }
                    if (newRelacionIE > 0 && newRelacionIE < 10) {
                        newRelacionIE = -10;
                    }
                }
                else if (flagMode == true) {
                    newVentilationMode --;
                    if (newVentilationMode > 2) {
                        newVentilationMode = 2;
                    }
                }
                break;
            case CONFIG_ALARM:
                if (flagPresion) {
                    maxPresion--;
                    if (maxPresion > MAX_PRESION) {
                        maxPresion = 0;
                    }
                }
                else if (flagFlujo) {
                    maxFlujo--;
                    if (maxFlujo > MAX_FLUJO) {
                        maxFlujo = MAX_FLUJO;
                    }
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
    flagAlreadyPrint = false;

}

/* ***************************************************************************
 * **** Ejecucion de tarea de Switch *****************************************
 * ***************************************************************************/
void switchRoutine() {
    // Serial.println("SW MENU");
    if (menu == CONFIG_MENU && !insideMenuFlag) {
        insideMenuFlag = !insideMenuFlag;
        flagFrecuencia = true;
        // Serial.println("SW MENU 1_1");
    }
    else if (menu == CONFIG_MENU && flagFrecuencia) {
        flagFrecuencia = false;
        flagIE = true;
        // Serial.println("SW MENU 1_2");
    }
    else if (menu == CONFIG_MENU && flagIE) {
        flagIE = false;
        flagMode = true;
    }
    else if (menu == CONFIG_MENU && flagMode) {
        flagMode = false;
        if (newVentilationMode == 2) {
            menu = CPAP_MENU;
        }
        else {
            menu = CONFIRM_MENU;
        }
    }
    else if (menu == CPAP_MENU) {
        menu = CONFIRM_MENU;
    }
    else if (menu == CONFIRM_MENU) {
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
    }
    else if (menu == CONFIG_ALARM && !insideMenuFlag) {
        insideMenuFlag = !insideMenuFlag;
        flagPresion = true;
        //Serial.println("Config maxPres");
    }
    else if (menu == CONFIG_ALARM && flagPresion) {
        flagPresion = false;
        //flagFlujo = true;
        insideMenuFlag = !insideMenuFlag;
        //Serial.println("Config maxFlujo");
        sendSerialData();
    }
    menuImprimir = menu;
    flagAlreadyPrint = false;
}

/* ***************************************************************************
 * **** Atencion a interrupcion por buttons **********************************
 * ***************************************************************************/
void silenceInterruptAttention() {
    if (flagSilenceInterrupt) {
        digitalWrite(BUZZER, LOW);
        contSilence++;
        if (contSilence > SILENCE_TIME) {
            portENTER_CRITICAL(&mux);
            flagSilenceInterrupt = false;
            portEXIT_CRITICAL(&mux);
            
            contSilence = 0;
        }
    }

    if (flagBatterySilence) {
        digitalWrite(BUZZER, LOW);
        contSilenceBattery++;
        if (contSilenceBattery > SILENCE_BATTERY) {
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
        if (contStandby > 400) {            
            portENTER_CRITICAL(&mux);
            attachInterrupt(digitalPinToInterrupt(STANDBY), standbyButtonInterrupt, FALLING);
            flagStandbyInterrupt = false;
            portEXIT_CRITICAL(&mux);

            contStandby = 0;
            if (stateMachine == STANDBY_STATE) {
                stateMachine = CYCLING_STATE;
                //Serial.println("I am on Cycling state");
            }
            else {
                stateMachine = STANDBY_STATE;
                //Serial.println("I am on Standby state");
            }
            //sendSerialData();
        }
    }
}

/************************************************************
 ***** Ejecucion de la rutina de refrescamiento del LCD *****
 ***********************************************************/
void lcd_show_comp() {

    menuAnterior = menuImprimir;
    flagAlreadyPrint = true;

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

    switch (menuImprimir) {
    case MAIN_MENU:
        // lcd.home();
        lcd.setCursor(0, 0);
        if (stateMachine == STANDBY_STATE) {
            lcd.print("    Modo Standby    ");
        }
        else {
            lcd.print("  InnspiraMED UdeA  ");
        }
        lcd.setCursor(0, 1);
        lcd.print("FR        PIP     ");
        lcd.setCursor(4, 1);
        lcd.print(currentFrecRespiratoria);
        lcd.setCursor(14, 1);
        lcd.print(String(Ppico, 0));
        lcd.setCursor(0, 2);
        lcd.print("I:E       PCON      ");
        lcd.setCursor(4, 2);
        lcd.print(relacion_IE);
        lcd.setCursor(15, 2);
        lcd.print(String(Pcon, 0));
        lcd.setCursor(0, 3);
        lcd.print("VT        PEEP      ");
        lcd.setCursor(4, 3);
        lcd.print(String(VT, 0));
        lcd.setCursor(15, 3);
        lcd.print(String(Peep, 0));

        frecRespiratoriaAnte = currentFrecRespiratoria;
        PpicoAnte = Ppico;
        IAnte = I;
        EAnte = E;
        PeepAnte = Peep;
        VTAnte = VT;
        PconAnte = Pcon;
        break;
    case CONFIG_MENU:
        lcd.setCursor(0, 0);
        lcd.print("   Configuracion    ");
        lcd.setCursor(0, 1);
        lcd.print("                    ");
        lcd.setCursor(0, 2);
        lcd.print(" Frec:              ");
        lcd.setCursor(7, 2);
        lcd.print(currentFrecRespiratoria);
        lcd.setCursor(12, 2);
        lcd.write(124);
        if (flagMode == true) {
            lcd.write(126);
            lcd.print("Modo: ");
        }
        else {
            lcd.print(" Modo: ");
        }
        lcd.setCursor(0, 3);
        lcd.print(" I:E:              ");
        lcd.setCursor(6, 3);
        lcd.print(relacion_IE);
        lcd.setCursor(12, 3);
        lcd.write(124);
        if (currentVentilationMode == 1) {
            lcd.print("  A/C  ");
        }
        else if (currentVentilationMode == 2) {
            lcd.print(" CPAP  ");
        }
        else {
            lcd.print(" P-CMV ");
        }
        frecRespiratoriaAnte = currentFrecRespiratoria;
        IAnte = I;
        EAnte = E;
        break;
    case CONFIG_ALARM:
        lcd.setCursor(0, 0);
        lcd.print("      Alarmas       ");
        lcd.setCursor(0, 1);
        lcd.print("                    ");
        lcd.setCursor(0, 2);
        lcd.print("  PIP:    cmH2O     ");
        lcd.setCursor(7, 2);
        lcd.print(maxPresion);
        lcd.setCursor(0, 3);
        lcd.print("                    ");
        maxPresionAnte = maxPresion;
        break;
    case ALE_PRES_PIP:
        // Serial.println("Alerta");
        lcd.setCursor(7, 0);
        lcd.print("ALERTA");
        lcd.setCursor(0, 2);
        lcd.print("Presion PIP elevada");
        break;
    case ALE_PRES_DES:
        // Serial.println("Alerta");
        lcd.setCursor(7, 0);
        lcd.print("ALERTA");
        lcd.setCursor(0, 2);
        lcd.print("Desconexion Paciente");
        break;
    case ALE_GENERAL:
        // Serial.println("Alerta");
        lcd.setCursor(7, 0);
        lcd.print("ALERTA");
        lcd.setCursor(4, 2);
        lcd.print("Obstruccion");
        break;
    case ALE_PRES_PEEP:
        // Serial.println("Alerta");
        lcd.setCursor(7, 0);
        lcd.print("ALERTA");
        lcd.setCursor(0, 2);
        lcd.print("  Perdida de PEEP   ");
        break;
    case BATTERY:
        // Serial.println("Alerta");
        lcd.setCursor(5, 0);
        lcd.print("ADVERTENCIA");
        lcd.setCursor(0, 2);
        lcd.print("Fallo red electrica");
        break;
    case CHECK_MENU:
        lcd.setCursor(0, 0);
        lcd.print("Comprobacion Inicial");
        lcd.setCursor(0, 1);
        lcd.print("                    ");
        lcd.setCursor(0, 2);
        lcd.print("Valvulas            ");
        lcd.setCursor(0, 3);
        lcd.print("Sensores            ");
        break;
    case CONFIRM_MENU:
        lcd.setCursor(0, 0);
        lcd.print(" Confirmar cambios  ");
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
        lcd.setCursor(0, 0);
        lcd.print(" Configuracion CPAP ");
        lcd.setCursor(0, 1);
        lcd.print("                    ");
        lcd.setCursor(0, 2);
        lcd.print("1. Ajustar PEEP     ");
        lcd.setCursor(0, 3);
        lcd.print("2. Ajustar PCON = 0");
        break;
    default:
        lcd.setCursor(0, 0);
        lcd.print("                    ");
        lcd.setCursor(0, 1);
        lcd.print("                    ");
        lcd.setCursor(0, 2);
        lcd.print("Default: ");
        lcd.print(12);                      // Escribimos el numero de segundos trascurridos
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
    if (newRelacionIE > 0) {
        newRelacion_IE = "1:" + String((float)newRelacionIE / 10, 1);
    }
    else {
        newRelacion_IE = String(-(float)newRelacionIE / 10, 1) + ":1";
    }
    //Serial.println("IE = " + String(I) + ':' + String(E));
    //Serial.println(currentRelacionIE);

    switch (menuImprimir) {
    case MAIN_MENU:
        lcd.setCursor(0, 0);
        if (stateMachine == STANDBY_STATE) {
            lcd.print("    Modo Standby    ");
        }
        else {
            lcd.print("  InnspiraMED UdeA  ");
        }
        if (currentFrecRespiratoria != frecRespiratoriaAnte) {
            lcd.setCursor(4, 1);
            lcd.print(currentFrecRespiratoria);
            if (currentFrecRespiratoria < 10) {
                lcd.print(" ");
            }
            frecRespiratoriaAnte = currentFrecRespiratoria;
            // Serial.println("Changed freq");
        }
        if (Ppico != PpicoAnte) {
            lcd.setCursor(14, 1);
            lcd.print(String(Ppico, 0));
            if (Ppico < 10) {
                lcd.print(" ");
            }
            PpicoAnte = Ppico;
            // Serial.println("Changed Ppico");
        }
        if ((I != IAnte) || (E != EAnte)) {
            lcd.setCursor(4, 2);
            lcd.print(relacion_IE);
            IAnte = I;
            EAnte = E;
            // Serial.println("Changed IE");
        }
        if (Pcon != PconAnte) {
        
            lcd.setCursor(15, 2);
            lcd.print(String(Pcon, 0));
            if (Pcon < 10)
            {
                lcd.print(" ");
            }
            PconAnte = Pcon;
        }
        if (Peep != PeepAnte) {
            lcd.setCursor(15, 3);
            lcd.print(String(Peep, 0));
            if (Peep < 10) {
                lcd.print(" ");
            }
            PeepAnte = Peep;
            // Serial.println("Changed Peep");
        }
        if (VT != VTAnte) {
            lcd.setCursor(4, 3);
            lcd.print(String(VT, 0));
            if (VT < 10) {
                lcd.print(" ");
            }
            VTAnte = VT;
            // Serial.println("Changed VT");
        }
        break;
    case CONFIG_MENU:
        lcd.setCursor(0, 2);
        if (flagFrecuencia) {
            lcd.write(126);
        }
        else {
            lcd.print(" ");
        }
        if (newFrecRespiratoria != frecRespiratoriaAnte) {
            lcd.setCursor(7, 2);
            lcd.print(newFrecRespiratoria);
            if (newFrecRespiratoria < 10) {
                lcd.print(" ");
            }
            frecRespiratoriaAnte = newFrecRespiratoria;
            // Serial.println("Changed freq");
        }
        lcd.setCursor(0, 3);
        if (flagIE) {
            lcd.write(126);
        }
        else {
            lcd.print(" ");
        }
        if (newRelacionIE != currentRelacionIE) {
            lcd.setCursor(6, 3);
            lcd.print(newRelacion_IE);
            IAnte = I;
            EAnte = E;
            // Serial.println("Changed IE");
        }
        if (flagMode == true) {
            lcd.setCursor(13, 2);
            lcd.write(126);
            lcd.setCursor(13, 3);
            if (newVentilationMode == 1) {
                lcd.print("  A/C  ");
            }
            else if (newVentilationMode == 2) {
                lcd.print(" CPAP  ");
            }
            else {
                lcd.print(" P-CMV ");
            }
        }
        break;
    case CONFIG_ALARM:
        lcd.setCursor(1, 2);
        if (flagPresion) {
            lcd.write(126);
        }
        else {
            lcd.print(" ");
        }
        if (maxPresion != maxPresionAnte) {
            lcd.setCursor(7, 2);
            lcd.print(maxPresion);
            if (maxPresion < 10) {
                lcd.print(" ");
            }
            maxPresionAnte = maxPresion;
            // Serial.println("Changed maxPress");
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
    }

    //Serial.println("I am in lcd_show()");
}

/* ***************************************************************************
 * **** Ejecucion de la rutina de comunicacion por serial ********************
 * ***************************************************************************/
 // Function to receive data from serial communication
void task_Receive(void* pvParameters) {

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
            VT = dataIn2[2].toFloat();
            alerPresionPIP = dataIn2[3].toInt();
            alerDesconexion = dataIn2[4].toInt();
            alerGeneral = dataIn2[5].toInt();
            alerPresionPeep = dataIn2[6].toInt();
            //currentBatteryAlert = dataIn2[6].toInt();
            Serial2.flush();
            //Serial.flush();  // solo para pruebas
            //Serial.println(String(Ppico) + ',' + String(Peep) + ',' + String(VT) + ',' + String(alerGeneral) + ',' + String(alerPresionPeep));
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
    /*String dataToSend = String(currentFrecRespiratoria) + ',' + String(I) + ',' +
        String(E) + ',' + String(maxPresion) + ',' + String(stateMachine) + 
        String(currentVentilationMode) + ';';*/
    String dataToSend = String(currentFrecRespiratoria) + ',' + String(I) + ',' +
        String(E) + ',' + String(maxPresion) + ',' + String(batteryAlert) + ',' +
        String(flagStabilityInterrupt) + ';';
    Serial2.print(dataToSend);
    //Serial.println(dataToSend);
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
        if (flagAlerta == true) {
            contAlertas++;
            if (contAlertas == 3) {
                if (flagSilenceInterrupt == false) {
                    digitalWrite(BUZZER, LOW);
                }
                digitalWrite(LUMINR, HIGH);
                digitalWrite(LUMING, HIGH);
                digitalWrite(LUMINB, HIGH);
                digitalWrite(SILENCE_LED, HIGH);
            }
            else if (contAlertas == 10) {
                menuImprimir = menuAlerta;
                flagAlreadyPrint = false;
                if (flagSilenceInterrupt == false) {
                    //digitalWrite(BUZZER, HIGH);
                }
                digitalWrite(LUMINR, LOW);
                digitalWrite(LUMING, LOW);
                digitalWrite(LUMINB, LOW);
                digitalWrite(SILENCE_LED, LOW);
            }
            else if (contAlertas == 13) {
                if (flagSilenceInterrupt == false) {
                    digitalWrite(BUZZER, LOW);
                }
                digitalWrite(LUMINR, HIGH);
                digitalWrite(LUMING, HIGH);
                digitalWrite(LUMINB, HIGH);
                digitalWrite(SILENCE_LED, HIGH);
            }
            else if (contAlertas == 20) {
                menuImprimir = menu;
                flagAlreadyPrint = false;
                if (flagSilenceInterrupt == false) {
                   // digitalWrite(BUZZER, HIGH);
                }
                digitalWrite(LUMINR, LOW);
                digitalWrite(LUMING, LOW);
                digitalWrite(LUMINB, LOW);
                digitalWrite(SILENCE_LED, LOW);
                contAlertas = 0;
            }
        }
        else if (flagBatteryAlert == true ) {
            contSilenceBattery++;  
            if (contSilenceBattery == 3) {
                if (flagBatterySilence == false) {
                    digitalWrite(BUZZER, LOW);
                }
                digitalWrite(LUMINR, HIGH);
                digitalWrite(LUMING, HIGH);
                digitalWrite(LUMINB, HIGH);
                digitalWrite(SILENCE_LED, HIGH);
            }
            else if (contSilenceBattery == 10) {
                menuImprimir = menuAlerta;
                flagAlreadyPrint = false;
                if (flagBatterySilence == false) {
                    //digitalWrite(BUZZER, HIGH);
                }
                digitalWrite(LUMINR, LOW);
                digitalWrite(LUMING, LOW);
                digitalWrite(LUMINB, LOW);
                digitalWrite(SILENCE_LED, LOW);
            }
            else if (contSilenceBattery == 13) {
                if (flagBatterySilence == false) {
                    digitalWrite(BUZZER, LOW);
                }
                digitalWrite(LUMINR, HIGH);
                digitalWrite(LUMING, HIGH);
                digitalWrite(LUMINB, HIGH);
                digitalWrite(SILENCE_LED, HIGH);
            }
            else if (contSilenceBattery == 20) {
                menuImprimir = menu;
                flagAlreadyPrint = false;
                if (flagBatterySilence == false) {
                    // digitalWrite(BUZZER, HIGH);
                }
                digitalWrite(LUMINR, LOW);
                digitalWrite(LUMING, LOW);
                digitalWrite(LUMINB, LOW);
                digitalWrite(SILENCE_LED, LOW);
                contSilenceBattery = 0;
            }
        }
        else {
            contAlertas++;
            if (contAlertas == 10) {
                menuImprimir = menu;
                flagAlreadyPrint = false;
                digitalWrite(LUMINR, LOW);
                digitalWrite(LUMING, LOW);
                digitalWrite(LUMINB, LOW);
                digitalWrite(SILENCE_LED, LOW);
                digitalWrite(BUZZER, LOW);
                contSilence = 0;
                flagSilenceInterrupt = false;
            }
            else if (contAlertas == 20) {
                menuImprimir = menu;
                flagAlreadyPrint = false;
                contAlertas = 0;
            }
        }

        /* ****************************************************************
         * **** Actualizacion de valores en pantalla LCD ******************
         * ***************************************************************/
        if ((menuAnterior != menuImprimir) && (flagAlreadyPrint == false)) {
            lcd.clear();
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

    // se crea el semáforo binario
    xSemaphoreEncoder = xSemaphoreCreateBinary();
    xSemaphoreTimer = xSemaphoreCreateBinary();

    // creo la tarea task_pulsador
    xTaskCreatePinnedToCore(task_Encoder, "task_Encoder", 2048, NULL, 4, NULL, taskCoreOne);
    // xTaskCreatePinnedToCore(task_Encoder_B, "task_Encoder_B", 10000, NULL, 1, NULL, taskCoreZero);

    xTaskCreatePinnedToCore(task_timer, "task_timer", 2048, NULL, 2, NULL, taskCoreOne);
    xTaskCreatePinnedToCore(task_Display, "task_Display", 2048, NULL, 3, NULL, taskCoreOne);  // se puede colocar en el core cero
    xTaskCreatePinnedToCore(task_Receive, "task_Receive", 2048, NULL, 1, NULL, taskCoreOne);

    // Clean Serial buffers
    delay(1000);
    Serial.flush();
    Serial2.flush();
}

/* ***************************************************************************
 * **** LOOP MAIN_MENU *******************************************************
 * ***************************************************************************/
void loop() {

}

/* ***************************************************************************
 * **** FIN DEL PROGRAMA *****************************************************
 * ***************************************************************************/
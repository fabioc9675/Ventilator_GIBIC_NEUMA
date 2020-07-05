/*
 * File:   controlUnit.ino
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

#include "initializer.h"
#include "timer.h"
#include "cyclingFunctions.h"
#include "acquisition.h"
#include "serialHMI.h"
#include "memoryManager.h"
#include "calibrationMenu.h"
#include "menuSelection.h"
#include "menuChange.h"

/** ****************************************************************************
 ** ************ DEFINES *******************************************************
 ** ****************************************************************************/

//********DEFINICIONES CONDICIONES******
#define TRUE 1
#define FALSE 0

//********DEFINICION DISPOSITIVO********
// #define SERIAL_DEVICE     "9GF100007LJD00006"
// #define SERIAL_DEVICE     "1NEUMA0001"

/** ****************************************************************************
 ** ************ EXTERN VARIABLES **********************************************
 ** ****************************************************************************/
// variables de operacion de calibracion
extern uint8_t servMenuStateCurrent;
extern uint8_t servMenuStateNew;
extern uint8_t modeCalibration;
extern uint8_t coeftype;
extern uint8_t placeCalibration; // variable para especificar si es calibracion de fabrica o de sitio

extern unsigned int contCiclos;  // variable que controla el numero de ciclos que ha realizado un ventilador
extern volatile int contCycling; // variable que cuenta los ms del ciclado para cambiar estado de electrovalvulas

// Variables de ventilacion
extern uint8_t flagMaxPresion; // bandera de maxima presion alcanzada
extern volatile float inspirationTime;
extern volatile float expirationTime;
extern int currentFrecRespiratoria;
extern int currentI;
extern int currentE;
extern float relI;
extern float relE;

extern int newTrigger;
extern int newPeepMax;
extern int maxFR;
extern int maxVE;
extern int apneaTime;
extern int minFR;
extern float UmbralPpmin;
extern float UmbralPpico;
extern float UmbralPpicoDistal;
extern float UmbralFmin;
extern float UmbralFmax;
extern float UmbralVmin;
extern float UmbralVmax;
extern float SFinMax;
extern float SFoutMax;
extern float SFinMaxInsp;
extern float SFtotalMax;

// Variables para el envio y recepcion de alarmas
extern int alerPresionPIP;
extern int alerDesconexion;
extern int alerObstruccion;
extern int alerPeep;
extern int alerBateria;
extern int alerGeneral;
extern int alerFR_Alta;
extern int alerVE_Alto;
extern int estabilidad;
extern int PeepEstable;

// variables para calculo de frecuencia y relacion IE en CPAP
extern float SFant;
extern float dFlow;

// banderas de deteccion de alarmas
extern volatile uint8_t flagAlarmPpico;
extern volatile uint8_t flagAlarmGeneral;
extern volatile uint8_t flagAlarmPatientDesconnection;
extern volatile uint8_t flagAlarmObstruccion;
extern volatile uint8_t flagAlarmFR_Alta;
extern volatile uint8_t flagAlarmVE_Alto;

/** ****************************************************************************
 ** ************ VARIABLES *****************************************************
 ** ****************************************************************************/
// definicion de los core para ejecucion
uint8_t taskCoreZero = 0;
uint8_t taskCoreOne = 1;

// manejadores de las tareas
xTaskHandle serviceTaskHandle;

// manejadores para los semaforos binarios
SemaphoreHandle_t xSemaphoreTimer = NULL;
SemaphoreHandle_t xSemaphoreRaspberry = NULL;
SemaphoreHandle_t xSemaphoreAdc = NULL;

// Cadenas de caracteres
String menuString;
String SerialID;

// bandera de activacion de timer
volatile uint8_t flagTimerInterrupt = false;
volatile uint8_t flagAdcInterrupt = false;
uint8_t flagService = false;
uint8_t flagRestartTask = false;

// bandera de modo calibracion
uint8_t flagFlowPrintCalibration = false;
uint8_t flagPcamPrintCalibration = false;
uint8_t flagPbagPrintCalibration = false;
uint8_t flagPpacPrintCalibration = false;
uint8_t flagFlowSitePrintCalibration = false;
uint8_t flagPcamSitePrintCalibration = false;
uint8_t flagPbagSitePrintCalibration = false;
uint8_t flagPpacSitePrintCalibration = false;

// banderas de estados de ventilacion
uint8_t flagAC = false;

// Variables maquina de estados de modos ventilatorios
int currentStateMachine = STANDBY_STATE;
int newStateMachine = STANDBY_STATE;
int currentVentilationMode = 0;
int newVentilationMode = 0;

// Variables de maquinas de estado de ciclado
unsigned int currentStateMachineCycling = START_CYCLING;
byte AC_stateMachine = 0;

// Calibracion de los sensores de presion - coeficientes regresion lineal
volatile float AMP_CAM_1 = 0;
volatile float OFFS_CAM_1 = 0;
volatile float AMP_BAG_2 = 0;
volatile float OFFS_BAG_2 = 0;
volatile float AMP_PAC_3 = 0;
volatile float OFFS_PAC_3 = 0;

// Calibracion de los sensores de flujo - coeficientes regresion lineal
// Sensor de flujo Inspiratorio
volatile float AMP_FI_1 = 0;
volatile float OFFS_FI_1 = 0;
volatile float LIM_FI_1 = 0;
volatile float AMP_FI_2 = 0;
volatile float OFFS_FI_2 = 0;
volatile float LIM_FI_2 = 0;
volatile float AMP_FI_3 = 0;
volatile float OFFS_FI_3 = 0;

// Sensor de flujo Espiratorio
volatile float AMP_FE_1 = 0;
volatile float OFFS_FE_1 = 0;
volatile float LIM_FE_1 = 0;
volatile float AMP_FE_2 = 0;
volatile float OFFS_FE_2 = 0;
volatile float LIM_FE_2 = 0;
volatile float AMP_FE_3 = 0;
volatile float OFFS_FE_3 = 0;

// variable para ajustar el nivel cero de flujo y calcular el volumen
volatile float VOL_SCALE = 0;      // Factor de escala para ajustar el volumen
volatile float VOL_SCALE_SITE = 0; // Factor de escala para ajustar el volumen

// **********************************************************
// Calibracion sensores Sitio
// Calibracion de los sensores de presion - coeficientes regresion lineal
volatile float AMP_CAM_1_SITE = 0;
volatile float OFFS_CAM_1_SITE = 0;
volatile float AMP_BAG_2_SITE = 0;
volatile float OFFS_BAG_2_SITE = 0;
volatile float AMP_PAC_3_SITE = 0;
volatile float OFFS_PAC_3_SITE = 0;

// Calibracion de los sensores de flujo - coeficientes regresion lineal
// Sensor de flujo Inspiratorio
volatile float AMP_FI_1_SITE = 0;
volatile float OFFS_FI_1_SITE = 0;
volatile float LIM_FI_1_SITE = 0;
volatile float AMP_FI_2_SITE = 0;
volatile float OFFS_FI_2_SITE = 0;
volatile float LIM_FI_2_SITE = 0;
volatile float AMP_FI_3_SITE = 0;
volatile float OFFS_FI_3_SITE = 0;

// Sensor de flujo Espiratorio
volatile float AMP_FE_1_SITE = 0;
volatile float OFFS_FE_1_SITE = 0;
volatile float LIM_FE_1_SITE = 0;
volatile float AMP_FE_2_SITE = 0;
volatile float OFFS_FE_2_SITE = 0;
volatile float LIM_FE_2_SITE = 0;
volatile float AMP_FE_3_SITE = 0;
volatile float OFFS_FE_3_SITE = 0;

// variables de calculo de parametros ventilatorios
float Ppico = 0;         // valor medido de Ppico
float PpicoProximal = 0; // medicion realizada con sensor distal a paciente
float PpicoDistal = 0;   // medicion realizada con sensor distal a paciente
float Peep = 0;          // valor medido de Peep
float Peep_AC = 0;       // medicion de Peep en el modo asistido controlado
float PeepProximal = 0;  // medicion realizada con sensor distal a paciente
float PeepDistal = 0;    // medicion realizada con sensor distal a paciente
float flowZero = 0;      // medicion del flujo cero para la grafica de volumen

float SPinADC = 0;  //Senal filtrada de presion en la camara
float SPoutADC = 0; //Senal filtrada de presion en la bolsa
float SPpacADC = 0; //Senal de presion en la via aerea del paciente
float SFinADC = 0;  //Senal de flujo inspiratorio
float SFoutADC = 0; //Senal de flujo espiratorio

float SPin = 0;  // Senal filtrada de presion en la camara
float SPout = 0; // Senal filtrada de presion en la bolsa
float SPpac = 0; // Senal de presion en la via aerea del paciente
float SFin = 0;  //Senal de flujo inspiratorio
float SFout = 0; //Senal de flujo espiratorio

float dPpac = 0; // Derivada de SPpac
float SFpac = 0; // Senal de flujo del paciente

float SVtidal = 0; // informacion de promedio para Vtidal
float Sfrec = 0;   // informacion de promedio para frecuencia

int frecRespiratoriaCalculada = 0;
int calculatedE = currentE;
int calculatedI = currentI;
int currentVE = 0;

// Mediciones derivadas de las senales del ventilador
float Pin_max = 0;
float Pout_max = 0;
float VT = 0;

float Pin_min = 0;
float Pout_min = 0;
float pmin = 0;
float pmax = 0;
float flmin = 0;
float flmax = 0;
float vmin = 0;
float vmax = 0;

// Variables para visualizacion
float VtidalV = 0; // senal de volumen para visualizacion
float VtidalC = 0; // senal de volumen para calculo
float SFpacV = 0;  // senal de flujo para visualizacion

// variables para el filtrado
float PinADC[40] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
float PoutADC[40] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
float PpacADC[40] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
float FinADC[40] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
float FoutADC[40] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
float FPacProm[40] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
float VTidProm[3] = {0, 0, 0};
float FreqProm[3] = {0, 0, 0};

// Variables recibidas en el Serial
int newFrecRespiratoria = currentFrecRespiratoria;
int newI = currentI;
int newE = currentE;

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

// Cadena de impresion en raspberry
String RaspberryChain = "";

char initial = 'R';

/* ***************************************************************************
 * **** CONFIGURACION ********************************************************
 * ***************************************************************************/
void setup()
{
    init_Memory();
    init_GPIO();

    init_TextPayload();
    Serial.begin(115200);
    Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);

    Serial2.setTimeout(100);

    vTaskDelay(1000 / portTICK_PERIOD_MS);

    if (Serial2.available() > 0)
    {
        initial = Serial2.read();
    }

    if (initial == 'S') // Modo servicio de sitio
    {
        flagService = true;
        servMenuStateNew = SERV_MAIN_MENU;
        servMenuStateCurrent = servMenuStateNew;
    }
    else if (initial == 'M') // modo servicio de fabrica
    {
        flagService = true;
        servMenuStateNew = SERV_MAIN_MENU;
        servMenuStateCurrent = servMenuStateNew;
    }
    else
    {
        flagService = false;
        // servMenuStateNew = SERV_MAIN_MENU;
        servMenuStateNew = SERV_NULL_MENU;
        servMenuStateCurrent = servMenuStateNew;
    }

    // nvs_flash_init();

    // se crea el semaforo binario
    xSemaphoreTimer = xSemaphoreCreateBinary();
    xSemaphoreAdc = xSemaphoreCreateBinary();
    xSemaphoreRaspberry = xSemaphoreCreateBinary();
    init_Timer();

    pinMode(2, OUTPUT); // para prueba de retardos en la ejecucion de tareas

    // nvs_flash_init();

    // Tareas de operacion normal del ventilador
    xTaskCreatePinnedToCore(task_Timer, "task_Timer", 2048, NULL, 7, NULL, taskCoreOne);
    xTaskCreatePinnedToCore(task_Adc, "task_Adc", 4096, NULL, 4, NULL, taskCoreOne);
    xTaskCreatePinnedToCore(task_Receive, "task_Receive", 4096, NULL, 1, NULL, taskCoreOne);
    xTaskCreatePinnedToCore(task_sendSerialData, "task_sendSerialData", 4096, NULL, 1, NULL, taskCoreOne);
    xTaskCreatePinnedToCore(task_Raspberry, "task_Raspberry", 4096, NULL, 4, NULL, taskCoreOne);

    // Tareas de operacion de servicio del ventilador
    xTaskCreatePinnedToCore(task_Service, "task_Service", 4096, NULL, 1, &serviceTaskHandle, taskCoreOne);
    xTaskCreatePinnedToCore(task_ReceiveService, "task_ReceiveService", 4096, NULL, 1, NULL, taskCoreOne);
    // xTaskCreatePinnedToCore(task_Encoder_B, "task_Encoder_B", 10000, NULL, 1, NULL, taskCoreZero);

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

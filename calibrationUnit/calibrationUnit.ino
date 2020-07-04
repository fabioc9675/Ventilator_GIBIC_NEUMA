/*
 * File:   calibrationUnit.ino
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

#include "timer.h"
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
#define SERIAL_DEVICE "1NEUMA0001"



/** ****************************************************************************
 ** ************ EXTERN VARIABLES **********************************************
 ** ****************************************************************************/
// variables de operacion de calibracion
extern uint8_t servMenuStateCurrent;
extern uint8_t servMenuStateNew;
extern uint8_t modeCalibration;
extern uint8_t coeftype;
extern uint8_t placeCalibration; // variable para especificar si es calibracion de fabrica o de sitio

extern portMUX_TYPE timerMux;



/** ****************************************************************************
 ** ************ VARIABLES *****************************************************
 ** ****************************************************************************/
// definicion de los core para ejecucion
static uint8_t taskCoreZero = 0;
static uint8_t taskCoreOne = 1;

// manejadores de las tareas
xTaskHandle serviceTaskHandle;

// manejadores para los semaforos binarios
SemaphoreHandle_t xSemaphoreTimer = NULL;
SemaphoreHandle_t xSemaphoreRaspberry = NULL;

// Cadenas de caracteres
String menuString;
String SerialID;
String dataCoeficients;

// bandera de activacion de timer
uint8_t flagTimerInterrupt = false;
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



/* **************************************************************************
 * **** TAREA PARA EL MANEJO DE LA INTERRUPCION DEL TIMER *******************
 * **************************************************************************/
void task_Timer(void *arg)
{

    uint16_t counterMs = 0;

    while (1)
    {
        // Se atiende la interrpcion del timer
        if (xSemaphoreTake(xSemaphoreTimer, portMAX_DELAY) == pdTRUE)
        {
            if (flagTimerInterrupt == true)
            {
                portENTER_CRITICAL(&timerMux);
                flagTimerInterrupt = false;
                portEXIT_CRITICAL(&timerMux);

                /* *************************************************************
				 * **** SECUENCIA DE FUNCIONAMIENTO, ESTADOS DEL VENTILADOR ****
				 * *************************************************************/
                counterMs++;
                if (counterMs % 50 == 0)
                {
                    xSemaphoreGive(xSemaphoreRaspberry); // asignacion y liberacion de semaforos
                }

                if (counterMs == 1000)
                {
                    counterMs = 0;

                    digitalWrite(2, !digitalRead(2));
                }
            }
        }
    }
    vTaskDelete(NULL);
}

/* **************************************************************************
 * **** TAREA PARA LA CALIBRACION Y MENU DE USUARIO *************************
 * **************************************************************************/
void task_Service(void *arg)
{
    //Serial.println("Task_Create");
    while (1)
    {
        if (flagService == true)
        {
            // Serial.println("Estoy en servicio");
            switch (servMenuStateCurrent)
            {
            case SERV_MAIN_MENU:
                printMainMenu();
                break;
            case SERV_FACT_CALI:
                printFactoryMenu();
                break;
            case SERV_SITE_CALI:
                printSiteMenu();
                break;
            default:
                break;
            }
            // Serial.println("In execution");
        }
        else
        {
            /* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++
             + ++++ ESTADO PARTA DESTRUIR LA TAREA DE CALIBRACION +++++
             + +++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
            if (servMenuStateCurrent == SERV_NULL_MENU)
            {
                // Serial.println("Task deleted");
                flagRestartTask = true;         // flag to habilitate the restart of task
                vTaskDelete(serviceTaskHandle); // delete the task Service
            }
        }
        vTaskDelay(250 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}


/* **************************************************************************
 * **** TAREA PARA LA RECEPCION DE DATOS SERIAL EN CALIBRACION **************
 * **************************************************************************/
void task_ReceiveService(void *pvParameters)
{

    String menuEntrance;
    menuEntrance.reserve(50);
    int selMenu = 0;
    float newCoeficient = 0;
    String newSerial;

    char bufferR = 0;

    uint8_t dataReady = false;

    uint8_t flagFirstPrint = false;

    // Clean Serial buffers
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    Serial.flush();
    Serial2.flush();

    while (1)
    {
        if (flagService == true)
        { // se encuentra en modo servicio
            if (Serial.available())
            {
                bufferR = Serial.read();
                if (bufferR == '\n')
                {
                    dataReady = true;
                }
                else
                {
                    menuEntrance = menuEntrance + bufferR;
                }
                Serial.print(bufferR);
            }

            if (dataReady == true)
            {
                dataReady = false;
                //menuEntrance = Serial.readStringUntil('\n');
                switch (servMenuStateCurrent)
                {
                case SERV_WAIT_MAIN:
                    selMenu = menuEntrance.toInt();
                    Serial.println(selMenu);
                    Serial.print("\n\n\n");
                    mainMenuOptionChange(selMenu);
                    break;
                case SERV_WAIT_FACT:
                    selMenu = menuEntrance.toInt();
                    Serial.println(selMenu);
                    Serial.print("\n\n\n");
                    factoryMenuOptionChange(selMenu);
                    break;
                case SERV_WAIT_SITE:
                    selMenu = menuEntrance.toInt();
                    Serial.println(selMenu);
                    Serial.print("\n\n\n");
                    siteMenuOptionChange(selMenu);
                    break;
                case SERV_WAIT_FAIN:
                    selMenu = menuEntrance.toInt();
                    Serial.println(selMenu);
                    Serial.print("\n\n\n");
                    factoryInternalMenuOptionChange(selMenu);
                    break;
                case SERV_WAIT_SIIN:
                    selMenu = menuEntrance.toInt();
                    Serial.println(selMenu);
                    Serial.print("\n\n\n");
                    siteInternalMenuOptionChange(selMenu);
                    break;
                case SERV_FACT_PRIN:
                    if (menuEntrance[0] == 'q')
                    {
                        if ((modeCalibration == FLUJO_INSPIRATORIO) || (modeCalibration == FLUJO_ESPIRATORIO))
                        {
                            flagFlowPrintCalibration = false;
                            servMenuStateNew = SERV_FAIN_CALI;
                            servMenuStateCurrent = servMenuStateNew;
                            printInternalFactoryMenu(modeCalibration);
                        }
                        if (modeCalibration == PRESION_CAMARA)
                        {
                            flagPcamPrintCalibration = false;
                            servMenuStateNew = SERV_FAIN_CALI;
                            servMenuStateCurrent = servMenuStateNew;
                            printInternalFactoryMenu(modeCalibration);
                        }
                        if (modeCalibration == PRESION_BOLSA)
                        {
                            flagPbagPrintCalibration = false;
                            servMenuStateNew = SERV_FAIN_CALI;
                            servMenuStateCurrent = servMenuStateNew;
                            printInternalFactoryMenu(modeCalibration);
                        }
                        if (modeCalibration == PRESION_PACIENTE)
                        {
                            flagPpacPrintCalibration = false;
                            servMenuStateNew = SERV_FAIN_CALI;
                            servMenuStateCurrent = servMenuStateNew;
                            printInternalFactoryMenu(modeCalibration);
                        }
                    }
                    break;
                case SERV_SITE_PRIN:
                    if (menuEntrance[0] == 'q')
                    {
                        if ((modeCalibration == FLUJO_INSPIRATORIO) || (modeCalibration == FLUJO_ESPIRATORIO))
                        {
                            flagFlowSitePrintCalibration = false;
                            servMenuStateNew = SERV_SIIN_CALI;
                            servMenuStateCurrent = servMenuStateNew;
                            printInternalFactoryMenu(modeCalibration);
                        }
                        if (modeCalibration == PRESION_CAMARA)
                        {
                            flagPcamSitePrintCalibration = false;
                            servMenuStateNew = SERV_SIIN_CALI;
                            servMenuStateCurrent = servMenuStateNew;
                            printInternalFactoryMenu(modeCalibration);
                        }
                        if (modeCalibration == PRESION_BOLSA)
                        {
                            flagPbagSitePrintCalibration = false;
                            servMenuStateNew = SERV_SIIN_CALI;
                            servMenuStateCurrent = servMenuStateNew;
                            printInternalFactoryMenu(modeCalibration);
                        }
                        if (modeCalibration == PRESION_PACIENTE)
                        {
                            flagPpacSitePrintCalibration = false;
                            servMenuStateNew = SERV_SIIN_CALI;
                            servMenuStateCurrent = servMenuStateNew;
                            printInternalFactoryMenu(modeCalibration);
                        }
                    }
                    break;
                case SERV_ACQI_DATA:
                    if (flagFirstPrint == false)
                    {
                        flagFirstPrint = true;
                        newCoeficient = menuEntrance.toFloat();
                        Serial.println(String(newCoeficient, 5));
                        Serial.print("\nDesea confirmar el cambio? y/n: ");
                    }
                    else
                    {
                        if (menuEntrance[0] == 'y')
                        {
                            changeMenu(coeftype, modeCalibration, newCoeficient, placeCalibration);
                            newCoeficient = 0;
                            modeCalibration = MODE_NULL;
                            if (placeCalibration == FACTORY)
                            {
                                servMenuStateNew = SERV_FACT_CALI;
                            }
                            else if (placeCalibration == SITE)
                            {
                                servMenuStateNew = SERV_SITE_CALI;
                            }
                            servMenuStateCurrent = servMenuStateNew;
                            flagFirstPrint = false;
                        }
                        else
                        {
                            newCoeficient = 0;
                            modeCalibration = MODE_NULL;
                            if (placeCalibration == FACTORY)
                            {
                                servMenuStateNew = SERV_FACT_CALI;
                            }
                            else if (placeCalibration == SITE)
                            {
                                servMenuStateNew = SERV_SITE_CALI;
                            }
                            servMenuStateCurrent = servMenuStateNew;
                            flagFirstPrint = false;
                            Serial.println("\n No se actualizo el valor\n");
                        }
                    }
                    break;
                case SERV_SERIAL_CH:
                    if (flagFirstPrint == false)
                    {
                        flagFirstPrint = true;
                        newSerial = menuEntrance;
                        Serial.println(newSerial);
                        Serial.print("\nDesea confirmar el cambio? y/n: ");
                    }
                    else
                    {
                        if (menuEntrance[0] == 'y')
                        {
                            writeString(eeprom_values::SERIAL_ADDR, newSerial);
                            SerialID = readString(eeprom_values::SERIAL_ADDR);
                            Serial.println("\n Serial actualizado con exito\n");
                            servMenuStateNew = SERV_FACT_CALI;
                            servMenuStateCurrent = servMenuStateNew;
                            flagFirstPrint = false;
                        }
                        else
                        {
                            servMenuStateNew = SERV_FACT_CALI;
                            servMenuStateCurrent = servMenuStateNew;
                            flagFirstPrint = false;
                            Serial.println("\n No se actualizo el Serial\n");
                        }
                    }
                    break;
                default:
                    break;
                }
                menuEntrance = "";
            }
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}

/* **************************************************************************
 * **** TAREA PARA EL ENVIO DE DATOS A LA RASPBERRY *************************
 * **************************************************************************/
void task_Raspberry(void *arg)
{
    float CalFin = 0;
    float CalFout = 0;
    float CalPin = 0;
    float CalPout = 0;
    float CalPpac = 0;

    while (1)
    {
        // Se atiende solicitud de envio a traves de serial 1 para raspberry
        if (xSemaphoreTake(xSemaphoreRaspberry, portMAX_DELAY) == pdTRUE)
        {

            dataCoeficients = SerialID + ',' + String(AMP_CAM_1, 5) + ',' + String(OFFS_CAM_1, 5) + ',' + String(AMP_BAG_2, 5) +
                              ',' + String(OFFS_BAG_2, 5) + ',' + String(AMP_PAC_3, 5) + ',' + String(OFFS_PAC_3, 5) + ',' + String(AMP_FI_1, 5) +
                              ',' + String(OFFS_FI_1, 5) + ',' + String(LIM_FI_1, 5) + ',' + String(AMP_FI_2, 5) + ',' + String(OFFS_FI_2, 5) +
                              ',' + String(LIM_FI_2, 5) + ',' + String(AMP_FI_3, 5) + ',' + String(OFFS_FI_3, 5) + ',' + String(AMP_FE_1, 5) +
                              ',' + String(OFFS_FE_1, 5) + ',' + String(LIM_FE_1, 5) + ',' + String(AMP_FE_2, 5) + ',' + String(OFFS_FE_2, 5) +
                              ',' + String(LIM_FE_2, 5) + ',' + String(AMP_FE_3, 5) + ',' + String(OFFS_FE_3, 5) + ',' + String(VOL_SCALE, 5) + ',' + String(VOL_SCALE_SITE, 5) +
                              ',' + String(AMP_CAM_1_SITE, 5) + ',' + String(OFFS_CAM_1_SITE, 5) + ',' + String(AMP_BAG_2_SITE, 5) +
                              ',' + String(OFFS_BAG_2_SITE, 5) + ',' + String(AMP_PAC_3_SITE, 5) + ',' + String(OFFS_PAC_3_SITE, 5) +
                              ',' + String(AMP_FI_1_SITE, 5) + ',' + String(OFFS_FI_1_SITE, 5) + ',' + String(LIM_FI_1_SITE, 5) +
                              ',' + String(AMP_FI_2_SITE, 5) + ',' + String(OFFS_FI_2_SITE, 5) + ',' + String(LIM_FI_2_SITE, 5) +
                              ',' + String(AMP_FI_3_SITE, 5) + ',' + String(OFFS_FI_3_SITE, 5) + ',' + String(AMP_FE_1_SITE, 5) +
                              ',' + String(OFFS_FE_1_SITE, 5) + ',' + String(LIM_FE_1_SITE, 5) + ',' + String(AMP_FE_2_SITE, 5) +
                              ',' + String(OFFS_FE_2_SITE, 5) + ',' + String(LIM_FE_2_SITE, 5) + ',' + String(AMP_FE_3_SITE, 5) +
                              ',' + String(OFFS_FE_3_SITE, 5);

            if (flagService == false)
            {
                Serial.println(dataCoeficients);
            }

            /* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++
             + ++++ ESTADO PARTA REINICIAR LA TAREA DE CALIBRACION ++++
             + +++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
            if (flagRestartTask == true)
            {
                flagService = true;
                servMenuStateNew = SERV_MAIN_MENU;
                servMenuStateCurrent = servMenuStateNew;
                flagRestartTask = false;

                xTaskCreatePinnedToCore(task_Service, "task_Service", 4096, NULL, 1, &serviceTaskHandle, taskCoreOne);
            }

            /* ********************************************************************
			  * **** ENVIO DE VARIABLES PARA CALIBRACION FABRICA *******************
			  * ********************************************************************/
            if (flagFlowPrintCalibration == true)
            {
                CalFin = CalFin + 0.1;
                CalFout = CalFout + 0.1;

                Serial.print(CalFin);
                Serial.print(",");
                Serial.println(CalFout); // informacion para calibracion de flujo
            }
            if (flagPcamPrintCalibration == true)
            {
                CalPin = CalPin + 0.1;
                Serial.println(CalPin);
            }
            if (flagPbagPrintCalibration == true)
            {
                CalPout = CalPout + 0.1;
                Serial.println(CalPout);
            }
            if (flagPpacPrintCalibration == true)
            {
                CalPpac = CalPpac + 0.1;
                Serial.println(CalPpac);
            }

            /* ********************************************************************
			  * **** ENVIO DE VARIABLES PARA CALIBRACION SITIO *********************
			  * ********************************************************************/
            if (flagFlowSitePrintCalibration == true)
            {
                CalFin = CalFin - 0.1;
                CalFout = CalFout - 0.1;

                Serial.print(CalFin);
                Serial.print(",");
                Serial.println(CalFout); // informacion para calibracion de flujo
            }
            if (flagPcamSitePrintCalibration == true)
            {
                CalPin = CalPin - 0.1;
                Serial.println(CalPin);
            }
            if (flagPbagSitePrintCalibration == true)
            {
                CalPout = CalPout - 0.1;
                Serial.println(CalPout);
            }
            if (flagPpacSitePrintCalibration == true)
            {
                CalPpac = CalPpac - 0.1;
                Serial.println(CalPpac);
            }
        }
        vTaskDelay(20 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}

/* ***************************************************************************
 * **** CONFIGURACION ********************************************************
 * ***************************************************************************/
void setup()
{
    init_Memory();
    init_TIMER();

    pinMode(2, OUTPUT); // para prueba de retardos en la ejecucion de tareas

    // se crea el semaforo binario
    xSemaphoreTimer = xSemaphoreCreateBinary();
    xSemaphoreRaspberry = xSemaphoreCreateBinary();

    Serial.begin(115200);

    flagService = true;
    servMenuStateNew = SERV_MAIN_MENU;
    servMenuStateCurrent = servMenuStateNew;

    dataCoeficients = SerialID + ',' + String(AMP_CAM_1, 5) + ',' + String(OFFS_CAM_1, 5) + ',' + String(AMP_BAG_2, 5) +
                      ',' + String(OFFS_BAG_2, 5) + ',' + String(AMP_PAC_3, 5) + ',' + String(OFFS_PAC_3, 5) + ',' + String(AMP_FI_1, 5) +
                      ',' + String(OFFS_FI_1, 5) + ',' + String(LIM_FI_1, 5) + ',' + String(AMP_FI_2, 5) + ',' + String(OFFS_FI_2, 5) +
                      ',' + String(LIM_FI_2, 5) + ',' + String(AMP_FI_3, 5) + ',' + String(OFFS_FI_3, 5) + ',' + String(AMP_FE_1, 5) +
                      ',' + String(OFFS_FE_1, 5) + ',' + String(LIM_FE_1, 5) + ',' + String(AMP_FE_2, 5) + ',' + String(OFFS_FE_2, 5) +
                      ',' + String(LIM_FE_2, 5) + ',' + String(AMP_FE_3, 5) + ',' + String(OFFS_FE_3, 5) + ',' + String(VOL_SCALE, 5) + ',' + String(VOL_SCALE_SITE, 5) +
                      ',' + String(AMP_CAM_1_SITE, 5) + ',' + String(OFFS_CAM_1_SITE, 5) + ',' + String(AMP_BAG_2_SITE, 5) +
                      ',' + String(OFFS_BAG_2_SITE, 5) + ',' + String(AMP_PAC_3_SITE, 5) + ',' + String(OFFS_PAC_3_SITE, 5) +
                      ',' + String(AMP_FI_1_SITE, 5) + ',' + String(OFFS_FI_1_SITE, 5) + ',' + String(LIM_FI_1_SITE, 5) +
                      ',' + String(AMP_FI_2_SITE, 5) + ',' + String(OFFS_FI_2_SITE, 5) + ',' + String(LIM_FI_2_SITE, 5) +
                      ',' + String(AMP_FI_3_SITE, 5) + ',' + String(OFFS_FI_3_SITE, 5) + ',' + String(AMP_FE_1_SITE, 5) +
                      ',' + String(OFFS_FE_1_SITE, 5) + ',' + String(LIM_FE_1_SITE, 5) + ',' + String(AMP_FE_2_SITE, 5) +
                      ',' + String(OFFS_FE_2_SITE, 5) + ',' + String(LIM_FE_2_SITE, 5) + ',' + String(AMP_FE_3_SITE, 5) +
                      ',' + String(OFFS_FE_3_SITE, 5);

    Serial.println(dataCoeficients);

    // nvs_flash_init();

    // creo la tarea task_pulsador
    xTaskCreatePinnedToCore(task_Timer, "task_Timer", 2048, NULL, 7, NULL, taskCoreOne);
    xTaskCreatePinnedToCore(task_Service, "task_Service", 4096, NULL, 1, &serviceTaskHandle, taskCoreOne);
    xTaskCreatePinnedToCore(task_ReceiveService, "task_ReceiveService", 4096, NULL, 1, NULL, taskCoreOne);
    xTaskCreatePinnedToCore(task_Raspberry, "task_Raspberry", 4096, NULL, 4, NULL, taskCoreOne);
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

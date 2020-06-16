/*
  Name:		configurationUnit.ino
  Created:	4/3/2020 11:40:24
  Author:	GIBIC UdeA
*/

#include <Arduino.h>
#include <Esp.h>
#include <EEPROM.h>
#include <nvs_flash.h>
#include <stdio.h>

//********DEFINICIONES CONDICIONES******
#define TRUE 1
#define FALSE 0

//********DEFINICION DISPOSITIVO********
#define SERIAL_DEVICE "1NEUMA0001"
#define SERIAL_LENGTH 10

// definiciones para menu principal de servicio
#define SERV_NULL_MENU 0
#define SERV_MAIN_MENU 1 // estado de menu de inicio
#define SERV_WAIT_MAIN 2 // estado de espera de seleccion de opcion
#define SERV_FACT_CALI 4 // estado de menu de calibracion de fabrica
#define SERV_WAIT_FACT 5 // estado de espera en el menu de seleccion
#define SERV_FAIN_CALI 6 // estado de menu de calibracion de fabrica por variables
#define SERV_WAIT_FAIN 7 // estado de espera en el menu de seleccion por variables
#define SERV_ACQI_DATA 8 // estado de adquisicion de datos
#define SERV_SERIAL_CH 9 // estado de cambio de serial

#define MAX_MAIN_MENU 5  // cantidad de opciones en menu principal
#define MAX_FACT_MENU 10 // cantidad de opciones en menu de calibracion de fabrica
#define MAX_FAIN_MENU 12 // cantidad de opciones en menu de calibracion de fabrica por variables

#define MODE_NULL 0
#define FLUJO_INSPIRATORIO 1
#define FLUJO_ESPIRATORIO 2

#define AMPL_1 1

uint8_t servMenuStateCurrent = SERV_NULL_MENU;
uint8_t servMenuStateNew = SERV_NULL_MENU;
uint8_t modeCalibration = MODE_NULL;
uint8_t coeftype = MODE_NULL;

String menuString;

String SerialID;

/* **********************************************************************
 * **** VARIABLES DE CALIBRACION ****************************************
 * **********************************************************************/
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
volatile float VOL_SCALE = 0; // Factor de escala para ajustar el volumen

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

//********MEMORY MNEMONICS**************
enum eeprom_values
{
    // Serial del DISPOSITIVO
    SERIAL_ADDR = 8,
    // Valores de fabrica
    // Flujo Inspiratorio
    AMP_FI_1_ADDR = 20,
    OFFS_FI_1_ADDR = 24,
    LIM_FI_1_ADDR = 28,
    AMP_FI_2_ADDR = 32,
    OFFS_FI_2_ADDR = 36,
    LIM_FI_2_ADDR = 40,
    AMP_FI_3_ADDR = 44,
    OFFS_FI_3_ADDR = 48,
    // Flujo expiratorio
    AMP_FE_1_ADDR = 52,
    OFFS_FE_1_ADDR = 56,
    LIM_FE_1_ADDR = 60,
    AMP_FE_2_ADDR = 64,
    OFFS_FE_2_ADDR = 68,
    LIM_FE_2_ADDR = 72,
    AMP_FE_3_ADDR = 76,
    OFFS_FE_3_ADDR = 80,
    // Presion Camara
    AMP_CAM_1_ADDR = 84,
    OFFS_CAM_1_ADDR = 88,
    // Presion Bolsa
    AMP_BAG_2_ADDR = 92,
    OFFS_BAG_2_ADDR = 96,
    // Presion Paciente
    AMP_PAC_3_ADDR = 100,
    OFFS_PAC_3_ADDR = 104,

    // Valores de sitio
    // Flujo Inspiratorio
    AMP_FI_1_SITE_ADDR = 110,
    OFFS_FI_1_SITE_ADDR = 114,
    LIM_FI_1_SITE_ADDR = 118,
    AMP_FI_2_SITE_ADDR = 122,
    OFFS_FI_2_SITE_ADDR = 126,
    LIM_FI_2_SITE_ADDR = 130,
    AMP_FI_3_SITE_ADDR = 134,
    OFFS_FI_3_SITE_ADDR = 138,
    // Flujo expiratorio
    AMP_FE_1_SITE_ADDR = 142,
    OFFS_FE_1_SITE_ADDR = 146,
    LIM_FE_1_SITE_ADDR = 150,
    AMP_FE_2_SITE_ADDR = 154,
    OFFS_FE_2_SITE_ADDR = 158,
    LIM_FE_2_SITE_ADDR = 162,
    AMP_FE_3_SITE_ADDR = 166,
    OFFS_FE_3_SITE_ADDR = 170,
    // Presion Camara
    AMP_CAM_1_SITE_ADDR = 174,
    OFFS_CAM_1_SITE_ADDR = 178,
    // Presion Bolsa
    AMP_BAG_2_SITE_ADDR = 182,
    OFFS_BAG_2_SITE_ADDR = 186,
    // Presion Paciente
    AMP_PAC_3_SITE_ADDR = 190,
    OFFS_PAC_3_SITE_ADDR = 194,
    // Volumen ajuste
    VOL_SCALE_ADDR = 198

};

String dataCoeficients;

// bandera de activacion de timer
uint8_t flagTimerInterrupt = false;
uint8_t flagService = false;

//creo el manejador para el semaforo como variable global
SemaphoreHandle_t xSemaphoreTimer = NULL;

// definicion de los core para ejecucion
static uint8_t taskCoreZero = 0;
static uint8_t taskCoreOne = 1;

// inicializacion del contador del timer
hw_timer_t *timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

// definicion de interrupciones
void IRAM_ATTR onTimer(void); // funcion de interrupcion

/* ***************************************************************************
 * **** CONFIGURACION MEMORIA ************************************************
 * ***************************************************************************/
void init_Memory(void)
{
    EEPROM.begin(1024);

    SerialID.reserve(12);

    // Carga del Serial del equipo
    SerialID = readString(eeprom_values::SERIAL_ADDR);
    // Calibracion de los sensores de presion - coeficientes regresion lineal
    AMP_CAM_1 = readFloat(eeprom_values::AMP_CAM_1_ADDR);
    AMP_BAG_2 = readFloat(eeprom_values::AMP_BAG_2_ADDR);
    AMP_PAC_3 = readFloat(eeprom_values::AMP_PAC_3_ADDR);
    OFFS_CAM_1 = readFloat(eeprom_values::OFFS_CAM_1_ADDR);
    OFFS_BAG_2 = readFloat(eeprom_values::OFFS_BAG_2_ADDR);
    OFFS_PAC_3 = readFloat(eeprom_values::OFFS_PAC_3_ADDR);
    // Calibracion de los sensores de flujo - coeficientes regresion lineal
    // Sensor de flujo Inspiratorio
    AMP_FI_1 = readFloat(eeprom_values::AMP_FI_1_ADDR);
    AMP_FI_2 = readFloat(eeprom_values::AMP_FI_2_ADDR);
    AMP_FI_3 = readFloat(eeprom_values::AMP_FI_3_ADDR);
    OFFS_FI_1 = readFloat(eeprom_values::OFFS_FI_1_ADDR);
    OFFS_FI_2 = readFloat(eeprom_values::OFFS_FI_2_ADDR);
    OFFS_FI_3 = readFloat(eeprom_values::OFFS_FI_3_ADDR);
    LIM_FI_1 = readFloat(eeprom_values::LIM_FI_1_ADDR);
    LIM_FI_2 = readFloat(eeprom_values::LIM_FI_2_ADDR);
    // Sensor de flujo Espiratorio
    AMP_FE_1 = readFloat(eeprom_values::AMP_FE_1_ADDR);
    AMP_FE_2 = readFloat(eeprom_values::AMP_FE_2_ADDR);
    AMP_FE_3 = readFloat(eeprom_values::AMP_FE_3_ADDR);
    OFFS_FE_1 = readFloat(eeprom_values::OFFS_FE_1_ADDR);
    OFFS_FE_2 = readFloat(eeprom_values::OFFS_FE_2_ADDR);
    OFFS_FE_3 = readFloat(eeprom_values::OFFS_FE_3_ADDR);
    LIM_FE_1 = readFloat(eeprom_values::LIM_FE_1_ADDR);
    LIM_FE_2 = readFloat(eeprom_values::LIM_FE_2_ADDR);
    // variable para ajustar el nivel cero de flujo y calcular el volumen
    VOL_SCALE = readFloat(eeprom_values::VOL_SCALE_ADDR); // Factor de escala para ajustar el volumen

    // Calibracion sensores Sitio
    AMP_CAM_1_SITE = readFloat(eeprom_values::AMP_CAM_1_SITE_ADDR);
    AMP_BAG_2_SITE = readFloat(eeprom_values::AMP_BAG_2_SITE_ADDR);
    AMP_PAC_3_SITE = readFloat(eeprom_values::AMP_PAC_3_SITE_ADDR);
    OFFS_CAM_1_SITE = readFloat(eeprom_values::OFFS_CAM_1_SITE_ADDR);
    OFFS_BAG_2_SITE = readFloat(eeprom_values::OFFS_BAG_2_SITE_ADDR);
    OFFS_PAC_3_SITE = readFloat(eeprom_values::OFFS_PAC_3_SITE_ADDR);
    // Calibracion de los sensores de flujo - coeficientes regresion lineal
    // Sensor de flujo Inspiratorio
    AMP_FI_1_SITE = readFloat(eeprom_values::AMP_FI_1_SITE_ADDR);
    AMP_FI_2_SITE = readFloat(eeprom_values::AMP_FI_2_SITE_ADDR);
    AMP_FI_3_SITE = readFloat(eeprom_values::AMP_FI_3_SITE_ADDR);
    OFFS_FI_1_SITE = readFloat(eeprom_values::OFFS_FI_1_SITE_ADDR);
    OFFS_FI_2_SITE = readFloat(eeprom_values::OFFS_FI_2_SITE_ADDR);
    OFFS_FI_3_SITE = readFloat(eeprom_values::OFFS_FI_3_SITE_ADDR);
    LIM_FI_1_SITE = readFloat(eeprom_values::LIM_FI_1_SITE_ADDR);
    LIM_FI_2_SITE = readFloat(eeprom_values::LIM_FI_2_SITE_ADDR);
    // Sensor de flujo Espiratorio
    AMP_FE_1_SITE = readFloat(eeprom_values::AMP_FE_1_SITE_ADDR);
    AMP_FE_2_SITE = readFloat(eeprom_values::AMP_FE_2_SITE_ADDR);
    AMP_FE_3_SITE = readFloat(eeprom_values::AMP_FE_3_SITE_ADDR);
    OFFS_FE_1_SITE = readFloat(eeprom_values::OFFS_FE_1_SITE_ADDR);
    OFFS_FE_2_SITE = readFloat(eeprom_values::OFFS_FE_2_SITE_ADDR);
    OFFS_FE_3_SITE = readFloat(eeprom_values::OFFS_FE_3_SITE_ADDR);
    LIM_FE_1_SITE = readFloat(eeprom_values::LIM_FE_1_SITE_ADDR);
    LIM_FE_2_SITE = readFloat(eeprom_values::LIM_FE_2_SITE_ADDR);

    // inicializacion de variables String
    menuString.reserve(512);
    menuString = "";
}

// funcion para escribir Strings en la memoria EEPROM
void writeString(eeprom_values eeAddress, String dataStr)
{
    int _size = SERIAL_LENGTH;
    for (int i = 0; i < _size; i++)
    {
        EEPROM.write(eeAddress + i, dataStr[i]);
    }
    EEPROM.write(eeAddress + _size, '\0'); // add termination NULL
    EEPROM.commit();
}

// funcion para leer Strings en la memoria EEPROM
String readString(eeprom_values eeAddress)
{
    int i = 0;
    char dataStr[12];
    int len = 0;
    unsigned char k;
    k = EEPROM.read(eeAddress);
    while (k != '\0' && len < 11)
    {
        k = EEPROM.read(eeAddress + len);
        dataStr[len] = k;
        len++;
    }
    dataStr[len] = '\0';
    return String(dataStr);
}

// funcion para obtener valores flotantes de la EEPROM
float readFloat(eeprom_values eeAddress)
{
    float varValue = 0;
    EEPROM.get(eeAddress, varValue);
    return varValue;
}

// funcion para escribir valores flotantes de la EEPROM
float writeFloat(eeprom_values eeAddress, float varValue)
{
    float recoverValue = 0;
    EEPROM.put(eeAddress, varValue);
    EEPROM.commit();
    EEPROM.get(eeAddress, recoverValue);
    return recoverValue;
}

/* ***************************************************************************
 * **** CONFIGURACION TIMER **************************************************
 * ***************************************************************************/
// Interrupcion por timer
void IRAM_ATTR onTimer(void)
{
    portENTER_CRITICAL_ISR(&timerMux);
    flagTimerInterrupt = true;                    // asignacion de banderas para atencion de interrupcion
    xSemaphoreGiveFromISR(xSemaphoreTimer, NULL); // asignacion y liberacion de semaforos
    portEXIT_CRITICAL_ISR(&timerMux);
}

void init_TIMER()
{
    // Configuracion del timer a 1 kHz
    timer = timerBegin(0, 80, true);             // Frecuencia de reloj 80 MHz, prescaler de 80, frec 1 MHz
    timerAttachInterrupt(timer, &onTimer, true); // Attach onTimer function to our timer
    timerAlarmWrite(timer, 1000, true);          // Interrupcion cada 1000 conteos del timer, es decir 100 Hz
    timerAlarmEnable(timer);                     // Habilita interrupcion por timer
}

/************************************************************
 ***** FUNCIONES DE ATENCION A INTERRUPCION TAREA TIMER *****
 ************************************************************/
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
                if (counterMs == 1000)
                {
                    counterMs = 0;

                    digitalWrite(2, !digitalRead(2));

                    dataCoeficients = SerialID + ',' + String(AMP_CAM_1, 5) + ',' + String(OFFS_CAM_1, 5) + ',' + String(AMP_BAG_2, 5) +
                                      ',' + String(OFFS_BAG_2, 5) + ',' + String(AMP_PAC_3, 5) + ',' + String(OFFS_PAC_3, 5) + ',' + String(AMP_FI_1, 5) +
                                      ',' + String(OFFS_FI_1, 5) + ',' + String(LIM_FI_1, 5) + ',' + String(AMP_FI_2, 5) + ',' + String(OFFS_FI_2, 5) +
                                      ',' + String(LIM_FI_2, 5) + ',' + String(AMP_FI_3, 5) + ',' + String(OFFS_FI_3, 5) + ',' + String(AMP_FE_1, 5) +
                                      ',' + String(OFFS_FE_1, 5) + ',' + String(LIM_FE_1, 5) + ',' + String(AMP_FE_2, 5) + ',' + String(OFFS_FE_2, 5) +
                                      ',' + String(LIM_FE_2, 5) + ',' + String(AMP_FE_3, 5) + ',' + String(OFFS_FE_3, 5) + ',' + String(VOL_SCALE, 5) +
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
            default:
                break;
            }
        }
        vTaskDelay(250 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}

void printMainMenu()
{
    menuString = "\n\n************ Menu Principal ************\n"
                 "1. Alimentacion. \n"
                 "2. Estado de valvulas. \n"
                 "3. Coeficientes de fabrica. \n"
                 "4. Coeficientes de sitio. \n"
                 "5. Salir. \n\nSeleccione una opcion: ";

    Serial.print(menuString);
    servMenuStateNew = SERV_WAIT_MAIN;
    servMenuStateCurrent = servMenuStateNew;
}

void printFactoryMenu()
{
    menuString = "\n\n*** Calibracion coeficientes fabrica ***\n"
                 "1. Cambiar Serial del Equipo (" +
                 SerialID + ").\n"
                            "2. Flujo inspiratorio.\n"
                            "3. Flujo expiratorio.\n"
                            "4. Presion Camara.\n"
                            "5. Presion Bolsa.\n"
                            "6. Presion Paciente.\n"
                            "7. Escala de volumen.\n"
                            "8. Menu anterior. \n"
                            "9. Salir.\n\nSeleccione una opcion: ";

    Serial.print(menuString);
    servMenuStateNew = SERV_WAIT_FACT;
    servMenuStateCurrent = servMenuStateNew;
}

void printInternalFactoryMenu(int mode)
{
    if (mode == FLUJO_INSPIRATORIO)
    {
        menuString = "\n\n************ FLUJO INSPIRATORIO ************\n"
                     "1. Imprimir datos 20 Hz.\n"
                     "*Curva de ajuste  N 1 \n2. Amplitud.\n3. Offset.\n4. Limite.\n"
                     "*Curva de ajuste  N 2 \n5. Amplitud.\n6. Offset.\n7. Limite.\n"
                     "*Curva de ajuste  N 3 \n8. Amplitud.\n9. Offset.\n"
                     "10. Ver constantes \n"
                     "11. Menu anterior. \n"
                     "12. Salir.\n\nSeleccione una opcion: ";
    }
    if (mode == FLUJO_ESPIRATORIO)
    {
        menuString = "\n\n************ FLUJO ESPIRATORIO ************\n"
                     "1. Imprimir datos 20 Hz.\n"
                     "*Curva de ajuste  N 1 \n2. Amplitud.\n3. Offset.\n4. Limite.\n"
                     "*Curva de ajuste  N 2 \n5. Amplitud.\n6. Offset.\n7. Limite.\n"
                     "*Curva de ajuste  N 3 \n8. Amplitud.\n9. Offset.\n"
                     "10. Ver constantes \n"
                     "11. Menu anterior. \n"
                     "12. Salir.\n\nSeleccione una opcion: ";
    }

    Serial.print(menuString);
    servMenuStateNew = SERV_WAIT_FAIN;
    servMenuStateCurrent = servMenuStateNew;
}

void mainMenuOptionChange(int selMenu)
{
    if ((selMenu > MAX_MAIN_MENU) || (selMenu == 0))
    {
        Serial.print("Opcion no valida \n");
        servMenuStateNew = SERV_MAIN_MENU;
        servMenuStateCurrent = servMenuStateNew;
    }
    else
    {
        switch (selMenu)
        {
        case 1: // seleccion del menu 1
            /* code */
            break;
        case 2: // seleccion del menu 2
            /* code */
            break;
        case 3: // seleccion del menu 3, calibracion de fabrica
            servMenuStateNew = SERV_FACT_CALI;
            servMenuStateCurrent = servMenuStateNew;
            break;
        case 4: // seleccion del menu 4
            /* code */
            break;
        case 5: // seleccion del menu 5, Salir
            Serial.print("Salida segura \n");
            servMenuStateNew = SERV_NULL_MENU;
            servMenuStateCurrent = servMenuStateNew;
            flagService = false;
            break;
        default:
            break;
        }

        // agregar las opciones de menu
        // Serial.println("Menu seleccionado");
    }
}

void factoryMenuOptionChange(int selMenu)
{
    if ((selMenu > MAX_FACT_MENU) || (selMenu == 0))
    {
        Serial.print("Opcion no valida \n");
        servMenuStateNew = SERV_FACT_CALI;
        servMenuStateCurrent = servMenuStateNew;
    }
    else
    {
        switch (selMenu)
        {
        case 1: // seleccion del menu 2
            servMenuStateNew = SERV_SERIAL_CH;
            servMenuStateCurrent = servMenuStateNew;
            Serial.print("Ingrese el serial del equipo: ");
            break;
        case 2: // seleccion del menu 1
            modeCalibration = FLUJO_INSPIRATORIO;
            servMenuStateNew = SERV_FAIN_CALI;
            servMenuStateCurrent = servMenuStateNew;
            printInternalFactoryMenu(modeCalibration);
            break;
        case 3: // seleccion del menu 2
            modeCalibration = FLUJO_ESPIRATORIO;
            servMenuStateNew = SERV_FAIN_CALI;
            servMenuStateCurrent = servMenuStateNew;
            printInternalFactoryMenu(modeCalibration);
            break;
        case 4: // seleccion del menu 3, calibracion de fabrica
            /* code */
            break;
        case 5: // seleccion del menu 4
            /* code */
            break;
        case 6: // seleccion del menu 5, Salir
            /* code */
            break;
        case 7: // seleccion del menu 5, Salir
            /* code */
            break;
        case 8: // seleccion del menu 5, Salir
            Serial.print("Salida Calibracion \n");
            servMenuStateNew = SERV_MAIN_MENU;
            servMenuStateCurrent = servMenuStateNew;
            break;
        case 9: // seleccion del menu 7, Salir
            Serial.print("Salida segura \n");
            servMenuStateNew = SERV_NULL_MENU;
            servMenuStateCurrent = servMenuStateNew;
            flagService = false;
            break;
        default:
            break;
        }
    }
}

void factoryInternalMenuOptionChange(int selMenu)
{
    float currentValue = 0;
    String dataPrint;

    if ((selMenu > MAX_FAIN_MENU) || (selMenu == 0))
    {
        Serial.print("Opcion no valida \n");
        servMenuStateNew = SERV_FAIN_CALI;
        servMenuStateCurrent = servMenuStateNew;
    }
    else
    {
        switch (selMenu)
        {
        case 1: // seleccion del menu 1
            /* code */
            break;
        case 2: // seleccion del menu 2
            /* code */
            if (modeCalibration == FLUJO_INSPIRATORIO)
            {
                coeftype = AMPL_1;
                currentValue = readFloat(eeprom_values::AMP_FI_1_ADDR);
                dataPrint = "Curva 1, Offset actual: " + String(currentValue, 5) + "\nIngrese nuevo valor: ";
                Serial.print(dataPrint);
                servMenuStateNew = SERV_ACQI_DATA;
                servMenuStateCurrent = servMenuStateNew;
            }
            if (modeCalibration == FLUJO_ESPIRATORIO)
            {
                coeftype = AMPL_1;
                currentValue = readFloat(eeprom_values::AMP_FE_1_ADDR);
                dataPrint = "Curva 1, Offset actual: " + String(currentValue, 5) + "\nIngrese nuevo valor: ";
                Serial.print(dataPrint);
                servMenuStateNew = SERV_ACQI_DATA;
                servMenuStateCurrent = servMenuStateNew;
            }

            break;
        case 3: // seleccion del menu 3, calibracion de fabrica
            /* code */
            break;
        case 4: // seleccion del menu 4
            /* code */
            break;
        case 5: // seleccion del menu 5, Salir
        /* code */
        case 11: // seleccion del menu 5, Salir
            Serial.print("Salida Calibracion \n");
            servMenuStateNew = SERV_MAIN_MENU;
            servMenuStateCurrent = servMenuStateNew;
            break;
        case 12: // seleccion del menu 7, Salir
            Serial.print("Salida segura \n");
            servMenuStateNew = SERV_NULL_MENU;
            servMenuStateCurrent = servMenuStateNew;
            flagService = false;
            break;
        default:
            break;
        }

        // agregar las opciones de menu
        Serial.println("Menu fabrica seleccionado");
    }
}

/* ***************************************************************************
 * **** Ejecucion de la rutina de comunicacion por serial ********************
 * ***************************************************************************/
// Function to receive data from serial communication
void task_Receive(void *pvParameters)
{

    String menuEntrance;
    menuEntrance.reserve(50);
    int selMenu = 0;
    float newCoeficient = 0;
    String newSerial;

    uint8_t flagFirstPrint = false;

    // Clean Serial buffers
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    Serial.flush();
    Serial2.flush();

    while (1)
    {
        if (flagService == false)
        {
            // if (Serial2.available() > 5)
            // {
            //     // if (Serial.available() > 5) {  // solo para pruebas
            //     String dataIn = Serial2.readStringUntil(';');
            //     // String dataIn = Serial.readStringUntil(';');  // solo para pruebas
            //     // Serial.println(dataIn);
            //     int contComas = 0;
            //     for (int i = 0; i < dataIn.length(); i++)
            //     {
            //         if (dataIn[i] == ',')
            //         {
            //             contComas++;
            //         }
            //     }
            //     String dataIn2[40];
            //     for (int i = 0; i < contComas + 1; i++)
            //     {
            //         dataIn2[i] = dataIn.substring(0, dataIn.indexOf(','));
            //         dataIn = dataIn.substring(dataIn.indexOf(',') + 1);
            //     }
            //     //cargue los datos aqui
            //     //para entero
            //     //contCiclos =dataIn2[0].toInt();
            //     //para float
            //     newFrecRespiratoria = dataIn2[0].toInt();
            //     newI = dataIn2[1].toInt();
            //     newE = dataIn2[2].toInt();
            //     maxPresion = dataIn2[3].toInt();
            //     alerBateria = dataIn2[4].toInt();
            //     estabilidad = dataIn2[5].toInt();
            //     newStateMachine = dataIn2[6].toInt();
            //     newVentilationMode = dataIn2[7].toInt();
            //     newTrigger = dataIn2[8].toInt();
            //     newPeepMax = dataIn2[9].toInt();
            //     maxFR = dataIn2[10].toInt();
            //     maxVE = dataIn2[11].toInt();
            //     apneaTime = dataIn2[12].toInt();
            //     Serial2.flush();
            //     // Serial.flush();  // solo para pruebas
            //     /*Serial.println("State = " + String(currentStateMachine));
            //     Serial.println(String(newFrecRespiratoria) + ',' + String(newI) + ',' +
            //     String(newE) + ',' + String(maxPresion) + ',' +
            //     String(alerBateria) + ',' + String(estabilidad) + ',' +
            //     String(newStateMachine) + ',' + String(newVentilationMode));*/
            // }
        }
        else
        { // se encuentra en modo servicio
            if (Serial.available() > 1)
            {
                menuEntrance = Serial.readStringUntil('\n');
                switch (servMenuStateCurrent)
                {
                case SERV_WAIT_MAIN:
                    selMenu = menuEntrance.toInt();
                    Serial.println(selMenu);
                    mainMenuOptionChange(selMenu);
                    break;
                case SERV_WAIT_FACT:
                    selMenu = menuEntrance.toInt();
                    Serial.println(selMenu);
                    factoryMenuOptionChange(selMenu);
                    break;
                case SERV_WAIT_FAIN:
                    selMenu = menuEntrance.toInt();
                    Serial.println(selMenu);
                    factoryInternalMenuOptionChange(selMenu);
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
                            changeMenu(coeftype, modeCalibration, newCoeficient);
                            newCoeficient = 0;
                            modeCalibration = MODE_NULL;
                            servMenuStateNew = SERV_FACT_CALI;
                            servMenuStateCurrent = servMenuStateNew;
                            flagFirstPrint = false;
                        }
                        else
                        {
                            newCoeficient = 0;
                            modeCalibration = MODE_NULL;
                            servMenuStateNew = SERV_FACT_CALI;
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
            }
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}

void changeMenu(uint8_t coeftype, uint8_t modeCalibration, float newCoeficient)
{
    float writeData = 0;
    String confirm;
    if (coeftype == AMPL_1)
    {
        if (modeCalibration == FLUJO_INSPIRATORIO)
        {
            writeData = writeFloat(eeprom_values::AMP_FI_1_ADDR, newCoeficient);
        }
        if (modeCalibration == FLUJO_ESPIRATORIO)
        {
            writeData = writeFloat(eeprom_values::AMP_FE_1_ADDR, newCoeficient);
        }
    }

    confirm = "Dato guardado: " + String(writeData, 5) + "\n";
    Serial.print(confirm);
}

/* *********************
******************************************************
 * **** CONFIGURACION ********************************************************
 * ***************************************************************************/
void setup()
{
    init_Memory();
    init_TIMER();

    pinMode(2, OUTPUT); // para prueba de retardos en la ejecucion de tareas

    // se crea el semaforo binario
    xSemaphoreTimer = xSemaphoreCreateBinary();

    Serial.begin(115200);

    flagService = true;
    servMenuStateNew = SERV_MAIN_MENU;
    servMenuStateCurrent = servMenuStateNew;

    dataCoeficients = SerialID + ',' + String(AMP_CAM_1, 5) + ',' + String(OFFS_CAM_1, 5) + ',' + String(AMP_BAG_2, 5) +
                      ',' + String(OFFS_BAG_2, 5) + ',' + String(AMP_PAC_3, 5) + ',' + String(OFFS_PAC_3, 5) + ',' + String(AMP_FI_1, 5) +
                      ',' + String(OFFS_FI_1, 5) + ',' + String(LIM_FI_1, 5) + ',' + String(AMP_FI_2, 5) + ',' + String(OFFS_FI_2, 5) +
                      ',' + String(LIM_FI_2, 5) + ',' + String(AMP_FI_3, 5) + ',' + String(OFFS_FI_3, 5) + ',' + String(AMP_FE_1, 5) +
                      ',' + String(OFFS_FE_1, 5) + ',' + String(LIM_FE_1, 5) + ',' + String(AMP_FE_2, 5) + ',' + String(OFFS_FE_2, 5) +
                      ',' + String(LIM_FE_2, 5) + ',' + String(AMP_FE_3, 5) + ',' + String(OFFS_FE_3, 5) + ',' + String(VOL_SCALE, 5) +
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
    xTaskCreatePinnedToCore(task_Service, "task_Service", 4096, NULL, 1, NULL, taskCoreOne);
    xTaskCreatePinnedToCore(task_Receive, "task_Receive", 4096, NULL, 1, NULL, taskCoreOne);
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

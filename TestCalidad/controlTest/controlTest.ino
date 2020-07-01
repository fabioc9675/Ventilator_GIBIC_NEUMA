/*
  Name:		controlTest.ino
  Created:  30/06/2020 11:40:24
  Author:	GIBIC UdeA
*/

#include <Arduino.h>
#include <Esp.h>
#include <EEPROM.h>
#include <stdio.h>

//********DEFINICIONES CONDICIONES******
#define TRUE 1
#define FALSE 0

//********DEFINICION DISPOSITIVO********
#define SERIAL_DEVICE "1NEUMA0000"
#define SERIAL_LENGTH 10

// Definiciones para el manejo del ADC
#define ADC_PRESS_1 32
#define ADC_PRESS_2 35
#define ADC_PRESS_3 34
#define ADC_PRESS_4 30
#define ADC_PRESS_5 36
#define ADC_PRESS_6 27

// Definiciones para el manejo del EV
#define EV_OUTPUT_1 18
#define EV_OUTPUT_2 5
#define EV_OUTPUT_3 4
#define EV_OUTPUT_4 12
#define EV_OUTPUT_5 13
#define EV_OUTPUT_6 2

// Variables de control del protocolo
#define RXD2 16
#define TXD2 17

// Variables de comunicacion con monitor
#define PIN_1 23
#define PIN_2 25

// Pines adicionales
#define IO_1 19
#define IO_2 21
#define IO_3 22

/* ***************************************************************
 * **** VARIABLES DE CALIBRACION A GUARDAR ***********************
 * ***************************************************************/
// Calibracion de los sensores de presion - coeficientes regresion lineal
#define AMP_CAM_1_W     0.073182
#define OFFS_CAM_1_W    -12.1276
#define AMP_BAG_2_W     0.028673
#define OFFS_BAG_2_W    -19.3990
#define AMP_PAC_3_W     0.028673
#define OFFS_PAC_3_W    -19.3990

// Calibracion de los sensores de flujo - coeficientes regresion lineal
// Sensor de flujo Inspiratorio
#define AMP_FI_1_W      0.139500
#define OFFS_FI_1_W     -264.343600
#define LIM_FI_1_W      1786
#define AMP_FI_2_W      0.640000
#define OFFS_FI_2_W     -1158.259200
#define LIM_FI_2_W      1834
#define AMP_FI_3_W      0.139500
#define OFFS_FI_3_W     -240.569600

// Sensor de flujo Espiratorio
#define AMP_FE_1_W      0.132000
#define OFFS_FE_1_W     -238.563900
#define LIM_FE_1_W      1692
#define AMP_FE_2_W      0.748000
#define OFFS_FE_2_W     -1280.839400
#define LIM_FE_2_W      1733
#define AMP_FE_3_W      0.132000
#define OFFS_FE_3_W     -213.529000

// variable para ajustar el nivel cero de flujo y calcular el volumen
#define VOL_SCALE_W          1.00 // Factor de escala para ajustar el volumen
#define VOL_SCALE_SITE_W     1.00 // Factor de escala para ajustar el volumen en sitio

// **********************************************************
// Calibracion sensores Sitio
// Calibracion de los sensores de presion - coeficientes regresion lineal
#define AMP_CAM_1_SITE_W    1.00
#define OFFS_CAM_1_SITE_W   0.00
#define AMP_BAG_2_SITE_W    1.00
#define OFFS_BAG_2_SITE_W   0.00
#define AMP_PAC_3_SITE_W    1.00
#define OFFS_PAC_3_SITE_W   0.00

// Calibracion de los sensores de flujo - coeficientes regresion lineal
// Sensor de flujo Inspiratorio
#define AMP_FI_1_SITE_W    1.00
#define OFFS_FI_1_SITE_W   0.00
#define LIM_FI_1_SITE_W    LIM_FI_1_W
#define AMP_FI_2_SITE_W    1.00
#define OFFS_FI_2_SITE_W   0.00
#define LIM_FI_2_SITE_W    LIM_FI_2_W
#define AMP_FI_3_SITE_W    1.00
#define OFFS_FI_3_SITE_W   0.00

// Sensor de flujo Espiratorio
#define AMP_FE_1_SITE_W    1.00
#define OFFS_FE_1_SITE_W   0.00
#define LIM_FE_1_SITE_W    LIM_FE_1_W
#define AMP_FE_2_SITE_W    1.00
#define OFFS_FE_2_SITE_W   0.00
#define LIM_FE_2_SITE_W    LIM_FE_2_W
#define AMP_FE_3_SITE_W    1.00
#define OFFS_FE_3_SITE_W   0.00

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
  VOL_SCALE_ADDR = 198, 
  VOL_SCALE_SITE_ADDR = 202

};

// Variables para escritura de datos

String SerialID;
String dataCoeficients;
String menuString;
String dataReceive;
String dataSend;
String charged;

// variable para el encendido y apagado de los actuadores
static uint8_t flagTimerInterrupt = false;
static uint8_t stateMosfet = 0;
static uint16_t counterTimer = 0;

// inicializacion del contador del timer
hw_timer_t *timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

// definicion de interrupciones
void IRAM_ATTR onTimer(void); // funcion de interrupcion

/* ***************************************************************************
 * **** CONFIGURACION TIMER **************************************************
 * ***************************************************************************/
// Interrupcion por timer
void IRAM_ATTR onTimer(void)
{
  portENTER_CRITICAL_ISR(&timerMux);
  flagTimerInterrupt = true; // asignacion de banderas para atencion de interrupcion
  portEXIT_CRITICAL_ISR(&timerMux);
}

void init_Timer()
{
  // Configuracion del timer a 1 kHz
  timer = timerBegin(0, 80, true);             // Frecuencia de reloj 80 MHz, prescaler de 80, frec 1 MHz
  timerAttachInterrupt(timer, &onTimer, true); // Attach onTimer function to our timer
  timerAlarmWrite(timer, 1000, true);          // Interrupcion cada 1000 conteos del timer, es decir 100 Hz
  timerAlarmEnable(timer);                     // Habilita interrupcion por timer
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
 * **** CONFIGURACION MEMORIA ************************************************
 * ***************************************************************************/
void init_Memory(void)
{
  EEPROM.begin(1024);

  SerialID.reserve(12);

  // Carga del Serial del equipo
  SerialID = readString(eeprom_values::SERIAL_ADDR);
  
  if (SerialID[1] != 'N')
  {
    writeString(eeprom_values::SERIAL_ADDR, SERIAL_DEVICE);
    SerialID = readString(eeprom_values::SERIAL_ADDR);

    // Calibracion de los sensores de presion - coeficientes regresion lineal
    AMP_CAM_1 = writeFloat(eeprom_values::AMP_CAM_1_ADDR, AMP_CAM_1_W);
    AMP_BAG_2 = writeFloat(eeprom_values::AMP_BAG_2_ADDR, AMP_BAG_2_W);
    AMP_PAC_3 = writeFloat(eeprom_values::AMP_PAC_3_ADDR, AMP_PAC_3_W);
    OFFS_CAM_1 = writeFloat(eeprom_values::OFFS_CAM_1_ADDR, OFFS_CAM_1_W);
    OFFS_BAG_2 = writeFloat(eeprom_values::OFFS_BAG_2_ADDR, OFFS_BAG_2_W);
    OFFS_PAC_3 = writeFloat(eeprom_values::OFFS_PAC_3_ADDR, OFFS_PAC_3_W);
    // Calibracion de los sensores de flujo - coeficientes regresion lineal
    // Sensor de flujo Inspiratorio
    AMP_FI_1 = writeFloat(eeprom_values::AMP_FI_1_ADDR, AMP_FI_1_W);
    AMP_FI_2 = writeFloat(eeprom_values::AMP_FI_2_ADDR, AMP_FI_2_W);
    AMP_FI_3 = writeFloat(eeprom_values::AMP_FI_3_ADDR, AMP_FI_3_W);
    OFFS_FI_1 = writeFloat(eeprom_values::OFFS_FI_1_ADDR, OFFS_FI_1_W);
    OFFS_FI_2 = writeFloat(eeprom_values::OFFS_FI_2_ADDR, OFFS_FI_2_W);
    OFFS_FI_3 = writeFloat(eeprom_values::OFFS_FI_3_ADDR, OFFS_FI_3_W);
    LIM_FI_1 = writeFloat(eeprom_values::LIM_FI_1_ADDR, LIM_FI_1_W);
    LIM_FI_2 = writeFloat(eeprom_values::LIM_FI_2_ADDR, LIM_FI_2_W);
    // Sensor de flujo Espiratorio
    AMP_FE_1 = writeFloat(eeprom_values::AMP_FE_1_ADDR, AMP_FE_1_W);
    AMP_FE_2 = writeFloat(eeprom_values::AMP_FE_2_ADDR, AMP_FE_2_W);
    AMP_FE_3 = writeFloat(eeprom_values::AMP_FE_3_ADDR, AMP_FE_3_W);
    OFFS_FE_1 = writeFloat(eeprom_values::OFFS_FE_1_ADDR, OFFS_FE_1_W);
    OFFS_FE_2 = writeFloat(eeprom_values::OFFS_FE_2_ADDR, OFFS_FE_2_W);
    OFFS_FE_3 = writeFloat(eeprom_values::OFFS_FE_3_ADDR, OFFS_FE_3_W);
    LIM_FE_1 = writeFloat(eeprom_values::LIM_FE_1_ADDR, LIM_FE_1_W);
    LIM_FE_2 = writeFloat(eeprom_values::LIM_FE_2_ADDR, LIM_FE_2_W);
    // variable para ajustar el nivel cero de flujo y calcular el volumen
    VOL_SCALE = writeFloat(eeprom_values::VOL_SCALE_ADDR, VOL_SCALE_W); // Factor de escala para ajustar el volumen
    VOL_SCALE_SITE = writeFloat(eeprom_values::VOL_SCALE_SITE_ADDR, VOL_SCALE_SITE_W); // Factor de escala para ajustar el volumen

    // Calibracion sensores Sitio
    AMP_CAM_1_SITE = writeFloat(eeprom_values::AMP_CAM_1_SITE_ADDR, AMP_CAM_1_SITE_W);
    AMP_BAG_2_SITE = writeFloat(eeprom_values::AMP_BAG_2_SITE_ADDR, AMP_BAG_2_SITE_W);
    AMP_PAC_3_SITE = writeFloat(eeprom_values::AMP_PAC_3_SITE_ADDR, AMP_PAC_3_SITE_W);
    OFFS_CAM_1_SITE = writeFloat(eeprom_values::OFFS_CAM_1_SITE_ADDR, OFFS_CAM_1_SITE_W);
    OFFS_BAG_2_SITE = writeFloat(eeprom_values::OFFS_BAG_2_SITE_ADDR, OFFS_BAG_2_SITE_W);
    OFFS_PAC_3_SITE = writeFloat(eeprom_values::OFFS_PAC_3_SITE_ADDR, OFFS_PAC_3_SITE_W);
    // Calibracion de los sensores de flujo - coeficientes regresion lineal
    // Sensor de flujo Inspiratorio
    AMP_FI_1_SITE = writeFloat(eeprom_values::AMP_FI_1_SITE_ADDR, AMP_FI_1_SITE_W);
    AMP_FI_2_SITE = writeFloat(eeprom_values::AMP_FI_2_SITE_ADDR, AMP_FI_2_SITE_W);
    AMP_FI_3_SITE = writeFloat(eeprom_values::AMP_FI_3_SITE_ADDR, AMP_FI_3_SITE_W);
    OFFS_FI_1_SITE = writeFloat(eeprom_values::OFFS_FI_1_SITE_ADDR, OFFS_FI_1_SITE_W);
    OFFS_FI_2_SITE = writeFloat(eeprom_values::OFFS_FI_2_SITE_ADDR, OFFS_FI_2_SITE_W);
    OFFS_FI_3_SITE = writeFloat(eeprom_values::OFFS_FI_3_SITE_ADDR, OFFS_FI_3_SITE_W);
    LIM_FI_1_SITE = writeFloat(eeprom_values::LIM_FI_1_SITE_ADDR, LIM_FI_1_SITE_W);
    LIM_FI_2_SITE = writeFloat(eeprom_values::LIM_FI_2_SITE_ADDR, LIM_FI_2_SITE_W);
    // Sensor de flujo Espiratorio
    AMP_FE_1_SITE = writeFloat(eeprom_values::AMP_FE_1_SITE_ADDR, AMP_FE_1_SITE_W);
    AMP_FE_2_SITE = writeFloat(eeprom_values::AMP_FE_2_SITE_ADDR, AMP_FE_2_SITE_W);
    AMP_FE_3_SITE = writeFloat(eeprom_values::AMP_FE_3_SITE_ADDR, AMP_FE_3_SITE_W);
    OFFS_FE_1_SITE = writeFloat(eeprom_values::OFFS_FE_1_SITE_ADDR, OFFS_FE_1_SITE_W);
    OFFS_FE_2_SITE = writeFloat(eeprom_values::OFFS_FE_2_SITE_ADDR, OFFS_FE_2_SITE_W);
    OFFS_FE_3_SITE = writeFloat(eeprom_values::OFFS_FE_3_SITE_ADDR, OFFS_FE_3_SITE_W);
    LIM_FE_1_SITE = writeFloat(eeprom_values::LIM_FE_1_SITE_ADDR, LIM_FE_1_SITE_W);
    LIM_FE_2_SITE = writeFloat(eeprom_values::LIM_FE_2_SITE_ADDR, LIM_FE_2_SITE_W);
    
    charged = "data Loaded, ";
  } else {
    charged = "data Read, ";
  }
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
  VOL_SCALE_SITE = readFloat(eeprom_values::VOL_SCALE_SITE_ADDR); // Factor de escala para ajustar el volumen

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

void setup()
{
  // put your setup code here, to run once:

  init_Memory();
  init_Timer();

  Serial.begin(115200);
  Serial2.begin(115200);

  Serial.println("Inicializacion CONTROL");

  pinMode(EV_OUTPUT_1, OUTPUT); // PIN 3   velocidad
  pinMode(EV_OUTPUT_2, OUTPUT); // PIN 6   velocidad
  pinMode(EV_OUTPUT_3, OUTPUT); // PIN 12  velocidad
  pinMode(EV_OUTPUT_4, OUTPUT); // PIN 3   velocidad
  pinMode(EV_OUTPUT_5, OUTPUT); // PIN 6   velocidad
  //pinMode(EV_OUTPUT_6, OUTPUT); // PIN 2  velocidad
  //pinMode(EV_OUTPUT_7, OUTPUT);  // PIN 3   velocidad
  //pinMode(EV_OUTPUT_8, OUTPUT);  // PIN 6   velocidad

  dataCoeficients = charged + SerialID + ',' + String(AMP_CAM_1, 5) + ',' + String(OFFS_CAM_1, 5) + ',' + String(AMP_BAG_2, 5) +
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

  dataReceive.reserve(40);
  dataSend.reserve(40);
}

void loop()
{
  // put your main code here, to run repeatedly:

  if (flagTimerInterrupt == true)
  {
    flagTimerInterrupt = false;

    counterTimer++;

    if (counterTimer % 100 == 0)
    {

      if (stateMosfet == 10)
      {
        stateMosfet = 0;
      }
      switch (stateMosfet)
      {
        // turn on
      case 0:
        digitalWrite(EV_OUTPUT_1, HIGH); // PIN 1   velocidad
        break;
      case 1:
        digitalWrite(EV_OUTPUT_2, HIGH); // PIN 1   velocidad
        break;
      case 2:
        digitalWrite(EV_OUTPUT_3, HIGH); // PIN 1   velocidad
        break;
      case 3:
        digitalWrite(EV_OUTPUT_4, HIGH); // PIN 1   velocidad
        break;
      case 4:
        digitalWrite(EV_OUTPUT_5, HIGH); // PIN 1   velocidad
        break;
      // turn off
      case 5:
        digitalWrite(EV_OUTPUT_1, LOW); // PIN 1   velocidad
        break;
      case 6:
        digitalWrite(EV_OUTPUT_2, LOW); // PIN 1   velocidad
        break;
      case 7:
        digitalWrite(EV_OUTPUT_3, LOW); // PIN 1   velocidad
        break;
      case 8:
        digitalWrite(EV_OUTPUT_4, LOW); // PIN 1   velocidad
        break;
      case 9:
        digitalWrite(EV_OUTPUT_5, LOW); // PIN 1   velocidad
        break;
      // prueba
      // case 10:
      //   digitalWrite(EV_OUTPUT_6, HIGH); // PIN 1   velocidad
      //   break;
      // case 11:
      //   digitalWrite(EV_OUTPUT_6, LOW); // PIN 1   velocidad
      //   break;

      default:
        break;
      }

      stateMosfet++;
    }

    if (counterTimer % 50 == 0)
    {

      dataSend = "Hola, soy CONTROL," + String(analogRead(ADC_PRESS_1)) +
                 ',' + String(analogRead(ADC_PRESS_2)) +
                 ',' + String(analogRead(ADC_PRESS_3)) +
                 ',' + String(analogRead(ADC_PRESS_4)) +
                 ',' + String(analogRead(ADC_PRESS_5)) +
                 ',' + String(analogRead(ADC_PRESS_6));

      Serial.println(dataSend);
    }

    if (counterTimer == 1000)
    {
      counterTimer = 0;
      Serial.println(dataCoeficients);
    }
  }

  if (Serial2.available() > 12)
  {
    dataReceive = Serial2.readStringUntil(';');
    if (dataReceive[0] == 'C' && dataReceive[1] == 'O')
    {
      Serial.println("dato recibido");
      Serial2.print("OK;");
    }
  }
}

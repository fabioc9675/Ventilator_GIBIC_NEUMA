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
    SERIAL_ADDR = 10,
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

// bandera de activacion de timer
uint8_t flagTimerInterrupt = false;

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
    EEPROM.begin(512);
}


// funcion para escribir Strings en la memoria EEPROM
void writeString(char add, String data)
{
    int _size = data.length();
    for (int i = 0; i < _size; i++)
    {
        EEPROM.write(add + i, data[i]);
    }
    EEPROM.write(add + _size, '\0'); // add termination NULL
    EEPROM.commit();
}

// funcion para leer Strings en la memoria EEPROM
String readString(char add)
{
    int i = 0;
    char data[100];
    int len = 0;
    unsigned char k;
    k = EEPROM.read(add);
    while (k != '\0' && len < 500)
    {
        k = EEPROM.read(add + len);
        data[len] = k;
        len++;
    }
    data[len] = '\0';
    return String(data);
}

void init_TIMER()
{
    // Configuracion del timer a 1 kHz
    timer = timerBegin(0, 80, true);             // Frecuencia de reloj 80 MHz, prescaler de 80, frec 1 MHz
    timerAttachInterrupt(timer, &onTimer, true); // Attach onTimer function to our timer
    timerAlarmWrite(timer, 1000, true);          // Interrupcion cada 1000 conteos del timer, es decir 100 Hz
    timerAlarmEnable(timer);                     // Habilita interrupcion por timer
}

/* *********************************************************************
 * **** FUNCIONES DE ATENCION A INTERRUPCION ***************************
 * *********************************************************************/
// Interrupcion por timer
void IRAM_ATTR onTimer(void)
{
    portENTER_CRITICAL_ISR(&timerMux);
    flagTimerInterrupt = true;                    // asignacion de banderas para atencion de interrupcion
    xSemaphoreGiveFromISR(xSemaphoreTimer, NULL); // asignacion y liberacion de semaforos
    portEXIT_CRITICAL_ISR(&timerMux);
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
                    Serial.println("Segundo");
                }
            }
        }
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

    // se crea el semaforo binario
	xSemaphoreTimer = xSemaphoreCreateBinary();

    Serial.begin(115200);

    String receiveData;

    Serial.print("Data to write = ");
    Serial.println(SERIAL_DEVICE);
    // writeString(10, SERIAL_DEVICE);
    delay(500);
    receiveData = readString(10);
    Serial.print("Data read = ");
    Serial.println(receiveData);

    
    // nvs_flash_init();

    

    // creo la tarea task_pulsador
    xTaskCreatePinnedToCore(task_Timer, "task_Timer", 2048, NULL, 4, NULL, taskCoreOne);
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

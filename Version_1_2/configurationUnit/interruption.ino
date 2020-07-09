/*
 * File:   interruption.ino
 * Author: GIBIC UdeA
 *
 * Created on July 4, 2020, 13:41 PM
 */

/** ****************************************************************************
 ** ************ INCLUDES ******************************************************
 ** ****************************************************************************/
#include "interruption.h"

/** ****************************************************************************
 ** ************ DEFINES *******************************************************
 ** ****************************************************************************/

/** ****************************************************************************
 ** ************ EXTERN VARIABLES **********************************************
 ** ****************************************************************************/
// **********************************************************
extern portMUX_TYPE mux;

// manejadores para los semaforos binarios
extern SemaphoreHandle_t xSemaphoreEncoder;

// bandera de activacion de encoder
extern volatile uint8_t flagAEncoder;
extern volatile uint8_t flagBEncoder;
extern volatile uint8_t flagSEncoder;

// banderas de botones de usuario
extern volatile uint8_t flagStandbyInterrupt;
extern volatile uint8_t flagSilenceInterrupt;
extern volatile uint8_t flagStabilityInterrupt;
extern volatile uint8_t flagBatterySilence;
extern volatile uint8_t flagAlerta;
extern volatile uint8_t flagBatteryAlert;

/** ****************************************************************************
 ** ************ VARIABLES *****************************************************
 ** ****************************************************************************/
// Variable de estado del encoder
unsigned int fl_StateEncoder = 0;

/** ****************************************************************************
 ** ************ FUNCTIONS *****************************************************
 ** ****************************************************************************/
// Interrupcion por presion del switch
void IRAM_ATTR swInterrupt(void)
{
    // da el semaforo para que quede libre para la tarea pulsador
    portENTER_CRITICAL_ISR(&mux);
    flagSEncoder = true;
    xSemaphoreGiveFromISR(xSemaphoreEncoder, NULL);
    portEXIT_CRITICAL_ISR(&mux);
}

// Interrupcion por encoder A
void IRAM_ATTR encoderInterrupt_A(void)
{
    portENTER_CRITICAL_ISR(&mux);
    flagAEncoder = true;
    xSemaphoreGiveFromISR(xSemaphoreEncoder, NULL);
    portEXIT_CRITICAL_ISR(&mux);
}

// Interrupcion por encoder B
void IRAM_ATTR encoderInterrupt_B(void)
{
    portENTER_CRITICAL_ISR(&mux);
    flagBEncoder = true;
    xSemaphoreGiveFromISR(xSemaphoreEncoder, NULL);
    portEXIT_CRITICAL_ISR(&mux);
}

// Interrupcion por boton Standby
void IRAM_ATTR standbyButtonInterrupt(void)
{
    portENTER_CRITICAL_ISR(&mux);
    detachInterrupt(digitalPinToInterrupt(STANDBY));
    flagStandbyInterrupt = true;
    portEXIT_CRITICAL_ISR(&mux);
}

// Interrupcion por button silence
void IRAM_ATTR silenceButtonInterrupt(void)
{
    if (flagBatteryAlert == true && flagBatterySilence == false)
    {
        portENTER_CRITICAL_ISR(&mux);
        flagBatterySilence = true;
        portEXIT_CRITICAL_ISR(&mux);
    }
    if (flagAlerta == true && flagSilenceInterrupt == false)
    {
        portENTER_CRITICAL_ISR(&mux);
        flagSilenceInterrupt = true;
        portEXIT_CRITICAL_ISR(&mux);
    }
}

// Interrupcion por boton estabilidad
void IRAM_ATTR stabilityButtonInterrupt(void)
{
    portENTER_CRITICAL_ISR(&mux);
    flagStabilityInterrupt = true;
    detachInterrupt(digitalPinToInterrupt(STABILITY_BTN));
    portEXIT_CRITICAL_ISR(&mux);
}

/** ****************************************************************************
 ** ************ END OF THE CODE ***********************************************
 ** ****************************************************************************/

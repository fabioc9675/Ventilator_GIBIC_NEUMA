/*
 * File:   encoder.ino
 * Author: GIBIC UdeA
 *
 * Created on July 4, 2020, 13:41 PM
 */

/** ****************************************************************************
 ** ************ INCLUDES ******************************************************
 ** ****************************************************************************/
#include "encoder.h"

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

// bandera de activacion de timer
extern volatile uint8_t flagAEncoder;
extern volatile uint8_t flagBEncoder;
extern volatile uint8_t flagSEncoder;

/** ****************************************************************************
 ** ************ VARIABLES *****************************************************
 ** ****************************************************************************/

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

/** ****************************************************************************
 ** ************ END OF THE CODE ***********************************************
 ** ****************************************************************************/

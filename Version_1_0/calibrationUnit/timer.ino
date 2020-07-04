/*
 * File:   timer.ino
 * Author: GIBIC UdeA
 *
 * Created on July 4, 2020, 13:41 PM
 */

/** ****************************************************************************
 ** ************ INCLUDES ******************************************************
 ** ****************************************************************************/
#include "timer.h"

/** ****************************************************************************
 ** ************ DEFINES *******************************************************
 ** ****************************************************************************/

/** ****************************************************************************
 ** ************ EXTERN VARIABLES **********************************************
 ** ****************************************************************************/
// **********************************************************

/** ****************************************************************************
 ** ************ VARIABLES *****************************************************
 ** ****************************************************************************/

// inicializacion del contador del timer
hw_timer_t *timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

/** ****************************************************************************
 ** ************ FUNCTIONS *****************************************************
 ** ****************************************************************************/

// Inicializacion del timer
void init_TIMER(void)
{
    // Configuracion del timer a 1 kHz
    timer = timerBegin(0, 80, true);             // Frecuencia de reloj 80 MHz, prescaler de 80, frec 1 MHz
    timerAttachInterrupt(timer, &onTimer, true); // Attach onTimer function to our timer
    timerAlarmWrite(timer, 1000, true);          // Interrupcion cada 1000 conteos del timer, es decir 100 Hz
    timerAlarmEnable(timer);                     // Habilita interrupcion por timer
}

// Interrupcion por timer
void IRAM_ATTR onTimer(void)
{
    portENTER_CRITICAL_ISR(&timerMux);
    flagTimerInterrupt = true;                    // asignacion de banderas para atencion de interrupcion
    xSemaphoreGiveFromISR(xSemaphoreTimer, NULL); // asignacion y liberacion de semaforos
    portEXIT_CRITICAL_ISR(&timerMux);
}

/** ****************************************************************************
 ** ************ END OF THE CODE ***********************************************
 ** ****************************************************************************/

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
extern volatile uint8_t flagTimerInterrupt;

extern SemaphoreHandle_t xSemaphoreTimer;

// variables de estado de ventilacion
extern byte stateMachine;
extern byte newVentilationMode;

// variable de estado de menu
extern volatile uint8_t insideMenuFlag;

// bandera de activacion de encoder
extern volatile uint8_t flagAEncoder;
extern volatile uint8_t flagBEncoder;
extern volatile uint8_t flagSEncoder;

extern volatile uint8_t flagDetachInterrupt_A;
extern volatile uint8_t flagDetachInterrupt_B;
extern volatile uint8_t flagDetachInterrupt_A_B;
extern volatile uint8_t flagDetachInterrupt_B_A;
extern volatile uint8_t flagDetachInterrupt_S;

extern unsigned int contDetachA;
extern unsigned int contDetachB;
extern unsigned int contDetachS;

// banderas de botones de usuario
extern volatile uint8_t flagStabilityInterrupt;

/** ****************************************************************************
 ** ************ VARIABLES *****************************************************
 ** ****************************************************************************/

// inicializacion del contador del timer
hw_timer_t *timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

// contadores de tiempo
int second = 0;
int milisecond = 0;
unsigned int contStability = 0;

/** ****************************************************************************
 ** ************ FUNCTIONS *****************************************************
 ** ****************************************************************************/

// Inicializacion del timer
void init_Timer(void)
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

/************************************************************
 ***** FUNCIONES DE ATENCION A INTERRUPCION TAREA TIMER *****
 ************************************************************/
void task_timer(void *arg)
{
    int contms = 0;
    uint16_t debounceENC = 0;
    uint16_t debounceENC_2 = 0;

    while (1)
    {
        // Se atiende la interrupcion del timer
        if (xSemaphoreTake(xSemaphoreTimer, portMAX_DELAY) == pdTRUE)
        {
            if (flagTimerInterrupt == true)
            {
                portENTER_CRITICAL(&timerMux);
                flagTimerInterrupt = false;
                portEXIT_CRITICAL(&timerMux);

                switch (stateMachine)
                {
                case CHECK_STATE:
                    //alertMonitoring();
                    //checkRoutine();
                    //hardwareInterruptAttention();
                    break;
                case STANDBY_STATE:
                    //alertMonitoring();
                    standbyInterruptAttention();
                    break;
                case PCMV_STATE:
                    //alertMonitoring();
                    standbyInterruptAttention();
                    break;
                case AC_STATE:
                    standbyInterruptAttention();
                    break;
                case CPAP_STATE:
                    standbyInterruptAttention();
                    break;
                case FAILURE_STATE:
                    break;
                default:
                    break;
                }
                contms++;
                if (contms % 25 == 0)
                {
                    // digitalWrite(LED, !digitalRead(LED));
                }
                if (contms == 1000)
                {
                    contms = 0;
                    // digitalWrite(LED, !digitalRead(LED));
                    // Serial.print("LED_SEGUNDO ");
                    // Serial.print("Core ");
                    // Serial.println(xPortGetCoreID());
                }
                /****************************************************************************
				****  En esta seccion del codigo se agregan de nuevo las interrupciones  ****
				****************************************************************************/
                if (insideMenuFlag)
                { // si esta configurando un parametro
                    debounceENC = DEBOUNCE_ENC;
                    debounceENC_2 = DEBOUNCE_ENC_2;
                }
                else
                {
                    debounceENC = DEBOUNCE_ENC_OUT;
                    debounceENC_2 = DEBOUNCE_ENC_OUT_2;
                }

                // Agregar interrupcion A
                if ((flagDetachInterrupt_A == true) || (flagDetachInterrupt_B_A == true))
                {
                    contDetachA++;
                    if ((contDetachA >= debounceENC) && (flagDetachInterrupt_A == true))
                    {
                        contDetachA = 0;
                        flagDetachInterrupt_A = false;
                        attachInterrupt(digitalPinToInterrupt(A), encoderInterrupt_A, FALLING);
                    }
                    if ((contDetachA >= debounceENC_2) && (flagDetachInterrupt_B_A == true))
                    {
                        contDetachB = 0;
                        flagDetachInterrupt_B_A = false;
                        attachInterrupt(digitalPinToInterrupt(B), encoderInterrupt_B, FALLING);
                    }
                }
                // Agregar interrupcion B
                if ((flagDetachInterrupt_B == true) || (flagDetachInterrupt_A_B == true))
                {
                    contDetachB++;
                    if ((contDetachB >= debounceENC) && (flagDetachInterrupt_B == true))
                    {
                        contDetachB = 0;
                        flagDetachInterrupt_B = false;
                        attachInterrupt(digitalPinToInterrupt(B), encoderInterrupt_B, FALLING);
                    }
                    if ((contDetachB >= debounceENC_2) && (flagDetachInterrupt_A_B == true))
                    {
                        contDetachB = 0;
                        flagDetachInterrupt_A_B = false;
                        attachInterrupt(digitalPinToInterrupt(A), encoderInterrupt_A, FALLING);
                    }
                }
                // Agregar interrupcion S
                if (flagDetachInterrupt_S == true)
                {
                    contDetachS++;
                    if (contDetachS >= DEBOUNCE_ENC_SW)
                    {
                        contDetachS = 0;
                        flagDetachInterrupt_S = false;
                        attachInterrupt(digitalPinToInterrupt(SW), swInterrupt, FALLING);
                    }
                } // Finaliza agregar interrupciones

                if (flagStabilityInterrupt == true)
                {
                    contStability++;
                    if (contStability > 400)
                    {
                        //Serial.println("Estabilidad");
                        contStability = 0;
                        sendSerialData();
                        portENTER_CRITICAL(&timerMux);
                        flagStabilityInterrupt = false;
                        attachInterrupt(digitalPinToInterrupt(STABILITY_BTN), stabilityButtonInterrupt, FALLING);
                        portEXIT_CRITICAL(&timerMux);
                        digitalWrite(STABILITY_LED, HIGH);
                        digitalWrite(LUMING, HIGH);
                    }
                }
                alarmMonitoring();
            }
        }
    }
}

/** ****************************************************************************
 ** ************ END OF THE CODE ***********************************************
 ** ****************************************************************************/

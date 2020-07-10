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

// variables de introduccion a los menus de configuracion
extern byte currentVentilationMode;

// variables de estado de ventilacion
extern byte stateMachine;

// variables contadores de conido de silencio y alarmas
extern unsigned int contSilence;
extern unsigned int contSilenceBattery;
extern unsigned int contStandby;

// Variables para menu lineaAnterior
extern volatile unsigned int lineaAnterior; // valor de menu anterior

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

// Atencion a interrupcion por boton de silencio
void silenceInterruptAttention(void)
{
    if (flagSilenceInterrupt == true)
    {
        //digitalWrite(BUZZER_BTN, LOW);
        contSilence++;
        if (contSilence > SILENCE_BTN_TIME)
        {
            portENTER_CRITICAL(&mux);
            flagSilenceInterrupt = false;
            portEXIT_CRITICAL(&mux);
            contSilence = 0;
        }
    }
    if (flagBatterySilence == true)
    {
        //digitalWrite(BUZZER_BTN, LOW);
        contSilenceBattery++;
        if (contSilenceBattery > SILENCE_BTN_BATTERY)
        {
            portENTER_CRITICAL(&mux);
            flagBatterySilence = false;
            portEXIT_CRITICAL(&mux);
            contSilenceBattery = 0;
        }
    }
}

// Atencion a interrupcion por boton de standby
void standbyInterruptAttention(void)
{
    if (flagStandbyInterrupt)
    {
        contStandby++;
        if (stateMachine == STANDBY_STATE && contStandby > 500 && digitalRead(STANDBY) == 0)
        {
            portENTER_CRITICAL(&mux);
            attachInterrupt(digitalPinToInterrupt(STANDBY), standbyButtonInterrupt, FALLING);
            flagStandbyInterrupt = false;
            portEXIT_CRITICAL(&mux);
            contStandby = 0;

            switch (currentVentilationMode)
            {
            case 0:
                stateMachine = PCMV_STATE;
                portENTER_CRITICAL_ISR(&mux);
                attachInterrupt(digitalPinToInterrupt(STABILITY_BTN), stabilityButtonInterrupt, FALLING);
                portEXIT_CRITICAL_ISR(&mux);
                break;
            case 1:
                stateMachine = AC_STATE;
                portENTER_CRITICAL_ISR(&mux);
                attachInterrupt(digitalPinToInterrupt(STABILITY_BTN), stabilityButtonInterrupt, FALLING);
                portEXIT_CRITICAL_ISR(&mux);
                break;
            case 2:
                stateMachine = CPAP_STATE;
                digitalWrite(STABILITY_LED, LOW);
                digitalWrite(LUMING, LOW);
                portENTER_CRITICAL_ISR(&mux);
                detachInterrupt(digitalPinToInterrupt(STABILITY_BTN));
                portEXIT_CRITICAL_ISR(&mux);
                break;
            default:
                stateMachine = STANDBY_STATE;
                digitalWrite(STANDBY_LED, HIGH);
                digitalWrite(STABILITY_LED, LOW);
                digitalWrite(LUMING, LOW);
                portENTER_CRITICAL_ISR(&mux);
                detachInterrupt(digitalPinToInterrupt(STABILITY_BTN));
                portEXIT_CRITICAL_ISR(&mux);
                break;
            }
            //Serial.println("I am on Cycling state");
            digitalWrite(STANDBY_LED, LOW);
            lineaAnterior = MODE_CHANGE;
            sendSerialData();
        }
        else if (stateMachine != STANDBY_STATE)
        {
            // Serial.println(digitalRead(STANDBY));
            if (contStandby < 3000 && digitalRead(STANDBY) == 1)
            {
                contStandby = 0;
                portENTER_CRITICAL_ISR(&mux);
                attachInterrupt(digitalPinToInterrupt(STANDBY), standbyButtonInterrupt, FALLING);
                flagStandbyInterrupt = false;
                portEXIT_CRITICAL_ISR(&mux);
            }
            else if (contStandby > 3000)
            {
                contStandby = 0;
                stateMachine = STANDBY_STATE;
                digitalWrite(STANDBY_LED, HIGH);
                digitalWrite(STABILITY_LED, LOW);
                digitalWrite(LUMING, LOW);
                portENTER_CRITICAL_ISR(&mux);
                attachInterrupt(digitalPinToInterrupt(STANDBY), standbyButtonInterrupt, FALLING);
                flagStandbyInterrupt = false;
                detachInterrupt(digitalPinToInterrupt(STABILITY_BTN));
                portEXIT_CRITICAL_ISR(&mux);
                lineaAnterior = MODE_CHANGE;
                sendSerialData();
                //Serial.println("I am on Standby state");
            }
        }
    }
}

/** ****************************************************************************
 ** ************ END OF THE CODE ***********************************************
 ** ****************************************************************************/

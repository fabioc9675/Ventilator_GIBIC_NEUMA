/*
 * File:   initializer.h
 * Author: GIBIC UdeA
 *
 * Created on July 4, 2020, 13:41 PM
 */

#ifndef INITIALIZER_H
#define INITIALIZER_H

#ifdef __cplusplus
extern "C"
{
#endif

// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>

/** ****************************************************************************
 ** ************ INCLUDES ******************************************************
 ** ****************************************************************************/
#include <Arduino.h>
#include <Esp.h>
#include <nvs_flash.h>

/** ****************************************************************************
 ** ************ DEFINES *******************************************************
 ** ****************************************************************************/
//********DEFINICIONES DE HARDWARE******
//********DEFINICION DE VERSION*********
#define VERSION_1_2 TRUE

//********COMPILACION CONDICIONAL*******
#ifdef VERSION_1_0

//********DEFINICION DE PINES***********
#define A 18 //variable A a pin digital 2 (DT en modulo)
#define B 19 //variable B a pin digital 4 (CLK en modulo)
#define SW 5 //sw a pin digital 3 (SW en modulo)

#define BUZZER_BTN 12
#define SILENCE_BTN 26 // Silenciar alarma cambiar
#define SILENCE_LED 27 // Led Boton silencio
#define STABILITY_BTN 25
#define STABILITY_LED 34
#define STANDBY 32     // Stabdby button
#define STANDBY_LED 33 // Stabdby button

#define ENCOD_INCREASE 1 // movimiento a la derecha, aumento
#define ENCOD_DECREASE 2 // movimiento a la derecha, aumento
#define ENCOD_COUNT 3    // cantidad de interrupciones antes de reconocer el conteo

#endif
#ifdef VERSION_1_1

//********DEFINICION DE PINES***********
#define A 18 //variable A a pin digital 2 (DT en modulo)
#define B 19 //variable B a pin digital 4 (CLK en modulo)
#define SW 5 //sw a pin digital 3 (SW en modulo)

#define BUZZER_BTN 12
#define SILENCE_BTN 26 // Silenciar alarma cambiar
#define SILENCE_LED 27 // Led Boton silencio
#define STABILITY_BTN 34
#define STABILITY_LED 25
#define STANDBY 32     // Stabdby button
#define STANDBY_LED 33 // Stabdby button

#define ENCOD_INCREASE 1 // movimiento a la derecha, aumento
#define ENCOD_DECREASE 2 // movimiento a la derecha, aumento
#define ENCOD_COUNT 3    // cantidad de interrupciones antes de reconocer el conteo

#endif
#ifdef VERSION_1_2

//********DEFINICION DE PINES***********
#define A 19 //variable A a pin digital 2 (DT en modulo)
#define B 18 //variable B a pin digital 4 (CLK en modulo)
#define SW 5 //sw a pin digital 3 (SW en modulo)

#define BUZZER_BTN 12
#define SILENCE_BTN 26 // Silenciar alarma cambiar
#define SILENCE_LED 27 // Led Boton silencio
#define STABILITY_BTN 34
#define STABILITY_LED 25
#define STANDBY 32     // Stabdby button
#define STANDBY_LED 33 // Stabdby button

#define ENCOD_INCREASE 1 // movimiento a la derecha, aumento
#define ENCOD_DECREASE 2 // movimiento a la derecha, aumento
#define ENCOD_COUNT 1    // cantidad de interrupciones antes de reconocer el conteo

#endif

#define LUMING 13 // Alarma luminosa
#define LUMINR 14
#define LUMINB 15
#define BATTALARM 4

#define LED 2

#define ESP_INTR_FLAG_DEFAULT 0

#define DEBOUNCE_ENC 50        // tiempo para realizar antirrebote
#define DEBOUNCE_ENC_2 400     // tiempo para realizar antirrebote
#define DEBOUNCE_ENC_OUT 300   // tiempo para realizar antirrebote
#define DEBOUNCE_ENC_OUT_2 800 // tiempo para realizar antirrebote
#define DEBOUNCE_ENC_SW 400    // tiempo para realizar antirrebote
#define LOW_ATT_INT 50         // Interrupcion cada 10 ms

// Variables de control del protocolo serial 2
#define RXD2 16
#define TXD2 17

    /** ****************************************************************************
 ** ************ VARIABLES *****************************************************
 ** ****************************************************************************/

    /** ****************************************************************************
 ** ************ FUNCTIONS *****************************************************
 ** ****************************************************************************/
    void init_GPIO(void); // inicializacion de los pines del microcontrolador

    /* *****************************************************************************
 * *****************************************************************************
 * ******************** USO DE MODULO ADS **************************************
 * *****************************************************************************
 * *****************************************************************************/

    /* *****************************************************************************
 * *****************************************************************************
 * ***************** PROTOTYPE DEFINITION **************************************
 * *****************************************************************************
 * *****************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* INITIALIZER_H */

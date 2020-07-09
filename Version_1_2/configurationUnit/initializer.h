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

// Valores maximos definidos para el firmware de configuracion
#define MENU_QUANTITY 4
#define MAX_FREC 30
#define MIN_FREC 6
#define MAX_PEEP 15
#define MIN_PEEP 1
#define MAX_RIE 40
#define MIN_RIE 10 // se cambio de 20 a 10 para evitar relaciones negativas
#define MAX_PRESION 40
#define MAX_MAX_FR 60
#define MIN_MAX_FR 5
#define MAX_MAX_VE 50
#define MIN_MAX_VE 1 // l/min
#define MIN_PRESION 10
#define MAX_TRIGGER 10
#define MIN_TRIGGER 1
#define MAX_FLUJO 40
#define SILENCE_BTN_TIME 2 * 60 * 1000 / LOW_ATT_INT // tiempo, 2 minutos a 20 Hz
#define SILENCE_BTN_BATTERY 30 * 60 * 1000 / LOW_ATT_INT
#define ALARM_QUANTITY 9

// Estados de alarmas por bateria
#define BATTERY_NO_ALARM 0
#define batteryAlarm 1
#define batteryAlarm10min 2
#define batteryAlarm5min 3

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

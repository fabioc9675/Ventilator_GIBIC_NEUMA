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

// Definiciones para controlar el shiel DFRobot quad motor driver
// Definiciones para controlar el shiel DFRobot quad motor driver
#define EV_INSPIRA 13 // out 7 // Valvula 3/2 de control de la via inspiratoria (pin 3 del shield, velocidad motor 1)
#define EV_ESC_CAM 14 // out 6 // Valvula 3/2 de activacion de la camara (pin 6 del shield, velocidad motor 4)
#define EV_ESPIRA 12  // out 8 // Valvula 3/2 de control de presiones PCON y PEEP (pin 11 del shield, velocidad motor 2)

// Definiciones para el manejo del ADC
#define ADC_PRESS_1 26 // ADC 7 // Sensor de presion xx (pin ADC para presion 1)
#define ADC_PRESS_2 35 // ADC 4 // Sensor de presion xx (pin ADC para presion 2)
#define ADC_PRESS_3 33 // ADC 6 // Sensor de presion via aerea del paciente (pin ADC para presion 3)
#define ADC_FLOW_1 39  // ADC 1 // Sensor de flujo linea xx (pin ADC para presion 2)
#define ADC_FLOW_2 36  // ADC 2 // Sensor de flujo linea xx (pin ADC para presion 3)

#endif
#ifdef VERSION_1_1

// Definiciones para controlar el shiel DFRobot quad motor driver
// Definiciones para controlar el shiel DFRobot quad motor driver
#define EV_INSPIRA 5  // out 3 // Valvula 3/2 de control de la via inspiratoria (pin 3 del shield, velocidad motor 1)
#define EV_ESPIRA 4   // out 2 // Valvula 3/2 de control de presiones PCON y PEEP (pin 11 del shield, velocidad motor 2)
#define EV_ESC_CAM 18 // out 1 // Valvula 3/2 de activaci�n de la camara (pin 6 del shield, velocidad motor 4)

// Definiciones para el manejo del ADC
#define ADC_PRESS_1 36 // ADC 6 // Sensor de presion xx (pin ADC para presion 1)
#define ADC_PRESS_2 39 // ADC 5 // Sensor de presion xx (pin ADC para presion 2)
#define ADC_PRESS_3 34 // ADC 4 // Sensor de presion via aerea del paciente (pin ADC para presion 3)
#define ADC_FLOW_1 32  // ADC 1 // Sensor de flujo linea xx (pin ADC para presion 2)
#define ADC_FLOW_2 33  // ADC 2 // Sensor de flujo linea xx (pin ADC para presion 3)  27, 35, 32

#endif
#ifdef VERSION_1_2
// Definiciones para controlar el shiel DFRobot quad motor driver
// Definiciones para controlar el shiel DFRobot quad motor driver
#define EV_ESC_CAM 18 // out 1 // Valvula 3/2 de activacion de la camara
#define EV_INSPIRA 5  // out 2 // Valvula 3/2 de control de la via inspiratoria
#define EV_ESPIRA 4   // out 3 // Valvula 3/2 de control de presiones PCON y PEEP

// Definiciones para el manejo del ADC
#define ADC_PRESS_1 27 // Sensor de presion para la camara
#define ADC_PRESS_2 39 // Sensor de presion distal de paciente (bolsa)
#define ADC_PRESS_3 35 // Sensor de presion proximal de paciente
#define ADC_FLOW_1 36  // Sensor de flujo linea inspiratoria
#define ADC_FLOW_2 34  // Sensor de flujo linea espiratoria

#endif

// Variables de control del protocolo serial 2
#define RXD2 16
#define TXD2 17

// variable para ajustar el nivel cero de flujo y calcular el volumen
#define FLOWUP_LIM 3
#define FLOWLO_LIM -3
#define FLOW_CONV 16.666666 // conversion de L/min a mL/second
#define DELTA_T 0.003       // 0.05         // delta de tiempo para realizar la integra

#define ADC_FAST 3  // muestreo cada 3 ms
#define ADC_SLOW 50 // muestreo cada 50 ms

   /** ****************************************************************************
 ** ************ VARIABLES *****************************************************
 ** ****************************************************************************/

   /** ****************************************************************************
 ** ************ FUNCTIONS *****************************************************
 ** ****************************************************************************/

   void init_GPIO(void);        // inicializacion de los pines del microcontrolador
   void init_TextPayload(void); // funcion de inicializacion de variables de texto

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

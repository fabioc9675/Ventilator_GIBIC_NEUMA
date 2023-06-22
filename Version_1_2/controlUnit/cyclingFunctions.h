/*
 * File:   cyclingFunctions.h
 * Author: GIBIC UdeA
 *
 * Created on July 4, 2020, 13:41 PM
 */

#ifndef CYCLINGFUNCTIONS_H
#define CYCLINGFUNCTIONS_H

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

#include "initializer.h"

/** ****************************************************************************
 ** ************ DEFINES *******************************************************
 ** ****************************************************************************/
// Definiciones para State machine
#define CHECK_STATE 0
#define STANDBY_STATE 1
#define PCMV_STATE 2
#define AC_STATE 3
#define CPAP_STATE 4
#define FAILURE_STATE 5
#define SERVICE_STATE 6

// Definiciones para ciclado en mode C-PMV
#define STOP_CYCLING 0
#define START_CYCLING 1
#define INSPIRATION_CYCLING 2
#define EXPIRATION_CYCLING 3

// Definiciones para ciclado en mode CPAP
#define COMP_FLOW_MAX_CPAP 2.5   // cambiado desde 3, variable para comparacion de flujo y entrar en modo Inspiratorio en CPAP
#define COMP_FLOW_MIN_CPAP -2.5  // cambiado desde 3, variable para comparacion de flujo y entrar en modo Inspiratorio en CPAP
#define COMP_DEL_F_MAX_CPAP 1.5  // cambiado desde 2, variable para comparacion de flujo y entrar en modo Inspiratorio en CPAP
#define COMP_DEL_F_MIN_CPAP -1.5 // cambiado desde 2, variable para comparacion de flujo y entrar en modo Inspiratorio en CPAP
#define CPAP_NONE 0              // Estado de inicializacion
#define CPAP_INIT 1              // Estado de inicio de CPAP
#define CPAP_INSPIRATION 2       // Entra en modo inspiratorio
#define CPAP_ESPIRATION 3        // Entra en modo espiratorio

// Definiciones para la determinacion de la obstruccion y las presiones de Back Up
#define OFFSET_PRESION 10 // se define un offset entre las ppico distal y proximal
#define REFERENCE_RATIO 40 // valor de referencia para entrar en modo backup

   /** ****************************************************************************
 ** ************ VARIABLES *****************************************************
 ** ****************************************************************************/

   /** ****************************************************************************
 ** ************ FUNCTIONS *****************************************************
 ** ****************************************************************************/
   void cycling(void);         // Cycling of the Mechanical Ventilator
   void alarmsDetection(void); // Funcion para la deteccion de alarmas del sistema
   void cpapRoutine(void);     // CPAP of the Mechanical Ventilator
   void standbyRoutine(void);  // Rutina de StandBy

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

#endif /* CYCLINGFUNCTIONS_H */
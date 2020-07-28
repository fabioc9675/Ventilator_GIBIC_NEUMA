/*
 * File:   encoder.h
 * Author: GIBIC UdeA
 *
 * Created on July 4, 2020, 13:41 PM
 */

#ifndef ENCODER_H
#define ENCODER_H

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
#include "interruption.h"
#include "serialCONT.h"

  /** ****************************************************************************
 ** ************ DEFINES *******************************************************
 ** ****************************************************************************/
// Definitions for menu operation
#define MAIN_MENU 0    // Menu principal
#define CONFIG_MENU 1  // Configuracion de frecuencias
#define CONFIG_ALARM 2 // Configuracion Alarma
#define VENT_MENU 3    // Ventilation menu selection
#define SERVICE_MENU 4  // modo servicio, prueba de figas

#define ALE_PRES_DES 5    // desconexion del paciente
#define ALE_OBSTRUCCION 6 // fallo OBSTRUCCION
#define BATTERY 7         // Bateria
#define CHECK_MENU 8      // Show in check state
#define CONFIRM_MENU 9
#define CPAP_MENU 10
#define ALE_PRES_PEEP 11 // Perdida de Peep
#define ALE_BATTERY_10MIN 12
#define ALE_BATTERY_5MIN 13
#define ALE_GENERAL 14
#define ALE_FR_ALTA 15
#define ALE_VE_ALTO 16
#define ALE_PRES_PIP 17 // presion pico
#define ALE_APNEA 18
#define MODE_CHANGE 19 // definicion para obligar al cambio entre StandBy y modo normal en el LCD

// State Machine to define the state of ventilator operation
#define CHECK_STATE 0
#define STANDBY_STATE 1
#define PCMV_STATE 2
#define AC_STATE 3
#define CPAP_STATE 4
#define FAILURE_STATE 5
#define SERVICE_STATE 6

  /** ****************************************************************************
 ** ************ VARIABLES *****************************************************
 ** ****************************************************************************/

  /** ****************************************************************************
 ** ************ FUNCTIONS *****************************************************
 ** ****************************************************************************/
  void encoderRoutine(void); // Ejecucion de incremento o decremento del encoder
  void switchRoutine(void);  // Ejecucion de tarea de Switch

  /****************************************************************************
 ***** Atencion a interrupcion por encoder **********************************
 ****************************************************************************/
  void task_Encoder(void *arg);

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

#endif /* ENCODER_H */

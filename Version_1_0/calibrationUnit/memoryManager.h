/*
 * File:   memoryManager.h
 * Author: GIBIC UdeA
 *
 * Created on July 4, 2020, 13:41 PM
 */

#ifndef MEMORYMANAGER_H
#define MEMORYMANAGER_H

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
#include "calibrationMenu.h"


/** ****************************************************************************
 ** ************ DEFINES *******************************************************
 ** ****************************************************************************/
#define SERIAL_LENGTH 10

   /** ****************************************************************************
 ** ************ VARIABLES *****************************************************
 ** ****************************************************************************/
   //********MEMORY MNEMONICS**************
   enum eeprom_values
   {
      // Serial del DISPOSITIVO
      SERIAL_ADDR = 8,
      // Valores de fabrica
      // Flujo Inspiratorio
      AMP_FI_1_ADDR = 20,
      OFFS_FI_1_ADDR = 24,
      LIM_FI_1_ADDR = 28,
      AMP_FI_2_ADDR = 32,
      OFFS_FI_2_ADDR = 36,
      LIM_FI_2_ADDR = 40,
      AMP_FI_3_ADDR = 44,
      OFFS_FI_3_ADDR = 48,
      // Flujo expiratorio
      AMP_FE_1_ADDR = 52,
      OFFS_FE_1_ADDR = 56,
      LIM_FE_1_ADDR = 60,
      AMP_FE_2_ADDR = 64,
      OFFS_FE_2_ADDR = 68,
      LIM_FE_2_ADDR = 72,
      AMP_FE_3_ADDR = 76,
      OFFS_FE_3_ADDR = 80,
      // Presion Camara
      AMP_CAM_1_ADDR = 84,
      OFFS_CAM_1_ADDR = 88,
      // Presion Bolsa
      AMP_BAG_2_ADDR = 92,
      OFFS_BAG_2_ADDR = 96,
      // Presion Paciente
      AMP_PAC_3_ADDR = 100,
      OFFS_PAC_3_ADDR = 104,

      // Valores de sitio
      // Flujo Inspiratorio
      AMP_FI_1_SITE_ADDR = 110,
      OFFS_FI_1_SITE_ADDR = 114,
      LIM_FI_1_SITE_ADDR = 118,
      AMP_FI_2_SITE_ADDR = 122,
      OFFS_FI_2_SITE_ADDR = 126,
      LIM_FI_2_SITE_ADDR = 130,
      AMP_FI_3_SITE_ADDR = 134,
      OFFS_FI_3_SITE_ADDR = 138,
      // Flujo expiratorio
      AMP_FE_1_SITE_ADDR = 142,
      OFFS_FE_1_SITE_ADDR = 146,
      LIM_FE_1_SITE_ADDR = 150,
      AMP_FE_2_SITE_ADDR = 154,
      OFFS_FE_2_SITE_ADDR = 158,
      LIM_FE_2_SITE_ADDR = 162,
      AMP_FE_3_SITE_ADDR = 166,
      OFFS_FE_3_SITE_ADDR = 170,
      // Presion Camara
      AMP_CAM_1_SITE_ADDR = 174,
      OFFS_CAM_1_SITE_ADDR = 178,
      // Presion Bolsa
      AMP_BAG_2_SITE_ADDR = 182,
      OFFS_BAG_2_SITE_ADDR = 186,
      // Presion Paciente
      AMP_PAC_3_SITE_ADDR = 190,
      OFFS_PAC_3_SITE_ADDR = 194,
      // Volumen ajuste
      VOL_SCALE_ADDR = 198,
      VOL_SCALE_SITE_ADDR = 202

   };

   /** ****************************************************************************
 ** ************ FUNCTIONS *****************************************************
 ** ****************************************************************************/
   void init_Memory(void); // funcion para lectura de datos en la memoria, valores de calibracion
   void writeString(eeprom_values eeAddress, String dataStr);
   String readString(eeprom_values eeAddress);
   float readFloat(eeprom_values eeAddress);
   float writeFloat(eeprom_values eeAddress, float varValue);
   void changeMenu(uint8_t coeftype, uint8_t modeCalibration, float newCoeficient, int place); // funcion para almacenar los cambios en las constantes de Calibracion

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

#endif /* MEMORYMANAGER_H */

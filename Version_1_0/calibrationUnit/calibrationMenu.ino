/*
 * File:   calibrationMenu.ino
 * Author: GIBIC UdeA
 *
 * Created on July 4, 2020, 13:41 PM
 */

/** ****************************************************************************
 ** ************ INCLUDES ******************************************************
 ** ****************************************************************************/
#include "calibrationMenu.h"

/** ****************************************************************************
 ** ************ DEFINES *******************************************************
 ** ****************************************************************************/

/** ****************************************************************************
 ** ************ EXTERN VARIABLES **********************************************
 ** ****************************************************************************/
// **********************************************************
extern String menuString;
extern String SerialID;

/** ****************************************************************************
 ** ************ VARIABLES *****************************************************
 ** ****************************************************************************/
uint8_t servMenuStateCurrent = SERV_NULL_MENU;
uint8_t servMenuStateNew = SERV_NULL_MENU;
uint8_t modeCalibration = MODE_NULL;
uint8_t coeftype = MODE_NULL;
uint8_t placeCalibration = MODE_NULL; // variable para especificar si es calibracion de fabrica o de sitio

/** ****************************************************************************
 ** ************ FUNCTIONS *****************************************************
 ** ****************************************************************************/

// Menu principal de calibracion
void printMainMenu(void)
{
    menuString = "\n\n************ Menu Principal ************\n"
                 "1. Alimentacion. \n"
                 "2. Estado de valvulas. \n"
                 "3. Coeficientes de fabrica. \n"
                 "4. Coeficientes de sitio. \n"
                 "5. Salir. \n\nSeleccione una opcion: ";

    Serial.print(menuString);
    servMenuStateNew = SERV_WAIT_MAIN;
    servMenuStateCurrent = servMenuStateNew;
}

// Menu de calibracion de fabrica
void printFactoryMenu(void)
{
    menuString = "\n\n*** Calibracion coeficientes fabrica ***\n"
                 "1. Cambiar Serial del Equipo (" +
                 SerialID + ").\n"
                            "2. Flujo inspiratorio.\n"
                            "3. Flujo expiratorio.\n"
                            "4. Presion Camara.\n"
                            "5. Presion Bolsa.\n"
                            "6. Presion Paciente.\n"
                            "7. Escala de volumen.\n"
                            "8. Menu anterior. \n"
                            "9. Salir.\n\nSeleccione una opcion: ";

    Serial.print(menuString);
    servMenuStateNew = SERV_WAIT_FACT;
    servMenuStateCurrent = servMenuStateNew;
}

// Menu de calibracion de sitio
void printSiteMenu(void)
{
    menuString = "\n\n*** Calibracion coeficientes de sitio ***\n"
                 "1. Flujo inspiratorio sitio.\n"
                 "2. Flujo expiratorio sitio.\n"
                 "3. Presion Camara sitio.\n"
                 "4. Presion Bolsa sitio.\n"
                 "5. Presion Paciente sitio.\n"
                 "6. Escala de volumen sitio.\n"
                 "7. Menu anterior. \n"
                 "8. Salir.\n\nSeleccione una opcion: ";

    Serial.print(menuString);
    servMenuStateNew = SERV_WAIT_SITE;
    servMenuStateCurrent = servMenuStateNew;
}

// menu interno de calibracion de fabrica y de sitio
void printInternalFactoryMenu(int mode)
{
    if (mode == FLUJO_INSPIRATORIO)
    {
        menuString = "\n\n************ FLUJO INSPIRATORIO ************\n"
                     "1. Imprimir datos 20 Hz. (q+ENTER para salir)\n"
                     "*Curva de ajuste  N 1 \n2. Amplitud.\n3. Offset.\n4. Limite.\n"
                     "*Curva de ajuste  N 2 \n5. Amplitud.\n6. Offset.\n7. Limite.\n"
                     "*Curva de ajuste  N 3 \n8. Amplitud.\n9. Offset.\n"
                     "10. Ver constantes \n"
                     "11. Menu anterior. \n"
                     "12. Salir.\n\nSeleccione una opcion: ";
    }
    if (mode == FLUJO_ESPIRATORIO)
    {
        menuString = "\n\n************ FLUJO ESPIRATORIO ************\n"
                     "1. Imprimir datos 20 Hz. (q+ENTER para salir)\n"
                     "*Curva de ajuste  N 1 \n2. Amplitud.\n3. Offset.\n4. Limite.\n"
                     "*Curva de ajuste  N 2 \n5. Amplitud.\n6. Offset.\n7. Limite.\n"
                     "*Curva de ajuste  N 3 \n8. Amplitud.\n9. Offset.\n"
                     "10. Ver constantes \n"
                     "11. Menu anterior. \n"
                     "12. Salir.\n\nSeleccione una opcion: ";
    }
    if (mode == PRESION_CAMARA)
    {
        menuString = "\n\n************ PRESION CAMARA ************\n"
                     "1. Imprimir datos 20 Hz. (q+ENTER para salir)\n"
                     "*Curva de ajuste  N 1 \n2. Amplitud.\n3. Offset.\n"
                     "4. Ver constantes \n"
                     "5. Menu anterior. \n"
                     "6. Salir.\n\nSeleccione una opcion: ";
    }
    if (mode == PRESION_BOLSA)
    {
        menuString = "\n\n************ PRESION BOLSA ************\n"
                     "1. Imprimir datos 20 Hz. (q+ENTER para salir)\n"
                     "*Curva de ajuste  N 1 \n2. Amplitud.\n3. Offset.\n"
                     "4. Ver constantes \n"
                     "5. Menu anterior. \n"
                     "6. Salir.\n\nSeleccione una opcion: ";
    }
    if (mode == PRESION_PACIENTE)
    {
        menuString = "\n\n************ PRESION PACIENTE ************\n"
                     "1. Imprimir datos 20 Hz. (q+ENTER para salir)\n"
                     "*Curva de ajuste  N 1 \n2. Amplitud.\n3. Offset.\n"
                     "4. Ver constantes \n"
                     "5. Menu anterior. \n"
                     "6. Salir.\n\nSeleccione una opcion: ";
    }
    if (mode == VOLUMEN_PACIENTE)
    {
        menuString = "\n\n************ VOLUMEN PACIENTE ************\n"
                     "*Curva de ajuste  N 1 \n1. Escala de ajuste.\n"
                     "2. Ver constantes \n"
                     "3. Menu anterior. \n"
                     "4. Salir.\n\nSeleccione una opcion: ";
    }

    Serial.print(menuString);
    if (placeCalibration == FACTORY)
    {
        servMenuStateNew = SERV_WAIT_FAIN;
    }
    else if (placeCalibration == SITE)
    {
        servMenuStateNew = SERV_WAIT_SIIN;
    }
    servMenuStateCurrent = servMenuStateNew;
}

/** ****************************************************************************
 ** ************ END OF THE CODE ***********************************************
 ** ****************************************************************************/

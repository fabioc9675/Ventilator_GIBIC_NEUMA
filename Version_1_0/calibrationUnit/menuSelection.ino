/*
 * File:   menuSelection.ino
 * Author: GIBIC UdeA
 *
 * Created on July 4, 2020, 13:41 PM
 */

/** ****************************************************************************
 ** ************ INCLUDES ******************************************************
 ** ****************************************************************************/
#include "menuSelection.h"

/** ****************************************************************************
 ** ************ DEFINES *******************************************************
 ** ****************************************************************************/

/** ****************************************************************************
 ** ************ EXTERN VARIABLES **********************************************
 ** ****************************************************************************/
// **********************************************************
// variables de operacion de calibracion
extern uint8_t servMenuStateCurrent;
extern uint8_t servMenuStateNew;
extern uint8_t modeCalibration;
extern uint8_t coeftype;
extern uint8_t placeCalibration; // variable para especificar si es calibracion de fabrica o de sitio

extern uint8_t flagService;

/** ****************************************************************************
 ** ************ VARIABLES *****************************************************
 ** ****************************************************************************/

/** ****************************************************************************
 ** ************ FUNCTIONS *****************************************************
 ** ****************************************************************************/

// menu para la seleccion de las opciones del menu principal
void mainMenuOptionChange(int selMenu)
{
    if ((selMenu > MAX_MAIN_MENU) || (selMenu == 0))
    {
        Serial.print("Opcion no valida \n");
        servMenuStateNew = SERV_MAIN_MENU;
        servMenuStateCurrent = servMenuStateNew;
    }
    else
    {
        switch (selMenu)
        {
        case 1: // seleccion del menu 1
            /* code */
            break;
        case 2: // seleccion del menu 2
            /* code */
            break;
        case 3: // seleccion del menu 3, calibracion de fabrica
            placeCalibration = FACTORY;
            servMenuStateNew = SERV_FACT_CALI;
            servMenuStateCurrent = servMenuStateNew;
            break;
        case 4: // seleccion del menu 4, calibracion de sitio
            placeCalibration = SITE;
            servMenuStateNew = SERV_SITE_CALI;
            servMenuStateCurrent = servMenuStateNew;
            break;
        case 5: // seleccion del menu 5, Salir
            Serial.print("Salida segura \n");
            servMenuStateNew = SERV_NULL_MENU;
            servMenuStateCurrent = servMenuStateNew;
            flagService = false;
            break;
        default:
            break;
        }

        // agregar las opciones de menu
        // Serial.println("Menu seleccionado");
    }
}

// menu para la seleccion de las opciones del menu de calibracion de fabrica
void factoryMenuOptionChange(int selMenu)
{
    if ((selMenu > MAX_FACT_MENU) || (selMenu == 0))
    {
        Serial.print("Opcion no valida \n");
        servMenuStateNew = SERV_FACT_CALI;
        servMenuStateCurrent = servMenuStateNew;
    }
    else
    {
        switch (selMenu)
        {
        case 1: // seleccion del menu 2
            servMenuStateNew = SERV_SERIAL_CH;
            servMenuStateCurrent = servMenuStateNew;
            Serial.print("Ingrese el serial del equipo: ");
            break;
        case 2: // seleccion del menu 1
            modeCalibration = FLUJO_INSPIRATORIO;
            servMenuStateNew = SERV_FAIN_CALI;
            servMenuStateCurrent = servMenuStateNew;
            printInternalFactoryMenu(modeCalibration);
            break;
        case 3: // seleccion del menu 2
            modeCalibration = FLUJO_ESPIRATORIO;
            servMenuStateNew = SERV_FAIN_CALI;
            servMenuStateCurrent = servMenuStateNew;
            printInternalFactoryMenu(modeCalibration);
            break;
        case 4: // seleccion del menu 3, calibracion de fabrica
            modeCalibration = PRESION_CAMARA;
            servMenuStateNew = SERV_FAIN_CALI;
            servMenuStateCurrent = servMenuStateNew;
            printInternalFactoryMenu(modeCalibration);
            break;
        case 5: // seleccion del menu 4
            modeCalibration = PRESION_BOLSA;
            servMenuStateNew = SERV_FAIN_CALI;
            servMenuStateCurrent = servMenuStateNew;
            printInternalFactoryMenu(modeCalibration);
            break;
        case 6: // seleccion del menu 5, Salir
            modeCalibration = PRESION_PACIENTE;
            servMenuStateNew = SERV_FAIN_CALI;
            servMenuStateCurrent = servMenuStateNew;
            printInternalFactoryMenu(modeCalibration);
            break;
        case 7: // seleccion del menu 5, Salir
            modeCalibration = VOLUMEN_PACIENTE;
            servMenuStateNew = SERV_FAIN_CALI;
            servMenuStateCurrent = servMenuStateNew;
            printInternalFactoryMenu(modeCalibration);
            break;
        case 8: // seleccion del menu 5, Salir
            Serial.print("Salida Calibracion \n");
            servMenuStateNew = SERV_MAIN_MENU;
            servMenuStateCurrent = servMenuStateNew;
            break;
        case 9: // seleccion del menu 7, Salir
            Serial.print("Salida segura \n");
            servMenuStateNew = SERV_NULL_MENU;
            servMenuStateCurrent = servMenuStateNew;
            flagService = false;
            break;
        default:
            break;
        }
    }
}

// menu para la seleccion de las opciones del menu de calibracion de sitio
void siteMenuOptionChange(int selMenu)
{
    if ((selMenu > MAX_SITE_MENU) || (selMenu == 0))
    {
        Serial.print("Opcion no valida \n");
        servMenuStateNew = SERV_SITE_CALI;
        servMenuStateCurrent = servMenuStateNew;
    }
    else
    {
        switch (selMenu)
        {
        case 1: // seleccion del menu 1
            modeCalibration = FLUJO_INSPIRATORIO;
            servMenuStateNew = SERV_SIIN_CALI;
            servMenuStateCurrent = servMenuStateNew;
            printInternalFactoryMenu(modeCalibration);
            break;
        case 2: // seleccion del menu 2
            modeCalibration = FLUJO_ESPIRATORIO;
            servMenuStateNew = SERV_SIIN_CALI;
            servMenuStateCurrent = servMenuStateNew;
            printInternalFactoryMenu(modeCalibration);
            break;
        case 3: // seleccion del menu 3, calibracion de fabrica
            modeCalibration = PRESION_CAMARA;
            servMenuStateNew = SERV_SIIN_CALI;
            servMenuStateCurrent = servMenuStateNew;
            printInternalFactoryMenu(modeCalibration);
            break;
        case 4: // seleccion del menu 4
            modeCalibration = PRESION_BOLSA;
            servMenuStateNew = SERV_SIIN_CALI;
            servMenuStateCurrent = servMenuStateNew;
            printInternalFactoryMenu(modeCalibration);
            break;
        case 5: // seleccion del menu 5, Salir
            modeCalibration = PRESION_PACIENTE;
            servMenuStateNew = SERV_SIIN_CALI;
            servMenuStateCurrent = servMenuStateNew;
            printInternalFactoryMenu(modeCalibration);
            break;
        case 6: // seleccion del menu 5, Salir
            modeCalibration = VOLUMEN_PACIENTE;
            servMenuStateNew = SERV_SIIN_CALI;
            servMenuStateCurrent = servMenuStateNew;
            printInternalFactoryMenu(modeCalibration);
            break;
        case 7: // seleccion del menu 5, Salir
            Serial.print("Salida Calibracion \n");
            servMenuStateNew = SERV_MAIN_MENU;
            servMenuStateCurrent = servMenuStateNew;
            break;
        case 8: // seleccion del menu 7, Salir
            Serial.print("Salida segura \n");
            servMenuStateNew = SERV_NULL_MENU;
            servMenuStateCurrent = servMenuStateNew;
            flagService = false;
            break;
        default:
            break;
        }
    }
}

/** ****************************************************************************
 ** ************ END OF THE CODE ***********************************************
 ** ****************************************************************************/
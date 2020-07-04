/*
 * File:   calibrationMenu.h
 * Author: GIBIC UdeA
 *
 * Created on July 4, 2020, 13:41 PM
 */

#ifndef CALIBRATIONMENU_H
#define CALIBRATIONMENU_H

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

    /** ****************************************************************************
 ** ************ DEFINES *******************************************************
 ** ****************************************************************************/

// definiciones para menu principal de servicio
#define SERV_NULL_MENU 0
#define SERV_MAIN_MENU 1  // estado de menu de inicio
#define SERV_WAIT_MAIN 2  // estado de espera de seleccion de opcion
#define SERV_FACT_CALI 4  // estado de menu de calibracion de fabrica
#define SERV_WAIT_FACT 5  // estado de espera en el menu de seleccion
#define SERV_FAIN_CALI 6  // estado de menu de calibracion de fabrica por variables
#define SERV_WAIT_FAIN 7  // estado de espera en el menu de seleccion por variables
#define SERV_ACQI_DATA 8  // estado de adquisicion de datos
#define SERV_SERIAL_CH 9  // estado de cambio de serial
#define SERV_FACT_PRIN 10 // estado de impresion de datos de flujo

#define SERV_SITE_CALI 11 // estado de menu de calibracion de sitio
#define SERV_WAIT_SITE 12 // estado de espera en el menu de seleccion
#define SERV_SIIN_CALI 13 // estado de menu de calibracion de sitio por variables
#define SERV_WAIT_SIIN 14 // estado de espera en el menu de seleccion por variables
#define SERV_SITE_PRIN 15 // estado de impresion de datos de flujo

#define MAX_MAIN_MENU 5  // cantidad de opciones en menu principal
#define MAX_FACT_MENU 10 // cantidad de opciones en menu de calibracion de fabrica
#define MAX_SITE_MENU 9  // cantidad de opciones en menu de calibracion de sitio
#define MAX_FAIN_MENU 12 // cantidad de opciones en menu de calibracion de fabrica por variables
#define MAX_SIIN_MENU 12 // cantidad de opciones en menu de calibracion de fabrica por variables

#define MODE_NULL 0
#define FLUJO_INSPIRATORIO 1
#define FLUJO_ESPIRATORIO 2
#define PRESION_CAMARA 3
#define PRESION_BOLSA 4
#define PRESION_PACIENTE 5
#define VOLUMEN_PACIENTE 6

#define AMPL_1 1
#define OFFS_1 2
#define LIMS_1 3
#define AMPL_2 4
#define OFFS_2 5
#define LIMS_2 6
#define AMPL_3 7
#define OFFS_3 8
#define LIMS_3 9

#define FACTORY 1
#define SITE 2



    /** ****************************************************************************
 ** ************ VARIABLES *****************************************************
 ** ****************************************************************************/

    /** ****************************************************************************
 ** ************ FUNCTIONS *****************************************************
 ** ****************************************************************************/

void printMainMenu(void); // Menu principal de calibracion
void printFactoryMenu(void); // Menu de calibracion de fabrica
void printSiteMenu(void); // Menu de calibracion de sitio
void printInternalFactoryMenu(int mode); // menu interno de calibracion de fabrica y de sitio


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

#endif /* CALIBRATIONMENU_H */

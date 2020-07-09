/*
 * File:   serialCONT.ino
 * Author: GIBIC UdeA
 *
 * Created on July 4, 2020, 13:41 PM
 */

/** ****************************************************************************
 ** ************ INCLUDES ******************************************************
 ** ****************************************************************************/
#include "serialCONT.h"

/** ****************************************************************************
 ** ************ DEFINES *******************************************************
 ** ****************************************************************************/

/** ****************************************************************************
 ** ************ EXTERN VARIABLES **********************************************
 ** ****************************************************************************/
// **********************************************************
// Variable de estado del encoder
extern unsigned int fl_StateEncoder;
// variables de estado de ventilacion
extern int stateMachine;
extern int newVentilationMode;

// variable de estado de menu
extern volatile uint8_t insideMenuFlag;

// banderas de cambio de valores
extern volatile uint8_t flagStabilityInterrupt;

extern volatile uint8_t flagFrecuencia;
extern volatile uint8_t flagPresion;
extern volatile uint8_t flagFlujo;
extern volatile uint8_t flagTrigger;
extern volatile uint8_t flagIE;
extern volatile uint8_t flagPeepMax;
extern volatile uint8_t flagSensibilityCPAP;
extern volatile uint8_t flagMode;
extern volatile uint8_t flagConfirm;
extern volatile uint8_t flagMinFR;
extern volatile uint8_t flagVE;

// contadores de configuraciones en el menu de usuario
extern String relacion_IE;

extern int newFrecRespiratoria;
extern int newRelacionIE;
extern int currentFrecRespiratoria;
extern int currentRelacionIE;
extern int trigger;
extern int PeepMax;
extern int maxPresion;
extern int maxFR;
extern int maxVE;

extern int batteryAlert;

// Variables calculadas
extern int frecRespiratoriaCalculada;
extern int I;
extern int E;
extern int calculatedE;
extern int apneaTime;

// variables de introduccion a los menus de configuracion
extern int optionConfigMenu;
extern int currentVentilationMode;
extern int optionVentMenu;

// Variables para iniciar impresion de datos en menu
extern volatile unsigned int menuImprimir;
extern volatile unsigned int lineaAlerta;
extern volatile uint8_t flagAlreadyPrint;

/** ****************************************************************************
 ** ************ VARIABLES *****************************************************
 ** ****************************************************************************/

/** ****************************************************************************
 ** ************ FUNCTIONS *****************************************************
 ** ****************************************************************************/
// envio de datos por serial para configuracion
void sendSerialData(void)
{
    if (currentRelacionIE > 0)
    {
        relacion_IE = "1:" + String((float)currentRelacionIE / 10, 1);
        I = 1;
        E = (char)currentRelacionIE;
    }
    else
    {
        relacion_IE = String(-(float)currentRelacionIE / 10, 1) + ":1";
        I = (char)(-currentRelacionIE);
        E = 1;
    }
    String dataToSend = String(currentFrecRespiratoria) + ',' + String(I) + ',' +
                        String(E) + ',' + String(maxPresion) + ',' + String(batteryAlert) + ',' +
                        String(flagStabilityInterrupt) + ',' + String(stateMachine) + ',' +
                        String(currentVentilationMode) + ',' + String(trigger) + ',' +
                        String(PeepMax) + ',' + String(maxFR) + ',' + String(maxVE) + ',' +
                        String(apneaTime) + ';';
    Serial2.print(dataToSend);
    //Serial.println(stateMachine);
}

/** ****************************************************************************
 ** ************ END OF THE CODE ***********************************************
 ** ****************************************************************************/

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

// Variable de estado del encoder
extern unsigned int fl_StateEncoder;
// variables de estado de ventilacion
extern byte stateMachine;
extern byte newVentilationMode;

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
extern volatile uint8_t flagToACBackUp;

// contadores de configuraciones en el menu de usuario
extern String relacion_IE;

extern byte newFrecRespiratoria;
extern int newRelacionIE;
extern byte currentFrecRespiratoria;
extern int currentRelacionIE;
extern byte trigger;
extern byte PeepMax;
extern byte maxPresion;
extern byte maxFR;
extern byte maxVE;

extern byte batteryAlert;

// Variables calculadas
extern byte frecRespiratoriaCalculada;
extern byte I;
extern byte E;
extern int calculatedE;
extern byte apneaTime;

// variables de introduccion a los menus de configuracion
extern byte optionConfigMenu;
extern byte currentVentilationMode;
extern byte optionVentMenu;

// Variables para iniciar impresion de datos en menu
extern volatile unsigned int menuImprimir;
extern volatile unsigned int lineaAlerta;
extern volatile uint8_t flagAlreadyPrint;

// Variables para menu actualizado
extern float Peep;
extern float Ppico;
extern float Pcon;
extern byte currentVE;
extern unsigned int VT;
extern int presPac;
extern int flowPac;
extern int presPacAnte;
extern int flowPacAnte;

// variables de alerta
extern int alerPresionPIP;
extern int alerDesconexion;
extern int alerObstruccion;
extern int alerGeneral;
extern int alerPresionPeep;
extern int alerFR_Alta;
extern int alerVE_Alto;

extern byte newVE;

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

/* ***************************************************************************
 * **** Ejecucion de la rutina de comunicacion por serial ********************
 * ***************************************************************************/
// Function to receive data from serial communication
void task_Receive(void *pvParameters)
{
    // Clean Serial buffers
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    Serial.flush();
    Serial2.flush();

    while (1)
    {
        if (Serial2.available() > 5)
        {
            // if (Serial.available() > 5) {  // solo para pruebas
            // Serial.println("Inside receiveData");
            String dataIn = Serial2.readStringUntil(';');
            // String dataIn = Serial.readStringUntil(';');  // solo para pruebas
            int contComas = 0;
            for (int i = 0; i < dataIn.length(); i++)
            {
                if (dataIn[i] == ',')
                {
                    contComas++;
                }
            }
            String dataIn2[40];
            for (int i = 0; i < contComas + 1; i++)
            {
                dataIn2[i] = dataIn.substring(0, dataIn.indexOf(','));
                dataIn = dataIn.substring(dataIn.indexOf(',') + 1);
            }
            //cargue los datos aqui
            //para entero
            //contCiclos =dataIn2[0].toInt();
            //para float
            Ppico = dataIn2[0].toFloat();
            Peep = dataIn2[1].toFloat();
            Pcon = Ppico - Peep;
            VT = dataIn2[2].toInt();
            alerPresionPIP = dataIn2[3].toInt();
            alerDesconexion = dataIn2[4].toInt();
            alerObstruccion = dataIn2[5].toInt();
            alerPresionPeep = dataIn2[6].toInt();
            alerGeneral = dataIn2[7].toInt();
            frecRespiratoriaCalculada = dataIn2[8].toInt();
            calculatedE = dataIn2[9].toInt();
            alerFR_Alta = dataIn2[10].toInt();
            alerVE_Alto = dataIn2[11].toInt();
            newVE = dataIn2[12].toInt();
            flagToACBackUp = dataIn2[13].toInt();
            presPac = dataIn2[14].toInt();
            flowPac = dataIn2[15].toInt();
            Serial2.flush();
            //Serial.flush();  // solo para pruebas
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
    //   vTaskDelete(NULL);
}

/** ****************************************************************************
 ** ************ END OF THE CODE ***********************************************
 ** ****************************************************************************/

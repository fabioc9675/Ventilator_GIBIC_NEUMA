/*
 * File:   serialHMI.ino
 * Author: GIBIC UdeA
 *
 * Created on July 4, 2020, 13:41 PM
 */

/** ****************************************************************************
 ** ************ INCLUDES ******************************************************
 ** ****************************************************************************/
#include "serialHMI.h"

/** ****************************************************************************
 ** ************ DEFINES *******************************************************
 ** ****************************************************************************/

/** ****************************************************************************
 ** ************ EXTERN VARIABLES **********************************************
 ** ****************************************************************************/
// Variables recibidas en el Serial
extern int newFrecRespiratoria;
extern int newI;
extern int newE;
extern int maxPresion;
extern int alerBateria;
extern int estabilidad;

// Variables maquina de estados de modos ventilatorios
extern int newStateMachine;
extern int newVentilationMode;

// Variables de ventilacion
extern int newTrigger;
extern int newPeepMax;
extern int maxFR;
extern int maxVE;
extern int apneaTime;

// banderas de deteccion de alarmas
extern volatile uint8_t flagAlarmPatientDesconnection;

extern float SPin;  // Senal filtrada de presion en la camara
extern float SPout; // Senal filtrada de presion en la bolsa
extern float SPpac; // Senal de presion en la via aerea del paciente
extern float SFin;  //Senal de flujo inspiratorio
extern float SFout; //Senal de flujo espiratorio

extern float SFpac;   // Senal de flujo del paciente
extern float SVtidal; // informacion de promedio para Vtidal
extern float VT;
extern float VtidalV;       // senal de volumen para visualizacion
extern float VtidalC;       // senal de volumen para calculo
extern float SFpacV;        // senal de flujo para visualizacion
extern float Ppico;         // valor medido de Ppico
extern float Peep;          // valor medido de Peep
extern float PpicoProximal; // medicion realizada con sensor distal a paciente
extern float PpicoDistal;   // medicion realizada con sensor distal a paciente
extern float PeepProximal;  // medicion realizada con sensor distal a paciente
extern float PeepDistal;    // medicion realizada con sensor distal a paciente
extern int frecRespiratoriaCalculada;
extern int calculatedE;
extern int calculatedI;
extern int currentVE;

// Variables para el envio y recepcion de alarmas
extern int alerPresionPIP;
extern int alerDesconexion;
extern int alerObstruccion;
extern int alerPeep;
extern int alerBateria;
extern int alerGeneral;
extern int alerFR_Alta;
extern int alerVE_Alto;
extern int estabilidad;
extern int PeepEstable;
extern int flagToACBackUp;

/** ****************************************************************************
 ** ************ VARIABLES *****************************************************
 ** ****************************************************************************/

/** ****************************************************************************
 ** ************ FUNCTIONS *****************************************************
 ** ****************************************************************************/

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
            String dataIn = Serial2.readStringUntil(';');
            // String dataIn = Serial.readStringUntil(';');  // solo para pruebas
            // Serial.println(dataIn);
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
            newFrecRespiratoria = dataIn2[0].toInt();
            newI = dataIn2[1].toInt();
            newE = dataIn2[2].toInt();
            maxPresion = dataIn2[3].toInt();
            alerBateria = dataIn2[4].toInt();
            estabilidad = dataIn2[5].toInt();
            newStateMachine = dataIn2[6].toInt();
            newVentilationMode = dataIn2[7].toInt();
            newTrigger = dataIn2[8].toInt();
            newPeepMax = dataIn2[9].toInt();
            maxFR = dataIn2[10].toInt();
            maxVE = dataIn2[11].toInt();
            apneaTime = dataIn2[12].toInt();
            Serial2.flush();
            // Serial.flush();  // solo para pruebas
            /*Serial.println("State = " + String(currentStateMachine));
			  Serial.println(String(newFrecRespiratoria) + ',' + String(newI) + ',' +
			  String(newE) + ',' + String(maxPresion) + ',' +
			  String(alerBateria) + ',' + String(estabilidad) + ',' +
			  String(newStateMachine) + ',' + String(newVentilationMode));*/
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}

// Function to rupdate LCD each 200 ms
void task_sendSerialData(void *arg)
{
    while (1)
    {
        // evaluacion de condicion de desconexion de paciente
        if (flagAlarmPatientDesconnection == true)
        {
            SPpac = 0;
            SFpac = 0;
            SFpacV = 0;
            VtidalV = 0;
            VtidalC = 0;
            Ppico = 0;
            Peep = 0;
            PpicoProximal = 0;
            PeepProximal = 0;
            PpicoDistal = 0;
            PeepDistal = 0;
            frecRespiratoriaCalculada = 0;
            calculatedE = 0;
            currentVE = 0;
            VT = 0;
        }
        String dataToSend = String(Ppico) + ',' + String(Peep) + ',' + String(VT) + ',' +
                            String(alerPresionPIP) + ',' + String(alerDesconexion) + ',' +
                            String(alerObstruccion) + ',' + String(alerPeep) + ',' + String(alerGeneral) + ',' +
                            String(int(frecRespiratoriaCalculada)) + ',' + String(int(calculatedE)) + ',' +
                            String(int(alerFR_Alta)) + ',' + String(int(alerVE_Alto)) + ',' + String(currentVE) + ',' + String(flagToACBackUp) + ';';

        // Serial.print(dataToSend);  // solo para pruebas
        Serial2.print(dataToSend);

        // flagToACBackUp = false;

        vTaskDelay(200 / portTICK_PERIOD_MS); // update each 200 ms
    }
    vTaskDelete(NULL);
}

/** ****************************************************************************
 ** ************ END OF THE CODE ***********************************************
 ** ****************************************************************************/

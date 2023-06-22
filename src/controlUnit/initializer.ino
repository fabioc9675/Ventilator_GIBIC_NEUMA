/*
 * File:   initializer.ino
 * Author: GIBIC UdeA
 *
 * Created on July 4, 2020, 13:41 PM
 */

/** ****************************************************************************
 ** ************ INCLUDES ******************************************************
 ** ****************************************************************************/
#include "initializer.h"

/** ****************************************************************************
 ** ************ DEFINES *******************************************************
 ** ****************************************************************************/

/** ****************************************************************************
 ** ************ EXTERN VARIABLES **********************************************
 ** ****************************************************************************/
// **********************************************************
// Variables y parametros de impresion en raspberry
extern String SerialID;

extern String idEqupiment;
extern String patientPress;
extern String patientFlow;
extern String patientVolume;
extern String pressPIP;
extern String pressPEEP;
extern String frequency;
extern String xSpeed;
extern String rInspir;
extern String rEspir;
extern String volumeT;
extern String alertPip;
extern String alertPeep;
extern String alertObstruction;
extern String alertConnPat;
extern String alertGeneralFailure;
extern String alertConnEquipment;
extern String alertFrequency;
extern String alertMinuteVentilation;
extern String alertValve1Fail;
extern String alertValve2Fail;
extern String alertValve3Fail;
extern String valve1Temp;
extern String valve2Temp;
extern String valve3Temp;
extern String valve1Current;
extern String valve2Current;
extern String valve3Current;
extern String source5v0Voltage;
extern String source5v0Current;
extern String source5v0SWVoltage;
extern String source5v0SWCurrent;
extern String cameraPress;
extern String bagPress;
extern String inspFlow;
extern String EspFlow;
extern String lPresSup;
extern String lPresInf;
extern String lFlowSup;
extern String lFlowInf;
extern String lVoluSup;
extern String lVoluInf;

// Cadena de impresion en raspberry
extern String RaspberryChain;

/** ****************************************************************************
 ** ************ VARIABLES *****************************************************
 ** ****************************************************************************/

/** ****************************************************************************
 ** ************ FUNCTIONS *****************************************************
 ** ****************************************************************************/
// inicializacion de los pines del microcontrolador
void init_GPIO(void)
{
    // Configuracion de los pines de conexion con del driver para manejo de electrovalvulas

    // inicializacion de los pines controladores de las EV como salidas
    pinMode(EV_ESC_CAM, OUTPUT); // PIN 6   velocidad
    pinMode(EV_INSPIRA, OUTPUT); // PIN 3   velocidad
    pinMode(EV_ESPIRA, OUTPUT);  // PIN 12  velocidad
    pinMode(12, OUTPUT);         // PIN 3   velocidad
    pinMode(13, OUTPUT);         // PIN 6   velocidad

    // Inicializacion de los pines de ADC para conversion Analogo-digitalPinToInterrupt
    adcAttachPin(ADC_PRESS_1);
    adcAttachPin(ADC_PRESS_2);
    adcAttachPin(ADC_PRESS_3);
    adcAttachPin(ADC_FLOW_1);
    adcAttachPin(ADC_FLOW_2);

    // desactiva todas las salidas de electrovalvulas
    digitalWrite(EV_ESC_CAM, LOW); // PIN 3   velocidad
    digitalWrite(EV_INSPIRA, LOW); // PIN 6   velocidad
    digitalWrite(EV_ESPIRA, LOW);  // PIN 12  velocidad
    digitalWrite(12, LOW);         // PIN 3   velocidad
    digitalWrite(13, LOW);         // PIN 6   velocidad
}

// funcion de inicializacion de variables de texto
void init_TextPayload(void) {

	//Inicializacion de los strings comunicacion con la Raspberry
	idEqupiment = String(SerialID);
	patientPress = String("");
	patientFlow = String("");
	patientVolume = String("");
	pressPIP = String("");
	pressPEEP = String("");
	frequency = String("");
	xSpeed = String("");
	rInspir = String("");
	rEspir = String("");
	volumeT = String("");
	alertPip = String("");
	alertPeep = String("");
	alertObstruction = String("");
	alertConnPat = String("");
	alertGeneralFailure = String("");
	alertConnEquipment = String("");
	alertFrequency = String("");
	alertMinuteVentilation = String("");
	alertValve1Fail = String("");
	alertValve2Fail = String("");
	alertValve3Fail = String("");
	valve1Temp = String("");
	valve2Temp = String("");
	valve3Temp = String("");
	valve1Current = String("");
	valve2Current = String("");
	valve3Current = String("");
	source5v0Voltage = String("");
	source5v0Current = String("");
	source5v0SWVoltage = String("");
	source5v0SWCurrent = String("");
	cameraPress = String("");
	bagPress = String("");
	inspFlow = String("");
	EspFlow = String("");
	lPresSup = String("");
	lPresInf = String("");
	lFlowSup = String("");
	lFlowInf = String("");
	lVoluSup = String("");
	lVoluInf = String("");

	RaspberryChain.reserve(512);
	RaspberryChain = String("");

}

/** ****************************************************************************
 ** ************ END OF THE CODE ***********************************************
 ** ****************************************************************************/
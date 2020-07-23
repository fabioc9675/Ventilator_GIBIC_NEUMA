/*
 * File:   cyclingFunctions.ino
 * Author: GIBIC UdeA
 *
 * Created on July 4, 2020, 13:41 PM
 */

/** ****************************************************************************
 ** ************ INCLUDES ******************************************************
 ** ****************************************************************************/
#include "cyclingFunctions.h"

/** ****************************************************************************
 ** ************ DEFINES *******************************************************
 ** ****************************************************************************/

/** ****************************************************************************
 ** ************ EXTERN VARIABLES **********************************************
 ** ****************************************************************************/
// **********************************************************
// Variables de maquinas de estado
extern unsigned int currentStateMachineCycling;
extern byte AC_stateMachine;

// Variables maquina de estados de modos ventilatorios
extern int currentStateMachine;
extern int newStateMachine;
extern int currentVentilationMode;
extern int newVentilationMode;

// variables de calculo de parametros ventilatorios
extern float Ppico;         // valor medido de Ppico
extern float PpicoProximal; // medicion realizada con sensor distal a paciente
extern float PpicoDistal;   // medicion realizada con sensor distal a paciente
extern float Peep;          // valor medido de Peep
extern float Peep_AC;       // medicion de Peep en el modo asistido controlado
extern float PeepProximal;  // medicion realizada con sensor distal a paciente
extern float PeepDistal;    // medicion realizada con sensor distal a paciente
extern float flowZero;      // medicion del flujo cero para la grafica de volumen
extern float dPpac;         // Derivada de SPpac

extern float SPin;  // Senal filtrada de presion en la camara
extern float SPout; // Senal filtrada de presion en la bolsa
extern float SPpac; // Senal de presion en la via aerea del paciente
extern float SFin;  //Senal de flujo inspiratorio
extern float SFout; //Senal de flujo espiratorio
extern float SFpac; // Senal de flujo del paciente

extern float SVtidal; // informacion de promedio para Vtidal

extern int frecRespiratoriaCalculada;
extern int calculatedE;
extern int calculatedI;
extern int currentVE;

// Mediciones derivadas de las senales del ventilador
extern float Pin_max;
extern float Pout_max;
extern float VT;

extern float Pin_min;
extern float Pout_min;
extern float pmin;
extern float pmax;
extern float flmin;
extern float flmax;
extern float vmin;
extern float vmax;

// Variables para visualizacion
extern float VtidalC; // senal de volumen para calculo
extern float VtidalV; // senal de volumen para visualizacion
extern float SFpacV;  // senal de flujo para visualizacion

// variables para el filtrado
extern float VTidProm[3];

// Variables recibidas en el Serial
extern int newFrecRespiratoria;
extern int newI;
extern int newE;

// banderas de estados de ventilacion
extern uint8_t flagAC;
extern int flagToACBackUp;

/** ****************************************************************************
 ** ************ VARIABLES *****************************************************
 ** ****************************************************************************/
volatile int contCycling = 0; // variable que cuenta los ms del ciclado para cambiar estado de electrovalvulas
uint8_t flagInsp = false;     // bandera que indica que inicio la inspiracion

// Variables de ventilacion
uint8_t flagMaxPresion = false;     // bandera de maxima presion alcanzada
volatile uint8_t flagInicio = true; // bandera de inicio de ventilacion

volatile float inspirationTime = 1.666;
volatile float expirationTime = 3.333;
int currentFrecRespiratoria = 12;
int currentI = 1;
int currentE = 20;
float relI = 0;
float relE = 0;

int newTrigger = 2;
int newPeepMax = 5;
int maxFR = 30;
int maxVE = 30;
int maxPresion = 30;
int apneaTime = 20;
int minFR = 4;
float UmbralPpmin = 100;
float UmbralPpico = -100;
float UmbralPpicoDistal = -100;
float UmbralFmin = 100;
float UmbralFmax = -100;
float UmbralVmin = 100;
float UmbralVmax = -100;
float SFinMax = 50;
float SFoutMax = -50;
float SFinMaxInsp = 50;
float SFtotalMax = -50;

// Variables para el envio y recepcion de alarmas
int alerPresionPIP = 0;
int alerDesconexion = 0;
int alerObstruccion = 0;
int alerPeep = 0;
int alerBateria = 0;
int alerGeneral = 0;
int alerFR_Alta = 0;
int alerVE_Alto = 0;
int estabilidad = 0;
int PeepEstable = 0;

float SUMVin_Ins = 0;
float SUMVout_Ins = 0;
float SUMVin_Esp = 0;
float SUMVout_Esp = 0;
float Vin_Ins = 0;
float Vout_Ins = 0;
float Vin_Esp = 0;
float Vout_Esp = 0;

// variables para calculo de frecuencia y relacion IE en CPAP
float SFant = 0;
float dFlow = 0;
int stateFrecCPAP = 0;
int contFrecCPAP = 0;
int contEspCPAP = 0;
int contInsCPAP = 0;

// banderas de deteccion de alarmas
volatile uint8_t flagAlarmPpico = false;
volatile uint8_t flagAlarmGeneral = false;
volatile uint8_t flagAlarmPatientDesconnection = false;
volatile uint8_t flagAlarmObstruccion = false;
volatile uint8_t flagAlarmFR_Alta = false;
volatile uint8_t flagAlarmVE_Alto = false;

/** ****************************************************************************
 ** ************ FUNCTIONS *****************************************************
 ** ****************************************************************************/
// Cycling of the Mechanical Ventilator
void cycling(void)
{
    contCycling++; // contador que incrementa cada ms en la funcion de ciclado
    // Maquina de estados del ciclado
    switch (currentStateMachineCycling)
    {
    case STOP_CYCLING:
        break;
    case START_CYCLING:
        if (contCycling >= 1)
        {                                   // Inicia el ciclado abriendo electrovalvula de entrada y cerrando electrovalvula de salida
            flagInsp = true;                // Activa bandera que indica que empezo la inspiracion
            digitalWrite(EV_INSPIRA, LOW);  // Piloto conectado a ambiente -> Desbloquea valvula piloteada y permite el paso de aire
            digitalWrite(EV_ESPIRA, HIGH);  //Piloto conectado a PIP -> Limita la presion de la via aerea a la PIP configurada
            digitalWrite(EV_ESC_CAM, HIGH); //Piloto conectado a Presion de activacion -> Presiona la camara
            currentStateMachineCycling = INSPIRATION_CYCLING;
        }
        break;
    case INSPIRATION_CYCLING:
        if (contCycling >= int(inspirationTime * 1000) || flagMaxPresion == true)
        {
            flagMaxPresion = false;
            //Calculo PIP
            if (Ppico < 0)
            {              // Si el valor de Ppico es negativo
                Ppico = 0; // Lo limita a 0
            }
            Ppico = int(round(Ppico));
            UmbralPpicoDistal = SPout;

            //Mediciones de presion del sistema
            Pin_max = SPin;   //Presion maxima de la camara
            Pout_max = SPout; //Presion maxima de la bolsa

            //Medicion de Volumen circulante
            if (VtidalC >= 0)
            {
                VTidProm[2] = VtidalC;
            }
            else
            {
                VTidProm[2] = 0;
            }
            // promediado del Vtidal
            for (int i = 2; i >= 1; i--)
            {
                VTidProm[2 - i] = VTidProm[2 - i + 1];
            }
            //- Inicializacion
            SVtidal = 0;
            //- Actualizacion
            for (int i = 0; i <= 2; i++)
            {
                SVtidal = SVtidal + VTidProm[i];
            }
            //- Calculo promedio
            VT = SVtidal / 3;

            //Mediciones de flujo cero
            // flowZero = SFin - SFout; // nivel cero de flujo para calculo de volumen
            flowZero = SFpacV;
            //Rutina de ciclado
            flagInsp = false;               // Desactiva la bandera, indicando que empezo la espiracion
            digitalWrite(EV_INSPIRA, HIGH); //Piloto conectado a presion de bloqueo -> Bloquea valvula piloteada y restringe el paso de aire
            digitalWrite(EV_ESC_CAM, LOW);  //Piloto conectado a PEEP -> Limita la presion de la via aerea a la PEEP configurada
            digitalWrite(EV_ESPIRA, LOW);   //Piloto conectado a ambiente -> Despresuriza la camara y permite el llenado de la bolsa
            currentStateMachineCycling = EXPIRATION_CYCLING;
        }
        break;
    case EXPIRATION_CYCLING:
        //Add para el modo A/C
        if (flagAC == true)
        {
            flagAC = false;
            frecRespiratoriaCalculada = 60.0 / ((float)contCycling / 1000.0);
            calculatedE = (int)((((60.0 / (float)frecRespiratoriaCalculada) / (float)inspirationTime) - 1) * currentI * 10) + 1;
            if (calculatedE < 10)
            {
                calculatedE = 10;
            }
            else if (calculatedE > 90)
            {
                calculatedE = 90;
            }
            contCycling = 0;

            //Calculo de Peep
            PeepProximal = Peep_AC; // Peep como la presion en la via aerea al final de la espiracion
            PeepDistal = SPout;

            /* *******************************************************************
			 * *** Aqui se debe verificar cual es el valor de Peep a utlizar *****
			 * *******************************************************************/
            if ((PeepDistal - PeepProximal) > 2)
            {
                Peep = PeepDistal; // PIP como la presion en la via aerea al final de la espiracion
                flagAlarmObstruccion = true;
            }
            else
            {
                Peep = PeepProximal; // Peep como la presion en la via aerea al final de la espiracion
                flagAlarmObstruccion = false;
            }

            if (Peep < 0)
            {             // Si el valor de Peep es negativo
                Peep = 0; // Lo limita a 0
            }
            Peep = int(round(Peep_AC));

            if (estabilidad)
            {
                PeepEstable = Peep;
                estabilidad = 0;
            }
            else
            {
                if (Peep <= PeepEstable - 1.5)
                {
                    alerPeep = 1;
                }
                else
                {
                    alerPeep = 0;
                }
            }
            //Ajuste del valor de volumen
            VtidalV = 0;
            VtidalC = 0;
            // flowZero = SFin - SFout; // nivel cero de flujo para calculo de volumen
            flowZero = SFpacV;

            //Calculos de volumenes
            //- Asignacion
            Vin_Ins = SUMVin_Ins / 1000;
            Vout_Ins = SUMVout_Ins / 1000;
            Vin_Esp = SUMVin_Esp / 1000;
            Vout_Esp = SUMVout_Esp / 1000;

            //- Reinio de acumuladores
            SUMVin_Ins = 0;
            SUMVout_Ins = 0;
            SUMVin_Esp = 0;
            SUMVout_Esp = 0;

            //Mediciones de presion del sistema
            Pin_min = SPin;   //Presion minima de la camara
            Pout_min = SPout; //Presion minima de la bolsa

            //Asignacion de valores maximos y minimos de presion
            pmin = UmbralPpmin; //asigna la presion minima encontrada en todo el periodo
            pmax = UmbralPpico; //asigna la presion maxima encontrada en todo el periodo
            flmin = UmbralFmin; //asigna el flujo minimo encontrada en todo el periodo
            flmax = UmbralFmax; //asigna el flujo maximo encontrada en todo el periodo
            vmin = UmbralVmin;  //asigna el volumen minimo encontrada en todo el periodo
            vmax = UmbralVmax;  //asigna el volumen maximo encontrada en todo el periodo
            // Ppico = pmax;
            //Calculo de PIP
            PpicoProximal = pmax;
            PpicoDistal = UmbralPpicoDistal;
            // PpicoDistal = SPout;

            UmbralPpmin = 100;  //Reinicia el umbral minimo de presion del paciente
            UmbralPpico = -100; //Reinicia el umbral maximo de presion del paciente
            UmbralFmin = 100;   //Reinicia el umbral minimo de flujo del paciente
            UmbralFmax = -100;  //Reinicia el umbral maximo de flujo del paciente
            UmbralVmin = 100;   //Reinicia el umbral minimo de volumen del paciente
            UmbralVmax = -100;  //Reinicia el umbral maximo de volumen del paciente

            /* *******************************************************************
			  * *** Aqui se debe verificar cual es el valor de Ppico a utlizar *****
			  * *******************************************************************/
            if ((PpicoProximal < 2) && (PpicoDistal - PpicoProximal - 10) > 4)
            {
                Ppico = PpicoDistal - 10; // PIP como la presion en la via aerea al final de la espiracion
                flagAlarmObstruccion = true;
            }
            else
            {
                Ppico = PpicoProximal; // PIP como la presion en la via aerea al final de la espiracion
                flagAlarmObstruccion = false;
            }

            //Metodo de exclusion de alarmas
            if (Ppico > 3 && Peep > 1)
            {
                flagInicio = false;
            }

            currentVE = (int)((VT * frecRespiratoriaCalculada) / 100.0); // calculo de la ventilacion minuto

            if (newStateMachine != currentStateMachine)
            {
                currentStateMachine = newStateMachine;
                PeepEstable = 0;
            }

            alarmsDetection(); // se ejecuta la rutina de deteccion de alarmas
            flagAlarmPpico = false;
            alerPresionPIP = 0;

            currentStateMachineCycling = START_CYCLING;
            AC_stateMachine = 0;
        }

        // Add to C-PMV mode
        if ((contCycling >= int(((inspirationTime + expirationTime) * 1000))))
        {
            frecRespiratoriaCalculada = 60.0 / ((float)contCycling / 1000.0);
            calculatedE = (int)((((60.0 / (float)frecRespiratoriaCalculada) / (float)inspirationTime) - 1) * currentI * 10) + 1;
            if (calculatedE < 10)
            {
                calculatedE = 10;
            }
            else if (calculatedE > 90)
            {
                calculatedE = 90;
            }
            contCycling = 0;
            //Calculo de Peep
            PeepProximal = SPpac;
            PeepDistal = SPout;
            /* *******************************************************************
			 * *** Aqui se debe verificar cual es el valor de Peep a utlizar *****
			 * *******************************************************************/
            if ((PeepDistal - PeepProximal) > 2)
            {
                Peep = PeepDistal; // PIP como la presion en la via aerea al final de la espiracion
                flagAlarmObstruccion = true;
            }
            else
            {
                Peep = PeepProximal; // Peep como la presion en la via aerea al final de la espiracion
                flagAlarmObstruccion = false;
            }

            if (Peep < 0)
            {             // Si el valor de Peep es negativo
                Peep = 0; // Lo limita a 0
            }
            Peep = int(round(Peep));
            if (estabilidad)
            {
                PeepEstable = Peep;
                estabilidad = 0;
            }
            else
            {
                if (Peep <= PeepEstable - 1.5)
                {
                    alerPeep = 1;
                }
                else
                {
                    alerPeep = 0;
                }
            }
            //Ajuste del valor de volumen
            VtidalV = 0;
            VtidalC = 0;
            // flowZero = SFin - SFout; // nivel cero de flujo para calculo de volumen
            flowZero = SFpacV;

            //Calculos de volumenes
            //- Asignacion
            Vin_Ins = SUMVin_Ins / 1000;
            Vout_Ins = SUMVout_Ins / 1000;
            Vin_Esp = SUMVin_Esp / 1000;
            Vout_Esp = SUMVout_Esp / 1000;

            //- Reinio de acumuladores
            SUMVin_Ins = 0;
            SUMVout_Ins = 0;
            SUMVin_Esp = 0;
            SUMVout_Esp = 0;

            //Mediciones de presion del sistema
            Pin_min = SPin;   //Presion minima de la camara
            Pout_min = SPout; //Presion minima de la bolsa

            //Asignacion de valores maximos y minimos de presion
            pmin = UmbralPpmin; //asigna la presion minima encontrada en todo el periodo
            pmax = UmbralPpico; //asigna la presion maxima encontrada en todo el periodo
            flmin = UmbralFmin; //asigna el flujo minimo encontrada en todo el periodo
            flmax = UmbralFmax; //asigna el flujo maximo encontrada en todo el periodo
            vmin = UmbralVmin;  //asigna el volumen minimo encontrada en todo el periodo
            vmax = UmbralVmax;  //asigna el volumen maximo encontrada en todo el periodo
            // Ppico = pmax;
            //Calculo de PIP
            PpicoProximal = pmax;
            PpicoDistal = UmbralPpicoDistal;
            // PpicoDistal = SPout;

            UmbralPpmin = 100;        //Reinicia el umbral minimo de presion del paciente
            UmbralPpico = -100;       //Reinicia el umbral maximo de presion del paciente
            UmbralPpicoDistal = -100; //Reinicia el umbral maximo de presion del paciente
            UmbralFmin = 100;         //Reinicia el umbral minimo de flujo del paciente
            UmbralFmax = -100;        //Reinicia el umbral maximo de flujo del paciente
            UmbralVmin = 100;         //Reinicia el umbral minimo de volumen del paciente
            UmbralVmax = -100;        //Reinicia el umbral maximo de volumen del paciente

            /* *******************************************************************
			  * *** Aqui se debe verificar cual es el valor de Ppico a utlizar *****
			  * *******************************************************************/
            if ((PpicoProximal < 2) && (PpicoDistal - PpicoProximal - 10) > 4)
            {
                Ppico = PpicoDistal - 10; // PIP como la presion en la via aerea al final de la espiracion
                flagAlarmObstruccion = true;
            }
            else
            {
                Ppico = PpicoProximal; // PIP como la presion en la via aerea al final de la espiracion
                flagAlarmObstruccion = false;
            }

            //Metodo de exclusion de alarmas
            if (Ppico > 2 && Peep > 2)
            {
                flagInicio = false;
            }

            currentVE = (int)((VT * frecRespiratoriaCalculada) / 100.0); // calculo de la ventilacion minuto

            alarmsDetection(); // se ejecuta la rutina de deteccion de alarmas
            flagAlarmPpico = false;
            alerPresionPIP = 0;
            currentStateMachineCycling = START_CYCLING;

            if (newStateMachine != currentStateMachine)
            {
                currentStateMachine = newStateMachine;
                contCycling = 0;
            }

            if ((newFrecRespiratoria != currentFrecRespiratoria) ||
                (newI != currentI) || (newE != !currentE))
            { // condicion implementada para terminar un ciclado normal cada que se cambie de modo o de parametros ventilatorios
                currentFrecRespiratoria = newFrecRespiratoria;
                currentI = newI;
                currentE = newE;
                // Calculo del tiempo I:E
                if (currentI == 1)
                { // calculo de la relacion IE en el modo controlado
                    inspirationTime = (float)(60.0 / (float)currentFrecRespiratoria) / (1 + (float)((float)currentE / 10.0));
                    expirationTime = (float)((float)currentE / 10.0) * (float)inspirationTime;
                }
                else
                {
                    expirationTime = (float)(60.0 / (float)currentFrecRespiratoria) / (1 + (float)((float)currentI / 10.0));
                    inspirationTime = (float)((float)currentI / 10.0) * (float)expirationTime;
                }
            }
        }
        break;
    default:
        break;
    }

    //Calculo de Volumenes en tiempo inspiratorio y espiratorio
    if (flagInsp == true)
    { //Durante el tiempo inspiratorio
        SUMVin_Ins = SUMVin_Ins + SFin;
        SUMVout_Ins = SUMVout_Ins + SFout;
    }
    else
    { //Durante el tiempo espiratorio
        SUMVin_Esp = SUMVin_Esp + SFin;
        SUMVout_Esp = SUMVin_Esp + SFout;
    }
} // end cycling()

// Funcion para la deteccion de alarmas del sistema
void alarmsDetection(void)
{
    // Ppico Alarm
    if (flagInicio == false)
    { //Si hay habilitacion de alarmas
        // Alarma por Ppico elevada
        /*if (Ppico > maxPresion) {
			flagAlarmPpico = true;
			alerPresionPIP = 1;
		}
		else {*/
        /*if (Ppico < maxPresion){
			flagAlarmPpico = false;
			alerPresionPIP = 0;
		}*/
        // Fallo general
        if ((Pin_max) < 0)
        {
            //flagAlarmGeneral = true;
            //alerGeneral = 1;
            //newStateMachine = 1;
        }
        else
        {
            // flagAlarmGeneral = false;
            /// alerGeneral = 0;
        }

        // Alarma por desconexion del paciente
        if ((Ppico) < 4)
        {
            flagAlarmPatientDesconnection = true;
            alerDesconexion = 1;
        }
        else
        {
            flagAlarmPatientDesconnection = false;
            alerDesconexion = 0;
        }

        // Alarma por obstruccion
        if (((Vout_Ins >= 0.5 * Vin_Ins) && (Peep < 3)) || (flagAlarmObstruccion == true))
        {
            alerObstruccion = 1;
        }
        else
        {
            alerObstruccion = 0;
        }

        SFinMax = SFin;
        SFoutMax = SFout;

        if (frecRespiratoriaCalculada > maxFR)
        {
            flagAlarmFR_Alta = true;
            alerFR_Alta = 1;
        }
        else if (frecRespiratoriaCalculada < minFR)
        {
            flagAlarmFR_Alta = true;
            alerFR_Alta = 2;
        }
        else
        {
            flagAlarmFR_Alta = false;
            alerFR_Alta = 0;
        }

        if (currentVE > maxVE * 10)
        {
            flagAlarmVE_Alto = true;
            alerVE_Alto = 1;
        }
        else
        {
            flagAlarmVE_Alto = false;
            alerVE_Alto = 0;
        }
    }
}

// CPAP of the Mechanical Ventilator
void cpapRoutine(void)
{
    float frecCalcCPAP = 0;
    // esta funcion se ejecuta cada milisegundo
    if (newStateMachine != currentStateMachine)
    {
        currentStateMachine = newStateMachine;
        PeepEstable = 0;
        stateFrecCPAP = CPAP_INIT;
        alerFR_Alta = 0;
        contFrecCPAP = 0; // reinicio del contador de frecuencia en CPAP
    }

    if (currentStateMachine == CPAP_STATE)
    {
        contCycling = 0;
        digitalWrite(EV_INSPIRA, LOW); //Piloto conectado a presion de bloqueo -> Bloquea valvula piloteada y restringe el paso de aire
        digitalWrite(EV_ESC_CAM, LOW); //Piloto conectado a PEEP -> Limita la presion de la via aerea a la PEEP configurada
        digitalWrite(EV_ESPIRA, LOW);  //Piloto conectado a ambiente -> Despresuriza la camara y permite el llenado de la bolsa

        if ((SFpac > COMP_FLOW_MAX_CPAP) && ((dFlow) > COMP_DEL_F_MAX_CPAP) && (stateFrecCPAP != CPAP_INSPIRATION))
        { // inicio de la inspiracion
            // Inicializa Maquina de estados para que inicie en CPAP
            stateFrecCPAP = CPAP_INSPIRATION;

            // Calculo de la frecuecnia respiratoria en CPAP
            frecCalcCPAP = 60.0 / ((float)contFrecCPAP / 1000.0);
            if (frecCalcCPAP < 50)
            {
                frecRespiratoriaCalculada = (int)frecCalcCPAP;
            }

            // Calculo de la relacion IE en CPAP
            if (contInsCPAP < contEspCPAP)
            {
                calculatedI = 1;
                calculatedE = int(10.0 * ((float)contEspCPAP / 1000.0) / ((60.0 / (float)frecCalcCPAP) - ((float)contEspCPAP) / 1000.0));
            }
            else if (contEspCPAP < contInsCPAP)
            {
                calculatedE = 10;
                calculatedI = int(10.0 * ((float)contInsCPAP / 1000.0) / ((60.0 / (float)frecCalcCPAP) - ((float)contInsCPAP) / 1000.0));
            }
            if (calculatedE < 10)
            {
                calculatedE = 10;
            }
            else if (calculatedE > 90)
            {
                calculatedE = 90;
            }

            // limita el valor maximo de frecuencia a 35

            if (frecRespiratoriaCalculada > 35)
            {
                frecRespiratoriaCalculada = 35;
            }

            currentVE = (int)((VT * frecRespiratoriaCalculada) / 100.0); // calculo de la ventilacion minuto

            //Calculo de Peep
            PeepProximal = SPpac;
            PeepDistal = SPout;
            /* *******************************************************************
		  * *** Aqui se debe verificar cual es el valor de Peep a utlizar *****
		  * *******************************************************************/
            /* *******************************************************************
			 * *** Aqui se debe verificar cual es el valor de Peep a utlizar *****
			 * *******************************************************************/
            if ((PeepDistal - PeepProximal) > 2)
            {
                Peep = PeepDistal; // PIP como la presion en la via aerea al final de la espiracion
                flagAlarmObstruccion = true;
            }
            else
            {
                Peep = PeepProximal; // Peep como la presion en la via aerea al final de la espiracion
                flagAlarmObstruccion = false;
            }

            if (Peep < newPeepMax)
            {
                alerPeep = 1;
            }
            else
            {
                alerPeep = 0;
            }

            //Ajuste del valor de volumen
            VtidalV = 0;
            VtidalC = 0;
            // flowZero = SFin - SFout; // nivel cero de flujo para calculo de volumen
            contFrecCPAP = 0;
            contEspCPAP = 0;
            contInsCPAP = 0;

            //Asignacion de valores maximos y minimos de presion
            pmin = UmbralPpmin;       //asigna la presion minima encontrada en todo el periodo
            pmax = UmbralPpico;       //asigna la presion maxima encontrada en todo el periodo
            flmin = UmbralFmin;       //asigna el flujo minimo encontrada en todo el periodo
            flmax = UmbralFmax;       //asigna el flujo maximo encontrada en todo el periodo
            vmin = UmbralVmin;        //asigna el volumen minimo encontrada en todo el periodo
            vmax = UmbralVmax;        //asigna el volumen maximo encontrada en todo el periodo
            UmbralPpmin = 100;        //Reinicia el umbral minimo de presion del paciente
            UmbralPpico = -100;       //Reinicia el umbral maximo de presion del paciente
            UmbralPpicoDistal = -100; //Reinicia el umbral maximo de presion del paciente
            UmbralFmin = 100;         //Reinicia el umbral minimo de flujo del paciente
            UmbralFmax = -100;        //Reinicia el umbral maximo de flujo del paciente
            UmbralVmin = 100;         //Reinicia el umbral minimo de volumen del paciente
            UmbralVmax = -100;        //Reinicia el umbral maximo de volumen del paciente

            if (frecRespiratoriaCalculada > maxFR)
            {
                flagAlarmFR_Alta = true;
                alerFR_Alta = 1;
            }
            // else if (frecRespiratoriaCalculada < minFR)
            // {
            //     flagAlarmFR_Alta = true;
            //     alerFR_Alta = 2;
            // }
            else
            {
                flagAlarmFR_Alta = false;
                alerFR_Alta = 0;
            }
        }
        if ((SFpac < COMP_FLOW_MIN_CPAP) && ((dFlow) < COMP_DEL_F_MIN_CPAP) && (stateFrecCPAP != CPAP_ESPIRATION))
        { // si inicia la espiracion
            stateFrecCPAP = CPAP_ESPIRATION;

            //Calculo de PIP
            PpicoProximal = SPpac;
            PpicoDistal = SPout;
            /* *******************************************************************
			  * *** Aqui se debe verificar cual es el valor de Ppico a utlizar *****
			  * *******************************************************************/
            if ((PpicoProximal < 2) && (PpicoDistal - PpicoProximal - 10) > 4)
            {
                Ppico = PpicoDistal - 10; // PIP como la presion en la via aerea al final de la espiracion
                flagAlarmObstruccion = true;
            }
            else
            {
                Ppico = PpicoProximal; // PIP como la presion en la via aerea al final de la espiracion
                flagAlarmObstruccion = false;
            }

            //Medicion de Volumen circulante
            if (VtidalC >= 0)
            {
                VTidProm[2] = VtidalC;
            }
            else
            {
                VTidProm[2] = 0;
            }
            // promediado del Vtidal
            for (int i = 2; i >= 1; i--)
            {
                VTidProm[2 - i] = VTidProm[2 - i + 1];
            }
            //- Inicializacion
            SVtidal = 0;
            //- Actualizacion
            for (int i = 0; i <= 2; i++)
            {
                SVtidal = SVtidal + VTidProm[i];
                // frecRespiratoriaCalculada = frecRespiratoriaCalculada + FreqProm[i];
            }
            //- Calculo promedio
            VT = SVtidal / 3;
            // frecRespiratoriaCalculada = (int) (frecRespiratoriaCalculada / 3);
        }

        // Maquina de estados para identificar la Inspiracion y la espiracion
        switch (stateFrecCPAP)
        {
            // Inicio de CPAP
        case CPAP_INIT:
            frecRespiratoriaCalculada = 0;
            VT = 0;
            VtidalV = 0;
            VtidalC = 0;
            // calculatedE = 0;
            // calculatedI = 0;
            flagToACBackUp = 0;
            contFrecCPAP = 0;

            PeepProximal = SPpac;
            PeepDistal = SPout;
            /* *******************************************************************
		  * *** Aqui se debe verificar cual es el valor de Peep a utlizar *****
		  * *******************************************************************/
            Peep = PeepProximal; // Peep como la presion en la via aerea al final de la espiracion
            break;
            // Ciclo Inspiratorio
        case CPAP_INSPIRATION:
            contFrecCPAP++; // Se incrementan los contadore para el calculo de frecuencia y relacion IE
            contInsCPAP++;
            break;
            // Ciclo Espiratorio
        case CPAP_ESPIRATION:
            contFrecCPAP++;
            contEspCPAP++;
            break;
        default:
            break;
        }

        if ((contFrecCPAP > ((apneaTime * 1000) - 5000)) && (stateFrecCPAP != CPAP_INIT))
        {
            flagAlarmFR_Alta = true;
            alerFR_Alta = 2;
            frecRespiratoriaCalculada = 0;
        }
        if ((contFrecCPAP > (apneaTime * 1000) - 500) && (stateFrecCPAP != CPAP_INIT))
        {
            flagToACBackUp = 1;
        }
        if ((contFrecCPAP > apneaTime * 1000) && (stateFrecCPAP != CPAP_INIT))
        {
            newStateMachine = AC_STATE;
            flagToACBackUp = 0;
            contFrecCPAP = 0; // reinicio del contador de frecuencia en CPAP
            newVentilationMode = 1; // A/C Ventilation Mode
        }
    }
}

// Rutina de StandBy
void standbyRoutine(void)
{
    if (newStateMachine != currentStateMachine)
    {                                          // si hay un cambio de estado de la maquina
        currentStateMachine = newStateMachine; // actualiza el estado de funcionamiento de la maquina a StandBy
        PeepEstable = 0;
        contCycling = 0; // detiene el contador de ciclado
    }

    flagAlarmPatientDesconnection = false;
    flagAlarmObstruccion = false;
    flagAlarmFR_Alta = false;
    flagAlarmVE_Alto = false;
    alerDesconexion = 0;
    alerObstruccion = 0;
    alerFR_Alta = 0;
    alerVE_Alto = 0;
    alerPresionPIP = 0;
    alerPeep = 0;
    alerGeneral = 0;

    digitalWrite(EV_INSPIRA, LOW); //Piloto conectado a presion de bloqueo -> Libera valvula piloteada y permite el paso de aire
    digitalWrite(EV_ESC_CAM, LOW); //Piloto conectado a Camara -> Despresuriza la camara y permite el llenado de la bolsa
    digitalWrite(EV_ESPIRA, LOW);  //Piloto conectado a PEEP -> Limita la presion de la via aerea a la PEEP configurada
}

/** ****************************************************************************
 ** ************ END OF THE CODE ***********************************************
 ** ****************************************************************************/
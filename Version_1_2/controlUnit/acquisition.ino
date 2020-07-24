/*
 * File:   acquisition.ino
 * Author: GIBIC UdeA
 *
 * Created on July 4, 2020, 13:41 PM
 */

/** ****************************************************************************
 ** ************ INCLUDES ******************************************************
 ** ****************************************************************************/
#include "acquisition.h"

/** ****************************************************************************
 ** ************ DEFINES *******************************************************
 ** ****************************************************************************/

/** ****************************************************************************
 ** ************ EXTERN VARIABLES **********************************************
 ** ****************************************************************************/
// **********************************************************
// definicion de los core para ejecucion
extern uint8_t taskCoreZero;
extern uint8_t taskCoreOne;

// manejadores de las tareas
extern xTaskHandle serviceTaskHandle;

// manejadores para los semaforos binarios
extern SemaphoreHandle_t xSemaphoreRaspberry;
extern SemaphoreHandle_t xSemaphoreAdc;

// banderas de deteccion de alarmas
extern volatile uint8_t flagAlarmPpico;
extern volatile uint8_t flagAlarmPatientDesconnection;
extern uint8_t flagMaxPresion; // bandera de maxima presion alcanzada
extern uint8_t flagService;
extern uint8_t flagRestartTask;

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

// variables para inpresion en modo calibracion
// bandera de modo calibracion
extern uint8_t flagFlowPrintCalibration;
extern uint8_t flagPcamPrintCalibration;
extern uint8_t flagPbagPrintCalibration;
extern uint8_t flagPpacPrintCalibration;
extern uint8_t flagFlowSitePrintCalibration;
extern uint8_t flagPcamSitePrintCalibration;
extern uint8_t flagPbagSitePrintCalibration;
extern uint8_t flagPpacSitePrintCalibration;
extern volatile uint8_t flagAlarmObstruccion;

// variables de operacion de calibracion
extern uint8_t servMenuStateCurrent;
extern uint8_t servMenuStateNew;

// banderas de estados de ventilacion
extern uint8_t flagAC;

// Variables de ventilacion
extern float UmbralPpmin;
extern float UmbralPpico;
extern float UmbralPpicoDistal;
extern float UmbralFmin;
extern float UmbralFmax;
extern float UmbralVmin;
extern float UmbralVmax;

// variables de control de ciclado
extern int maxPresion;
extern unsigned int contCiclos;  // variable que controla el numero de ciclos que ha realizado un ventilador
extern volatile int contCycling; // variable que cuenta los ms del ciclado para cambiar estado de electrovalvulas
extern volatile float inspirationTime;
extern volatile float expirationTime;
extern int currentFrecRespiratoria;
extern int currentI;
extern int currentE;
extern float relI;
extern float relE;
extern byte AC_stateMachine;
extern int newTrigger;

// Variables maquina de estados de modos ventilatorios
extern int currentVentilationMode;
extern int newVentilationMode;
extern int currentStateMachine;

// Banderas de ejecucion de timer
extern volatile uint8_t flagAdcInterrupt;

// variable para el control de la interrupcion de timer
extern portMUX_TYPE timerMux;

// Variables para envio y recepcion de alarmas
extern int alerGeneral;

// variables para el filtrado
extern float PinADC[40];
extern float PoutADC[40];
extern float PpacADC[40];
extern float FinADC[40];
extern float FoutADC[40];
extern float FPacProm[40];
extern float VTidProm[3];
extern float FreqProm[3];

// variables de calculo de parametros ventilatorios
extern float Ppico;         // valor medido de Ppico
extern float PpicoProximal; // medicion realizada con sensor distal a paciente
extern float PpicoDistal;   // medicion realizada con sensor distal a paciente
extern float Peep;          // valor medido de Peep
extern float Peep_AC;       // medicion de Peep en el modo asistido controlado
extern float PeepProximal;  // medicion realizada con sensor distal a paciente
extern float PeepDistal;    // medicion realizada con sensor distal a paciente
extern float PcontDistal;   // Presion control distal
extern float PcontProximal; // Presion control PcontProximal
extern int ratioPcont;      // Ratio de comparacion de las presiones control

extern float SPinADC;  //Senal filtrada de presion en la camara
extern float SPoutADC; //Senal filtrada de presion en la bolsa
extern float SPpacADC; //Senal de presion en la via aerea del paciente
extern float SFinADC;  //Senal de flujo inspiratorio
extern float SFoutADC; //Senal de flujo espiratorio

extern float SPin;     // Senal filtrada de presion en la camara
extern float SPout;    // Senal filtrada de presion en la bolsa
extern float SPpac;    // Senal de presion en la via aerea del paciente
extern float SFin;     //Senal de flujo inspiratorio
extern float SFout;    //Senal de flujo espiratorio
extern float flowZero; // medicion del flujo cero para la grafica de volumen
extern float dPpac;    // Derivada de SPpac
extern int frecRespiratoriaCalculada;
extern float VT;
extern int calculatedE;
extern int calculatedI;
extern int currentVE;

extern float dPpac; // Derivada de SPpac
extern float SFpac; // Senal de flujo del paciente

// Mediciones derivadas de las senales del ventilador
extern float Pin_min;
extern float Pout_min;
extern float pmin;
extern float pmax;
extern float flmin;
extern float flmax;
extern float vmin;
extern float vmax;

// Variables para visualizacion
extern float VtidalV; // senal de volumen para visualizacion
extern float VtidalC; // senal de volumen para calculo
extern float SFpacV;  // senal de flujo para visualizacion

// variables para calculo de frecuencia y relacion IE en CPAP
extern float SFant;
extern float dFlow;

// Calibracion sensores fabrica
// Calibracion de los sensores de presion - coeficientes regresion lineal
extern volatile float AMP_CAM_1;
extern volatile float OFFS_CAM_1;
extern volatile float AMP_BAG_2;
extern volatile float OFFS_BAG_2;
extern volatile float AMP_PAC_3;
extern volatile float OFFS_PAC_3;

// Calibracion de los sensores de flujo - coeficientes regresion lineal
// Sensor de flujo Inspiratorio
extern volatile float AMP_FI_1;
extern volatile float OFFS_FI_1;
extern volatile float LIM_FI_1;
extern volatile float AMP_FI_2;
extern volatile float OFFS_FI_2;
extern volatile float LIM_FI_2;
extern volatile float AMP_FI_3;
extern volatile float OFFS_FI_3;

// Sensor de flujo Espiratorio
extern volatile float AMP_FE_1;
extern volatile float OFFS_FE_1;
extern volatile float LIM_FE_1;
extern volatile float AMP_FE_2;
extern volatile float OFFS_FE_2;
extern volatile float LIM_FE_2;
extern volatile float AMP_FE_3;
extern volatile float OFFS_FE_3;

// variable para ajustar el nivel cero de flujo y calcular el volumen
extern volatile float VOL_SCALE;      // Factor de escala para ajustar el volumen
extern volatile float VOL_SCALE_SITE; // Factor de escala para ajustar el volumen

// **********************************************************
// Calibracion sensores Sitio
// Calibracion de los sensores de presion - coeficientes regresion lineal
extern volatile float AMP_CAM_1_SITE;
extern volatile float OFFS_CAM_1_SITE;
extern volatile float AMP_BAG_2_SITE;
extern volatile float OFFS_BAG_2_SITE;
extern volatile float AMP_PAC_3_SITE;
extern volatile float OFFS_PAC_3_SITE;

// Calibracion de los sensores de flujo - coeficientes regresion lineal
// Sensor de flujo Inspiratorio
extern volatile float AMP_FI_1_SITE;
extern volatile float OFFS_FI_1_SITE;
extern volatile float LIM_FI_1_SITE;
extern volatile float AMP_FI_2_SITE;
extern volatile float OFFS_FI_2_SITE;
extern volatile float LIM_FI_2_SITE;
extern volatile float AMP_FI_3_SITE;
extern volatile float OFFS_FI_3_SITE;

// Sensor de flujo Espiratorio
extern volatile float AMP_FE_1_SITE;
extern volatile float OFFS_FE_1_SITE;
extern volatile float LIM_FE_1_SITE;
extern volatile float AMP_FE_2_SITE;
extern volatile float OFFS_FE_2_SITE;
extern volatile float LIM_FE_2_SITE;
extern volatile float AMP_FE_3_SITE;
extern volatile float OFFS_FE_3_SITE;

// Variables de maquinas de estado de ciclado
extern unsigned int currentStateMachineCycling;

// Variables y parametros de impresion en raspberry
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
// Variables de manejo de ADC
volatile int contADC = 0;
volatile int contADCfast = 0;
int ADC1_Value = 0;
int ADC2_Value = 0;
int ADC3_Value = 0;
int ADC4_Value = 0;
int ADC5_Value = 0;

// variables para calibracion de sensores
float SFinFACTORY = 0;  //Senal de flujo inspiratorio
float SFoutFACTORY = 0; //Senal de flujo espiratorio
float SPpacFACTORY = 0; //Senal de presion en la via aerea del paciente
float SPinFACTORY = 0;  //Senal filtrada de presion en la camara
float SPoutFACTORY = 0; //Senal filtrada de presion en la bolsa

float CalFin = 0;  // almacena valor ADC para calibracion
float CalFout = 0; // almacena valor ADC para calibracion
float CalPpac = 0; // almacena valor ADC para calibracion
float CalPin = 0;  // almacena valor ADC para calibracion
float CalPout = 0; // almacena valor ADC para calibracion

float flowTotalV = 0;
float flowTotalC = 0;

unsigned int contSendData = 0;

// variables para el control de la derivada
float SPpac0 = 0;
float SPpac1 = 0;
float SPpac2 = 0;
float SPpac3 = 0;
float SPpac4 = 0;

/** ****************************************************************************
 ** ************ FUNCTIONS *****************************************************
 ** ****************************************************************************/
/************************************************************
 ***** FUNCIONES DE ATENCION A INTERRUPCION TAREA ADC *******
 ************************************************************/
void task_Adc(void *arg)
{
    while (1)
    {
        // Se atiende la interrpcion del timer
        if (xSemaphoreTake(xSemaphoreAdc, portMAX_DELAY) == pdTRUE)
        {
            if (flagAdcInterrupt == true)
            {
                portENTER_CRITICAL(&timerMux);
                flagAdcInterrupt = false;
                portEXIT_CRITICAL(&timerMux);

                /* *************************************************************
				* **** SECUENCIA DE MEDICION, ADQUISICION DE VARIABLES *********
				* *************************************************************/
                contADC++;
                contADCfast++;
                // muestreo rapido de ADC
                if (contADCfast == ADC_FAST)
                {
                    contADCfast = 0;

                    // Lectura de valores ADC
                    ADC4_Value = analogRead(ADC_FLOW_1);  // ADC flujo de entrada
                    ADC5_Value = analogRead(ADC_FLOW_2);  // ADC flujo de salida
                    ADC1_Value = analogRead(ADC_PRESS_1); // ADC presion camara
                    ADC2_Value = analogRead(ADC_PRESS_2); // ADC presion equipo
                    ADC3_Value = analogRead(ADC_PRESS_3); // ADC presion de la via aerea

                    // Procesamiento senales
                    //- Almacenamiento
                    FinADC[39] = ADC4_Value;
                    FoutADC[39] = ADC5_Value;
                    PpacADC[39] = ADC3_Value;
                    PinADC[39] = ADC1_Value;
                    PoutADC[39] = ADC2_Value;

                    //- Corrimiento inicial
                    for (int i = 39; i >= 1; i--)
                    {
                        FinADC[39 - i] = FinADC[39 - i + 1];
                        FoutADC[39 - i] = FoutADC[39 - i + 1];
                        PinADC[39 - i] = PinADC[39 - i + 1];
                        PoutADC[39 - i] = PoutADC[39 - i + 1];
                        PpacADC[39 - i] = PpacADC[39 - i + 1];
                    }

                    //- Inicializacion
                    SFinADC = 0;
                    SFoutADC = 0;
                    SPinADC = 0;
                    SPoutADC = 0;
                    SPpacADC = 0;

                    //- Actualizacion
                    for (int i = 0; i <= 39; i++)
                    {
                        SFinADC = SFinADC + FinADC[i];
                        SFoutADC = SFoutADC + FoutADC[i];
                        SPinADC = SPinADC + PinADC[i];
                        SPoutADC = SPoutADC + PoutADC[i];
                        SPpacADC = SPpacADC + PpacADC[i];
                    }

                    //- Calculo promedio
                    SFinADC = SFinADC / 40;
                    SFoutADC = SFoutADC / 40;
                    SPinADC = SPinADC / 40;
                    SPoutADC = SPoutADC / 40;
                    SPpacADC = SPpacADC / 40;

                    // Actualizacion de valores para realizar calibracion
                    CalFin = SFinADC;
                    CalFout = SFoutADC;
                    CalPpac = SPpacADC;
                    CalPin = SPinADC;
                    CalPout = SPoutADC;

                    //- Conversion ADC-Presion de fabrica
                    SPinFACTORY = AMP_CAM_1 * float(SPinADC) + OFFS_CAM_1;
                    SPoutFACTORY = AMP_BAG_2 * float(SPoutADC) + OFFS_BAG_2;
                    SPpacFACTORY = AMP_PAC_3 * float(SPpacADC) + OFFS_PAC_3; // Presion de la via aerea

                    // Conversion ADC Flujo Inspiratorio de fabrica, ajuste por tramos para linealizacion
                    if (SFinADC <= LIM_FI_1)
                    {
                        SFinFACTORY = AMP_FI_1 * float(SFinADC) + OFFS_FI_1;
                    }
                    else if (SFinADC <= LIM_FI_2)
                    {
                        SFinFACTORY = AMP_FI_2 * float(SFinADC) + OFFS_FI_2;
                    }
                    else
                    {
                        SFinFACTORY = AMP_FI_3 * float(SFinADC) + OFFS_FI_3;
                    }

                    // Conversion ADC Flujo Espiratorio de fabrica, ajuste por tramos para linealizacion
                    if (SFoutADC <= LIM_FE_1)
                    {
                        SFoutFACTORY = AMP_FE_1 * float(SFoutADC) + OFFS_FE_1;
                    }
                    else if (SFoutADC <= LIM_FE_2)
                    {
                        SFoutFACTORY = AMP_FE_2 * float(SFoutADC) + OFFS_FE_2;
                    }
                    else
                    {
                        SFoutFACTORY = AMP_FE_3 * float(SFoutADC) + OFFS_FE_3;
                    }

                    // *********************************************************
                    // Conversion de valores de fabrica a valores de sitio
                    //- Conversion ADC-Presion de fabrica
                    SPin = AMP_CAM_1_SITE * float(SPinFACTORY) + OFFS_CAM_1_SITE;
                    SPout = AMP_BAG_2_SITE * float(SPoutFACTORY) + OFFS_BAG_2_SITE;
                    SPpac = AMP_PAC_3_SITE * float(SPpacFACTORY) + OFFS_PAC_3_SITE; // Presion de la via aerea

                    // Conversion ADC Flujo Inspiratorio de fabrica, ajuste por tramos para linealizacion
                    if (SFinADC <= LIM_FI_1_SITE)
                    {
                        SFin = AMP_FI_1_SITE * float(SFinFACTORY) + OFFS_FI_1_SITE;
                    }
                    else if (SFinADC <= LIM_FI_2_SITE)
                    {
                        SFin = AMP_FI_2_SITE * float(SFinFACTORY) + OFFS_FI_2_SITE;
                    }
                    else
                    {
                        SFin = AMP_FI_3_SITE * float(SFinFACTORY) + OFFS_FI_3_SITE;
                    }

                    // Conversion ADC Flujo Espiratorio de fabrica, ajuste por tramos para linealizacion
                    if (SFoutADC <= LIM_FE_1_SITE)
                    {
                        SFout = AMP_FE_1_SITE * float(SFoutFACTORY) + OFFS_FE_1_SITE;
                    }
                    else if (SFoutADC <= LIM_FE_2_SITE)
                    {
                        SFout = AMP_FE_2_SITE * float(SFoutFACTORY) + OFFS_FE_2_SITE;
                    }
                    else
                    {
                        SFout = AMP_FE_3_SITE * float(SFoutFACTORY) + OFFS_FE_3_SITE;
                    }

                    SFpac = SFin - SFout; // flujo del paciente

                    // filtro moving average para la senal de flujo
                    FPacProm[L_F_PROM - 1] = SFpac;
                    //- Corrimiento inicial
                    for (int i = L_F_PROM - 1; i >= 1; i--)
                    {
                        FPacProm[L_F_PROM - 1 - i] = FPacProm[L_F_PROM - 1 - i + 1];
                    }
                    //- Inicializacion
                    SFpacV = 0;
                    //- Actualizacion
                    for (int i = 0; i <= L_F_PROM - 1; i++)
                    {
                        SFpacV = SFpacV + FPacProm[i];
                    }
                    //- Calculo promedio
                    SFpacV = SFpacV / L_F_PROM;

                    // Calculo de volumen circulante
                    flowTotalV = SFpacV - flowZero;
                    flowTotalC = SFpac;
                    //SFant = SFpac;

                    // Calculo de volumen
                    if (alerGeneral == 0)
                    {
                        if ((flowTotalC <= FLOWLO_LIM) || (flowTotalC >= FLOWUP_LIM))
                        {
                            VtidalC = VtidalC + (flowTotalC * DELTA_T * FLOW_CONV * VOL_SCALE * VOL_SCALE_SITE);
                            if (VtidalC < 0)
                            {
                                VtidalC = 0;
                            }
                            if (VtidalC > 3000)
                            {
                                VtidalC = 3000;
                            }
                        }
                        if ((flowTotalV <= FLOWLO_LIM) || (flowTotalV >= FLOWUP_LIM))
                        {
                            VtidalV = VtidalV + (flowTotalV * DELTA_T * FLOW_CONV * VOL_SCALE * VOL_SCALE_SITE);
                            if (VtidalV < 0)
                            {
                                VtidalV = 0;
                            }
                            if (VtidalV > 3000)
                            {
                                VtidalV = 3000;
                            }
                        }
                    }
                    else
                    {
                        VtidalC = 0;
                        VtidalV = 0;
                    }
                }

                // muestreo lento de ADC
                if (contADC == ADC_SLOW)
                {
                    contADC = 0;
                    // Derivada SPpac
                    SPpac0 = SPpac1;
                    SPpac1 = SPpac2;
                    SPpac2 = SPpac3;
                    SPpac3 = SPpac4;
                    SPpac4 = SPpac;
                    dPpac = SPpac4 - SPpac0;
                    //Serial.println(String(SPpac) + ';' + String(10*dPpac));
                    if (currentStateMachineCycling == INSPIRATION_CYCLING)
                    {
                        if (SPpac > maxPresion && flagAlarmPpico == false)
                        {
                            flagAlarmPpico = true;
                            flagMaxPresion = true;
                            alerPresionPIP = 1;
                        }
                    }
                    else if ((newVentilationMode == 1) && (currentStateMachineCycling == EXPIRATION_CYCLING) &&
                             (contCycling >= int(inspirationTime * 1000 + expirationTime * 100)))
                    {
                        switch (AC_stateMachine)
                        {
                        case 0:
                            //Serial.println("Estado 0 AC");
                            if (dPpac > DERIVATE_DO_THRESHOLD && dPpac < DERIVATE_UP_THRESHOLD)
                            { // dP/dt
                                AC_stateMachine = 1;
                            }
                            break;
                        case 1:
                            //Calculo de Peep
                            if (dPpac > DERIVATE_DO_THRESHOLD && dPpac < DERIVATE_UP_THRESHOLD)
                            { // dP/dt
                                Peep_AC = SPpac1;
                                if (Peep_AC < 0)
                                {                // Si el valor de Peep es negativo
                                    Peep_AC = 0; // Lo limita a 0
                                }
                            }
                            if (dPpac < DERIVATE_LO_THRESHOLD)
                            {
                                AC_stateMachine = 2;
                            }
                            break;
                        case 2:
                            //Serial.println("Estado 2 AC");
                            if (SPpac4 < Peep_AC - newTrigger)
                            {
                                flagAC = true;
                                AC_stateMachine = 0;
                            }
                            else
                            {
                                AC_stateMachine = 1;
                            }
                        }
                    }
                    // Calculo de relaciones I:E
                    if (currentI != 1)
                    {
                        relI = (float)(currentI / 10.0);
                    }
                    else
                    {
                        relI = (float)(currentI);
                    }
                    if (currentE != 1)
                    {
                        relE = (float)(currentE / 10.0);
                    }
                    else
                    {
                        relE = (float)(currentE);
                    }

                    // // Calculo de volumen circulante
                    // flowTotalV = SFin - SFout - flowZero;
                    // flowTotalC = SFin - SFout;
                    dFlow = SFpac - SFant;
                    SFant = SFpac;

                    // Calculo Presiones maximas y minimas en la via aerea
                    if (UmbralPpmin > SPpac)
                    {
                        UmbralPpmin = SPpac;
                    }
                    if (UmbralPpico < SPpac)
                    {
                        UmbralPpico = SPpac;
                    }
                    // if (UmbralPpicoDistal < SPout)
                    // {
                    //     UmbralPpicoDistal = SPout;
                    // }

                    // Calculo Flujos maximos y minimos en la via aerea
                    if (UmbralFmin > SFpac)
                    {
                        UmbralFmin = SFpac;
                    }
                    if (UmbralFmax < SFpac)
                    {
                        UmbralFmax = SFpac;
                    }

                    // Calculo Volumenes maximos y minimos en la via aerea
                    if (UmbralVmin > VtidalV)
                    {
                        UmbralVmin = VtidalV;
                    }
                    if (UmbralVmax < VtidalV)
                    {
                        UmbralVmax = VtidalV;
                    }

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
                        currentFrecRespiratoria = 0;
                        relI = 0;
                        relE = 0;
                        VT = 0;
                    }

                    if (currentStateMachine == STANDBY_STATE)
                    {
                        SPpac = 0;
                        SFpac = 0;
                        VtidalV = 0;
                        VtidalC = 0;
                        //Ppico = 0;
                        //Peep = 0;
                        //PpicoProximal = 0;
                        //PeepProximal = 0;
                        //PpicoDistal = 0;
                        //PeepDistal = 0;
                        //currentFrecRespiratoria = 0;
                        //relI = 0;
                        //relE = 0;
                        VT = 0;
                    }

                    // activacion de secuencia para el envío por Raspberry
                    xSemaphoreGive(xSemaphoreRaspberry); // asignacion y liberacion de semaforos
                }
            }
        }
    }
    vTaskDelete(NULL);
}

/************************************************************
 ***** ENVÍO DE TRAMA DE DATOS HACIA LA RASPBERRY ***********
 ************************************************************/
void task_Raspberry(void *arg)
{
    while (1)
    {
        // Se atiende solicitud de envio a traves de serial 1 para raspberry
        if (xSemaphoreTake(xSemaphoreRaspberry, portMAX_DELAY) == pdTRUE)
        {
            // Almacenamiento de los datos para envio a la raspberry
            patientPress = String(SPpac, 1);
            //}
            patientFlow = String(SFpacV, 1);
            patientVolume = String(VtidalV, 1);
            pressPIP = String(int(Ppico));
            pressPEEP = String(int(Peep));
            // Frequency = String(currentFrecRespiratoria);
            frequency = String(int(frecRespiratoriaCalculada));
            // Seleccion de la velocidad de graficacion
            if (frecRespiratoriaCalculada < 6)
            {
                // xSpeed = String(int(6));
                xSpeed = String(int(12));
            }
            else if (frecRespiratoriaCalculada < 20)
            {
                // xSpeed = String(int(12));
                xSpeed = String(int(12));
            }
            else
            {
                // xSpeed = String(int(20));
                xSpeed = String(int(12));
            }
            // Envio de relacion I:E
            if (currentI == 1)
            {
                rInspir = String(int(relI));
            }
            else
            {
                rInspir = String(relI, 1);
            }
            if (currentE == 1)
            {
                rEspir = String(int(calculatedE / 10.0));
            }
            else
            {
                rEspir = String(calculatedE / 10.0, 1);
            }
            volumeT = String(int(VT));
            alertPip = String(alerPresionPIP);
            alertPeep = String(alerPeep);
            alertObstruction = String(alerObstruccion);
            alertConnPat = String(alerDesconexion);
            alertGeneralFailure = String(alerGeneral);
            alertConnEquipment = String(alerBateria);
            alertFrequency = String(int(alerFR_Alta));
            alertMinuteVentilation = String(int(alerVE_Alto));
            alertValve1Fail = String(0);
            alertValve2Fail = String(0);
            alertValve3Fail = String(0);
            valve1Temp = String(int(32));
            valve2Temp = String(int(33));
            valve3Temp = String(int(34));
            valve1Current = String(int(500));
            valve2Current = String(int(400));
            valve3Current = String(int(450));
            source5v0Voltage = String(5.0);
            source5v0Current = String(int(1500));
            source5v0SWVoltage = String(5.1);
            source5v0SWCurrent = String(int(800));
            cameraPress = String(SPin, 1);
            bagPress = String(SPout, 1);
            inspFlow = String(SFin, 1);
            EspFlow = String(SFout, 1);

            if (pmax < 10)
            {
                lPresSup = String(int(10));
            }
            else if (pmax < 25)
            {
                lPresSup = String(int(25));
            }
            else if (pmax < 35)
            {
                lPresSup = String(int(35));
            }
            else if (pmax < 45)
            {
                lPresSup = String(int(45));
            }
            else if (pmax < 60)
            {
                lPresSup = String(int(60));
            }
            else
            {
                lPresSup = String(int(100));
            }
            lPresInf = String(int(-5));
            // lPresInf = String(int(pmin));

            if (flmax < 15)
            {
                lFlowSup = String(int(15));
                lFlowInf = String(int(-15));
            }
            else if (flmax < 30)
            {
                lFlowSup = String(int(30));
                lFlowInf = String(int(-30));
            }
            else if (flmax < 45)
            {
                lFlowSup = String(int(45));
                lFlowInf = String(int(-45));
            }
            else if (flmax < 60)
            {
                lFlowSup = String(int(60));
                lFlowInf = String(int(-60));
            }
            else
            {
                lFlowSup = String(int(100));
                lFlowInf = String(int(-100));
            }

            // lFlowSup = String(int(flmax));
            // lFlowInf = String(int(flmin));

            if (vmax < 400)
            {
                lVoluSup = String(int(400));
            }
            else if (vmax < 600)
            {
                lVoluSup = String(int(600));
            }
            else if (vmax < 800)
            {
                lVoluSup = String(int(800));
            }
            else if (vmax < 1000)
            {
                lVoluSup = String(int(1000));
            }
            else if (vmax < 1200)
            {
                lVoluSup = String(int(1200));
            }
            else
            {
                lVoluSup = String(int(1600));
            }
            lVoluInf = String(int(-50));

            // lVoluSup = String(int(vmax));
            // lVoluInf = String(int(vmin));

            // alertValve4Fail = String(0);
            // alertValve5Fail = String(0);
            // alertValve6Fail = String(0);
            // alertValve7Fail = String(0);
            // alertValve8Fail = String(0);
            // valve4Temp = String(int(32));
            // valve5Temp = String(int(32));
            // valve6Temp = String(int(32));
            // valve7Temp = String(int(32));
            // valve8Temp = String(int(32));

            //- Composicion de cadena
            /* RaspberryChain = idEqupiment + ',' + patientPress + ',' + patientFlow + ',' + patientVolume + ',' +
							  pressPIP + ',' + pressPEEP + ',' + frequency + ',' + rInspir + ',' + rEspir + ',' + volumeT + ',' +
							  alertPip + ',' + alertPeep + ',' + alertObstruction + ',' + alertConnPat + ',' + alertGeneralFailure + ',' +
							  alertConnEquipment + ',' + alertValve1Fail + ',' + alertValve2Fail + ',' + alertValve3Fail + ',' +
							  alertValve4Fail + ',' + alertValve5Fail + ',' + alertValve6Fail + ',' + alertValve7Fail + ',' +
							  alertValve8Fail + ',' + valve1Temp + ',' + valve2Temp + ',' + valve3Temp + ',' + valve4Temp + ',' +
							  valve5Temp + ',' + valve6Temp + ',' + valve7Temp + ',' + valve8Temp + ',' + cameraPress + ',' + bagPress + ',' +
							  inspFlow + ',' + EspFlow; */
            RaspberryChain = idEqupiment + ',' + patientPress + ',' + patientFlow + ',' + patientVolume + ',' + pressPIP + ',' +
                             pressPEEP + ',' + frequency + ',' + xSpeed + ',' + rInspir + ',' + rEspir + ',' + volumeT + ',' +
                             alertPip + ',' + alertPeep + ',' + alertObstruction + ',' + alertConnPat + ',' + alertGeneralFailure + ',' +
                             alertConnEquipment + ',' + alertFrequency + ',' + alertMinuteVentilation + ',' + alertValve1Fail + ',' + alertValve2Fail + ',' +
                             alertValve3Fail + ',' + valve1Temp + ',' + valve2Temp + ',' + valve3Temp + ',' + valve1Current + ',' + valve2Current + ',' +
                             valve3Current + ',' + source5v0Voltage + ',' + source5v0Current + ',' + source5v0SWVoltage + ',' + source5v0SWCurrent + ',' +
                             cameraPress + ',' + bagPress + ',' + inspFlow + ',' + EspFlow + ',' + lPresSup + ',' + lPresInf + ',' + lFlowSup + ',' +
                             lFlowInf + ',' + lVoluSup + ',' + lVoluInf;

            // Envio de la cadena de datos (visualizacion Raspberry)
            if (flagService == false)
            {
                contSendData++;
                if (contSendData == 1)
                {
                    contSendData = 0;
                    Serial.println(RaspberryChain);

                    // Serial.print(PcontProximal);
                    // Serial.print(',');
                    // Serial.print(PcontDistal);
                    // Serial.print(',');
                    // Serial.print(ratioPcont);
                    // Serial.print(',');
                    // Serial.print(flagAlarmObstruccion);
                    // Serial.print(',');
                    // Serial.println(alerObstruccion);
                }
            }
            else
            {
                /* ********************************************************************
			  * **** ENVIO DE VARIABLES PARA CALIBRACION FABRICA *******************
			  * ********************************************************************/
                if (flagFlowPrintCalibration == true)
                {
                    Serial.print(CalFin);
                    Serial.print(",");
                    Serial.println(CalFout); // informacion para calibracion de flujo
                }
                if (flagPcamPrintCalibration == true)
                {
                    Serial.println(CalPin);
                }
                if (flagPbagPrintCalibration == true)
                {
                    Serial.println(CalPout);
                }
                if (flagPpacPrintCalibration == true)
                {
                    Serial.println(CalPpac);
                }

                /* ********************************************************************
			  * **** ENVIO DE VARIABLES PARA CALIBRACION SITIO *********************
			  * ********************************************************************/
                if (flagFlowSitePrintCalibration == true)
                {
                    Serial.print(SFinFACTORY);
                    Serial.print(",");
                    Serial.println(SFoutFACTORY); // informacion para calibracion de flujo
                }
                if (flagPcamSitePrintCalibration == true)
                {
                    Serial.println(SPinFACTORY);
                }
                if (flagPbagSitePrintCalibration == true)
                {
                    Serial.println(SPoutFACTORY);
                }
                if (flagPpacSitePrintCalibration == true)
                {
                    Serial.println(SPpacFACTORY);
                }
            }

            /* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++
             + ++++ ESTADO PARTA REINICIAR LA TAREA DE CALIBRACION ++++
             + +++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
            if (flagRestartTask == true)
            {
                flagService = true;
                servMenuStateNew = SERV_MAIN_MENU;
                servMenuStateCurrent = servMenuStateNew;
                flagRestartTask = false;

                xTaskCreatePinnedToCore(task_Service, "task_Service", 4096, NULL, 1, &serviceTaskHandle, taskCoreOne);
            }

            // Serial.print(dPpac);
            // Serial.print(',');
            // Serial.print(SPpac);
            // Serial.print(',');
            // Serial.print(Peep_AC);
            // Serial.print(',');
            // Serial.println(Peep);

            /* ********************************************************************
			  * **** ENVIO DE VARIABLES PARA CALIBRACION ***************************
			  * ********************************************************************/
            //  Serial.print(CalFin);
            //  Serial.print(",");
            //  Serial.println(CalFout);  // informacion para calibracion de flujo
            //  Serial.println(CalPpac);
            //  Serial.println(CalPin);
            //  Serial.println(CalPout); // informacion para calibracion de presion
        }
        vTaskDelay(20 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}

/** ****************************************************************************
 ** ************ END OF THE CODE ***********************************************
 ** ****************************************************************************/

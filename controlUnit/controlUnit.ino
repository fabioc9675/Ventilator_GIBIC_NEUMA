/*
 Name:		controlUnit.ino
 Created:	4/3/2020 17:28:49
 Author:    Helber Carvajal
*/

#include <Arduino.h>
#include <math.h>
#include <EEPROM.h>

#define VERSION_1_1       true           

#ifdef VERSION_1_0

// Definiciones para controlar el shiel DFRobot quad motor driver
// Definiciones para controlar el shiel DFRobot quad motor driver
#define EV_INSPIRA   13  // Valvula 3/2 de control de la via inspiratoria (pin 3 del shield, velocidad motor 1)
#define EV_ESC_CAM   14  // Valvula 3/2 de activacion de la camara (pin 6 del shield, velocidad motor 4)
#define EV_ESPIRA    12  // Valvula 3/2 de control de presiones PCON y PEEP (pin 11 del shield, velocidad motor 2)

// Definiciones para el manejo del ADC
#define ADC_PRESS_1     26  // Sensor de presion xx (pin ADC para presion 1)
#define ADC_PRESS_2     35  // Sensor de presion xx (pin ADC para presion 2)
#define ADC_PRESS_3     33  // Sensor de presion via aerea del paciente (pin ADC para presion 3)
#define ADC_FLOW_1      39  // Sensor de flujo linea xx (pin ADC para presion 2)
#define ADC_FLOW_2      36  // Sensor de flujo linea xx (pin ADC para presion 3)

#elif VERSION_1_1

// Definiciones para controlar el shiel DFRobot quad motor driver
// Definiciones para controlar el shiel DFRobot quad motor driver
#define EV_INSPIRA   5   // out 3 // Valvula 3/2 de control de la via inspiratoria (pin 3 del shield, velocidad motor 1)
#define EV_ESPIRA    4  // out 2 // Valvula 3/2 de control de presiones PCON y PEEP (pin 11 del shield, velocidad motor 2)
#define EV_ESC_CAM   18   // out 1 // Valvula 3/2 de activaci�n de la camara (pin 6 del shield, velocidad motor 4)

// Definiciones para el manejo del ADC
#define ADC_PRESS_1     33  // ADC 6 // Sensor de presion xx (pin ADC para presion 1)
#define ADC_PRESS_2     32  // ADC 5 // Sensor de presion xx (pin ADC para presion 2)
#define ADC_PRESS_3     34  // ADC 4 // Sensor de presion via aerea del paciente (pin ADC para presion 3)
#define ADC_FLOW_1      36  // ADC 1 // Sensor de flujo linea xx (pin ADC para presion 2)
#define ADC_FLOW_2      39  // ADC 2 // Sensor de flujo linea xx (pin ADC para presion 3)

#endif

// Calibracion de los sensores de presion - coeficientes regresion lineal
#define AMP1       0.0262
#define OFFS1      -15.092
#define AMP2       0.0293
#define OFFS2      -20.598
#define AMP3       0.0292
#define OFFS3      -21.429

// Calibracion de los sensores de flujo - coeficientes regresion lineal
// Sensor de flujo Inspiratorio
#define AMP_FI_1      0.116300
#define OFFS_FI_1     -211.888900
#define LIM_FI_1      1599
#define AMP_FI_2      0.610700
#define OFFS_FI_2     -1002.337700
#define LIM_FI_2      1684
#define AMP_FI_3      0.116300
#define OFFS_FI_3     -169.789300

// Sensor de flujo Espiratorio
#define AMP_FE_1      0.091800
#define OFFS_FE_1     -183.289800
#define LIM_FE_1      1714
#define AMP_FE_2      0.381000
#define OFFS_FE_2     -678.984800
#define LIM_FE_2      1850
#define AMP_FE_3      0.091800
#define OFFS_FE_3     -143.815400

// variable para ajustar el nivel cero de flujo y calcular el volumen
#define FLOWUP_LIM        3
#define FLOWLO_LIM        -3
#define FLOW_CONV         16.666666    // conversion de L/min a mL/second
#define DELTA_T           0.05         // delta de tiempo para realizar la integra
#define VOL_SCALE         0.85         // Factor de escala para ajustar el volumen

// Variables de control del protocolo
#define RXD2 16
#define TXD2 17

// Variables y parametros de impresion en raspberry
String SERIALEQU;
String patientPress;
String patientFlow;
String patientVolume;
String pressPIP;
String pressPEEP;
String frequency;
String rInspir;
String rEspir;
String volumeT;
String alertPip;
String alertPeep;
String alertDiffPress;
String alertConnPat;
String alertLeak;
String alertConnEquipment;
String alertValve1Fail;
String alertValve2Fail;
String alertValve3Fail;
String alertValve4Fail;
String alertValve5Fail;
String alertValve6Fail;
String alertValve7Fail;
String alertValve8Fail;
String valve1Temp;
String valve2Temp;
String valve3Temp;
String valve4Temp;
String valve5Temp;
String valve6Temp;
String valve7Temp;
String valve8Temp;
String cameraPress;
String bagPress;
String inspFlow;
String inspExp;

//Variables para el envio de las alarmas
int alerPresionPIP = 0;
int alerDesconexion = 0;
int alerObstruccion = 0;
int alerPeep = 0;
int alerBateria = 0;
int alerGeneral = 0;
int estabilidad = 0;
int PeepEstable = 0;

String RaspberryChain = "";

// Variables de manejo de ADC
volatile int contADC = 0;
volatile int contADCfast = 0;
bool fl_ADC = false;
int ADC1_Value = 0;
int ADC2_Value = 0;
int ADC3_Value = 0;
int ADC4_Value = 0;
int ADC5_Value = 0;
float Pressure1 = 0;
float Pressure2 = 0;
float Pressure3 = 0;
float flow1 = 0;
float flow2 = 0;
float flowZero = 0;
float flowTotal = 0;

// Variables para la atencion de interrupciones
bool flagTimerInterrupt = false;
volatile bool flagSwInterrupt = false;
volatile bool flagEncoderInterrupt_A = false;
volatile bool flagEncoderInterrupt_B = false;
volatile bool flagDettachInterrupt_A = false;
volatile bool flagDettachInterrupt_B = false;
volatile bool flagDetach = false;
bool flagAlarmPpico = false;
bool flagAlarmGeneral = false;
bool flagAlarmPatientDesconnection = false;
bool flagAlarmObstruccion = false;
volatile unsigned int contDetach = 0;
unsigned int contCiclos = 0;
unsigned int contEscrituraEEPROM = 0;
unsigned int contUpdateData = 0;
bool flagStandbyInterrupt = false;
unsigned int contStandby = 0;

// State machine
#define CHECK_STATE		0
#define STANDBY_STATE	1
#define PCMV_STATE		2
#define AC_STATE		3
#define CPAP_STATE		4
#define FAILURE_STATE	5
int currentStateMachine = STANDBY_STATE;
int newStateMachine = STANDBY_STATE;
int currentVentilationMode = 0;
int newVentilationMode = 0;
int newTrigger = 2;
int newPeepMax = 5;

#define STOP_CYCLING			0
#define START_CYCLING			1
#define INSPIRATION_CYCLING		2
#define EXPIRATION_CYCLING		3
unsigned int currentStateMachineCycling = START_CYCLING;
//***********************************
// datos para prueba de transmision
int pruebaDato = 0;
int second = 0;
int milisecond = 0;
// *********************************

// Variables de calculo
//- Mediciones
float Peep = 0;
float Ppico = 0;
float Vtidal = 0;
float VT = 0;
volatile float inspirationTime = 1.666;
volatile float expirationTime = 3.333;
float SUMVin_Ins = 0;
float SUMVout_Ins = 0;
float SUMVin_Esp = 0;
float SUMVout_Esp = 0;
float Vin_Ins = 0;
float Vout_Ins = 0;
float Vin_Esp = 0;
float Vout_Esp = 0;

// variables para calibracion de sensores
float CalFin = 0; // almacena valor ADC para calibracion
float CalFout = 0; // almacena valor ADC para calibracion
float CalPpac = 0; // almacena valor ADC para calibracion
float CalPin = 0; // almacena valor ADC para calibracion
float CalPout = 0; // almacena valor ADC para calibracion
//- Se�ales
float SFin = 0; //Se�al de flujo inspiratorio
float SFout = 0; //Se�al de flujo espiratorio
float SPpac = 0; //Se�al de presi�n en la via aerea del paciente
float SPin = 0; //Se�al filtrada de presion en la camara
float SPout = 0; //Se�al filtrada de presion en la bolsa

//- Filtrado
float Pin[40] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
float Pout[40] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
float Ppac[40] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
float Fin[40] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
float Fout[40] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

//- Mediciones derivadas
float UmbralPpmin = 100;
float UmbralPpico = -100;
float SFinMax = 50;
float SFoutMax = -50;
float SFinMaxInsp = 50;
float SFtotalMax = -50;
float Pin_max = 0;
float Pout_max = 0;
float Pin_min = 0;
float Pout_min = 0;
float pmin = 0;
float pmax = 0;
unsigned char BandInsp = 0;

unsigned char BandGeneral = 0;
unsigned char ContGeneral = 0;

// definiciones del timer
volatile int contCycling = 0;

bool flagInicio = true;

int currentFrecRespiratoria = 12;
int newFrecRespiratoria = currentFrecRespiratoria;
int currentI = 1;
int currentE = 20;
int newI = currentI;
int newE = currentE;
float relI = 0;
float relE = 0;
int maxPresion = 30;


// inicializacion del contador del timer
hw_timer_t* timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

void IRAM_ATTR onTimer();  // funcion de interrupcion
void receiveData();
void sendSerialData();
void alarmsDetection();

int eeprom_wr_int(int dataIn = 0, char process = 'r') {
    int dataRead = 0;
    if (process == 'w') {
        byte r1 = (dataIn & 0xff);
        EEPROM.write(0, r1);
        r1 = (dataIn & 0xff00) >> 8;
        EEPROM.write(1, r1);
        r1 = (dataIn & 0xff0000) >> 16;
        EEPROM.write(2, r1);
        r1 = (dataIn & 0xff000000) >> 24;
        EEPROM.write(3, r1);
        EEPROM.commit();
        return dataRead;
    }
    else if (process == 'r') {
        dataRead = EEPROM.read(0);
        dataRead = (EEPROM.read(1) << 8) + dataRead;
        dataRead = (EEPROM.read(2) << 16) + dataRead;
        dataRead = (EEPROM.read(3) << 24) + dataRead;
        return dataRead;
    }
    return dataRead;
}

// the setup function runs once when you press reset or power the board
void setup() {
    // Configuracion del timer a 1 kHz
    timer = timerBegin(0, 80, true);               // Frecuencia de reloj 80 MHz, prescaler de 80, frec 1 MHz
    timerAttachInterrupt(timer, &onTimer, true);
    timerAlarmWrite(timer, 1000, true);             // Interrupcion cada 1000 conteos del timer, es decir 100 Hz
    timerAlarmEnable(timer);                        // Habilita interrupcion por timer

    // Configuraci�n de los pines de conexion con del driver para manejo de electrovalvulas
    pinMode(2, OUTPUT);		// PIN 3   velocidad
    pinMode(4, OUTPUT);		// PIN 6   velocidad
    pinMode(5, OUTPUT);		// PIN 12  velocidad
    pinMode(12, OUTPUT);	// PIN 3   velocidad
    pinMode(13, OUTPUT);	// PIN 6   velocidad
    pinMode(14, OUTPUT);	// PIN 12  velocidad
    pinMode(15, OUTPUT);	// PIN 3   velocidad
    pinMode(18, OUTPUT);	// PIN 6   velocidad

    // inicializacion de los pines controladores de las EV como salidas
    pinMode(EV_INSPIRA, OUTPUT);	// PIN 3   velocidad
    pinMode(EV_ESC_CAM, OUTPUT);	// PIN 6   velocidad
    pinMode(EV_ESPIRA, OUTPUT);		// PIN 12  velocidad

    Serial.begin(115200);
    Serial2.begin(115200); // , SERIAL_8N1, RXD2, TXD2);
    Serial2.setTimeout(10);

    adcAttachPin(ADC_PRESS_1);
    adcAttachPin(ADC_PRESS_2);
    adcAttachPin(ADC_PRESS_3);
    adcAttachPin(ADC_FLOW_1);
    adcAttachPin(ADC_FLOW_2);

    EEPROM.begin(4);
    contCiclos = eeprom_wr_int();

    //Inicializacion de los strings comunicacion con la Raspberry
    SERIALEQU = String("1A");
    patientPress = String("");
    patientFlow = String("");
    patientVolume = String("");
    pressPIP = String("");
    pressPEEP = String("");
    frequency = String("");
    rInspir = String("");
    rEspir = String("");
    volumeT = String("");
    alertPip = String("");
    alertPeep = String("");
    alertDiffPress = String("");
    alertConnPat = String("");
    alertLeak = String("");
    alertConnEquipment = String("");
    alertValve1Fail = String("");
    alertValve2Fail = String("");
    alertValve3Fail = String("");
    alertValve4Fail = String("");
    alertValve5Fail = String("");
    alertValve6Fail = String("");
    alertValve7Fail = String("");
    alertValve8Fail = String("");
    valve1Temp = String("");
    valve2Temp = String("");
    valve3Temp = String("");
    valve4Temp = String("");
    valve5Temp = String("");
    valve6Temp = String("");
    valve7Temp = String("");
    valve8Temp = String("");
    cameraPress = String("");
    bagPress = String("");
    inspFlow = String("");
    inspExp = String("");

    RaspberryChain = String("");

    digitalWrite(2, LOW);	// PIN 3   velocidad
    digitalWrite(4, LOW);	// PIN 6   velocidad
    digitalWrite(5, LOW);   // PIN 12  velocidad
    digitalWrite(12, LOW);  // PIN 3   velocidad
    digitalWrite(13, LOW);  // PIN 6   velocidad
    digitalWrite(14, LOW);  // PIN 12  velocidad
    digitalWrite(15, LOW);  // PIN 3   velocidad
    digitalWrite(18, LOW);  // PIN 6   velocidad

    // Clean Serial buffers
    delay(1000);
    Serial.flush();
    Serial2.flush();
}

// the loop function runs over and over again until power down or reset
void loop() {
    // *************************************************
    // **** Atencion a rutina de interrupcion por timer
    // *************************************************
    if (flagTimerInterrupt) {
        portENTER_CRITICAL(&timerMux);
        flagTimerInterrupt = false;
        portEXIT_CRITICAL(&timerMux);

        receiveData();

        switch (currentStateMachine) {
        case CHECK_STATE:
            break;
        case STANDBY_STATE:
            standbyRoutine();
            adcReading();
            //Update data on LCD each 200ms
            contUpdateData++;
            if (contUpdateData >= 200) {
                contUpdateData = 0;
                sendSerialData();
            }
            //Serial.println("Standby state on control Unit");
            break;
        case PCMV_STATE:
            //Serial.println("I am on PCMV_STATE");
            cycling();
            adcReading();
            //Update data on LCD each 200ms
            contUpdateData++;
            if (contUpdateData >= 200) {
                contUpdateData = 0;
                sendSerialData();
            }
            //Write the EEPROM each 10 minuts
            contEscrituraEEPROM++;
            if (contEscrituraEEPROM > 3600000) {
                contEscrituraEEPROM = 0;
                eeprom_wr_int(contCiclos, 'w');
            }
            break;
        case AC_STATE:
            acRoutine();
            adcReading();

            //Update data on LCD each 200ms
            contUpdateData++;
            if (contUpdateData >= 200) {
                contUpdateData = 0;
                sendSerialData();
            }
            //Write the EEPROM each 10 minuts
            contEscrituraEEPROM++;
            if (contEscrituraEEPROM > 3600000) {
                contEscrituraEEPROM = 0;
                eeprom_wr_int(contCiclos, 'w');
            }
        case CPAP_STATE:
            standbyRoutine();
            adcReading();
            //Update data on LCD each 200ms
            contUpdateData++;
            if (contUpdateData >= 200) {
                contUpdateData = 0;
                sendSerialData();
            }
            break;
        case FAILURE_STATE:
            currentStateMachine = STANDBY_STATE;
            sendSerialData();
            break;
        default:
            currentStateMachine = STANDBY_STATE;
            sendSerialData();
            break;
        }
    }
}

void IRAM_ATTR onTimer() {
    portENTER_CRITICAL_ISR(&timerMux);
    flagTimerInterrupt = true;
    portEXIT_CRITICAL_ISR(&timerMux);
}

// Cycling of the Mechanical Ventilator
void cycling() {
    contCycling++;

    if ((newFrecRespiratoria != currentFrecRespiratoria) ||
        (newI != currentI) || (newE != !currentE)) {
        currentFrecRespiratoria = newFrecRespiratoria;
        currentI = newI;
        currentE = newE;
        // Calculo del tiempo I:E
        if (currentI == 1) {
            inspirationTime = (float)(60.0 / currentFrecRespiratoria) / (1 + (float)(currentE / 10.0));
            expirationTime = (float)(currentE / 10.0) * inspirationTime;
        }
        else {
            expirationTime = (float)(60.0 / currentFrecRespiratoria) / (1 + (float)(currentI / 10.0));
            inspirationTime = (float)(currentI / 10.0) * expirationTime;
        }
    }

    switch (currentStateMachineCycling) {
    case STOP_CYCLING:
        break;
    case START_CYCLING:
        if (contCycling >= 1) {        // Inicia el ciclado abriendo electrovalvula de entrada y cerrando electrovalvula de salida
            BandInsp = 1;// Activa bandera que indica que empezo la inspiraci�n
            digitalWrite(EV_INSPIRA, LOW);//Piloto conectado a ambiente -> Desbloquea valvula piloteada y permite el paso de aire
            digitalWrite(EV_ESPIRA, HIGH);//Piloto conectado a PIP -> Limita la presi�n de la via aerea a la PIP configurada
            digitalWrite(EV_ESC_CAM, HIGH);//Piloto conectado a Presi�n de activaci�n -> Presiona la camara
            currentStateMachineCycling = INSPIRATION_CYCLING;
        }//- Mitad de la inspiraci�n
        else if (contCycling == int(((inspirationTime) * 1000) / 2)) {
            SFinMax = SFin;//
            SFoutMax = SFout;//
        }
        break;
    case INSPIRATION_CYCLING:
        if (contCycling >= int(inspirationTime * 1000)) {
            //Calculo PIP
            //Ppico = SPpac;//Deteccion de Ppico como la presi�n al final de la inspiraci�n
            //Ppico = -0.0079 * (Ppico * Ppico) + 1.6493 * Ppico - 33.664;//Ajuste de Ppico
            if (Ppico < 0) {// Si el valor de Ppico es negativo
                Ppico = 0;// Lo limita a 0
            }
            Ppico = int(Ppico);

            //Mediciones de presion del sistema
            Pin_max = SPin;//Presi�n maxima de la camara
            Pout_max = SPout;//Presi�n maxima de la bolsa

            //Medicion de Volumen circulante
            if (Vtidal >= 0) {
                VT = Vtidal;
            }
            else {
                VT = 0;
            }

            //Mediciones de flujo
            SFinMaxInsp = SFin;
            SFtotalMax = SFin - SFout;

            flowZero = SFin - SFout; // nivel cero de flujo para calculo de volumen
            //Rutina de ciclado
            BandInsp = 0;	// Desactiva la bandera, indicando que empezo la espiraci�n
            digitalWrite(EV_INSPIRA, HIGH);//Piloto conectado a presi�n de bloqueo -> Bloquea valvula piloteada y restringe el paso de aire
            digitalWrite(EV_ESC_CAM, LOW);//Piloto conectado a PEEP -> Limita la presion de la via aerea a la PEEP configurada
            digitalWrite(EV_ESPIRA, LOW);//Piloto conectado a ambiente -> Despresuriza la camara y permite el llenado de la bolsa
            currentStateMachineCycling = EXPIRATION_CYCLING;
        }
        break;
    case EXPIRATION_CYCLING:
        //Add para el modo A/C
        if ((newVentilationMode == 1) && (SPpac <= Peep - newTrigger) && (contCycling >= int(inspirationTime)*1000 + (expirationTime * 100))) {
            contCycling = 0;
            
            if (Peep < 0) {// Si el valor de Peep es negativo
                Peep = 0;// Lo limita a 0
            }
            Peep = int(Peep);
            if (estabilidad) {
                PeepEstable = Peep;
                estabilidad = 0;
            }
            else {
                if (Peep <= PeepEstable - 1.5) {
                    alerPeep = 1;
                }
                else {
                    alerPeep = 0;
                }
            }
            //Ajuste del valor de volumen
            Vtidal = 0;
            flowZero = SFin - SFout; // nivel cero de flujo para calculo de volumen

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
            Pin_min = SPin;//Presi�n minima de la camara
            Pout_min = SPout;//Presi�n minima de la bolsa

            //Asignaci�n de valores maximos y minimos de presi�n
            pmin = UmbralPpmin;//asigna la presion minima encontrada en todo el periodo
            pmax = UmbralPpico;//asigna la presion maxima encontrada en todo el periodo
            Ppico = pmax;
            UmbralPpmin = 100;//Reinicia el umbral minimo de presion del paciente
            UmbralPpico = -100;//Reinicia el umbral maximo de presion del paciente

            //Metodo de exclusi�n de alarmas
            if (Ppico > 2 && Peep > 2) {
                flagInicio = false;
            }

            alarmsDetection();
            currentStateMachineCycling = START_CYCLING;
            
        }
        if ((newVentilationMode == 0) && (contCycling >= int(((inspirationTime + expirationTime) * 1000)))) {
            contCycling = 0;
            //Calculo de Peep
            Peep = SPpac;// Peep como la presion en la via aerea al final de la espiracion

            if (Peep < 0) {// Si el valor de Peep es negativo
                Peep = 0;// Lo limita a 0
            }
            Peep = int(Peep);
            if (estabilidad) {
                PeepEstable = Peep;
                estabilidad = 0;
            }
            else {
                if (Peep <= PeepEstable - 1.5) {
                    alerPeep = 1;
                }
                else {
                    alerPeep = 0;
                }
            }
            //Ajuste del valor de volumen
            Vtidal = 0;
            flowZero = SFin - SFout; // nivel cero de flujo para calculo de volumen

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
            Pin_min = SPin;//Presi�n minima de la camara
            Pout_min = SPout;//Presi�n minima de la bolsa

            //Asignaci�n de valores maximos y minimos de presi�n
            pmin = UmbralPpmin;//asigna la presion minima encontrada en todo el periodo
            pmax = UmbralPpico;//asigna la presion maxima encontrada en todo el periodo
            Ppico = pmax;
            UmbralPpmin = 100;//Reinicia el umbral minimo de presion del paciente
            UmbralPpico = -100;//Reinicia el umbral maximo de presion del paciente

            //Metodo de exclusi�n de alarmas
            if (Ppico > 2 && Peep > 2) {
                flagInicio = false;
            }

            alarmsDetection();
            currentStateMachineCycling = START_CYCLING;

            if (newStateMachine != currentStateMachine) {
                currentStateMachine = newStateMachine;
            }
        }
        break;
    default:
        break;
    }

    //Calculo de Volumenes en tiempo inspiratorio y espiratorio
    if (BandInsp == 1) {//Durante el tiempo inspiratorio
        SUMVin_Ins = SUMVin_Ins + SFin;
        SUMVout_Ins = SUMVout_Ins + SFout;
    }
    else {//Durante el tiempo espiratorio
        SUMVin_Esp = SUMVin_Esp + SFin;
        SUMVout_Esp = SUMVin_Esp + SFout;
    }

    milisecond++;
    if (milisecond == 1000) {
        milisecond = 0;
        second++;
        if (second == 60) {
            second = 0;
        }
    }
} // end cycling()

void cpapRoutine() {
    if (newStateMachine != currentStateMachine) {
        currentStateMachine = newStateMachine;
    }
    contCycling = 0;
    digitalWrite(EV_INSPIRA, LOW);  //Piloto conectado a presion de bloqueo -> Bloquea valvula piloteada y restringe el paso de aire
    digitalWrite(EV_ESC_CAM, LOW);  //Piloto conectado a PEEP -> Limita la presion de la via aerea a la PEEP configurada
    digitalWrite(EV_ESPIRA, LOW);   //Piloto conectado a ambiente -> Despresuriza la camara y permite el llenado de la bolsa
}

void acRoutine() {

}

void standbyRoutine() {
    if (newStateMachine != currentStateMachine) {
        currentStateMachine = newStateMachine;
    }
    contCycling = 0;
    digitalWrite(EV_INSPIRA, LOW);  //Piloto conectado a presion de bloqueo -> Bloquea valvula piloteada y restringe el paso de aire
    digitalWrite(EV_ESC_CAM, LOW);  //Piloto conectado a PEEP -> Limita la presion de la via aerea a la PEEP configurada
    digitalWrite(EV_ESPIRA, LOW);   //Piloto conectado a ambiente -> Despresuriza la camara y permite el llenado de la bolsa
}

// Function to receive data from serial communication
void receiveData() {
    if (Serial2.available() > 5) {
        String dataIn = Serial2.readStringUntil(';');
        //Serial.println(dataIn);
        int contComas = 0;
        for (int i = 0; i < dataIn.length(); i++) {
            if (dataIn[i] == ',') {
                contComas++;
            }
        }
        String dataIn2[40];
        for (int i = 0; i < contComas + 1; i++) {
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
        Serial2.flush();

        /*Serial.println("State = " + String(currentStateMachine));
          Serial.println(String(newFrecRespiratoria) + ',' + String(newI) + ',' +
            String(newE) + ',' + String(maxPresion) + ',' +
            String(alerBateria) + ',' + String(estabilidad) + ',' +
            String(newStateMachine) + ',' + String(newVentilationMode));*/
            //Serial.println(String(alerPeep));
            /*for (int i = 0; i < contComas + 1; i++) {
                Serial.println(dataIn2[i]);
              }*/

              // Calculo del tiempo I:E
        if (newI == 1) {
            inspirationTime = (float)(60.0 / (float)newFrecRespiratoria) / (1 + (float)((float)newE / 10.0));
            expirationTime = (float)((float)newE / 10.0) * (float)inspirationTime;
            //Serial.println("IC=I = " + String(I) + ":" + String(E) + "-" + String(inspirationTime) + " E = " + String(expirationTime));
        }
        else {
            expirationTime = (float)(60.0 / (float)newFrecRespiratoria) / (1 + (float)((float)newI / 10.0));
            inspirationTime = (float)((float)newI / 10.0) * (float)expirationTime;
            //Serial.println("EC=I = " + String(I) + ":" + String(E) + "-" + String(inspirationTime) + " E = " + String(expirationTime));
        }
    }
}

// Read ADC BATTALARM (each 50ms)
void adcReading() {
    contADC++;
    contADCfast++;
    if (contADCfast == 3) {
        contADCfast = 0;

        // Lectura de valores ADC
        ADC4_Value = analogRead(ADC_FLOW_1);
        ADC5_Value = analogRead(ADC_FLOW_2);
        ADC1_Value = analogRead(ADC_PRESS_1);
        ADC2_Value = analogRead(ADC_PRESS_2);
        ADC3_Value = analogRead(ADC_PRESS_3);// ADC presion de la via aerea

        // Procesamiento senales
        //- Almacenamiento
        Fin[39] = ADC4_Value;
        Fout[39] = ADC5_Value;
        Ppac[39] = ADC3_Value;
        Pin[39] = ADC1_Value;
        Pout[39] = ADC2_Value;

        //- Corrimiento inicial
        for (int i = 39; i >= 1; i--) {
            Fin[39 - i] = Fin[39 - i + 1];
            Fout[39 - i] = Fout[39 - i + 1];
            Pin[39 - i] = Pin[39 - i + 1];
            Pout[39 - i] = Pout[39 - i + 1];
            Ppac[39 - i] = Ppac[39 - i + 1];
        }

        //- Inicializaci�n
        SFin = 0;
        SFout = 0;
        SPin = 0;
        SPout = 0;
        SPpac = 0;

        //- Actualizacion
        for (int i = 0; i <= 39; i++) {
            SFin = SFin + Fin[i];
            SFout = SFout + Fout[i];
            SPin = SPin + Pin[i];
            SPout = SPout + Pout[i];
            SPpac = SPpac + Ppac[i];
        }

        //- Calculo promedio
        SFin = SFin / 40;
        SFout = SFout / 40;
        SPin = SPin / 40;
        SPout = SPout / 40;
        SPpac = SPpac / 40;

        // Actualizacion de valores para realizar calibracion
        CalFin = SFin;
        CalFout = SFout;
        CalPpac = SPpac;
        CalPin = SPin;
        CalPout = SPout;
        //- Conversi�n ADC-Presi�n
        SPin = AMP1 * float(SPin) + OFFS1;
        SPout = AMP2 * float(SPout) + OFFS2;
        SPpac = AMP3 * float(SPpac) + OFFS3;// Presi�n de la via aerea

        // Conversion ADC Flujo Inspiratorio, ajuste por tramos para linealizacion
        if (SFin <= LIM_FI_1) {
            SFin = AMP_FI_1 * float(SFin) + OFFS_FI_1;
        }
        else if (SFin <= LIM_FI_2) {
            SFin = AMP_FI_2 * float(SFin) + OFFS_FI_2;
        }
        else {
            SFin = AMP_FI_3 * float(SFin) + OFFS_FI_3;
        }

        // Conversion ADC Flujo Espiratorio, ajuste por tramos para linealizacion
        if (SFout <= LIM_FE_1) {
            SFout = AMP_FE_1 * float(SFout) + OFFS_FE_1;
        }
        else if (SFout <= LIM_FE_2) {
            SFout = AMP_FE_2 * float(SFout) + OFFS_FE_2;
        }
        else {
            SFout = AMP_FE_3 * float(SFout) + OFFS_FE_3;
        }

    }


    if (contADC == 50) {
        contADC = 0;

        // Calculo Presiones maximas y minimas en la via aerea
        if (UmbralPpmin > SPpac) {
            UmbralPpmin = SPpac;
        }
        if (UmbralPpico < SPpac) {
            UmbralPpico = SPpac;
        }

        // Calculo de relaciones I:E
        if (currentI != 1) {
            relI = (float)(currentI / 10.0);
        }
        else {
            relI = (float)(currentI);
        }
        if (currentE != 1) {
            relE = (float)(currentE / 10.0);
        }
        else {
            relE = (float)(currentE);
        }

        // Calculo de volumen circulante
        flowTotal = SFin - SFout - flowZero;
        if (alerGeneral == 0) {
            if ((flowTotal <= FLOWLO_LIM) || (flowTotal >= FLOWUP_LIM)) {
                Vtidal = Vtidal + (flowTotal * DELTA_T * FLOW_CONV * VOL_SCALE);

                if (Vtidal < 0) {
                    Vtidal = 0;
                }
            }
        }
        else {
            Vtidal = 0;
        }


        // Transmic�n serial
        //- Asignaci�n de variables
        //if (alerDesconexion == 1) {
        //	patientPress = String(0);
        //}
        //else {
        patientPress = String(SPpac);
        //}
        patientFlow = String(SFin - SFout - 10);
        patientVolume = String(Vtidal);
        pressPIP = String(Ppico, 0);
        pressPEEP = String(Peep, 0);
        frequency = String(currentFrecRespiratoria);
        if (currentI == 1) {
            rInspir = String(relI, 0);
        }
        else {
            rInspir = String(relI, 1);
        }
        if (currentE == 1) {
            rEspir = String(relE, 0);
        }
        else {
            rEspir = String(relE, 1);
        }

        volumeT = String(VT, 0);
        alertPip = String(alerPresionPIP);
        alertPeep = String(alerPeep);
        alertDiffPress = String(alerObstruccion);
        alertConnPat = String(alerDesconexion);
        alertLeak = String(alerGeneral);
        alertConnEquipment = String(alerBateria);
        //alertConnEquipment = String(0);
        alertValve1Fail = String(0);
        alertValve2Fail = String(0);
        alertValve3Fail = String(0);
        alertValve4Fail = String(0);
        alertValve5Fail = String(0);
        alertValve6Fail = String(0);
        alertValve7Fail = String(0);
        alertValve8Fail = String(0);
        valve1Temp = String(25.1);
        valve2Temp = String(25.2);
        valve3Temp = String(25.3);
        valve4Temp = String(25.4);
        valve5Temp = String(24.1);
        valve6Temp = String(24.2);
        valve7Temp = String(24.3);
        valve8Temp = String(24.4);
        cameraPress = String(SPin);
        bagPress = String(SPout);
        inspFlow = String(SFin);
        inspExp = String(SFout);

        //- Composicion de cadena
        RaspberryChain = SERIALEQU + ',' + patientPress + ',' + patientFlow + ',' + patientVolume + ',' +
            pressPIP + ',' + pressPEEP + ',' + frequency + ',' + rInspir + ',' + rEspir + ',' + volumeT + ',' +
            alertPip + ',' + alertPeep + ',' + alertDiffPress + ',' + alertConnPat + ',' + alertLeak + ',' +
            alertConnEquipment + ',' + alertValve1Fail + ',' + alertValve2Fail + ',' + alertValve3Fail + ',' +
            alertValve4Fail + ',' + alertValve5Fail + ',' + alertValve6Fail + ',' + alertValve7Fail + ',' +
            alertValve8Fail + ',' + valve1Temp + ',' + valve2Temp + ',' + valve3Temp + ',' + valve4Temp + ',' +
            valve5Temp + ',' + valve6Temp + ',' + valve7Temp + ',' + valve8Temp + ',' + cameraPress + ',' + bagPress + ',' +
            inspFlow + ',' + inspExp;

        // Envio de la cadena de datos (visualizacion Raspberry)
        Serial.println(RaspberryChain);

        /* ********************************************************************
         * **** ENVIO DE VARIABLES PARA CALIBRACION ***************************
         * ********************************************************************/
         //        Serial.print(CalFin);
         //        Serial.print(",");
         //        Serial.println(CalFout);  // informacion para calibracion de flujo
             //    Serial.println(CalPpac);
             //    Serial.println(CalPin);
             //    Serial.println(CalPout); // informacion para calibracion de presion
         //    Serial.print(SFin);
         //    Serial.print(",");
         //    Serial.print(SFout);
         //    Serial.print(",");
         //    Serial.print(Vtidal);
         //    Serial.print(",");
         //    Serial.println(SFin-SFout);


        // Envio de la cadena de datos
        //    Serial.print(", Ppac = ");
        //    Serial.print(SPpac);
        //    Serial.print(", SUMVin_Ins = ");
        //    Serial.print(SUMVin_Ins);
        //    Serial.print(", SUMVout_Ins = ");
        //    Serial.print(SUMVout_Ins);
        //    Serial.print(", Vin_Ins = ");
        //    Serial.print(Vin_Ins);
        //    Serial.print(", Vout_Ins = ");
        //    Serial.print(Vout_Ins);
        //    Serial.print(", Vin_Esp = ");
        //    Serial.print(Vin_Esp);
        //    Serial.print(", Vout_Esp = ");
        //    Serial.println(Vout_Esp);
        //        Serial.print("Fin = ");
        //    Serial.print(SFin);
        //    Serial.print(", Fout = ");
        //    Serial.print(SFout);
        //    Serial.print("F = ");
        //    Serial.print(SFin-SFout-10);
        //    Serial.print(", V = ");
        //    Serial.println(Vtidal);
        //Serial.print(", Ppac = ");
        //Serial.print(SPpac);
        //Serial.print(", Pin = ");
        //Serial.println(SPin);

    //    Serial.print(", Vin_Ins = ");
    //    Serial.print(Vin_Ins);
    //    Serial.print(", Vout_Esp = ");
    //    Serial.print(Vout_Esp);
    //    Serial.print(", Vin_Esp = ");
    //    Serial.print(Vin_Esp);
    //    Serial.print(", Vout_Ins = ");
    //    Serial.print(Vout_Ins);
    //    Serial.print(", Vt = ");
    //    Serial.print(Vtidal);
    //    Serial.print(", Vt2 = ");
    //    Serial.println(Vin_Ins+Vout_Esp);




        //    Serial.print(", Pin = ");
        //    Serial.print(SPin);
        //    Serial.print(", Pin_max = ");
        //    Serial.println(Pin_max);
    }
}

void sendSerialData() {
    String dataToSend = String(Ppico) + ',' + String(Peep) + ',' + String(VT) + ',' +
        String(alerPresionPIP) + ',' + String(alerDesconexion) + ',' +
        String(alerObstruccion) + ',' + String(alerPeep) + ',' + String(alerGeneral) + ';';
    Serial2.print(dataToSend);
}

void alarmsDetection() {
    // Ppico Alarm
    if (flagInicio == false) {//Si hay habilitaci�n de alarmas
      // Alarma por Ppico elevada
        if (Ppico > maxPresion) {
            flagAlarmPpico = true;
            alerPresionPIP = 1;
        }
        else {
            flagAlarmPpico = false;
            alerPresionPIP = 0;
        }
        // Fallo general
        if ((Pin_max) < 10) {
            flagAlarmGeneral = true;
            alerGeneral = 1;
            newStateMachine = 1;
        }
        else {
            flagAlarmGeneral = false;
            alerGeneral = 0;
        }

        // Alarma por desconexi�n del paciente
        if ((Ppico) < 2) {
            flagAlarmPatientDesconnection = true;
            alerDesconexion = 1;
        }
        else {
            flagAlarmPatientDesconnection = false;
            alerDesconexion = 0;
        }

        // Alarma por obstrucci�n
        if ((Vout_Ins >= 0.5 * Vin_Ins) && (Peep < 1)) {
            flagAlarmObstruccion = true;
            alerObstruccion = 1;
        }
        else {
            flagAlarmObstruccion = false;
            alerObstruccion = 0;
        }

        SFinMax = SFin;
        SFoutMax = SFout;
    }
}


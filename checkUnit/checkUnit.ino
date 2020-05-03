/*
  Name:		controlUnit.ino
  Created:	29/04/2020
*/
#include <math.h>
#include <EEPROM.h>
//************************************************ DEFINICIONES ************************************************//
// Definiciones para controlar el shiel DFRobot quad motor driver
#define EV_INSPIRA   13  // Valvula 3/2 de control de la via inspiratoria (pin 3 del shield, velocidad motor 1)
#define EV_ESC_CAM   14  // Valvula 3/2 de activación de la camara (pin 6 del shield, velocidad motor 4)
#define EV_ESPIRA    12  // Valvula 3/2 de control de presiones PCON y PEEP (pin 11 del shield, velocidad motor 2)

// Definiciones para el manejo del ADC
#define ADC_PRESS_1     33  // Sensor de presión xx (pin ADC para presion 1)
#define ADC_PRESS_2     32  // Sensor de presión xx (pin ADC para presion 2)
#define ADC_PRESS_3     35  // Sensor de presión via aerea del paciente (pin ADC para presion 3)
#define ADC_FLOW_1      36  // Sensor de flujo linea xx (pin ADC para presion 2)
#define ADC_FLOW_2      39  // Sensor de flujo linea xx (pin ADC para presion 3)

// Calibracion de los sensores de presion - coeficientes regresion lineal
#define AMP1       0.0299
#define OFFS1      -21.582
#define AMP2       0.0299
#define OFFS2      -21.582
#define AMP3       0.0299
#define OFFS3      -21.582

// Variables de control del protocolo de comunicación
#define RXD2 16
#define TXD2 17

//**************************************************************************************************************//

//************************************************** VARIABLES *************************************************//
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
int alerGeneral = 0;
int alerPeep = 0;
int alerBateria = 0;
int estabilidad = 0;
int PeepEstable = 0;

// Variables de transmisión de datos a Raspberry
String RaspberryChain = "";

// Variables de manejo de ADC
volatile int contADC = 0;
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

// Variables para la atencion de interrupciones
bool flagTimerInterrupt = false;
volatile bool flagSwInterrupt = false;
volatile bool flagEncoderInterrupt_A = false;
volatile bool flagEncoderInterrupt_B = false;
volatile bool flagDettachInterrupt_A = false;
volatile bool flagDettachInterrupt_B = false;
volatile bool flagDetach = false;
bool flagAlarmPpico = false;
bool flagAlarmPatientDesconnection = false;
bool flagAlarmGeneral = false;
volatile unsigned int contDetach = 0;
unsigned int contCiclos = 0;
unsigned int contEscrituraEEPROM = 0;
unsigned int contUpdateData = 0;
bool flagStandbyInterrupt = false;
unsigned int contStandby = 0;

//Variables de prueba de transmisión
int pruebaDato = 0;
int second = 0;
int milisecond = 0;

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

//- Señales
float SFin = 0; //Señal de flujo inspiratorio
float SFout = 0; //Señal de flujo espiratorio
float SPpac = 0; //Señal de presión en la via aerea del paciente
float SPin = 0; //Señal filtrada de presion en la camara
float SPout = 0; //Señal filtrada de presion en la bolsa

//- Filtrado
float Pin[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
float Pout[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
float Ppac[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
float Fin[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
float Fout[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

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


// Variables de control del timer
volatile int interruptCounter = 0;

// Variables de control de alarmas
bool flagInicio = true;

// Variables de recepcion de LCD
int frecRespiratoria = 12;
int I = 1;
int E = 20;
float relI = 0;
float relE = 0;
int maxPresion = 30;

//**************************************************************************************************************//

//******************************************** Configuraciones generales ***************************************//
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
        return 0;
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
//**************************************************************************************************************//

//****************************************** Configuración inicial *********************************************//
void setup() {
    // Configuracion del timer a 1 kHz
    timer = timerBegin(0, 80, true);               // Frecuencia de reloj 80 MHz, prescaler de 80, frec 1 MHz
    timerAttachInterrupt(timer, &onTimer, true);
    timerAlarmWrite(timer, 1000, true);             // Interrupcion cada 1000 conteos del timer, es decir 1ms
    timerAlarmEnable(timer);                        // Habilita interrupcion por timer

    // Configuración de los pines de conexion con del driver para manejo de electrovalvulas
    pinMode(2, OUTPUT);  // PIN 3   velocidad
    pinMode(4, OUTPUT);  // PIN 6   velocidad
    pinMode(5, OUTPUT);   // PIN 12  velocidad
    pinMode(12, OUTPUT);  // PIN 3   velocidad
    pinMode(13, OUTPUT);  // PIN 6   velocidad
    pinMode(14, OUTPUT);   // PIN 12  velocidad
    pinMode(15, OUTPUT);  // PIN 3   velocidad
    pinMode(18, OUTPUT);  // PIN 6   velocidad

    // Configuración de los pines de control de las EV
    pinMode(EV_INSPIRA, OUTPUT);	// PIN 3   velocidad
    pinMode(EV_ESC_CAM, OUTPUT);	// PIN 6   velocidad
    pinMode(EV_ESPIRA, OUTPUT);		// PIN 12  velocidad

    //pinMode(BATTALARM, INPUT);   // PIN 5  velocidad

    //Configuración de la comunicación serial
    Serial.begin(115200);//Transmición de datos a equipo
    Serial2.begin(9600); //Transmisión de datos a LCD (SERIAL_8N1, RXD2, TXD2);
    Serial2.setTimeout(10);

    //Configuración EEprom
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

    //  digitalWrite(2, LOW);  // PIN 3   velocidad
    //  digitalWrite(4, LOW);  // PIN 6   velocidad
    //  digitalWrite(5, LOW);   // PIN 12  velocidad
    //  digitalWrite(12, LOW);  // PIN 3   velocidad
    //  digitalWrite(13, LOW);  // PIN 6   velocidad
    //  digitalWrite(14, LOW);   // PIN 12  velocidad
    //  digitalWrite(15, LOW);  // PIN 3   velocidad
    //  digitalWrite(18, LOW);  // PIN 6   velocidad
    //
    //  digitalWrite(2, HIGH);  // PIN 1   velocidad
    //  delay(1000);
    //  digitalWrite(18, HIGH);  // PIN 2   velocidad
    //  delay(1000);
    //  digitalWrite(5, HIGH);   // PIN 3  velocidad
    //  delay(1000);
    //  digitalWrite(4, HIGH);  // PIN 4   velocidad
    //  delay(1000);
    //  digitalWrite(15, HIGH);  // PIN 5   velocidad
    //  delay(1000);
    //  digitalWrite(13, HIGH);  // PIN 6   velocidad
    //  delay(1000);
    //  digitalWrite(12, HIGH);  // PIN 7   velocidad
    //  delay(1000);
    //  digitalWrite(14, HIGH);   // PIN 8  velocidad
    //  delay(1000);
    //

    digitalWrite(2, LOW);  // PIN 3   velocidad
    digitalWrite(4, LOW);  // PIN 6   velocidad
    digitalWrite(5, LOW);   // PIN 12  velocidad
    digitalWrite(12, LOW);  // PIN 3   velocidad
    digitalWrite(13, LOW);  // PIN 6   velocidad
    digitalWrite(14, LOW);   // PIN 12  velocidad
    digitalWrite(15, LOW);  // PIN 3   velocidad
    digitalWrite(18, LOW);  // PIN 6   velocidad

    delay(1000);
    Serial.flush();
    Serial2.flush();
}
//**************************************************************************************************************//

//********************************************* Ciclo infinito *************************************************//
void loop() {

    //**** Funcion de ciclado
    cycling();

    //**** Atencion a rutina de adquisicion ADC
    if (fl_ADC) {//Si han pasado 50ms habilita la lectura del ADC
        fl_ADC = false;//reinicia la bandera de atención de ADC
        //alerBateria =  digitalRead(BATTALARM);
        // Lectura de valores ADC
        ADC1_Value = analogRead(ADC_PRESS_1);
        ADC2_Value = analogRead(ADC_PRESS_2);
        ADC3_Value = analogRead(ADC_PRESS_3);// ADC presión de la via aerea
        ADC4_Value = analogRead(ADC_FLOW_1);
        ADC5_Value = analogRead(ADC_FLOW_2);

        // Conversiones de ADC a presiones
        Pressure1 = AMP1 * float(ADC1_Value) + OFFS1;
        Pressure2 = AMP2 * float(ADC2_Value) + OFFS2;
        Pressure3 = AMP3 * float(ADC3_Value) + OFFS3;// Presión de la via aerea
        flow1 = float(ADC4_Value - 1695) / 10;
        flow2 = float(ADC5_Value - 1768) / 10;

        // Procesamiento señales
        //- Almacenamiento
        Ppac[9] = Pressure3;
        Pin[9] = Pressure1;
        Pout[9] = Pressure2;
        Fin[9] = flow1;
        Fout[9] = flow2;
        //- Corrimiento inicial
        for (int i = 9; i >= 1; i--) {
            Pin[9 - i] = Pin[9 - i + 1];
            Pout[9 - i] = Pout[9 - i + 1];
            Ppac[9 - i] = Ppac[9 - i + 1];
            Fin[9 - i] = Fin[9 - i + 1];
            Fout[9 - i] = Fout[9 - i + 1];
        }
        //- Inicialización
        SPin = 0;
        SPout = 0;
        SPpac = 0;
        SFin = 0;
        SFout = 0;
        //- Actualización
        for (int i = 0; i <= 9; i++) {
            SPin = SPin + Pin[i];
            SPout = SPout + Pout[i];
            SPpac = SPpac + Ppac[i];
            SFin = SFin + Fin[i];
            SFout = SFout + Fout[i];
        }
        //- Calculo promedio
        SPin = SPin / 10;
        SPout = SPout / 10;
        SPpac = SPpac / 10;
        SFin = SFin / 10;
        SFout = SFout / 10;

        //- machetazo flujo
        SFin = (SFin - 10);
        SFout = (SFout + 12);
        SFout = SFout * 2 + 3.5;

        // Calculo Presiones maximas y minimas en la via aerea
        if (UmbralPpmin > SPpac) {
            UmbralPpmin = SPpac;
        }
        if (UmbralPpico < SPpac) {
            UmbralPpico = SPpac;
        }

        // Calculo de relaciones I:E
        if (I != 1) {
            relI = (float)(I / 10.0);
        }
        else {
            relI = (float)(I);
        }
        if (E != 1) {
            relE = (float)(E / 10.0);
        }
        else {
            relE = (float)(E);
        }

        // Calculo de volumen circulante	
        Vtidal = Vtidal + (SFin - SFout - 10) * .5;

        // Transmicón serial
        //- Asignación de variables
        if (alerDesconexion == 1) {
            patientPress = String(0);
        }
        else {
            patientPress = String(SPpac);
        }
        patientFlow = String(SFin - SFout - 10);
        patientVolume = String(Vtidal);
        pressPIP = String(Ppico, 0);
        pressPEEP = String(Peep, 0);
        frequency = String(frecRespiratoria, 0);
        if (I == 1) {
            rInspir = String(relI, 0);
        }
        else {
            rInspir = String(relI, 1);
        }
        if (E == 1) {
            rEspir = String(relE, 0);
        }
        else {
            rEspir = String(relE, 1);
        }


        volumeT = String(VT, 0);
        alertPip = String(alerPresionPIP, 0);

        alertPeep = String(alerGeneral, 0);

        alertDiffPress = String(0);
        alertConnPat = String(alerDesconexion, 0);
        alertLeak = String(0);
        //alertConnEquipment = String(alerBateria);
        alertConnEquipment = String(0);
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
        //Serial.println(RaspberryChain);

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
    }
}
//**************************************************************************************************************//

//****************************** Atencion a rutina de interrupcion por timer ***********************************//
void IRAM_ATTR onTimer() {
    portENTER_CRITICAL(&timerMux);
    flagTimerInterrupt = true;
    portEXIT_CRITICAL(&timerMux);
}
//**************************************************************************************************************//

//************************************** Rutina de ciclado del ventilador **************************************//
void cycling() {
    //**** Atencion a rutina del timer
    if (flagTimerInterrupt) {// cada 1 ms

      // Reinicio del contador de interrupción del timer
        portENTER_CRITICAL_ISR(&timerMux);
        flagTimerInterrupt = false;
        portEXIT_CRITICAL_ISR(&timerMux);
        interruptCounter++;//Incrementa el contador de tiempo cada 1ms

        //Conteo de tiempo
        milisecond++;
        if (milisecond == 1000) {
            milisecond = 0;
            second++;
            if (second == 60) {
                second = 0;
            }
        }
        // Actualización de datos del LCD
        receiveData();          //Recepción de datos por serial con LCD
        contUpdateData++;       //Incrementa el contador de milisegundos para actualización del LCD
        if (contUpdateData >= 200) {
            contUpdateData = 0;
            sendSerialData();
        }
        // Varificación EEPROM
        contEscrituraEEPROM++;//Incrementa el contador de milisegundos para manejo de la EEPROM
        if (contEscrituraEEPROM > 3600000) {
            contEscrituraEEPROM = 0;
            eeprom_wr_int(contCiclos, 'w');
        }

        //Varificación de 50 ms para el conteo del ADC
        contADC++;//Incrementa el contador de milisegundos para verificación del ADC
        if (contADC == 50) {//si han pasado 50 ms
            fl_ADC = true;//Habilita la lectura del ADC
            contADC = 0;//Reinicia el contador de tiempo del ADC
        }

        //Ciclo respiratorio
        //- Inicio inspiración
        if (interruptCounter == 1) {
            BandInsp = 1;// Activa bandera que indica que empezo la inspiración
            digitalWrite(EV_INSPIRA, LOW);//Piloto conectado a ambiente -> Desbloquea valvula piloteada y permite el paso de aire
            digitalWrite(EV_ESPIRA, HIGH);//Piloto conectado a PIP -> Limita la presión de la via aerea a la PIP configurada
            digitalWrite(EV_ESC_CAM, HIGH);//Piloto conectado a Presión de activación -> Presiona la camara      
        }
        //- Mitad de la inspiración
        else if (interruptCounter == int(((inspirationTime) * 1000) / 2)) {
            SFinMax = SFin;//
            SFoutMax = SFout;//
        }
        //- Inicio espiración
        else if (interruptCounter == int(inspirationTime * 1000)) {
            //Calculo PIP
            Ppico = SPpac;//Detección de Ppico como la presión al final de la inspiración
            Ppico = -0.0079 * (Ppico * Ppico) + 1.6493 * Ppico - 33.664;//Ajuste de Ppico
            if (Ppico < 0) {// Si el valor de Ppico es negativo
                Ppico = 0;// Lo limita a 0
            }
            Ppico = int(Ppico);

            //Mediciones de presion del sistema
            Pin_max = SPin;//Presión maxima de la camara
            Pout_max = SPout;//Presión maxima de la bolsa

            //Medicion de Volumen circulante
            VT = Vtidal;

            //Mediciones de flujo
            SFinMaxInsp = SFin;
            SFtotalMax = SFin - SFout;

            //Rutina de ciclado
            BandInsp = 0;// Desactiva la bandera, indicando que empezo la espiración
            digitalWrite(EV_INSPIRA, HIGH);//Piloto conectado a presión de bloqueo -> Bloquea valvula piloteada y restringe el paso de aire
            digitalWrite(EV_ESC_CAM, LOW);//Piloto conectado a PEEP -> Limita la presión de la via aerea a la PEEP configurada
            digitalWrite(EV_ESPIRA, LOW);//Piloto conectado a ambiente -> Despresuriza la camara y permite el llenado de la bolsa
        }
        //- Finalización periodo respiratorio
        else if (interruptCounter >= int(((inspirationTime + expirationTime) * 1000))) {
            //Calculo de Peep
            Peep = SPpac;// Peep como la presion en la via aerea al final de la espiración

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

            //Calculos de volumenes
            //- Asignación
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
            Pin_min = SPin;//Presión minima de la camara
            Pout_min = SPout;//Presión minima de la bolsa

            //Asignación de valores maximos y minimos de presión      
            pmin = UmbralPpmin;//asigna la presion minima encontrada en todo el periodo      
            pmax = UmbralPpico;//asigna la presion maxima encontrada en todo el periodo
            UmbralPpmin = 100;//Reinicia el umbral minimo de presion del paciente
            UmbralPpico = -100;//Reinicia el umbral maximo de presion del paciente

            //Metodo de exclusión de alarmas
            if (Ppico > 2 && Peep > 2) {
                flagInicio = false;
            }
            alarmsDetection();

            //Reinicia el conteo de ms cuando termina el periodo respiratorio
            interruptCounter = 0;
        }
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
}
//**************************************************************************************************************//

//***************************************** Función de recepción serial ****************************************//
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
        frecRespiratoria = dataIn2[0].toInt();
        I = dataIn2[1].toInt();
        E = dataIn2[2].toInt();
        maxPresion = dataIn2[3].toInt();
        alerBateria = dataIn2[4].toInt();
        estabilidad = dataIn2[5].toInt();
        Serial2.flush();

        Serial.println(String(frecRespiratoria) + ',' + String(I) + ',' + String(E) + ',' + String(maxPresion) + ',' + String(alerBateria) + ',' + String(estabilidad));
        Serial.println(String(alerPeep));
        /*for (int i = 0; i < contComas + 1; i++) {
            Serial.println(dataIn2[i]);
          }*/

          // Calculo del tiempo I:E
        if (I == 1) {
            inspirationTime = (float)(60.0 / (float)frecRespiratoria) / (1 + (float)((float)E / 10.0));
            expirationTime = (float)((float)E / 10.0) * (float)inspirationTime;
            //Serial.println("IC=I = " + String(I) + ":" + String(E) + "-" + String(inspirationTime) + " E = " + String(expirationTime));
        }
        else {
            expirationTime = (float)(60.0 / (float)frecRespiratoria) / (1 + (float)((float)I / 10.0));
            inspirationTime = (float)((float)I / 10.0) * (float)expirationTime;
            //Serial.println("EC=I = " + String(I) + ":" + String(E) + "-" + String(inspirationTime) + " E = " + String(expirationTime));
        }
        Serial.println("Frec = " + String(frecRespiratoria) + " I = " + String(inspirationTime) + " E = " + String(expirationTime));
    }
}
//**************************************************************************************************************//

//*************************************** Función de transmisión serial ****************************************//
void sendSerialData() {
    String dataToSend = String(Ppico) + ',' + String(Peep) + ',' + String(VT) + ',' +
        String(alerPresionPIP) + ',' + String(alerDesconexion) + ',' +
        String(alerGeneral) + ',' + String(alerPeep) + ';';
    Serial2.print(dataToSend);
}
//**************************************************************************************************************//

//******************************************** Detección de alarmas ********************************************//
void alarmsDetection() {

    if (flagInicio == false) {//Si hay habilitación de alarmas
      // Alarma por Ppico elevada
        if (Ppico > maxPresion) {
            flagAlarmPpico = true;
            alerPresionPIP = 1;
        }
        else {
            flagAlarmPpico = false;
            alerPresionPIP = 0;
        }

        // Alarma por desconexión del paciente
        if ((SFinMax - SFtotalMax) <= 10) {
            flagAlarmPatientDesconnection = true;
            alerDesconexion = 1;
        }
        else {
            flagAlarmPatientDesconnection = false;
            alerDesconexion = 0;
        }

        // Alarma por obstrucción
        if ((SFinMax - SFoutMax) <= 5) {
            flagAlarmGeneral = true;
            alerGeneral = 1;
        }
        else {
            flagAlarmGeneral = false;
            alerGeneral = 0;
        }

        SFinMax = SFin;
        SFoutMax = SFout;
    }
}
//**************************************************************************************************************//

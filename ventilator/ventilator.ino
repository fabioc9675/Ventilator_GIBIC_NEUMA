/*
  Name:		main.ino
  Created:	3/22/2020 19:13:49
  Author:	Helber Carvajal
*/

// Definiciones para controlar el shiel DFRobot quad motor driver
#define EV_01_P1        25  // equivale al pin 3 del shield, velocidad motor 1
#define EV_01_P2        17  // equivale al pin 4 del shield, direccion motor 1
#define EV_02_P1        27  // equivale al pin 6 del shield, velocidad motor 4
#define EV_02_P2        14  // equivale al pin 7 del shield, direccion motor 4
#define EV_03_P1        12  // equivale al pin 11 del shield, velocidad motor 2
#define EV_03_P2        13  // equivale al pin 12 del shield, direccion motor 2

// Definiciones para el manejo del ADC
#define ADC_PRESS_1     39  // pin ADC para presion 1
#define ADC_PRESS_2     36  // pin ADC para presion 1

// Definiciones para controlar el sensor de flujo
#define FLANCO          18  // pin digital numero 2 para deteccion de flujo IRQ

#define AMP1       0.0308
#define OFFS1      -22.673
#define AMP2       0.0309
#define OFFS2      -32.354

struct Fan {
	const uint8_t PIN;
	uint32_t numberOfPulses;
	bool aspa;
};

Fan fan1 = { FLANCO, 0, false };

// Definiciones del ADC
volatile int contADC = 0;
bool fl_ADC = false;
bool flagTimerInterrupt = false;
int ADC1_Value = 0;     //
int ADC2_Value = 0;
int numeroPulsos = 0;

// definiciones del ADC
float Pressure1 = 0;
float Pressure2 = 0;
float Pin[10] = { 0,0,0,0,0,0,0,0,0,0 };
float Pout[10] = { 0,0,0,0,0,0,0,0,0,0 };
float SPin = 0;
float SPout = 0;
float Peep = 0;
float Ppico = 0;
float UmbralPeep = 100;
float UmbralPpico = -100;

float Pin_max = 0;
float Pout_max = 0;
float Pin_min = 0;
float Pout_min = 0;
float dpmin = 0;
float dpmax = 0;
float Comp = 0;
float C1 = 1.3836;
float C2 = 38.2549;
float C3 = 291.7069;
float VT = 0;
float V_Comp[10] = { 0,0,0,0,0,0,0,0,0,0 };
float V_dpmax[10] = { 0,0,0,0,0,0,0,0,0,0 };
float V_dpmin[10] = { 0,0,0,0,0,0,0,0,0,0 };
float SComp = 0;
float Sdpmax = 0;
float Sdpmin = 0;

// Some global variables available anywhere in the program
volatile unsigned long startMillis;
volatile unsigned long currentMillis;
volatile float inspirationTime = 0;
volatile float expirationTime = 0;

// definiciones del timer
volatile int interruptCounter = 0;


// inicializacion del contador del timer
hw_timer_t* timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

// definicion de interrupciones
void IRAM_ATTR onTimer();  // funcion de interrupcion


void setup() {
	// Configuracion del timer a 1 kHz
	timer = timerBegin(0, 800, true);               // Frecuencia de reloj 80 MHz, prescaler de 800, frec 0.1 MHz
	timerAttachInterrupt(timer, &onTimer, true);
	timerAlarmWrite(timer, 1000, true);             // Interrupcion cada 1000 conteos del timer, es decir 100 Hz
	timerAlarmEnable(timer);                        // Habilita interrupcion por timer

	// configuracion del pin de interrupcion
	pinMode(fan1.PIN, INPUT_PULLUP);
	//attachInterrupt(fan1.PIN, isr, RISING);
	// inicializacion de los pines controladores de las EV como salidas
	pinMode(EV_01_P1, OUTPUT);  // PIN 3   velocidad
	pinMode(EV_01_P2, OUTPUT);  // PIN 4   direccion
	pinMode(EV_02_P1, OUTPUT);  // PIN 6   velocidad
	pinMode(EV_02_P2, OUTPUT);  // PIN 7   direccion
	Serial.begin(115200);   // incializacion de comunicacion serie a 9600 bps
	lcd_setup();
}

void loop() {
	//*******refresh display every 0.5 s************
	static unsigned long timeToShowLcd = 0;
	if ((millis() - timeToShowLcd) > 300) {
		timeToShowLcd = millis();
		lcd_show();
	}
	//********************************************

	switchRoutine();
	encoderRoutine();
	currentMillis = millis();

	if (flagTimerInterrupt) {
		flagTimerInterrupt = false;
		interruptCounter++;
		contADC++;
		if (contADC == 50) {
			fl_ADC = true;
			contADC = 0;
		}
		if (interruptCounter == 1) {        // Inicia el ciclado abriendo electrovalvula de entrada y cerrando electrovalvula de salida
            //Electrovalvulas
            //-Abre electrovalvula de entrada actuador
            digitalWrite(EV_01_P2, HIGH);   // turn the LED on (HIGH is the voltage level)
            digitalWrite(EV_01_P1, HIGH);   // turn the LED on (HIGH is the voltage level)
            //-Cierra electrovalvula de salida actuador
            digitalWrite(EV_02_P1, LOW);   // turn the LED on (HIGH is the voltage level)
            digitalWrite(EV_02_P2, LOW);   // turn the LED on (HIGH is the voltage level)
            //-Cierra electrovalvula de salida circuitos exalación
            digitalWrite(EV_03_P1, LOW);   // turn the LED on (HIGH is the voltage level)
            digitalWrite(EV_03_P2, LOW);   // turn the LED on (HIGH is the voltage level)
            //Calculos
            Pin_min = SPin;
            Pout_min = SPout;
		}
		else if (interruptCounter == int(inspirationTime * 1000) / 10) { // espera 1 segundo y cierra electrovalvula de entrada y abre electrovalvula de salida
            //-Cierra electrovalvula de entrada actuador
            digitalWrite(EV_01_P2, LOW);    // turn the LED off by making the voltage LOW
            digitalWrite(EV_01_P1, LOW);   // turn the LED on (HIGH is the voltage level)
            //-Abre electrovalvula de salida actuador
            digitalWrite(EV_02_P1, HIGH);    // turn the LED off by making the voltage LOW
            digitalWrite(EV_02_P2, HIGH);   // turn the LED on (HIGH is the voltage level)
            //-Abre electrovalvula de salida actuador
            digitalWrite(EV_03_P1, HIGH);    // turn the LED off by making the voltage LOW
            digitalWrite(EV_03_P2, HIGH);   // turn the LED on (HIGH is the voltage level)
            //Calculos
            Pin_max = SPin;
            Pout_max = SPout;
            dpmax = Pin_max - Pin_min;
            if (dpmax < 0) {
                dpmax = -dpmax;
            }
            dpmin = Pout_max - Pout_min;
            if (dpmin < 0) {
                dpmin = -dpmin;
            }
		}
		else if (interruptCounter == int(((inspirationTime + expirationTime) * 1000)) / 10) {
			interruptCounter = 0;
            Peep = UmbralPeep;
            UmbralPeep = 100;
            Ppico = UmbralPpico - 2;
            UmbralPpico = -100;
            Comp = (C1 * Peep * Peep) - (C2 * Peep) + C3;
            //Comp=(-14.52*Peep)+(198.75);

            //Filtrado media movil VT y COMP
            //-Almacenamiento de valores
            V_Comp[9] = Comp;
            V_dpmax[9] = dpmax;
            V_dpmin[9] = dpmin;
            //-Corrimiento inicial
            for (char i = 9; i >= 1; i--) {
                V_Comp[9 - i] = V_Comp[9 - i + 1];
                V_dpmax[9 - i] = V_dpmax[9 - i + 1];
                V_dpmin[9 - i] = V_dpmin[9 - i + 1];
            }
            //-Calculo promedio
            SComp = 0;
            Sdpmax = 0;
            Sdpmin = 0;
            for (char i = 0; i <= 9; i++) {
                SComp = SComp + V_Comp[i];
                Sdpmax = Sdpmax + V_dpmax[i];
                Sdpmin = Sdpmin + V_dpmin[i];
            }
            SComp = SComp / 10;
            Sdpmax = Sdpmax / 10;
            Sdpmin = Sdpmin / 10;
            VT = SComp * (Sdpmax - Sdpmin);
		}

	}

	if (fl_ADC) {
        //Lectura ADC
        fl_ADC = false;
        ADC1_Value = analogRead(ADC_PRESS_1);
        ADC2_Value = analogRead(ADC_PRESS_2);
        //Calculo presiones instantaneas
        Pressure1 = AMP1 * float(ADC1_Value) + OFFS1;
        Pressure2 = AMP2 * float(ADC2_Value) + OFFS2;
        //Filtrado media movil
        //-Almacenamiento de valores
        Pin[9] = Pressure1;
        Pout[9] = Pressure2;
        //-Corrimiento inicial
        for (char i = 9; i >= 1; i--) {
            Pin[9 - i] = Pin[9 - i + 1];
            Pout[9 - i] = Pout[9 - i + 1];
        }
        //-Calculo promedio
        SPin = 0;
        SPout = 0;
        for (char i = 0; i <= 9; i++) {
            SPin = SPin + Pin[i];
            SPout = SPout + Pout[i];
        }
        SPin = SPin / 10;
        SPout = SPout / 10;
        //-Calculo Peep
        if (UmbralPeep > SPin) {
            UmbralPeep = SPin;
        }
        //-Calculo Ppico
        if (UmbralPpico < SPin) {
            UmbralPpico = SPin;
        }
        // conteo de los pulsos
        numeroPulsos = fan1.numberOfPulses;  //Numero de pulsos en 50ms
        fan1.numberOfPulses = 0;
        numeroPulsos = (numeroPulsos * 2000 / 75); //(#pulsos/1 s)=7.5*Q [L/min] (considera un decimal)

        //Visualización UART
        //    Serial.print("SPin = ");
        //    // Serial.print(ADC1_Value);
        //    Serial.print(SPin);
        //    Serial.print(", SPout = ");
        //    // Serial.println(ADC2_Value);
        //    Serial.print(SPout);
        //    //Serial.print(", Puls = ");
        //    //Serial.println(numeroPulsos);

        //    Serial.print(", VT = ");
        //    Serial.println(SVT);
        //    Serial.print(", Comp = ");
        //    Serial.println(SComp);
        //    Serial.print(",");
        //    Serial.print("dpmin = ");
        //    Serial.print(Sdpmin);
        //    Serial.print(", dpmax = ");
        //    Serial.println(Sdpmax);
        Serial.print("VT = ");
        Serial.print(VT);
        Serial.print(", Peep = ");
        Serial.println(Peep);
        Serial.print(", Ppico = ");
        Serial.println(Ppico);
	}
}

void IRAM_ATTR onTimer() {
	portENTER_CRITICAL(&timerMux);
	//Serial.println("I am inside onTimer");
	flagTimerInterrupt = true;
	portEXIT_CRITICAL(&timerMux);
}

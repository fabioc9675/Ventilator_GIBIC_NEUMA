/*
  Name:		ventilator.ino
  Created:	3/22/2020 19:13:49
  Author:	Helber Carvajal
*/

// Control del shield DFRobot quad motor driver
#define EV_01_P1        A0  // equivale al pin 3 del shield, velocidad motor 1
#define EV_01_P2        A1  // equivale al pin 4 del shield, direccion motor 1
#define EV_02_P1        A2  // equivale al pin 6 del shield, velocidad motor 2
#define EV_02_P2        A3  // equivale al pin 7 del shield, direccion motor 2

// Manejo del ADC
#define ADC_PRESS_1     A4  // pin ADC para presion 1
#define ADC_PRESS_2     A5  // pin ADC para presion 1

// Sensor de flujo
#define FLANCO          5  // pin digital numero 2 para deteccion de flujo IRQ

struct Fan {
    const uint8_t PIN;
    uint32_t numberOfPulses;
    bool aspa;
};

Fan fan1 = { FLANCO, 0, false };

// Definiciones del ADC
volatile int contADC = 0;
bool fl_ADC = false;
int ADC1_Value = 0;     //
int ADC2_Value = 0;
int numeroPulsos = 0;

// Some global variables available anywhere in the program
unsigned long startMillis;
unsigned long currentMillis;

// definiciones del timer
volatile int interruptCounter = 0;

// inicializacion del contador del timer
//hw_timer_t* timer = NULL;
//portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;


// definicion de interrupciones
void onTimer();  // funcion de interrupcion

void setup() {
    SerialUSB.begin(9600);   // incializacion de comunicacion serie a 9600 bps
    lcd_setup();
}
void loop() {
    lcd_show();

    if (millis() - startMillis > 50) {
        onTimer();
    }

    //if (fl_ADC) {
    //    fl_ADC = false;
    //    ADC1_Value = analogRead(ADC_PRESS_1);
    //    ADC2_Value = analogRead(ADC_PRESS_2);
    //    // falta realizar la conversion de adc a presion

    //    // conteo de los pulsos
    //    numeroPulsos = fan1.numberOfPulses;  //Numero de pulsos en 50ms  
    //    fan1.numberOfPulses = 0;
    //    numeroPulsos = (numeroPulsos * 2000 / 75); //(#pulsos/1 s)=7.5*Q [L/min] (considera un decimal)


    //    Serial.print("Pres1 = ");
    //    Serial.print(ADC1_Value);
    //    Serial.print(", Pres2 = ");
    //    Serial.println(ADC2_Value + 180);
    //    //Serial.print(", Puls = ");
    //    //Serial.println(numeroPulsos);
    //}
}


void onTimer() {

    interruptCounter++;
    contADC++;

    fl_ADC = true;
    contADC = 0;

    if (interruptCounter == 1) {        // inicia el ciclado abriendo electrovalvula de entrada y cerrando electrovalvula de salida
        digitalWrite(EV_01_P2, HIGH);   // turn the LED on (HIGH is the voltage level)
        digitalWrite(EV_01_P1, HIGH);   // turn the LED on (HIGH is the voltage level)

        digitalWrite(EV_02_P1, LOW);    // turn the LED on (HIGH is the voltage level)
        digitalWrite(EV_02_P2, LOW);    // turn the LED on (HIGH is the voltage level)
    }
    else
        if (interruptCounter == 1000 / 50) {  // espera 1 segundo y cierra electrovalvula de entrada y abre electrovalvula de salida
            digitalWrite(EV_02_P1, HIGH);    // turn the LED off by making the voltage LOW
            digitalWrite(EV_02_P2, HIGH);   // turn the LED on (HIGH is the voltage level)

            digitalWrite(EV_01_P2, LOW);    // turn the LED off by making the voltage LOW
            digitalWrite(EV_01_P1, LOW);   // turn the LED on (HIGH is the voltage level)
        }
        else
            if (interruptCounter == 3000 / 50) {
                interruptCounter = 0;
            }
}
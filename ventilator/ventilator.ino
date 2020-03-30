/*
  Name:		ventilator.ino
  Created:	3/22/2020 19:13:49
  Author:	Helber Carvajal
*/

// Definiciones para controlar el shiel DFRobot quad motor driver
#define EV_01_P1        25  // equivale al pin 3 del shield, velocidad motor 1
#define EV_01_P2        17  // equivale al pin 4 del shield, direccion motor 1
#define EV_02_P1        27  // equivale al pin 6 del shield, velocidad motor 2
#define EV_02_P2        14  // equivale al pin 7 del shield, direccion motor 2

// Definiciones para el manejo del ADC
#define ADC_PRESS_1     39  // pin ADC para presion 1
#define ADC_PRESS_2     36  // pin ADC para presion 1

// Definiciones para controlar el sensor de flujo
#define FLANCO          26  // pin digital numero 2 para deteccion de flujo IRQ



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
volatile unsigned long startMillis;
volatile unsigned long currentMillis;
volatile float inspirationTime = 0;
volatile float expirationTime = 0;
// definiciones del timer
volatile int interruptCounter = 0;

// inicializacion del contador del timer
//hw_timer_t* timer = NULL;
//portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

void setup() {
  Serial.begin(115200);   // incializacion de comunicacion serie a 9600 bps
  // configuracion del pin de interrupcion
  pinMode(fan1.PIN, INPUT_PULLUP);
  //attachInterrupt(fan1.PIN, isr, RISING);
  // inicializacion de los pines controladores de las EV como salidas
  pinMode(EV_01_P1, OUTPUT);  // PIN 3   velocidad
  pinMode(EV_01_P2, OUTPUT);  // PIN 4   direccion
  pinMode(EV_02_P1, OUTPUT);  // PIN 6   velocidad
  pinMode(EV_02_P2, OUTPUT);  // PIN 7   direccion
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
  
  refreshData();
  currentMillis = millis();

  if (currentMillis - startMillis > 10) { // Ingresa cada 10 ms (100Hz)
    startMillis = millis();
    onTimer();
    //Serial.println("I am here bb");
  }
  if (fl_ADC) {
    fl_ADC = false;
    ADC1_Value = analogRead(ADC_PRESS_1);
    ADC2_Value = analogRead(ADC_PRESS_2);
    // falta realizar la conversion de adc a presion

    // conteo de los pulsos
    numeroPulsos = fan1.numberOfPulses;  //Numero de pulsos en 50ms
    fan1.numberOfPulses = 0;
    numeroPulsos = (numeroPulsos * 2000 / 75); //(#pulsos/1 s)=7.5*Q [L/min] (considera un decimal)

    /*Serial.print("Pres1 = ");
      Serial.print(ADC1_Value);
      Serial.print(", Pres2 = ");
      Serial.println(ADC2_Value + 180);
      Serial.print(", Puls = ");
      Serial.println(numeroPulsos);*/
  }
}

void onTimer() {
  //Serial.println("I am inside onTimer");
  interruptCounter++;
  contADC++;
  fl_ADC = true;
  contADC = 0;

  if (interruptCounter == 1) {        // Inicia el ciclado abriendo electrovalvula de entrada y cerrando electrovalvula de salida
    digitalWrite(EV_01_P2, HIGH);   // turn the LED on (HIGH is the voltage level)
    digitalWrite(EV_01_P1, HIGH);   // turn the LED on (HIGH is the voltage level)

    digitalWrite(EV_02_P1, LOW);    // turn the LED on (HIGH is the voltage level)
    digitalWrite(EV_02_P2, LOW);    // turn the LED on (HIGH is the voltage level)
  }
  else if (interruptCounter == int(inspirationTime * 1000) / 10) { // espera 1 segundo y cierra electrovalvula de entrada y abre electrovalvula de salida
    digitalWrite(EV_02_P1, HIGH);     // turn the LED off by making the voltage LOW
    digitalWrite(EV_02_P2, HIGH);     // turn the LED on (HIGH is the voltage level)

    digitalWrite(EV_01_P2, LOW);    // turn the LED off by making the voltage LOW
    digitalWrite(EV_01_P1, LOW);    // turn the LED on (HIGH is the voltage level)
  }
  else if (interruptCounter == int(((inspirationTime + expirationTime) * 1000)) / 10) {
    interruptCounter = 0;
  }
}

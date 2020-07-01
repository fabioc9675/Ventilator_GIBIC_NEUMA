

// Definiciones para el manejo del ADC
#define ADC_PRESS_1         32
#define ADC_PRESS_2         35
#define ADC_PRESS_3         34
#define ADC_PRESS_4         30
#define ADC_PRESS_5         36
#define ADC_PRESS_6         27

// Definiciones para el manejo del EV
#define EV_OUTPUT_1         18
#define EV_OUTPUT_2         5
#define EV_OUTPUT_3         4
#define EV_OUTPUT_4         12
#define EV_OUTPUT_5         13
#define EV_OUTPUT_6         13


// Variables de control del protocolo
#define RXD2             16
#define TXD2             17

// Variables de comunicacion con monitor
#define PIN_1            23
#define PIN_2            25

// Pines adicionales
#define IO_1            19
#define IO_2            21
#define IO_3            22


String dataReceive;



void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);
  Serial2.begin(115200);

  Serial.println("Inicializacion CONTROL");


  pinMode(EV_OUTPUT_1, OUTPUT);  // PIN 3   velocidad
  pinMode(EV_OUTPUT_2, OUTPUT);  // PIN 6   velocidad
  pinMode(EV_OUTPUT_3, OUTPUT);   // PIN 12  velocidad
  pinMode(EV_OUTPUT_4, OUTPUT);  // PIN 3   velocidad
  pinMode(EV_OUTPUT_5, OUTPUT);  // PIN 6   velocidad
  //pinMode(EV_OUTPUT_6, OUTPUT);   // PIN 12  velocidad
  //pinMode(EV_OUTPUT_7, OUTPUT);  // PIN 3   velocidad
  //pinMode(EV_OUTPUT_8, OUTPUT);  // PIN 6   velocidad


  digitalWrite(EV_OUTPUT_1, LOW);  // PIN 1   velocidad
  delay(100);
  digitalWrite(EV_OUTPUT_2, LOW);  // PIN 2   velocidad
  delay(100);
  digitalWrite(EV_OUTPUT_3, LOW);   // PIN 3  velocidad
  delay(100);
  digitalWrite(EV_OUTPUT_4, LOW);  // PIN 4   velocidad
  delay(100);
  digitalWrite(EV_OUTPUT_5, LOW);  // PIN 5   velocidad
  delay(100);
//  digitalWrite(EV_OUTPUT_6, LOW);  // PIN 6   velocidad
//  delay(100);
//  digitalWrite(EV_OUTPUT_7, LOW);  // PIN 7   velocidad
//  delay(100);
//  digitalWrite(EV_OUTPUT_8, LOW);   // PIN 8  velocidad
//  delay(100);


  digitalWrite(EV_OUTPUT_1, HIGH);  // PIN 1   velocidad
  delay(100);
  digitalWrite(EV_OUTPUT_2, HIGH);  // PIN 2   velocidad
  delay(100);
  digitalWrite(EV_OUTPUT_3, HIGH);   // PIN 3  velocidad
  delay(100);
  digitalWrite(EV_OUTPUT_4, HIGH);  // PIN 4   velocidad
  delay(100);
  digitalWrite(EV_OUTPUT_5, HIGH);  // PIN 5   velocidad
  delay(100);
//  digitalWrite(EV_OUTPUT_6, HIGH);  // PIN 6   velocidad
//  delay(100);
//  digitalWrite(EV_OUTPUT_7, HIGH);  // PIN 7   velocidad
//  delay(100);
//  digitalWrite(EV_OUTPUT_8, HIGH);   // PIN 8  velocidad
//  delay(100);

dataReceive.reserve(40);



}

void loop() {
  // put your main code here, to run repeatedly:

  Serial.print("Hola, soy CONTROL");
  Serial.print(",");
  Serial.print(analogRead(ADC_PRESS_1));
  Serial.print(",");
  Serial.print(analogRead(ADC_PRESS_2));
  Serial.print(",");
  Serial.print(analogRead(ADC_PRESS_3));
  Serial.print(",");
  Serial.print(analogRead(ADC_PRESS_4));
  Serial.print(",");
  Serial.print(analogRead(ADC_PRESS_5));
  Serial.print(",");
  Serial.println(analogRead(ADC_PRESS_6));
//  Serial.print(",");
//  Serial.print(analogRead(ADC_PRESS_7));
//  Serial.print(",");
//  Serial.println(analogRead(ADC_PRESS_8));

  if(Serial2.available() > 12){
    dataReceive = Serial2.readStringUntil(';');
			if (dataReceive[0] == 'C' && dataReceive[1] == 'O')
			{
				Serial.println("dato recibido");
        Serial2.print("OK;");
			}
  }


  delay(50);


}

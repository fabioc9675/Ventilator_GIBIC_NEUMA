
#include <OneWire.h>
#include <DallasTemperature.h>

// Definiciones para el manejo del ADC
#define MUX_EN              2
#define MUX_C               4
#define MUX_B               5
#define MUX_A               12
#define TEMP_IO             13
#define FAN_POW             19

#define RESET_CTRL          23
#define RESET_HMI           25

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(TEMP_IO);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

// arrays to hold device address
DeviceAddress insideThermometer;


int Tinit = 0;
int Tend = 0;


void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);




  pinMode(MUX_EN, OUTPUT);  // PIN 3   velocidad
  pinMode(MUX_C, OUTPUT);  // PIN 6   velocidad
  pinMode(MUX_B, OUTPUT);   // PIN 12  velocidad
  pinMode(MUX_A, OUTPUT);  // PIN 3   velocidad
  pinMode(FAN_POW, OUTPUT);  // PIN 6   velocidad


  digitalWrite(MUX_C, LOW);  // PIN 1   velocidad
  digitalWrite(MUX_B, LOW);  // PIN 2   velocidad
  digitalWrite(MUX_A, LOW);   // PIN 3  velocidad
  digitalWrite(MUX_EN, LOW);  // PIN 4   velocidad

  Tinit = millis();
  Serial.print("Tiempo inicio = ");
  Serial.println(Tinit);


}


void loop() {
  Tend = millis();

  // call sensors.requestTemperatures() to issue a global temperature
  // request to all devices on the bus
  Serial.print("Requesting temperatures...");
  sensors.requestTemperatures(); // Send the command to get temperatures
  Serial.println("DONE");
  // After we got the temperatures, we can print them here.
  // We use the function ByIndex, and as an example get the temperature from the first sensor only.
  Serial.print("Temperature for the device 1 (index 0) is: ");
  Serial.println(sensors.getTempCByIndex(0));

  Serial.println(Tend - Tinit);

  digitalWrite(FAN_POW, !digitalRead(FAN_POW));

  delay(500);

  if (Tend - Tinit >= 15000) {
        Serial.println("Restarting");
        pinMode(RESET_HMI, OUTPUT);  // PIN 3   velocidad
        pinMode(RESET_CTRL, OUTPUT);  // PIN 3   velocidad
        digitalWrite(RESET_CTRL, HIGH);  // PIN 4   velocidad
        digitalWrite(RESET_HMI, HIGH);  // PIN 4   velocidad
        delay(50);
        digitalWrite(RESET_CTRL, LOW);  // PIN 4   velocidad
        digitalWrite(RESET_HMI, LOW);  // PIN 4   velocidad
        delay(50);
        digitalWrite(RESET_CTRL, HIGH);  // PIN 4   velocidad
        digitalWrite(RESET_HMI, HIGH);  // PIN 4   velocidad
        delay(50);
        pinMode(RESET_CTRL, INPUT);  // PIN 3   velocidad
        pinMode(RESET_HMI, INPUT);  // PIN 3   velocidad

    Tinit = millis();
  }

}

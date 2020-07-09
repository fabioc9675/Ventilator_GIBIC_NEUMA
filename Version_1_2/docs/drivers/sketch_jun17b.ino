#include <Wire.h>                     // Include Wire.h for I2C functionality
#include <ADS7828.h>
ADS7828 adc(0b1001000); //

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);                 // Start Serial
  adc.init();

}

void loop() {
  int x=1;  //valor de 0 a 7
  int read_value=0; 
  read_value = adc.read(x, SD);
  Serial.print("Channel ");                 // Prints...
  Serial.print(x);                          // Prints channel number
  Serial.print(": ");                       // Prints...
  Serial.println(read_value);
  // put your main code here, to run repeatedly:
  delay(500);
}

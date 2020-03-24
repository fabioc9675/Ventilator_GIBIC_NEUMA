/*
  Name:		ventilator.ino
  Created:	3/22/2020 19:13:49
  Author:	Helber Carvajal
*/

void setup() {
    SerialUSB.begin(9600);   // incializacion de comunicacion serie a 9600 bps
    lcd_setup();
}
void loop() {
    lcd_show();
}
/*
 Name:		ventilator.ino
 Created:	3/22/2020 19:13:49
 Author:	Helber Carvajal
*/

#include <LiquidCrystal.h>

int A = 2;       //variable A a pin digital 2 (DT en modulo)
int B = 4;       //variable B a pin digital 4 (CLK en modulo)
int SW = 3;      //sw a pin digital 3 (SW en modulo)

int ANTERIOR = 50;    // almacena valor anterior de la variable POSICION
int inspirationTime = 60;
int expirationTime = 120;

volatile int POSICION = 50; // variable POSICION con valor inicial de 50 y definida
volatile unsigned int menu = 0;
// como global al ser usada en loop e ISR (encoder)
unsigned long tiempo1 = 0;
unsigned int relacionIE = 0;
unsigned int frecRespiratoria = 0;
boolean insideMenuFlag = false;

//Crear el objeto LCD con los números correspondientes (rs, en, d4, d5, d6, d7)
//LiquidCrystal lcd(25, 26, 34, 35, 32, 33);
//LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
LiquidCrystal lcd(11, 12, 7, 8, 9, 10);

void setup() {
    pinMode(A, INPUT);    // A como entrada
    pinMode(B, INPUT);    // B como entrada
    pinMode(SW, INPUT);   // SW como entrada

    SerialUSB.begin(9600);   // incializacion de comunicacion serie a 9600 bps

    attachInterrupt(digitalPinToInterrupt(A), encoderInterrupt, LOW);// interrupcion sobre pin A con
    // funcion ISR encoder y modo LOW
    attachInterrupt(digitalPinToInterrupt(SW), swInterrupt, LOW);
    SerialUSB.println("Listo");  // imprime en monitor SerialUSB Listo

    // Inicializar el LCD con el número de  columnas y filas del LCD
    lcd.begin(20, 4);
    // Escribimos el Mensaje en el LCD
    lcd.setCursor(4, 0);
    lcd.print("InnspiraMED");
    lcd.cursor();

    //lcd.blink();
}

void loop() {

    if ((millis() - tiempo1) > 1000) {
        tiempo1 = millis();
        SerialUSB.println("I am in the loop");
    }

    switch (menu) {
        case 0:
            lcd.home();
            lcd.setCursor(0, 0);
            lcd.print("    InnspiraMED     ");
            lcd.setCursor(0, 1);
            lcd.print("                   ");
            lcd.setCursor(0, 2);
            lcd.print("Frecuencia:         ");
            lcd.setCursor(11, 2);
            lcd.print(frecRespiratoria);
            lcd.setCursor(0, 3);
            lcd.print("Relacion IE:        ");
            lcd.setCursor(12, 3);
            lcd.print(relacionIE);
            delay(100);
            break;
        case 1:
            lcd.setCursor(0, 0);
            lcd.print(" Frec. Respiratoria ");
            lcd.setCursor(0, 1);
            lcd.print("                    ");
            lcd.setCursor(0, 2);
            lcd.print("frec:               ");
            lcd.setCursor(5, 2);
            lcd.print(frecRespiratoria); // Escribimos el número de segundos trascurridos
            lcd.setCursor(0, 3);
            lcd.print("                    ");
            break;
        default:
            lcd.setCursor(0, 0);
            lcd.print("                    ");
            lcd.setCursor(0, 1);
            lcd.print("                    ");
            lcd.setCursor(0, 2);
            lcd.print("Default: ");
            lcd.print(12); // Escribimos el número de segundos trascurridos
            lcd.print("           ");
            lcd.setCursor(0, 3);
            lcd.print("                    ");
            SerialUSB.println("estoy en default");
            break;
    }
    delay(100);

}

void swInterrupt() {
    static unsigned long ultimaInterrupcion = 0;  // variable static con ultimo valor de
    // tiempo de interrupcion
    unsigned long tiempoInterrupcion = millis();  // variable almacena valor de func. millis

    if (tiempoInterrupcion - ultimaInterrupcion > 5) {
        if (menu != 0)    
            insideMenuFlag = !insideMenuFlag;
        ultimaInterrupcion = tiempoInterrupcion;  // guarda valor actualizado del tiempo

    }

}

void encoderInterrupt() {
    static unsigned long ultimaInterrupcion = 0;  // variable static con ultimo valor de
    // tiempo de interrupcion
    unsigned long tiempoInterrupcion = millis();  // variable almacena valor de func. millis
    if (tiempoInterrupcion - ultimaInterrupcion > 5) {  // rutina antirebote desestima
      //lcd.clear();

        if (digitalRead(SW))SerialUSB.println("´push pressed");
        if (insideMenuFlag == false) 
            menu++;
        else {
            switch (menu) {
            case 1:
                frecRespiratoria++;
                break;
            case 2:
                relacionIE++;
                break;
            }
        }

        if (menu > 1)
            menu = 0;
        SerialUSB.println(menu);

        // pulsos menores a 5 mseg.
        if (digitalRead(B) == HIGH)     // si B es HIGH, sentido horario
        {
            POSICION++;        // incrementa POSICION en 1
        }
        else {          // si B es LOW, senti anti horario
            POSICION--;        // decrementa POSICION en 1
        }

        POSICION = min(100, max(0, POSICION));      // establece limite inferior de 0 y
                                                    // superior de 100 para POSICION
        ultimaInterrupcion = tiempoInterrupcion;    // guarda valor actualizado del tiempo
    }                                               // de la interrupcion en variable static
}
#include <LiquidCrystal.h>

//**********VALORES MAXIMOS**********
#define MENU_QUANTITY 3
#define MAX_FREC 20
#define MAX_RIE 20


//********DEFINICION DE PINES***********
#define A     2       //variable A a pin digital 2 (DT en modulo)
#define B     4       //variable B a pin digital 4 (CLK en modulo)
#define SW    3      //sw a pin digital 3 (SW en modulo)  


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


//Crear el objeto LCD con los n�meros correspondientes (rs, en, d4, d5, d6, d7)
LiquidCrystal lcd(11, 12, 7, 8, 9, 10);
void lcd_setup() {
    pinMode(A, INPUT);    // A como entrada
    pinMode(B, INPUT);    // B como entrada
    pinMode(SW, INPUT);   // SW como entrada
    attachInterrupt(digitalPinToInterrupt(A), encoderInterrupt, LOW);// interrupcion sobre pin A con
    // funcion ISR encoder y modo LOW
    attachInterrupt(digitalPinToInterrupt(SW), swInterrupt, LOW);
    lcd.begin(20, 4);

}


void lcd_show() {
    switch (menu) {
    case 0:
        lcd.home();
        lcd.setCursor(0, 0);
        lcd.print("    InnspiraMED     ");
        lcd.setCursor(0, 1);
        lcd.print("                   ");
        lcd.setCursor(0, 2);
        lcd.print("Frecuencia:         ");
        lcd.setCursor(16, 2);
        lcd.print(frecRespiratoria);
        lcd.setCursor(0, 3);
        lcd.print("Relacion IE:        ");
        lcd.setCursor(16, 3);
        lcd.print(relacionIE);
        delay(100);
        break;
    case 1:
        lcd.setCursor(0, 0);
        lcd.print(" Frec. Respiratoria ");
        lcd.setCursor(0, 1);
        lcd.print("                    ");
        lcd.setCursor(0, 2);
        lcd.print("  Frec:             ");
        lcd.setCursor(10, 2);
        lcd.print(frecRespiratoria); // Escribimos el n�mero de segundos trascurridos
        lcd.setCursor(0, 3);
        lcd.print("                    ");
        break;
    case 2:
        lcd.setCursor(0, 0);
        lcd.print(" Relacion Insp Exp  ");
        lcd.setCursor(0, 1);
        lcd.print("                    ");
        lcd.setCursor(0, 2);
        lcd.print("  RIE:              ");
        lcd.setCursor(10, 2);
        lcd.print(relacionIE); // Escribimos el n�mero de segundos trascurridos
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
        lcd.print(12); // Escribimos el n�mero de segundos trascurridos
        lcd.print("           ");
        lcd.setCursor(0, 3);
        lcd.print("                    ");
        SerialUSB.println("estoy en default");
        break;
    }


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
      // if (digitalRead(SW))SerialUSB.println("�push pressed");
        if (insideMenuFlag == false)
            menu++;
        else {
            switch (menu) {
            case 1:
                if (digitalRead(B) == HIGH) {
                    frecRespiratoria++;
                    if (frecRespiratoria > MAX_FREC) {
                        frecRespiratoria = MAX_FREC;
                    }
                }
                else {
                    frecRespiratoria--;
                    if (frecRespiratoria > MAX_FREC) {
                        frecRespiratoria = 0;
                    }
                }
                break;
            case 2:
                SerialUSB.println("inside rie");
                if (digitalRead(B) == HIGH) {
                    relacionIE++;
                    if (relacionIE > MAX_RIE) {
                        relacionIE = MAX_RIE;
                    }
                }
                else {
                    relacionIE--;
                    if (relacionIE > MAX_RIE) {
                        relacionIE = 0;
                    }
                }
                break;
            }
        }
        if (menu > MENU_QUANTITY - 1)
            menu = 0;
        SerialUSB.println(menu);



        POSICION = min(100, max(0, POSICION));      // establece limite inferior de 0 y
        // superior de 100 para POSICION
        ultimaInterrupcion = tiempoInterrupcion;    // guarda valor actualizado del tiempo
    }                                               // de la interrupcion en variable static
}


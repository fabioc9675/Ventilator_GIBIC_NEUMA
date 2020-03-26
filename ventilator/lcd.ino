#include <LiquidCrystal.h>

//**********VALORES MAXIMOS**********
#define MENU_QUANTITY 3
#define MAX_FREC 20
#define MAX_RIE 4


//********DEFINICION DE PINES***********
#define A     2      //variable A a pin digital 2 (DT en modulo)
#define B     4      //variable B a pin digital 4 (CLK en modulo)
#define SW    3      //sw a pin digital 3 (SW en modulo)  

volatile int POSICION = 50;
int ANTERIOR = 50;    // almacena valor anterior de la variable POSICION

volatile unsigned int menu = 0;
// como global al ser usada en loop e ISR (encoder)
unsigned long tiempo1 = 0;
float relacionIE = 4;
unsigned int frecRespiratoria = 10;
boolean insideMenuFlag = false;

volatile unsigned long miTiempo = 0;

//Crear el objeto LCD con los numeros correspondientes (rs, en, d4, d5, d6, d7)
LiquidCrystal lcd(11, 12, 7, 8, 9, 10);
void lcd_setup() {
    pinMode(A, INPUT);    // A como entrada
    pinMode(B, INPUT);    // B como entrada
    pinMode(SW, INPUT);   // SW como entrada
    // interrupcion sobre pin A con
    attachInterrupt(digitalPinToInterrupt(A), encoderInterrupt, LOW);
    // funcion ISR encoder y modo LOW
    attachInterrupt(digitalPinToInterrupt(SW), swInterrupt, LOW);
    lcd.begin(20, 4);
}

void lcd_show() {
    String relacion_IE = "1:4.1";
    if (relacionIE > 0) {
        relacion_IE = "1:" + String(relacionIE, 1);   
    }
    else {
        relacion_IE = String(-relacionIE, 1) + ":1";  
    }
    if (millis() - miTiempo > 500) {
        miTiempo = millis();
        SerialUSB.println(relacion_IE);
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
        lcd.setCursor(14, 2);
        lcd.print(frecRespiratoria);
        lcd.setCursor(0, 3);
        lcd.print("Relacion I:E:        ");
        lcd.setCursor(14, 3);
        lcd.print(relacion_IE);
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
        lcd.print(frecRespiratoria); // Escribimos el numero de segundos trascurridos
        lcd.setCursor(0, 3);
        lcd.print("                    ");
        break;
    case 2:
        lcd.setCursor(0, 0);
        lcd.print("    Relacion I:E    ");
        lcd.setCursor(0, 1);
        lcd.print("                    ");
        lcd.setCursor(0, 2);
        lcd.print("  I:E:              ");
        lcd.setCursor(10, 2);
        lcd.print(relacion_IE); // Escribimos el numero de segundos trascurridos
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
        lcd.print(12);                      // Escribimos el numero de segundos trascurridos
        lcd.print("           ");
        lcd.setCursor(0, 3);
        lcd.print("                    ");
        break;
    }
    //SerialUSB.println("I am in lcd_show()");
}

void swInterrupt() {
    static unsigned long ultimaInterrupcion = 0;
    unsigned long tiempoInterrupcion = millis();

    if (tiempoInterrupcion - ultimaInterrupcion > 5) {
        //SerialUSB.println("I am in swInterrupt");
        if (menu != 0)
            insideMenuFlag = !insideMenuFlag;
        }
    ultimaInterrupcion = tiempoInterrupcion;
}

void encoderInterrupt() {
    
    static unsigned long ultimaInterrupcion = 0;
    unsigned long tiempoInterrupcion = millis();

    if (tiempoInterrupcion - ultimaInterrupcion > 5) {  // rutina antirebote desestima
        //SerialUSB.println("I am in encoderInterrupt");
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
                if (digitalRead(B) == HIGH) {
                    relacionIE = relacionIE - 0.1;
                    if (relacionIE <= -MAX_RIE) {
                        relacionIE = -MAX_RIE;
                    }
                    if (relacionIE > 0 && relacionIE < 1) {
                        relacionIE = -1;
                    }
                }
                else {
                    relacionIE = relacionIE + 0.1;
                    if (relacionIE >= MAX_RIE) {
                        relacionIE = MAX_RIE;;
                    }
                    if (relacionIE > -1.0 && relacionIE < 0) {
                        relacionIE = 1;
                    }
                }

                // Calculo del tiempo I:E
                if (relacionIE > 0) {
                    inspirationTime = (60 / frecRespiratoria) / (1 + relacionIE);
                    expirationTime = relacionIE * inspirationTime;
                }
                else {
                    expirationTime = (60 / frecRespiratoria) / (1 - relacionIE);
                    inspirationTime = -relacionIE * expirationTime;
                }
                SerialUSB.println("I :" + String(inspirationTime));
                SerialUSB.println("E :" + String(expirationTime));
                break;
            }
        }
        if (menu > MENU_QUANTITY - 1)
            menu = 0;
        //SerialUSB.println("menu = " + String(menu));
        POSICION = min(100, max(0, POSICION));  // Establece limite inferior de 0 y
                                                // superior de 100 para POSICION
    }  
    ultimaInterrupcion = tiempoInterrupcion;                                         
    
}


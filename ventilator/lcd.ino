#include <LiquidCrystal.h>

//**********VALORES MAXIMOS**********
#define MENU_QUANTITY 3
#define MAX_FREC 20
#define MAX_RIE 4
#define MAX_PRESION 40
#define MAX_FLUJO 40

//********DEFINICION DE PINES***********
//#define A     2      //variable A a pin digital 2 (DT en modulo)
//#define B     4      //variable B a pin digital 4 (CLK en modulo)
//#define SW    3      //sw a pin digital 3 (SW en modulo)  
#define A     32      //variable A a pin digital 2 (DT en modulo)
#define B     33      //variable B a pin digital 4 (CLK en modulo)
#define SW    34      //sw a pin digital 3 (SW en modulo)  

//Define pins for LCD
#define rs 26
#define en 16
#define d4 5
#define d5 23
#define d6 19
#define d7 18

volatile unsigned int menu = 0;
// como global al ser usada en loop e ISR (encoder)
unsigned long tiempo1 = 0;
float relacionIE = 4;
float maxPresion = 4;
float maxFlujo = 4;
unsigned int frecRespiratoria = 10;
boolean insideMenuFlag = false;
boolean flagPresion = false;
boolean flagFlujo = false;
boolean flagFrecuencia = false;
boolean flagIE = false;

volatile unsigned long miTiempo = 0;

void IRAM_ATTR swInterrupt();
void IRAM_ATTR encoderInterrupt();

//Crear el objeto LCD con los numeros correspondientes (rs, en, d4, d5, d6, d7)
//LiquidCrystal lcd(11, 12, 7, 8, 9, 10);
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
void lcd_setup() {
    pinMode(A, INPUT_PULLUP);    // A como entrada
    pinMode(B, INPUT_PULLUP);    // B como entrada
    pinMode(SW, INPUT_PULLUP);   // SW como entrada
    // interrupcion sobre pin A con
    attachInterrupt(A, encoderInterrupt, RISING);
    attachInterrupt(SW, swInterrupt, RISING);
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
        //Serial.println(relacion_IE);
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
        lcd.print("   Configuracion    ");
        lcd.setCursor(0, 1);
        lcd.print("                    ");
        lcd.setCursor(0, 2);
        if (flagFrecuencia) {
            lcd.print(" ");
            lcd.write(126);
            lcd.print("Frec:            ");
        }
        else {
            lcd.print("  Frec:             ");
        }
        lcd.setCursor(10, 2);
        lcd.print(frecRespiratoria);
        lcd.setCursor(0, 3);
        if (flagIE) {
            lcd.print(" ");
            lcd.write(126);
            lcd.print("I:E:              ");
        }
        else {
            lcd.print("  I:E:              ");
        }
        lcd.setCursor(10, 3);
        lcd.print(relacion_IE);
        break;
    case 2:
        lcd.setCursor(0, 0);
        lcd.print("      Alarmas       ");
        lcd.setCursor(0, 1);
        lcd.print("                    ");
        lcd.setCursor(0, 2);
        if (flagPresion) {
            lcd.print(" ");
            lcd.write(126);
            lcd.print("Presion:            ");
        }
        else {
            lcd.print("  Presion:            ");
        }
        lcd.setCursor(12, 2);
        lcd.print(maxPresion);
        lcd.setCursor(0, 3);
        if (flagFlujo) {
            lcd.print(" ");
            lcd.write(126);
            lcd.print("Flujo:            ");
        }
        else {
            lcd.print("  Flujo:            ");
        }
        lcd.setCursor(12, 3);
        lcd.print(maxFlujo);
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
    //Serial.println("I am in lcd_show()");
}

void IRAM_ATTR swInterrupt() {
    static unsigned long ultimaInterrupcion = 0;
    unsigned long tiempoInterrupcion = millis();
    if (tiempoInterrupcion - ultimaInterrupcion > 5) {
        //Serial.println("I am in swInterrupt");
        if (menu == 1 && !insideMenuFlag) {
            insideMenuFlag = !insideMenuFlag;
            flagFrecuencia = true;
        }
        else if(menu == 1 && flagFrecuencia){
            flagFrecuencia = false;
            flagIE = true;
        }
        else if (menu == 1 && flagIE) {
            insideMenuFlag = !insideMenuFlag;
            flagIE = false;
        }
        else if (menu == 2 && !insideMenuFlag) {
            insideMenuFlag = !insideMenuFlag;
            flagPresion = true;
            Serial.println("Config maxPres");
        }
        else if (menu == 2 && flagPresion) {
            flagPresion = false;
            flagFlujo = true;
            Serial.println("Config maxFlujo");
        }
        else if (menu == 2 && flagFlujo) {
            insideMenuFlag = !insideMenuFlag;
            flagFlujo = false;
        }
        ultimaInterrupcion = tiempoInterrupcion;
    } 
}

void IRAM_ATTR encoderInterrupt() {
    static unsigned long ultimaInterrupcion = 0;
    unsigned long tiempoInterrupcion = millis();
    if (tiempoInterrupcion - ultimaInterrupcion > 5) {  // Antirrebote
        Serial.println("I am in encoderInterrupt");
        // Rutina para controlar el menu
        if (insideMenuFlag == false) {
            if (digitalRead(B) == HIGH) {
                menu++;
                if (menu < 0 || menu > MENU_QUANTITY - 1)
                    menu = 0;
            }
            else {
                menu--;
                if (menu < 0 || menu > MENU_QUANTITY - 1)
                    menu = MENU_QUANTITY - 1;
            }
            Serial.println("menu = " + String(menu));
        }
        else {
            switch (menu) {
            case 1:
                if (flagFrecuencia) {
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
                }
                else if (flagIE) {
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
                }
                //Serial.println("I :" + String(inspirationTime));
                //Serial.println("E :" + String(expirationTime));
                break;
            case 2:
                if (flagPresion) {
                    if (digitalRead(B) == HIGH) {
                        maxPresion++;
                        if (maxPresion > MAX_PRESION) {
                            maxPresion = MAX_PRESION;
                        }
                    }
                    else {
                        maxPresion--;
                        if (maxPresion > MAX_PRESION) {
                            maxPresion = MAX_PRESION;
                        }
                    }
                }
                else if(flagFlujo){
                    if (digitalRead(B) == HIGH) {
                        maxFlujo++;
                        if (maxFlujo > MAX_FLUJO) {
                            maxFlujo = MAX_FLUJO;
                        }
                    }
                    else {
                        maxFlujo--;
                        if (maxFlujo > MAX_FLUJO) {
                            maxFlujo = MAX_FLUJO;
                        }
                    }
                }
                break;
            }
        }
    
        ultimaInterrupcion = tiempoInterrupcion;
    }                                           
}


//#include <LiquidCrystal.h>
#include <LiquidCrystal_I2C.h>
//********DEFINICION DE PINES***********
#define A     4//32      //variable A a pin digital 2 (DT en modulo)
#define B     35//33     //variable B a pin digital 4 (CLK en modulo)
#define SW    34      //sw a pin digital 3 (SW en modulo)  

//**********VALORES MAXIMOS**********
#define MENU_QUANTITY 3
#define MAX_FREC 20
#define MAX_RIE 4
#define MAX_PRESION 40
#define MAX_FLUJO 40

volatile unsigned int menu = 0;
volatile unsigned long ultimaInterrupcionA = 0;
volatile unsigned long ultimaInterrupcionB = 0;
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
void IRAM_ATTR encoder_Interrupt();


//temporl var borrar
int band1=0;

//****************

//Crear el objeto LCD
LiquidCrystal_I2C lcd(0x27, 20,4);
void lcd_setup() {
    pinMode(A, INPUT_PULLUP);    // A como entrada
    pinMode(B, INPUT_PULLUP);    // B como entrada
    pinMode(SW, INPUT_PULLUP);   // SW como entrada
    // interrupcion sobre pin A con
    attachInterrupt(A, encoder_Interrupt, FALLING);
    attachInterrupt(SW, swInterrupt, RISING);
    lcd.init();
    lcd.backlight();
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
            Serial.println("Flujo:  ");
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

void IRAM_ATTR encoder_Interrupt() {
    static unsigned long debounceTimer = 0;
    if ((millis() - debounceTimer) > 100) {
        debounceTimer = millis();
        if (digitalRead(B) == HIGH) {
            band1=0;
        }    
        else {
            band1=1;
        }        
    }    
}

void refreshDisplayData() {
  static int cont=0;
  if (band1 == 1) {
    if (insideMenuFlag == false) {
      menu++;
      if (menu < 0 || menu > MENU_QUANTITY - 1)
        menu = 0;
      //Serial.println("menu = " + String(menu));
    }
    else {
      switch (menu) {
        case 1:
          if (flagFrecuencia) {
            frecRespiratoria++;
            if (frecRespiratoria > MAX_FREC) {
              frecRespiratoria = MAX_FREC;
            }
          }
          else if (flagIE) {

            relacionIE = relacionIE + 0.1;
            if (relacionIE >= MAX_RIE) {
              relacionIE = MAX_RIE;;
            }
            if (relacionIE > -1.0 && relacionIE < 0) {
              relacionIE = 1;

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

            maxPresion++;
            if (maxPresion > MAX_PRESION) {
              maxPresion = MAX_PRESION;
            }

          }
          else if (flagFlujo) {

            maxFlujo++;
            if (maxFlujo > MAX_FLUJO) {
              maxFlujo = MAX_FLUJO;
            }

          }
          break;
      }
    }

  }
  else if(band1==0) {
    if (insideMenuFlag == false) {
      menu--;
      if (menu < 0 || menu > MENU_QUANTITY - 1)
        menu = MENU_QUANTITY - 1;
      //Serial.println("menu = " + String(menu));
    }
    else {
      switch (menu) {
        case 1:
          if (flagFrecuencia) {
            if (digitalRead(B) == HIGH) {
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
              maxPresion--;
              if (maxPresion > MAX_PRESION) {
                maxPresion = MAX_PRESION;
              }
            }
          }
          else if (flagFlujo) {
            if (digitalRead(B) == HIGH) {
              maxFlujo--;
              if (maxFlujo > MAX_FLUJO) {
                maxFlujo = MAX_FLUJO;
              }
            }
          }
          break;
      }
    }
  }
  band1=3;
}

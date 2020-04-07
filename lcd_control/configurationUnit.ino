/*
 Name:		lcd_control.ino
 Created:	4/3/2020 11:40:24
 Author:	Helber Carvajal
*/

#include <LiquidCrystal_I2C.h>

//**********VALORES MAXIMOS**********
#define MENU_QUANTITY 3
#define MAX_FREC 40
#define MAX_RIE 40
#define MAX_PRESION 50
#define MAX_FLUJO 40

// UART Configuration
#define RXD1 3      // RX UART 0
#define TXD1 1      // TX UART 0
#define RXD2 16     // RX UART 2
#define TXD2 17     // TX UART 2

// Casos Menu
#define ALE_PRES_PIP        4  // presion pico
#define ALE_PRES_DES        5  // desconexion del paciente
#define ALE_GENERAL         6  // fallo general
#define BATERIA             7  // Bateria

// variables para la medicion de milisegundos del programa
unsigned long tiempoInterrupcion = 0;
static unsigned long ultimaInterrupcion = 0;
static unsigned long debounceUltima_A = 0;
unsigned long tiempoDebounce_A = 0;
static unsigned long debounceUltima_B = 0;
unsigned long tiempoDebounce_B = 0;
unsigned int debounceVal = 1;  // valor para el antirrebote

//********ENCODER SETUP***********
#define A     19     //variable A a pin digital 2 (DT en modulo)
#define B     18     //variable B a pin digital 4 (CLK en modulo)
#define SW    5      //sw a pin digital 3 (SW en modulo)  

volatile unsigned int menu = 0;
volatile unsigned int menuImprimir = 0;
volatile unsigned int menuAlerta = 0;
bool flagAlerta = false;
unsigned int contAlertas = 0;

// Global al ser usada en loop e ISR (encoder)
unsigned long tiempo1 = 0;
int relacionIE = 20;
String relacion_IE = "1:4.1";
byte maxPresion = 30;
float maxFlujo = 4;
byte frecRespiratoria = 12;
byte I = 1;
byte E = 41;

// Banderas utilizadas en las interrupciones
boolean insideMenuFlag = false;
boolean flagPresion = false;
boolean flagFlujo = false;
boolean flagFrecuencia = false;
boolean flagIE = false;
int flagEncoder = 0;

// variables para la atencion de interrupciones
bool flagTimerInterrupt = false;
volatile bool flagSwInterrupt = false;
volatile bool flagEncoderInterrupt_A = false;
volatile bool flagEncoderInterrupt_B = false;
volatile bool flagDettachInterrupt_A = false;
volatile bool flagDettachInterrupt_B = false;
volatile bool flagDetach = false;
volatile unsigned int contDetach = 0;
unsigned int contAlarm = 0;


float Peep = 0;
float Ppico = 0;
float VT = 0;

int alerPresionPIP = 0;
int alerDesconexion = 0;
int alerGeneral = 0;
int alerBateria = 0;

LiquidCrystal_I2C lcd(0x27, 20, 4);   // Objeto LCD

// inicializacion del contador del timer
hw_timer_t* timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

// Interrupt routines
void IRAM_ATTR onTimer();  // funcion de interrupcion
void IRAM_ATTR swInterrupt();
void IRAM_ATTR encoderInterrupt_A();
void IRAM_ATTR encoderInterrupt_B();

// the setup function runs once when you press reset or power the board
void setup() {
    // Configuracion del timer a 1 kHz
    timer = timerBegin(0, 80, true);               // Frecuencia de reloj 80 MHz, prescaler de 80, frec 1 MHz
    timerAttachInterrupt(timer, &onTimer, true);
    timerAlarmWrite(timer, 1000, true);             // Interrupcion cada 1000 conteos del timer, es decir 100 Hz
    timerAlarmEnable(timer);                        // Habilita interrupcion por timer

    //Encoder setup
    pinMode(A, INPUT_PULLUP);    // A como entrada
    pinMode(B, INPUT_PULLUP);    // B como entrada
    pinMode(SW, INPUT_PULLUP);   // SW como entrada

    attachInterrupt(digitalPinToInterrupt(A), encoderInterrupt_A, FALLING);
    attachInterrupt(digitalPinToInterrupt(B), encoderInterrupt_B, FALLING);
    attachInterrupt(digitalPinToInterrupt(SW), swInterrupt, RISING);

    Serial.begin(9600);
    Serial2.begin(9600);
    lcd.init();
    lcd.backlight();
    lcd_show();
}

// the loop function runs over and over again until power down or reset
void loop() {
    
    receiveData();

    // Timer interrupt
    if (flagTimerInterrupt) {
        portENTER_CRITICAL(&timerMux);
        flagTimerInterrupt = false;
        portEXIT_CRITICAL(&timerMux);
        
        // ***********************************************
    // **** Atencion a las alertas
    // ***********************************************

        if (alerPresionPIP == 1) {
            menuAlerta = ALE_PRES_PIP;
            flagAlerta = true;
            //Serial.println('1');
        }
        if (alerDesconexion == 1) {
            menuAlerta = ALE_PRES_DES;
            flagAlerta = true;
            //Serial.println('2');
        }
        if (alerGeneral == 1) {
            menuAlerta = ALE_GENERAL;
            flagAlerta = true;
            //Serial.println('3');
        }
        if (alerBateria == 1) {
            menuAlerta = BATERIA;
            flagAlerta = true;
            //Serial.println('4');
        }
        if ((alerPresionPIP == 0) && (alerDesconexion == 0) && (alerGeneral == 0) && (alerBateria == 0)) {
            menuAlerta = 0;
            flagAlerta = false;
            //Serial.print('0');
        }

        if (flagAlerta == true) {
            contAlertas++;
            if (contAlertas == 300) {
                lcd.backlight();
            }
            else if (contAlertas == 1000) {
                menuImprimir = menuAlerta;
                lcd.noBacklight();
                lcd_show();

            }
            else if (contAlertas == 1300) {
                lcd.backlight();
            }
            else if (contAlertas == 2000) {
                menuImprimir = menu;
                lcd.noBacklight();
                lcd_show();
                contAlertas = 0;
            }
        }
        else {
            contAlertas++;
            if (contAlertas == 1000) {
                menuImprimir = menu;
                lcd.backlight();
                lcd_show();

            }
            else if (contAlertas == 2000) {
                menuImprimir = menu;
                lcd_show();
                contAlertas = 0;
            }
        }
    }

    // *************************************************
    // **** Atencion a rutina de interrupcion por switch
    // *************************************************
    if (flagSwInterrupt) {
        flagSwInterrupt = false;
        // Atencion a la interrupcion
        swInterruptAttention();
    }  // Final interrupcion Switch


    // *************************************************
    // **** Atencion a rutina de interrupcion por encoder
    // *************************************************
    if (flagEncoderInterrupt_A) {
        flagEncoderInterrupt_A = false;
        // Atencion a la interrupcion
        encoderInterruptAttention_A();
    }
    // Final interrupcion encoder


    // *************************************************
    // **** Atencion a rutina de interrupcion por encoder
    // *************************************************
    if (flagEncoderInterrupt_B) {
        flagEncoderInterrupt_B = false;
        // Atencion a la interrupcion
        encoderInterruptAttention_B();
    }// Final interrupcion encoder
}

void IRAM_ATTR onTimer() {
    portENTER_CRITICAL_ISR(&timerMux);
    flagTimerInterrupt = true;
    portEXIT_CRITICAL_ISR(&timerMux);
}

void lcd_show() {

    if (relacionIE > 0) {
        relacion_IE = "1:" + String((float)relacionIE / 10, 1);
        I = 1;
        E = (char)relacionIE;
    }
    else {
        relacion_IE = String(-(float)relacionIE / 10, 1) + ":1";
        I = (char)(-relacionIE);
        E = 1;
    }
    //Serial.println("IE = " + String(I) + ':' + String(E));
    //Serial.println(relacionIE);

    switch (menuImprimir) {
    case 0:
        // lcd.home();
        lcd.setCursor(0, 0);
        lcd.print("  InnspiraMED UdeA  ");
        lcd.setCursor(0, 1);
        lcd.print("FR        PIP     ");
        lcd.setCursor(4, 1);
        lcd.print(frecRespiratoria);
        lcd.setCursor(14, 1);
        lcd.print(String(Ppico, 1));
        lcd.setCursor(0, 2);
        lcd.print("I:E       PEEP      ");
        lcd.setCursor(4, 2);
        lcd.print(relacion_IE);
        lcd.setCursor(15, 2);
        lcd.print(String(Peep, 1));
        lcd.setCursor(0, 3);
        lcd.print("VT                  ");
        lcd.setCursor(4, 3);
        lcd.print(String(VT, 1));
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
            lcd.print("Frec:             ");
        }
        else {
            lcd.print("  Frec:              ");
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
            lcd.print("PIP:    cmH2O     ");
        }
        else {
            lcd.print("  PIP:    cmH2O     ");
        }
        lcd.setCursor(7, 2);
        lcd.print(maxPresion);
        lcd.setCursor(0, 3);
        lcd.print("                    ");
        //if (flagFlujo) {
        //    lcd.print(" ");
        //    lcd.write(126);
        //    lcd.print("Flujo:            ");
        //    //Serial.println("Flujo:  ");
        //}
        //else {
        //    lcd.print("  Flujo:            ");
        //}
        //lcd.setCursor(12, 3);
        //lcd.print(maxFlujo);
        break;

    case ALE_PRES_PIP:
        lcd.setCursor(0, 0);
        lcd.print("       ALERTA       ");
        lcd.setCursor(0, 1);
        lcd.print("                    ");
        lcd.setCursor(0, 2);
        lcd.print("Presion PIP superada");
        lcd.setCursor(0, 3);
        lcd.print("                    ");
        break;

    case ALE_PRES_DES:
        lcd.setCursor(0, 0);
        lcd.print("       ALERTA       ");
        lcd.setCursor(0, 1);
        lcd.print("                    ");
        lcd.setCursor(0, 2);
        lcd.print("Desconexion paciente");
        lcd.setCursor(0, 3);
        lcd.print("                    ");
        break;

    case ALE_GENERAL:
        lcd.setCursor(0, 0);
        lcd.print("       ALERTA       ");
        lcd.setCursor(0, 1);
        lcd.print("                    ");
        lcd.setCursor(0, 2);
        lcd.print("   Fallo general    ");
        lcd.setCursor(0, 3);
        lcd.print("                    ");
        break;

    case BATERIA:
        lcd.setCursor(0, 0);
        lcd.print("       ALERTA       ");
        lcd.setCursor(0, 1);
        lcd.print("                    ");
        lcd.setCursor(0, 2);
        lcd.print("      Bateria       ");
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
    //Serial.println("I am in lcd_show()");
}


void IRAM_ATTR swInterrupt() {
    portENTER_CRITICAL_ISR(&mux);
    flagSwInterrupt = true;
    tiempoInterrupcion = millis();
    portEXIT_CRITICAL_ISR(&mux);
}

void swInterruptAttention() {
    if (tiempoInterrupcion - ultimaInterrupcion > 100) {
        // portENTER_CRITICAL_ISR(&mux);
        switchRoutine();
        // Serial.println(tiempoInterrupcion - ultimaInterrupcion);
        // portEXIT_CRITICAL_ISR(&mux);
        ultimaInterrupcion = tiempoInterrupcion;
    }
}

// Interrupcion por encoder
void IRAM_ATTR encoderInterrupt_A() {
    portENTER_CRITICAL_ISR(&mux);
    flagEncoderInterrupt_A = true;
    tiempoDebounce_A = millis();
    portEXIT_CRITICAL_ISR(&mux);
}
// Interrupcion por encoder
void IRAM_ATTR encoderInterrupt_B() {
    portENTER_CRITICAL_ISR(&mux);
    flagEncoderInterrupt_B = true;
    tiempoDebounce_B = millis();
    portEXIT_CRITICAL_ISR(&mux);
}

void encoderInterruptAttention_A() {
    if (tiempoDebounce_A - debounceUltima_A > debounceVal) {
        // Serial.print("A = ");
        // Serial.println(tiempoDebounce_A - debounceUltima_A);
        if ((digitalRead(B) == HIGH) && (digitalRead(A) == LOW)) {
            // portENTER_CRITICAL_ISR(&mux);
            flagEncoder = 0;
            flagDettachInterrupt_B = true;
            contDetach = 0;
            encoderRoutine();
            // portEXIT_CRITICAL_ISR(&mux);
        }
        debounceUltima_A = tiempoDebounce_A;
    }
}

void encoderInterruptAttention_B() {
    if (tiempoDebounce_B - debounceUltima_B > debounceVal) {
        // Serial.print("B = ");
        // Serial.println(tiempoDebounce_B - debounceUltima_B);
        if ((digitalRead(A) == HIGH) && (digitalRead(B) == LOW)) {
            // portENTER_CRITICAL_ISR(&mux);
            flagEncoder = 1;
            flagDettachInterrupt_A = true;
            contDetach = 0;
            encoderRoutine();
            // portEXIT_CRITICAL_ISR(&mux);
        }
        debounceUltima_B = tiempoDebounce_B;
    }
}

void encoderRoutine() {
    static int cont = 0;
    // Incremento
    if (flagEncoder == 1) {
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
                    relacionIE = relacionIE + 1;
                    if (relacionIE >= MAX_RIE) {
                        relacionIE = MAX_RIE;;
                    }
                    if (relacionIE > -10 && relacionIE < 0) {
                        relacionIE = 10;
                    }
                }
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
    // Decremento
    else if (flagEncoder == 0) {
        if (insideMenuFlag == false) {
            menu--;
            if (menu < 0 || menu > MENU_QUANTITY - 1)
                menu = MENU_QUANTITY - 1;
        }
        else {
            switch (menu) {
            case 1:
                if (flagFrecuencia) {
                    frecRespiratoria--;
                    if (frecRespiratoria > MAX_FREC) {
                        frecRespiratoria = 0;
                    }
                }
                else if (flagIE) {
                    relacionIE = relacionIE - 1;
                    if (relacionIE <= -MAX_RIE) {
                        relacionIE = -MAX_RIE;
                    }
                    if (relacionIE > 0 && relacionIE < 10) {
                        relacionIE = -10;
                    }
                }
                break;
            case 2:
                if (flagPresion) {
                    maxPresion--;
                    if (maxPresion > MAX_PRESION) {
                        maxPresion = 0;
                    }
                }
                else if (flagFlujo) {
                    maxFlujo--;
                    if (maxFlujo > MAX_FLUJO) {
                        maxFlujo = MAX_FLUJO;
                    }
                }
                break;
            }
        }
    }
    // portENTER_CRITICAL_ISR(&mux);
    flagEncoder = 3;
    menuImprimir = menu;
    lcd_show();
    // portEXIT_CRITICAL_ISR(&mux);
}

void switchRoutine() {
    // Serial.println("SW MENU");
    if (menu == 1 && !insideMenuFlag) {
        insideMenuFlag = !insideMenuFlag;
        flagFrecuencia = true;
        // Serial.println("SW MENU 1_1");
    }
    else if (menu == 1 && flagFrecuencia) {
        flagFrecuencia = false;
        flagIE = true;
        // Serial.println("SW MENU 1_2");
    }
    else if (menu == 1 && flagIE) {
        insideMenuFlag = !insideMenuFlag;
        flagIE = false;
        sendSerialData();
    }
    else if (menu == 2 && !insideMenuFlag) {
        insideMenuFlag = !insideMenuFlag;
        flagPresion = true;
        //Serial.println("Config maxPres");
    }
    else if (menu == 2 && flagPresion) {
        flagPresion = false;
        //flagFlujo = true;
        insideMenuFlag = !insideMenuFlag;
        //Serial.println("Config maxFlujo");
        sendSerialData();
    }
    /*else if (menu == 2 && flagFlujo) {
        insideMenuFlag = !insideMenuFlag;
        flagFlujo = false;
    }*/
    menuImprimir = menu;
    lcd_show();
}

// Function to receive data from serial communication
void receiveData() {
    if (Serial2.available() > 5) {
        Serial.println("Inside receiveData");
        String dataIn = Serial2.readStringUntil(';');
        int contComas = 0;
        for (int i = 0; i < dataIn.length(); i++) {
            if (dataIn[i] == ',') {
                contComas++;
            }
        }
        String dataIn2[40];
        for (int i = 0; i < contComas + 1; i++) {
            dataIn2[i] = dataIn.substring(0, dataIn.indexOf(','));
            dataIn = dataIn.substring(dataIn.indexOf(',') + 1);
        }
        //cargue los datos aqui
        //para entero
        //contCiclos =dataIn2[0].toInt();
        //para float
        Ppico = dataIn2[0].toFloat();
        Peep = dataIn2[1].toFloat();
        VT = dataIn2[2].toFloat();
        alerPresionPIP = dataIn2[3].toInt();
        alerDesconexion = dataIn2[4].toInt();
        alerGeneral = dataIn2[5].toInt();
        alerBateria = dataIn2[6].toInt();
        Serial2.flush();
        //Serial.println(String(frecRespiratoria) + ',' + String(I) + ',' + String(E));
        /*for (int i = 0; i < contComas + 1; i++) {
            Serial.println(dataIn2[i]);
        }*/
    }
}

void sendSerialData() {
    String dataToSend = String(frecRespiratoria) + ',' + String(I) + ',' + String(E) + ',' + String(maxPresion) + ';';
    Serial2.print(dataToSend);
}



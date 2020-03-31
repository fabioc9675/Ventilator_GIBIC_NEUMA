// funciones para el control de la configracion del monitor
extern volatile bool flagSwInterrupt;
extern volatile bool flagEncoderInterrupt_A;
extern volatile bool flagEncoderInterrupt_B;
extern volatile bool flagDettachInterrupt_A;
extern volatile bool flagDettachInterrupt_B;
extern volatile bool flagDetach;
extern volatile unsigned int contDetach;


// variables para la medicion de milisegundos del programa
unsigned long tiempoInterrupcion = 0;
static unsigned long ultimaInterrupcion = 0;
static unsigned long debounceUltima_A = 0;
unsigned long tiempoDebounce_A = 0;
static unsigned long debounceUltima_B = 0;
unsigned long tiempoDebounce_B = 0;
unsigned int debounceVal = 1;  // valor para el antirrebote


// Interrupcion por presion del switch
void IRAM_ATTR swInterrupt() {
  portENTER_CRITICAL_ISR(&mux);
  flagSwInterrupt = true;
  tiempoInterrupcion = millis();
  portEXIT_CRITICAL_ISR(&mux);
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

void swInterruptAttention() {

  if (tiempoInterrupcion - ultimaInterrupcion > 100) {
    // portENTER_CRITICAL_ISR(&mux);
    switchRoutine();
    // Serial.println(tiempoInterrupcion - ultimaInterrupcion);
    // portEXIT_CRITICAL_ISR(&mux);
    ultimaInterrupcion = tiempoInterrupcion;
  }
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
    // Serial.println("SW MENU 1_3");
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

  lcd_show();

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
  // Decremento
  else if (flagEncoder == 0) {
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
              frecRespiratoria--;
              if (frecRespiratoria > MAX_FREC) {
                frecRespiratoria = 0;
              }
          }
          else if (flagIE) {
              relacionIE = relacionIE - 0.1;
              if (relacionIE <= -MAX_RIE) {
                relacionIE = -MAX_RIE;
              }
              if (relacionIE > 0 && relacionIE < 1) {
                relacionIE = -1;
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
              maxPresion--;
              if (maxPresion > MAX_PRESION) {
                maxPresion = MAX_PRESION;
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
  //lcd_show();

  // portEXIT_CRITICAL_ISR(&mux);
}

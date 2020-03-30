

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
            encoderFlag=0;
        }    
        else {
            encoderFlag=1;
        }        
    }    
}

void refreshData() {
  static int cont=0;
  if (encoderFlag == 1) {
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
  else if(encoderFlag==0) {
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
  encoderFlag=3;
}

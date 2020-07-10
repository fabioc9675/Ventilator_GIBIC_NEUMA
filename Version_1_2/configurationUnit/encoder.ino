/*
 * File:   encoder.ino
 * Author: GIBIC UdeA
 *
 * Created on July 4, 2020, 13:41 PM
 */

/** ****************************************************************************
 ** ************ INCLUDES ******************************************************
 ** ****************************************************************************/
#include "encoder.h"

/** ****************************************************************************
 ** ************ DEFINES *******************************************************
 ** ****************************************************************************/

/** ****************************************************************************
 ** ************ EXTERN VARIABLES **********************************************
 ** ****************************************************************************/
// **********************************************************
// Variables de manejo de semaforos
extern SemaphoreHandle_t xSemaphoreEncoder;
extern portMUX_TYPE mux;

// bandera de activacion de encoder
extern volatile uint8_t flagAEncoder;
extern volatile uint8_t flagBEncoder;
extern volatile uint8_t flagSEncoder;

extern volatile uint8_t flagDetachInterrupt_A;
extern volatile uint8_t flagDetachInterrupt_B;
extern volatile uint8_t flagDetachInterrupt_A_B;
extern volatile uint8_t flagDetachInterrupt_B_A;
extern volatile uint8_t flagDetachInterrupt_S;

extern unsigned int contDetachA;
extern unsigned int contDetachB;
extern unsigned int contDetachS;

// Variable de estado del encoder
extern unsigned int fl_StateEncoder;
// variables de estado de ventilacion
extern int stateMachine;
extern int newVentilationMode;

// variable de estado de menu
extern volatile uint8_t insideMenuFlag;

// banderas de cambio de valores
extern volatile uint8_t flagFrecuencia;
extern volatile uint8_t flagPresion;
extern volatile uint8_t flagFlujo;
extern volatile uint8_t flagTrigger;
extern volatile uint8_t flagIE;
extern volatile uint8_t flagPeepMax;
extern volatile uint8_t flagSensibilityCPAP;
extern volatile uint8_t flagMode;
extern volatile uint8_t flagConfirm;
extern volatile uint8_t flagMinFR;
extern volatile uint8_t flagVE;

// contadores de configuraciones en el menu de usuario
extern int newFrecRespiratoria;
extern int newRelacionIE;
extern int currentFrecRespiratoria;
extern int currentRelacionIE;
extern int trigger;
extern int PeepMax;
extern int maxPresion;
extern int maxFR;
extern int maxVE;

// variables de introduccion a los menus de configuracion
extern int optionConfigMenu;
extern int currentVentilationMode;
extern int optionVentMenu;

// Variables para iniciar impresion de datos en menu
extern volatile unsigned int menuImprimir;
extern volatile unsigned int lineaAlerta;
extern volatile uint8_t flagAlreadyPrint;

/** ****************************************************************************
 ** ************ VARIABLES *****************************************************
 ** ****************************************************************************/
// variables de menu
volatile signed int menu = MAIN_MENU;

unsigned int contRecogIntA = 0; // contador para reconocimiento del conteo del encoder
unsigned int contRecogIntB = 0; // contador para reconocimiento del conteo del encoder

/** ****************************************************************************
 ** ************ FUNCTIONS *****************************************************
 ** ****************************************************************************/
// Ejecucion de incremento o decremento del encoder
void encoderRoutine(void)
{
    switch (fl_StateEncoder)
    {
        // Incremento
    case ENCOD_INCREASE:
        if (insideMenuFlag == false)
        {
            menu++;
            if (menu < 0 || menu > MENU_QUANTITY - 1)
                menu = 0;
            //Serial.println("menu = " + String(menu));
        }
        else
        {
            switch (menu)
            {
            case CONFIG_MENU:
                if (flagFrecuencia)
                {
                    newFrecRespiratoria++;
                    if (newFrecRespiratoria > MAX_FREC)
                    {
                        newFrecRespiratoria = MAX_FREC;
                    }
                }
                else if (flagIE)
                {
                    newRelacionIE++;
                    if (newRelacionIE >= MAX_RIE)
                    {
                        newRelacionIE = MAX_RIE;
                    }
                    if (newRelacionIE >= 10 && newRelacionIE < 0)
                    {
                        newRelacionIE = 10;
                    }
                }
                else if (flagTrigger == true)
                {
                    trigger++;
                    if (trigger > MAX_TRIGGER)
                    {
                        trigger = MAX_TRIGGER;
                    }
                }
                else if (flagPeepMax == true)
                {
                    PeepMax++;
                    if (PeepMax > MAX_PEEP)
                    {
                        PeepMax = MAX_PEEP;
                    }
                }
                else if (insideMenuFlag == true)
                {
                    optionConfigMenu++;
                    if (currentVentilationMode == 0)
                    {
                        if (optionConfigMenu > 2)
                        {
                            optionConfigMenu = 0;
                        }
                    }
                    else if (currentVentilationMode == 1)
                    {
                        if (optionConfigMenu > 3)
                        {
                            optionConfigMenu = 0;
                        }
                    }
                    else
                    {
                        if (optionConfigMenu > 1)
                        {
                            optionConfigMenu = 0;
                        }
                    }
                }
                break;
            case CONFIG_ALARM:
                if (flagPresion == true)
                {
                    maxPresion++;
                    if (maxPresion > MAX_PRESION)
                    {
                        maxPresion = MAX_PRESION;
                    }
                }
                else if (flagMinFR == true)
                {
                    maxFR++;
                    if (maxFR > MAX_MAX_FR)
                    {
                        maxFR = MAX_MAX_FR;
                    }
                }
                else if (flagVE == true)
                {
                    maxVE++;
                    if (maxVE > MAX_MAX_VE)
                    {
                        maxVE = MAX_MAX_VE;
                    }
                }
                else if (insideMenuFlag == true)
                {
                    optionConfigMenu++;
                    if (optionConfigMenu > 3)
                    {
                        optionConfigMenu = 0;
                    }
                }
                break;
            case VENT_MENU:
                optionVentMenu++;
                if (optionVentMenu > 3)
                {
                    optionVentMenu = 0;
                }
                break;
            case CONFIRM_MENU:
                flagConfirm = !flagConfirm;
                break;
            }
        }
        fl_StateEncoder = 0;
        break;
    // Decremento
    case ENCOD_DECREASE:
        if (insideMenuFlag == false)
        {
            menu--;
            if (menu < 0 || menu > MENU_QUANTITY - 1)
                menu = MENU_QUANTITY - 1;
        }
        else
        {
            switch (menu)
            {
            case CONFIG_MENU:
                if (flagFrecuencia)
                {
                    newFrecRespiratoria--;
                    if (newFrecRespiratoria < MIN_FREC)
                    {
                        newFrecRespiratoria = MIN_FREC;
                    }
                }
                else if (flagIE)
                {
                    newRelacionIE--;
                    if (newRelacionIE <= -MIN_RIE)
                    {
                        newRelacionIE = -MIN_RIE;
                    }
                    if (newRelacionIE < 10 && newRelacionIE > 0)
                    {
                        newRelacionIE = 10;
                    }
                }
                else if (flagTrigger == true)
                {
                    trigger--;
                    if (trigger < MIN_TRIGGER)
                    {
                        trigger = MIN_TRIGGER;
                    }
                }
                else if (flagPeepMax == true)
                {
                    PeepMax--;
                    if (PeepMax < MIN_PEEP)
                    {
                        PeepMax = MIN_PEEP;
                    }
                }
                else if (insideMenuFlag == true)
                {
                    optionConfigMenu--;
                    if (currentVentilationMode == 0)
                    {
                        if (optionConfigMenu > 2)
                        {
                            optionConfigMenu = 2;
                        }
                    }
                    else if (currentVentilationMode == 1)
                    {
                        if (optionConfigMenu > 3)
                        {
                            optionConfigMenu = 3;
                        }
                    }
                    else
                    {
                        if (optionConfigMenu > 1)
                        {
                            optionConfigMenu = 1;
                        }
                    }
                }
                break;
            case CONFIG_ALARM:
                if (flagPresion == true)
                {
                    maxPresion--;
                    if (maxPresion < MIN_PRESION)
                    {
                        maxPresion = MIN_PRESION;
                    }
                }
                else if (flagMinFR == true)
                {
                    maxFR--;
                    if (maxFR > MAX_MAX_FR)
                    {
                        maxFR = MIN_MAX_FR;
                    }
                }
                else if (flagVE == true)
                {
                    maxVE--;
                    if (maxVE > MAX_MAX_VE)
                    {
                        maxVE = MIN_MAX_VE;
                    }
                }
                else if (insideMenuFlag == true)
                {
                    optionConfigMenu--;
                    if (optionConfigMenu > 3)
                    {
                        optionConfigMenu = 3;
                    }
                }
                break;
            case VENT_MENU:
                optionVentMenu--;
                if (optionVentMenu > 3)
                {
                    optionVentMenu = 3;
                }
                break;
            case CONFIRM_MENU:
                flagConfirm = !flagConfirm;
                break;
            }
        }
        fl_StateEncoder = 0;
        break;
    default:
        fl_StateEncoder = 0;
        break;
    }

    menuImprimir = menu;
    lineaAlerta = menu;
    flagAlreadyPrint = false;
}

// Ejecucion de tarea de Switch
void switchRoutine(void)
{
    // Serial.println("SW MENU");
    switch (menu)
    {
    case CONFIG_MENU:
        if (optionConfigMenu == 0)
        {
            if (insideMenuFlag == false)
            {
                insideMenuFlag = true;
            }
            else
            {
                insideMenuFlag = false;
                menu = MAIN_MENU;
                sendSerialData();
            }
        }
        else if (currentVentilationMode == 2)
        {
            if (optionConfigMenu == 1)
            {
                flagPeepMax = !flagPeepMax;
            }
        }
        else if (currentVentilationMode == 1)
        {
            if (optionConfigMenu == 1)
            {
                flagTrigger = !flagTrigger;
            }
            if (optionConfigMenu == 2)
            {
                flagFrecuencia = !flagFrecuencia;
                currentFrecRespiratoria = newFrecRespiratoria;
            }
            if (optionConfigMenu == 3)
            {
                flagIE = !flagIE;
                currentRelacionIE = newRelacionIE;
            }
        }
        else
        {
            if (optionConfigMenu == 1)
            {
                flagFrecuencia = !flagFrecuencia;
                currentFrecRespiratoria = newFrecRespiratoria;
            }
            if (optionConfigMenu == 2)
            {
                flagIE = !flagIE;
                currentRelacionIE = newRelacionIE;
            }
        }
        break;
    case VENT_MENU:
        if (optionVentMenu == 0)
        {
            insideMenuFlag = !insideMenuFlag;
        }
        else if (optionVentMenu == 1)
        {
            menu = CONFIG_MENU;
            if (stateMachine != STANDBY_STATE)
            {
                stateMachine = PCMV_STATE;
            }
            currentVentilationMode = 0;
            optionVentMenu = 0;
        }
        else if (optionVentMenu == 2)
        {
            menu = CONFIG_MENU;
            if (stateMachine != STANDBY_STATE)
            {
                stateMachine = AC_STATE;
            }
            currentVentilationMode = 1;
            optionVentMenu = 0;
        }
        else if (optionVentMenu == 3)
        {
            menu = CONFIG_MENU;
            if (stateMachine != STANDBY_STATE)
            {
                stateMachine = CPAP_STATE;
            }
            currentVentilationMode = 2;
            optionVentMenu = 0;
        }
        break;
    case CPAP_MENU:
        menu = CONFIRM_MENU;
        break;
    case CONFIRM_MENU:
        insideMenuFlag = !insideMenuFlag;
        menu = CONFIG_MENU;
        if (flagConfirm == true)
        {
            flagConfirm = false;
            currentFrecRespiratoria = newFrecRespiratoria;
            currentRelacionIE = newRelacionIE;
            currentVentilationMode = newVentilationMode;
            sendSerialData();
        }
        else
        {
            newFrecRespiratoria = currentFrecRespiratoria;
            newRelacionIE = currentRelacionIE;
            newVentilationMode = currentVentilationMode;
        }
        break;
    case CONFIG_ALARM:
        if (optionConfigMenu == 0)
        {
            if (insideMenuFlag == false)
            {
                insideMenuFlag = true;
            }
            else
            {
                insideMenuFlag = false;
                sendSerialData();
            }
        }
        else if (optionConfigMenu == 1)
        {
            flagPresion = !flagPresion;
        }
        else if (optionConfigMenu == 2)
        {
            flagMinFR = !flagMinFR;
        }
        else if (optionConfigMenu == 3)
        {
            flagVE = !flagVE;
        }
        break;
    }
    menuImprimir = menu;
    lineaAlerta = menu;
    flagAlreadyPrint = false;
}

/****************************************************************************
 ***** Atencion a interrupcion por encoder **********************************
 ****************************************************************************/
void task_Encoder(void *arg)
{
    while (1)
    {
        // Espero por la notificacion de la ISR por A
        if (xSemaphoreTake(xSemaphoreEncoder, portMAX_DELAY) == pdTRUE)
        {
            if (flagAEncoder == true)
            {
                portENTER_CRITICAL(&mux);
                flagAEncoder = false;
                portEXIT_CRITICAL(&mux);

                if ((digitalRead(B) == HIGH) && (digitalRead(A) == LOW))
                {
                    detachInterrupt(digitalPinToInterrupt(A));
                    detachInterrupt(digitalPinToInterrupt(B));
                    flagDetachInterrupt_A = true;
                    flagDetachInterrupt_B_A = true;
                    contDetachA = 0;
                    fl_StateEncoder = ENCOD_INCREASE;
                    // ejecucion de la tarea de incremento, esta funcion se coloca por sensibildad del encoder,
                    // si se cambia por menor sensibilidad, solo necesitara ejecutar la funcion  encoderRoutine();
                    contRecogIntA++;
                    if (contRecogIntA == ENCOD_COUNT)
                    {
                        contRecogIntA = 0;
                        encoderRoutine();
                    }
                }
            }

            if (flagBEncoder == true)
            {
                portENTER_CRITICAL(&mux);
                flagBEncoder = false;
                portEXIT_CRITICAL(&mux);

                if ((digitalRead(A) == HIGH) && (digitalRead(B) == LOW))
                {
                    detachInterrupt(digitalPinToInterrupt(B));
                    detachInterrupt(digitalPinToInterrupt(A));
                    flagDetachInterrupt_B = true;
                    flagDetachInterrupt_A_B = true;
                    contDetachB = 0;
                    fl_StateEncoder = ENCOD_DECREASE;
                    // ejecucion de la tarea de decremento, esta funcion se coloca por sensibildad del encoder,
                    // si se cambia por menor sensibilidad, solo necesitara ejecutar la funcion  encoderRoutine();
                    contRecogIntB++;
                    if (contRecogIntB == ENCOD_COUNT)
                    {
                        contRecogIntB = 0;
                        encoderRoutine();
                    }
                }
            }

            if (flagSEncoder == true)
            {
                portENTER_CRITICAL(&mux);
                flagSEncoder = false;
                portEXIT_CRITICAL(&mux);

                detachInterrupt(digitalPinToInterrupt(SW));
                flagDetachInterrupt_S = true;
                contDetachS = 0;
                // ejecucion de la tarea de Switch
                contRecogIntA = 0;
                contRecogIntB = 0;
                switchRoutine();
            }
        }
    }
}

/** ****************************************************************************
 ** ************ END OF THE CODE ***********************************************
 ** ****************************************************************************/

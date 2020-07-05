/*
 * File:   calibrationMenu.ino
 * Author: GIBIC UdeA
 *
 * Created on July 4, 2020, 13:41 PM
 */

/** ****************************************************************************
 ** ************ INCLUDES ******************************************************
 ** ****************************************************************************/
#include "calibrationMenu.h"

/** ****************************************************************************
 ** ************ DEFINES *******************************************************
 ** ****************************************************************************/

/** ****************************************************************************
 ** ************ EXTERN VARIABLES **********************************************
 ** ****************************************************************************/
// **********************************************************
// manejadores de las tareas
extern xTaskHandle serviceTaskHandle;

extern String menuString;
extern String SerialID;

// bandera de modo calibracion
extern uint8_t flagService;
extern uint8_t flagFlowPrintCalibration;
extern uint8_t flagPcamPrintCalibration;
extern uint8_t flagPbagPrintCalibration;
extern uint8_t flagPpacPrintCalibration;
extern uint8_t flagFlowSitePrintCalibration;
extern uint8_t flagPcamSitePrintCalibration;
extern uint8_t flagPbagSitePrintCalibration;
extern uint8_t flagPpacSitePrintCalibration;

extern uint8_t flagService;
extern uint8_t flagRestartTask;

/** ****************************************************************************
 ** ************ VARIABLES *****************************************************
 ** ****************************************************************************/
uint8_t servMenuStateCurrent = SERV_NULL_MENU;
uint8_t servMenuStateNew = SERV_NULL_MENU;
uint8_t modeCalibration = MODE_NULL;
uint8_t coeftype = MODE_NULL;
uint8_t placeCalibration = MODE_NULL; // variable para especificar si es calibracion de fabrica o de sitio

/** ****************************************************************************
 ** ************ FUNCTIONS *****************************************************
 ** ****************************************************************************/

// Menu principal de calibracion
void printMainMenu(void)
{
    menuString = "\n\n************ Menu Principal ************\n"
                 "1. Alimentacion. \n"
                 "2. Estado de valvulas. \n"
                 "3. Coeficientes de fabrica. \n"
                 "4. Coeficientes de sitio. \n"
                 "5. Salir. \n\nSeleccione una opcion: ";

    Serial.print(menuString);
    servMenuStateNew = SERV_WAIT_MAIN;
    servMenuStateCurrent = servMenuStateNew;
}

// Menu de calibracion de fabrica
void printFactoryMenu(void)
{
    menuString = "\n\n*** Calibracion coeficientes fabrica ***\n"
                 "1. Cambiar Serial del Equipo (" +
                 SerialID + ").\n"
                            "2. Flujo inspiratorio.\n"
                            "3. Flujo expiratorio.\n"
                            "4. Presion Camara.\n"
                            "5. Presion Bolsa.\n"
                            "6. Presion Paciente.\n"
                            "7. Escala de volumen.\n"
                            "8. Menu anterior. \n"
                            "9. Salir.\n\nSeleccione una opcion: ";

    Serial.print(menuString);
    servMenuStateNew = SERV_WAIT_FACT;
    servMenuStateCurrent = servMenuStateNew;
}

// Menu de calibracion de sitio
void printSiteMenu(void)
{
    menuString = "\n\n*** Calibracion coeficientes de sitio ***\n"
                 "1. Flujo inspiratorio sitio.\n"
                 "2. Flujo expiratorio sitio.\n"
                 "3. Presion Camara sitio.\n"
                 "4. Presion Bolsa sitio.\n"
                 "5. Presion Paciente sitio.\n"
                 "6. Escala de volumen sitio.\n"
                 "7. Menu anterior. \n"
                 "8. Salir.\n\nSeleccione una opcion: ";

    Serial.print(menuString);
    servMenuStateNew = SERV_WAIT_SITE;
    servMenuStateCurrent = servMenuStateNew;
}

// menu interno de calibracion de fabrica y de sitio
void printInternalFactoryMenu(int mode)
{
    if (mode == FLUJO_INSPIRATORIO)
    {
        menuString = "\n\n************ FLUJO INSPIRATORIO ************\n"
                     "1. Imprimir datos 20 Hz. (q+ENTER para salir)\n"
                     "*Curva de ajuste  N 1 \n2. Amplitud.\n3. Offset.\n4. Limite.\n"
                     "*Curva de ajuste  N 2 \n5. Amplitud.\n6. Offset.\n7. Limite.\n"
                     "*Curva de ajuste  N 3 \n8. Amplitud.\n9. Offset.\n"
                     "10. Ver constantes \n"
                     "11. Menu anterior. \n"
                     "12. Salir.\n\nSeleccione una opcion: ";
    }
    if (mode == FLUJO_ESPIRATORIO)
    {
        menuString = "\n\n************ FLUJO ESPIRATORIO ************\n"
                     "1. Imprimir datos 20 Hz. (q+ENTER para salir)\n"
                     "*Curva de ajuste  N 1 \n2. Amplitud.\n3. Offset.\n4. Limite.\n"
                     "*Curva de ajuste  N 2 \n5. Amplitud.\n6. Offset.\n7. Limite.\n"
                     "*Curva de ajuste  N 3 \n8. Amplitud.\n9. Offset.\n"
                     "10. Ver constantes \n"
                     "11. Menu anterior. \n"
                     "12. Salir.\n\nSeleccione una opcion: ";
    }
    if (mode == PRESION_CAMARA)
    {
        menuString = "\n\n************ PRESION CAMARA ************\n"
                     "1. Imprimir datos 20 Hz. (q+ENTER para salir)\n"
                     "*Curva de ajuste  N 1 \n2. Amplitud.\n3. Offset.\n"
                     "4. Ver constantes \n"
                     "5. Menu anterior. \n"
                     "6. Salir.\n\nSeleccione una opcion: ";
    }
    if (mode == PRESION_BOLSA)
    {
        menuString = "\n\n************ PRESION BOLSA ************\n"
                     "1. Imprimir datos 20 Hz. (q+ENTER para salir)\n"
                     "*Curva de ajuste  N 1 \n2. Amplitud.\n3. Offset.\n"
                     "4. Ver constantes \n"
                     "5. Menu anterior. \n"
                     "6. Salir.\n\nSeleccione una opcion: ";
    }
    if (mode == PRESION_PACIENTE)
    {
        menuString = "\n\n************ PRESION PACIENTE ************\n"
                     "1. Imprimir datos 20 Hz. (q+ENTER para salir)\n"
                     "*Curva de ajuste  N 1 \n2. Amplitud.\n3. Offset.\n"
                     "4. Ver constantes \n"
                     "5. Menu anterior. \n"
                     "6. Salir.\n\nSeleccione una opcion: ";
    }
    if (mode == VOLUMEN_PACIENTE)
    {
        menuString = "\n\n************ VOLUMEN PACIENTE ************\n"
                     "*Curva de ajuste  N 1 \n1. Escala de ajuste.\n"
                     "2. Ver constantes \n"
                     "3. Menu anterior. \n"
                     "4. Salir.\n\nSeleccione una opcion: ";
    }

    Serial.print(menuString);
    if (placeCalibration == FACTORY)
    {
        servMenuStateNew = SERV_WAIT_FAIN;
    }
    else if (placeCalibration == SITE)
    {
        servMenuStateNew = SERV_WAIT_SIIN;
    }
    servMenuStateCurrent = servMenuStateNew;
}



/* **************************************************************************
 * **** TAREA PARA LA RECEPCION DE DATOS SERIAL EN CALIBRACION **************
 * **************************************************************************/
void task_ReceiveService(void *pvParameters)
{

    String menuEntrance;
    menuEntrance.reserve(50);
    int selMenu = 0;
    float newCoeficient = 0;
    String newSerial;

    char bufferR = 0;

    uint8_t dataReady = false;

    uint8_t flagFirstPrint = false;

    // Clean Serial buffers
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    Serial.flush();
    Serial2.flush();

    while (1)
    {
        if (flagService == true)
        { // se encuentra en modo servicio
            if (Serial.available())
            {
                bufferR = Serial.read();
                if (bufferR == '\n')
                {
                    dataReady = true;
                }
                else
                {
                    menuEntrance = menuEntrance + bufferR;
                }
                Serial.print(bufferR);
            }

            if (dataReady == true)
            {
                dataReady = false;
                //menuEntrance = Serial.readStringUntil('\n');
                switch (servMenuStateCurrent)
                {
                case SERV_WAIT_MAIN:
                    selMenu = menuEntrance.toInt();
                    Serial.println(selMenu);
                    Serial.print("\n\n\n");
                    mainMenuOptionChange(selMenu);
                    break;
                case SERV_WAIT_FACT:
                    selMenu = menuEntrance.toInt();
                    Serial.println(selMenu);
                    Serial.print("\n\n\n");
                    factoryMenuOptionChange(selMenu);
                    break;
                case SERV_WAIT_SITE:
                    selMenu = menuEntrance.toInt();
                    Serial.println(selMenu);
                    Serial.print("\n\n\n");
                    siteMenuOptionChange(selMenu);
                    break;
                case SERV_WAIT_FAIN:
                    selMenu = menuEntrance.toInt();
                    Serial.println(selMenu);
                    Serial.print("\n\n\n");
                    factoryInternalMenuOptionChange(selMenu);
                    break;
                case SERV_WAIT_SIIN:
                    selMenu = menuEntrance.toInt();
                    Serial.println(selMenu);
                    Serial.print("\n\n\n");
                    siteInternalMenuOptionChange(selMenu);
                    break;
                case SERV_FACT_PRIN:
                    if (menuEntrance[0] == 'q')
                    {
                        if ((modeCalibration == FLUJO_INSPIRATORIO) || (modeCalibration == FLUJO_ESPIRATORIO))
                        {
                            flagFlowPrintCalibration = false;
                            servMenuStateNew = SERV_FAIN_CALI;
                            servMenuStateCurrent = servMenuStateNew;
                            printInternalFactoryMenu(modeCalibration);
                        }
                        if (modeCalibration == PRESION_CAMARA)
                        {
                            flagPcamPrintCalibration = false;
                            servMenuStateNew = SERV_FAIN_CALI;
                            servMenuStateCurrent = servMenuStateNew;
                            printInternalFactoryMenu(modeCalibration);
                        }
                        if (modeCalibration == PRESION_BOLSA)
                        {
                            flagPbagPrintCalibration = false;
                            servMenuStateNew = SERV_FAIN_CALI;
                            servMenuStateCurrent = servMenuStateNew;
                            printInternalFactoryMenu(modeCalibration);
                        }
                        if (modeCalibration == PRESION_PACIENTE)
                        {
                            flagPpacPrintCalibration = false;
                            servMenuStateNew = SERV_FAIN_CALI;
                            servMenuStateCurrent = servMenuStateNew;
                            printInternalFactoryMenu(modeCalibration);
                        }
                    }
                    break;
                case SERV_SITE_PRIN:
                    if (menuEntrance[0] == 'q')
                    {
                        if ((modeCalibration == FLUJO_INSPIRATORIO) || (modeCalibration == FLUJO_ESPIRATORIO))
                        {
                            flagFlowSitePrintCalibration = false;
                            servMenuStateNew = SERV_SIIN_CALI;
                            servMenuStateCurrent = servMenuStateNew;
                            printInternalFactoryMenu(modeCalibration);
                        }
                        if (modeCalibration == PRESION_CAMARA)
                        {
                            flagPcamSitePrintCalibration = false;
                            servMenuStateNew = SERV_SIIN_CALI;
                            servMenuStateCurrent = servMenuStateNew;
                            printInternalFactoryMenu(modeCalibration);
                        }
                        if (modeCalibration == PRESION_BOLSA)
                        {
                            flagPbagSitePrintCalibration = false;
                            servMenuStateNew = SERV_SIIN_CALI;
                            servMenuStateCurrent = servMenuStateNew;
                            printInternalFactoryMenu(modeCalibration);
                        }
                        if (modeCalibration == PRESION_PACIENTE)
                        {
                            flagPpacSitePrintCalibration = false;
                            servMenuStateNew = SERV_SIIN_CALI;
                            servMenuStateCurrent = servMenuStateNew;
                            printInternalFactoryMenu(modeCalibration);
                        }
                    }
                    break;
                case SERV_ACQI_DATA:
                    if (flagFirstPrint == false)
                    {
                        flagFirstPrint = true;
                        newCoeficient = menuEntrance.toFloat();
                        Serial.println(String(newCoeficient, 5));
                        Serial.print("\nDesea confirmar el cambio? y/n: ");
                    }
                    else
                    {
                        if (menuEntrance[0] == 'y')
                        {
                            changeMenu(coeftype, modeCalibration, newCoeficient, placeCalibration);
                            newCoeficient = 0;
                            modeCalibration = MODE_NULL;
                            if (placeCalibration == FACTORY)
                            {
                                servMenuStateNew = SERV_FACT_CALI;
                            }
                            else if (placeCalibration == SITE)
                            {
                                servMenuStateNew = SERV_SITE_CALI;
                            }
                            servMenuStateCurrent = servMenuStateNew;
                            flagFirstPrint = false;
                        }
                        else
                        {
                            newCoeficient = 0;
                            modeCalibration = MODE_NULL;
                            if (placeCalibration == FACTORY)
                            {
                                servMenuStateNew = SERV_FACT_CALI;
                            }
                            else if (placeCalibration == SITE)
                            {
                                servMenuStateNew = SERV_SITE_CALI;
                            }
                            servMenuStateCurrent = servMenuStateNew;
                            flagFirstPrint = false;
                            Serial.println("\n No se actualizo el valor\n");
                        }
                    }
                    break;
                case SERV_SERIAL_CH:
                    if (flagFirstPrint == false)
                    {
                        flagFirstPrint = true;
                        newSerial = menuEntrance;
                        Serial.println(newSerial);
                        Serial.print("\nDesea confirmar el cambio? y/n: ");
                    }
                    else
                    {
                        if (menuEntrance[0] == 'y')
                        {
                            writeString(eeprom_values::SERIAL_ADDR, newSerial);
                            SerialID = readString(eeprom_values::SERIAL_ADDR);
                            Serial.println("\n Serial actualizado con exito\n");
                            servMenuStateNew = SERV_FACT_CALI;
                            servMenuStateCurrent = servMenuStateNew;
                            flagFirstPrint = false;
                        }
                        else
                        {
                            servMenuStateNew = SERV_FACT_CALI;
                            servMenuStateCurrent = servMenuStateNew;
                            flagFirstPrint = false;
                            Serial.println("\n No se actualizo el Serial\n");
                        }
                    }
                    break;
                default:
                    break;
                }
                menuEntrance = "";
            }
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}



/* **************************************************************************
 * **** TAREA PARA LA CALIBRACION Y MENU DE USUARIO *************************
 * **************************************************************************/
void task_Service(void *arg)
{
    //Serial.println("Task_Create");
    while (1)
    {
        if (flagService == true)
        {
            // Serial.println("Estoy en servicio");
            switch (servMenuStateCurrent)
            {
            case SERV_MAIN_MENU:
                printMainMenu();
                break;
            case SERV_FACT_CALI:
                printFactoryMenu();
                break;
            case SERV_SITE_CALI:
                printSiteMenu();
                break;
            default:
                break;
            }
            // Serial.println("In execution");
        }
        else
        {
            /* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++
             + ++++ ESTADO PARTA DESTRUIR LA TAREA DE CALIBRACION +++++
             + +++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
            if (servMenuStateCurrent == SERV_NULL_MENU)
            {
                // Serial.println("Task deleted");
                // flagRestartTask = true;         // flag to habilitate the restart of task
                vTaskDelete(serviceTaskHandle); // delete the task Service
            }
        }
        vTaskDelay(250 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}


/** ****************************************************************************
 ** ************ END OF THE CODE ***********************************************
 ** ****************************************************************************/

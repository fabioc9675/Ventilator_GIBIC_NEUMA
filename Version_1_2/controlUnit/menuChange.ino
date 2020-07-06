/*
 * File:   menuChange.ino
 * Author: GIBIC UdeA
 *
 * Created on July 4, 2020, 13:41 PM
 */

/** ****************************************************************************
 ** ************ INCLUDES ******************************************************
 ** ****************************************************************************/
#include "menuChange.h"

/** ****************************************************************************
 ** ************ DEFINES *******************************************************
 ** ****************************************************************************/

/** ****************************************************************************
 ** ************ EXTERN VARIABLES **********************************************
 ** ****************************************************************************/
// **********************************************************
// variables de operacion de calibracion
extern uint8_t servMenuStateCurrent;
extern uint8_t servMenuStateNew;
extern uint8_t modeCalibration;
extern uint8_t coeftype;
extern uint8_t placeCalibration; // variable para especificar si es calibracion de fabrica o de sitio

extern uint8_t flagFlowPrintCalibration;
extern uint8_t flagPcamPrintCalibration;
extern uint8_t flagPbagPrintCalibration;
extern uint8_t flagPpacPrintCalibration;
extern uint8_t flagFlowSitePrintCalibration;
extern uint8_t flagPcamSitePrintCalibration;
extern uint8_t flagPbagSitePrintCalibration;
extern uint8_t flagPpacSitePrintCalibration;
extern uint8_t ServiceMode;
extern uint8_t flagService;

/** ****************************************************************************
 ** ************ VARIABLES *****************************************************
 ** ****************************************************************************/

/** ****************************************************************************
 ** ************ FUNCTIONS *****************************************************
 ** ****************************************************************************/

// funcion para el cambio de coeficientes de sitio
void siteInternalMenuOptionChange(int selMenu)
{
    float currentValue = 0;
    String dataPrint;

    float amp1 = 0;
    float amp2 = 0;
    float amp3 = 0;
    float off1 = 0;
    float off2 = 0;
    float off3 = 0;
    float lim1 = 0;
    float lim2 = 0;

    if ((selMenu > MAX_SIIN_MENU) || (selMenu == 0))
    {
        Serial.print("Opcion no valida \n");
        servMenuStateNew = SERV_SIIN_CALI;
        servMenuStateCurrent = servMenuStateNew;
    }
    else
    {
        switch (selMenu)
        {
        case 1: // seleccion del menu 1
            if ((modeCalibration == FLUJO_INSPIRATORIO) || (modeCalibration == FLUJO_ESPIRATORIO))
            {
                flagFlowSitePrintCalibration = true;
                servMenuStateNew = SERV_SITE_PRIN;
                servMenuStateCurrent = servMenuStateNew;
            }
            if (modeCalibration == PRESION_CAMARA)
            {
                flagPcamSitePrintCalibration = true;
                servMenuStateNew = SERV_SITE_PRIN;
                servMenuStateCurrent = servMenuStateNew;
            }
            if (modeCalibration == PRESION_BOLSA)
            {
                flagPbagSitePrintCalibration = true;
                servMenuStateNew = SERV_SITE_PRIN;
                servMenuStateCurrent = servMenuStateNew;
            }
            if (modeCalibration == PRESION_PACIENTE)
            {
                flagPpacSitePrintCalibration = true;
                servMenuStateNew = SERV_SITE_PRIN;
                servMenuStateCurrent = servMenuStateNew;
            }
            if (modeCalibration == VOLUMEN_PACIENTE)
            {
                coeftype = AMPL_1;
                currentValue = readFloat(eeprom_values::VOL_SCALE_SITE_ADDR);
                dataPrint = "Curva 1, Escala actual: " + String(currentValue, 5) + "\nIngrese nuevo valor: ";
                Serial.print(dataPrint);
                servMenuStateNew = SERV_ACQI_DATA;
                servMenuStateCurrent = servMenuStateNew;
            }
            /* code */
            break;
        case 2: // seleccion del menu 2
            /* code */
            if (modeCalibration == FLUJO_INSPIRATORIO)
            {
                coeftype = AMPL_1;
                currentValue = readFloat(eeprom_values::AMP_FI_1_SITE_ADDR);
                dataPrint = "Curva 1, Amplitud actual: " + String(currentValue, 5) + "\nIngrese nuevo valor: ";
                Serial.print(dataPrint);
                servMenuStateNew = SERV_ACQI_DATA;
                servMenuStateCurrent = servMenuStateNew;
            }
            if (modeCalibration == FLUJO_ESPIRATORIO)
            {
                coeftype = AMPL_1;
                currentValue = readFloat(eeprom_values::AMP_FE_1_SITE_ADDR);
                dataPrint = "Curva 1, Amplitud actual: " + String(currentValue, 5) + "\nIngrese nuevo valor: ";
                Serial.print(dataPrint);
                servMenuStateNew = SERV_ACQI_DATA;
                servMenuStateCurrent = servMenuStateNew;
            }
            if (modeCalibration == PRESION_CAMARA)
            {
                coeftype = AMPL_1;
                currentValue = readFloat(eeprom_values::AMP_CAM_1_SITE_ADDR);
                dataPrint = "Curva 1, Amplitud actual: " + String(currentValue, 5) + "\nIngrese nuevo valor: ";
                Serial.print(dataPrint);
                servMenuStateNew = SERV_ACQI_DATA;
                servMenuStateCurrent = servMenuStateNew;
            }
            if (modeCalibration == PRESION_BOLSA)
            {
                coeftype = AMPL_1;
                currentValue = readFloat(eeprom_values::AMP_BAG_2_SITE_ADDR);
                dataPrint = "Curva 1, Amplitud actual: " + String(currentValue, 5) + "\nIngrese nuevo valor: ";
                Serial.print(dataPrint);
                servMenuStateNew = SERV_ACQI_DATA;
                servMenuStateCurrent = servMenuStateNew;
            }
            if (modeCalibration == PRESION_PACIENTE)
            {
                coeftype = AMPL_1;
                currentValue = readFloat(eeprom_values::AMP_PAC_3_SITE_ADDR);
                dataPrint = "Curva 1, Amplitud actual: " + String(currentValue, 5) + "\nIngrese nuevo valor: ";
                Serial.print(dataPrint);
                servMenuStateNew = SERV_ACQI_DATA;
                servMenuStateCurrent = servMenuStateNew;
            }
            if (modeCalibration == VOLUMEN_PACIENTE)
            {
                amp1 = readFloat(eeprom_values::VOL_SCALE_SITE_ADDR);

                dataPrint = "Coeficientes escala volumen \n\n"
                            "VOL_SCALE_SITE = " +
                            String(amp1, 5) + "\n\n";
                Serial.print(dataPrint);

                servMenuStateNew = SERV_SIIN_CALI;
                servMenuStateCurrent = servMenuStateNew;
                printInternalFactoryMenu(modeCalibration);
            }

            break;
        case 3: // seleccion del menu 3, calibracion de fabrica
            /* code */
            if (modeCalibration == FLUJO_INSPIRATORIO)
            {
                coeftype = OFFS_1;
                currentValue = readFloat(eeprom_values::OFFS_FI_1_SITE_ADDR);
                dataPrint = "Curva 1, Offset actual: " + String(currentValue, 5) + "\nIngrese nuevo valor: ";
                Serial.print(dataPrint);
                servMenuStateNew = SERV_ACQI_DATA;
                servMenuStateCurrent = servMenuStateNew;
            }
            if (modeCalibration == FLUJO_ESPIRATORIO)
            {
                coeftype = OFFS_1;
                currentValue = readFloat(eeprom_values::OFFS_FE_1_SITE_ADDR);
                dataPrint = "Curva 1, Offset actual: " + String(currentValue, 5) + "\nIngrese nuevo valor: ";
                Serial.print(dataPrint);
                servMenuStateNew = SERV_ACQI_DATA;
                servMenuStateCurrent = servMenuStateNew;
            }
            if (modeCalibration == PRESION_CAMARA)
            {
                coeftype = OFFS_1;
                currentValue = readFloat(eeprom_values::OFFS_CAM_1_SITE_ADDR);
                dataPrint = "Curva 1, Offset actual: " + String(currentValue, 5) + "\nIngrese nuevo valor: ";
                Serial.print(dataPrint);
                servMenuStateNew = SERV_ACQI_DATA;
                servMenuStateCurrent = servMenuStateNew;
            }
            if (modeCalibration == PRESION_BOLSA)
            {
                coeftype = OFFS_1;
                currentValue = readFloat(eeprom_values::OFFS_BAG_2_SITE_ADDR);
                dataPrint = "Curva 1, Offset actual: " + String(currentValue, 5) + "\nIngrese nuevo valor: ";
                Serial.print(dataPrint);
                servMenuStateNew = SERV_ACQI_DATA;
                servMenuStateCurrent = servMenuStateNew;
            }
            if (modeCalibration == PRESION_PACIENTE)
            {
                coeftype = OFFS_1;
                currentValue = readFloat(eeprom_values::OFFS_PAC_3_SITE_ADDR);
                dataPrint = "Curva 1, Offset actual: " + String(currentValue, 5) + "\nIngrese nuevo valor: ";
                Serial.print(dataPrint);
                servMenuStateNew = SERV_ACQI_DATA;
                servMenuStateCurrent = servMenuStateNew;
            }
            if (modeCalibration == VOLUMEN_PACIENTE)
            {
                Serial.print("Salida Calibracion \n");
                if (ServiceMode == FACTORY)
                {
                    servMenuStateNew = SERV_MAFA_MENU;
                }
                else if (ServiceMode == SITE)
                {
                    servMenuStateNew = SERV_MAIN_MENU;
                }
                servMenuStateCurrent = servMenuStateNew;
            }

            break;
        case 4: // seleccion del menu 4
            /* code */

            if (modeCalibration == FLUJO_INSPIRATORIO)
            {
                coeftype = LIMS_1;
                currentValue = readFloat(eeprom_values::LIM_FI_1_SITE_ADDR);
                dataPrint = "Curva 1, Limite actual: " + String(currentValue, 5) + "\nIngrese nuevo valor: ";
                Serial.print(dataPrint);
                servMenuStateNew = SERV_ACQI_DATA;
                servMenuStateCurrent = servMenuStateNew;
            }
            if (modeCalibration == FLUJO_ESPIRATORIO)
            {
                coeftype = LIMS_1;
                currentValue = readFloat(eeprom_values::LIM_FE_1_SITE_ADDR);
                dataPrint = "Curva 1, Limite actual: " + String(currentValue, 5) + "\nIngrese nuevo valor: ";
                Serial.print(dataPrint);
                servMenuStateNew = SERV_ACQI_DATA;
                servMenuStateCurrent = servMenuStateNew;
            }
            if (modeCalibration == PRESION_CAMARA)
            {

                amp1 = readFloat(eeprom_values::AMP_CAM_1_SITE_ADDR);
                off1 = readFloat(eeprom_values::OFFS_CAM_1_SITE_ADDR);

                dataPrint = "Coeficientes presion camara sitio\n\n"
                            "AMP_CAM_SITE = " +
                            String(amp1, 5) +
                            "\nOFF_CAM_SITE = " + String(off1, 5) + "\n\n";

                Serial.print(dataPrint);

                servMenuStateNew = SERV_SIIN_CALI;
                servMenuStateCurrent = servMenuStateNew;
                printInternalFactoryMenu(modeCalibration);
            }
            if (modeCalibration == PRESION_BOLSA)
            {
                amp1 = readFloat(eeprom_values::AMP_BAG_2_SITE_ADDR);
                off1 = readFloat(eeprom_values::OFFS_BAG_2_SITE_ADDR);

                dataPrint = "Coeficientes presion bolsa sitio\n\n"
                            "AMP_BAG_SITE = " +
                            String(amp1, 5) +
                            "\nOFF_BAG_SITE = " + String(off1, 5) + "\n\n";
                Serial.print(dataPrint);

                servMenuStateNew = SERV_SIIN_CALI;
                servMenuStateCurrent = servMenuStateNew;
                printInternalFactoryMenu(modeCalibration);
            }
            if (modeCalibration == PRESION_PACIENTE)
            {
                amp1 = readFloat(eeprom_values::AMP_PAC_3_SITE_ADDR);
                off1 = readFloat(eeprom_values::OFFS_PAC_3_SITE_ADDR);

                dataPrint = "Coeficientes presion paciente sitio\n\n"
                            "AMP_PAC_SITE = " +
                            String(amp1, 5) +
                            "\nOFF_PAC_SITE = " + String(off1, 5) + "\n\n";
                Serial.print(dataPrint);

                servMenuStateNew = SERV_SIIN_CALI;
                servMenuStateCurrent = servMenuStateNew;
                printInternalFactoryMenu(modeCalibration);
            }
            if (modeCalibration == VOLUMEN_PACIENTE)
            {
                Serial.print("Salida segura \n");
                servMenuStateNew = SERV_NULL_MENU;
                servMenuStateCurrent = servMenuStateNew;
                flagService = false;
            }

            break;
        case 5: // seleccion del menu 5, Salir

            if (modeCalibration == FLUJO_INSPIRATORIO)
            {
                coeftype = AMPL_2;
                currentValue = readFloat(eeprom_values::AMP_FI_2_SITE_ADDR);
                dataPrint = "Curva 2, Amplitud actual: " + String(currentValue, 5) + "\nIngrese nuevo valor: ";
                Serial.print(dataPrint);
                servMenuStateNew = SERV_ACQI_DATA;
                servMenuStateCurrent = servMenuStateNew;
            }
            if (modeCalibration == FLUJO_ESPIRATORIO)
            {
                coeftype = AMPL_2;
                currentValue = readFloat(eeprom_values::AMP_FE_2_SITE_ADDR);
                dataPrint = "Curva 2, Amplitud actual: " + String(currentValue, 5) + "\nIngrese nuevo valor: ";
                Serial.print(dataPrint);
                servMenuStateNew = SERV_ACQI_DATA;
                servMenuStateCurrent = servMenuStateNew;
            }

            if ((modeCalibration == PRESION_CAMARA) || (modeCalibration == PRESION_BOLSA) || (modeCalibration == PRESION_PACIENTE))
            {
                Serial.print("Salida Calibracion \n");
                if (ServiceMode == FACTORY)
                {
                    servMenuStateNew = SERV_MAFA_MENU;
                }
                else if (ServiceMode == SITE)
                {
                    servMenuStateNew = SERV_MAIN_MENU;
                }
                servMenuStateCurrent = servMenuStateNew;
            }

            break;
        case 6: // seleccion del menu 5, Salir

            if (modeCalibration == FLUJO_INSPIRATORIO)
            {
                coeftype = OFFS_2;
                currentValue = readFloat(eeprom_values::OFFS_FI_2_SITE_ADDR);
                dataPrint = "Curva 2, Offset actual: " + String(currentValue, 5) + "\nIngrese nuevo valor: ";
                Serial.print(dataPrint);
                servMenuStateNew = SERV_ACQI_DATA;
                servMenuStateCurrent = servMenuStateNew;
            }
            if (modeCalibration == FLUJO_ESPIRATORIO)
            {
                coeftype = OFFS_2;
                currentValue = readFloat(eeprom_values::OFFS_FE_2_SITE_ADDR);
                dataPrint = "Curva 2, Offset actual: " + String(currentValue, 5) + "\nIngrese nuevo valor: ";
                Serial.print(dataPrint);
                servMenuStateNew = SERV_ACQI_DATA;
                servMenuStateCurrent = servMenuStateNew;
            }

            if ((modeCalibration == PRESION_CAMARA) || (modeCalibration == PRESION_BOLSA) || (modeCalibration == PRESION_PACIENTE))
            {
                Serial.print("Salida segura \n");
                servMenuStateNew = SERV_NULL_MENU;
                servMenuStateCurrent = servMenuStateNew;
                flagService = false;
            }

            break;

        case 7:
            if (modeCalibration == FLUJO_INSPIRATORIO)
            {
                coeftype = LIMS_2;
                currentValue = readFloat(eeprom_values::LIM_FI_2_SITE_ADDR);
                dataPrint = "Curva 2, Limite actual: " + String(currentValue, 5) + "\nIngrese nuevo valor: ";
                Serial.print(dataPrint);
                servMenuStateNew = SERV_ACQI_DATA;
                servMenuStateCurrent = servMenuStateNew;
            }
            if (modeCalibration == FLUJO_ESPIRATORIO)
            {
                coeftype = LIMS_2;
                currentValue = readFloat(eeprom_values::LIM_FE_2_SITE_ADDR);
                dataPrint = "Curva 2, Limite actual: " + String(currentValue, 5) + "\nIngrese nuevo valor: ";
                Serial.print(dataPrint);
                servMenuStateNew = SERV_ACQI_DATA;
                servMenuStateCurrent = servMenuStateNew;
            }
            break;

        case 8:

            if (modeCalibration == FLUJO_INSPIRATORIO)
            {
                coeftype = AMPL_3;
                currentValue = readFloat(eeprom_values::AMP_FI_3_SITE_ADDR);
                dataPrint = "Curva 3, Amplitud actual: " + String(currentValue, 5) + "\nIngrese nuevo valor: ";
                Serial.print(dataPrint);
                servMenuStateNew = SERV_ACQI_DATA;
                servMenuStateCurrent = servMenuStateNew;
            }
            if (modeCalibration == FLUJO_ESPIRATORIO)
            {
                coeftype = AMPL_3;
                currentValue = readFloat(eeprom_values::AMP_FE_3_SITE_ADDR);
                dataPrint = "Curva 3, Amplitud actual: " + String(currentValue, 5) + "\nIngrese nuevo valor: ";
                Serial.print(dataPrint);
                servMenuStateNew = SERV_ACQI_DATA;
                servMenuStateCurrent = servMenuStateNew;
            }

            break;

        case 9:
            if (modeCalibration == FLUJO_INSPIRATORIO)
            {
                coeftype = OFFS_3;
                currentValue = readFloat(eeprom_values::OFFS_FI_3_SITE_ADDR);
                dataPrint = "Curva 3, Offset actual: " + String(currentValue, 5) + "\nIngrese nuevo valor: ";
                Serial.print(dataPrint);
                servMenuStateNew = SERV_ACQI_DATA;
                servMenuStateCurrent = servMenuStateNew;
            }
            if (modeCalibration == FLUJO_ESPIRATORIO)
            {
                coeftype = OFFS_3;
                currentValue = readFloat(eeprom_values::OFFS_FE_3_SITE_ADDR);
                dataPrint = "Curva 3, Offset actual: " + String(currentValue, 5) + "\nIngrese nuevo valor: ";
                Serial.print(dataPrint);
                servMenuStateNew = SERV_ACQI_DATA;
                servMenuStateCurrent = servMenuStateNew;
            }
            break;

        case 10:
            if (modeCalibration == FLUJO_INSPIRATORIO)
            {
                amp1 = readFloat(eeprom_values::AMP_FI_1_SITE_ADDR);
                off1 = readFloat(eeprom_values::OFFS_FI_1_SITE_ADDR);
                lim1 = readFloat(eeprom_values::LIM_FI_1_SITE_ADDR);
                amp2 = readFloat(eeprom_values::AMP_FI_2_SITE_ADDR);
                off2 = readFloat(eeprom_values::OFFS_FI_2_SITE_ADDR);
                lim2 = readFloat(eeprom_values::LIM_FI_2_SITE_ADDR);
                amp3 = readFloat(eeprom_values::AMP_FI_3_SITE_ADDR);
                off3 = readFloat(eeprom_values::OFFS_FI_3_SITE_ADDR);

                dataPrint = "Coeficientes flujo inspiratorio sitio \n\n"
                            "AMP_FI_1_SITE = " +
                            String(amp1, 5) +
                            "\nOFF_FI_1_SITE = " + String(off1, 5) +
                            "\nLIM_FI_1_SITE = " + String(lim1, 5) +
                            "\nAMP_FI_2_SITE = " + String(amp2, 5) +
                            "\nOFF_FI_2_SITE = " + String(off2, 5) +
                            "\nLIM_FI_2_SITE = " + String(lim2, 5) +
                            "\nAMP_FI_3_SITE = " + String(amp3, 5) +
                            "\nOFF_FI_3_SITE = " + String(off3, 5) + "\n\n";
                Serial.print(dataPrint);

                servMenuStateNew = SERV_SIIN_CALI;
                servMenuStateCurrent = servMenuStateNew;
                printInternalFactoryMenu(modeCalibration);
            }
            if (modeCalibration == FLUJO_ESPIRATORIO)
            {
                amp1 = readFloat(eeprom_values::AMP_FE_1_SITE_ADDR);
                off1 = readFloat(eeprom_values::OFFS_FE_1_SITE_ADDR);
                lim1 = readFloat(eeprom_values::LIM_FE_1_SITE_ADDR);
                amp2 = readFloat(eeprom_values::AMP_FE_2_SITE_ADDR);
                off2 = readFloat(eeprom_values::OFFS_FE_2_SITE_ADDR);
                lim2 = readFloat(eeprom_values::LIM_FE_2_SITE_ADDR);
                amp3 = readFloat(eeprom_values::AMP_FE_3_SITE_ADDR);
                off3 = readFloat(eeprom_values::OFFS_FE_3_SITE_ADDR);

                dataPrint = "Coeficientes flujo inspiratorio \n\n"
                            "AMP_FI_1_SITE = " +
                            String(amp1, 5) +
                            "\nOFF_FI_1_SITE = " + String(off1, 5) +
                            "\nLIM_FI_1_SITE = " + String(lim1, 5) +
                            "\nAMP_FI_2_SITE = " + String(amp2, 5) +
                            "\nOFF_FI_2_SITE = " + String(off2, 5) +
                            "\nLIM_FI_2_SITE = " + String(lim2, 5) +
                            "\nAMP_FI_3_SITE = " + String(amp3, 5) +
                            "\nOFF_FI_3_SITE = " + String(off3, 5) + "\n\n";
                Serial.print(dataPrint);

                servMenuStateNew = SERV_SIIN_CALI;
                servMenuStateCurrent = servMenuStateNew;
                printInternalFactoryMenu(modeCalibration);
            }

            break;

        case 11: // seleccion del menu 5, Salir
            if ((modeCalibration == FLUJO_INSPIRATORIO) || (modeCalibration == FLUJO_ESPIRATORIO))
            {
                Serial.print("Salida Calibracion \n");
                if (ServiceMode == FACTORY)
                {
                    servMenuStateNew = SERV_MAFA_MENU;
                }
                else if (ServiceMode == SITE)
                {
                    servMenuStateNew = SERV_MAIN_MENU;
                }
                servMenuStateCurrent = servMenuStateNew;
            }
            break;
        case 12: // seleccion del menu 7, Salir
            if ((modeCalibration == FLUJO_INSPIRATORIO) || (modeCalibration == FLUJO_ESPIRATORIO))
            {
                Serial.print("Salida segura \n");
                servMenuStateNew = SERV_NULL_MENU;
                servMenuStateCurrent = servMenuStateNew;
                flagService = false;
            }
            break;
        default:
            break;
        }

        // agregar las opciones de menu
        Serial.println("Menu sitio seleccionado");
    }
}

// funcion para el cambio de coeficientes de fabrica
void factoryInternalMenuOptionChange(int selMenu)
{
    float currentValue = 0;
    String dataPrint;

    float amp1 = 0;
    float amp2 = 0;
    float amp3 = 0;
    float off1 = 0;
    float off2 = 0;
    float off3 = 0;
    float lim1 = 0;
    float lim2 = 0;

    if ((selMenu > MAX_FAIN_MENU) || (selMenu == 0))
    {
        Serial.print("Opcion no valida \n");
        servMenuStateNew = SERV_FAIN_CALI;
        servMenuStateCurrent = servMenuStateNew;
    }
    else
    {
        switch (selMenu)
        {
        case 1: // seleccion del menu 1
            if ((modeCalibration == FLUJO_INSPIRATORIO) || (modeCalibration == FLUJO_ESPIRATORIO))
            {
                flagFlowPrintCalibration = true;
                servMenuStateNew = SERV_FACT_PRIN;
                servMenuStateCurrent = servMenuStateNew;
            }
            if (modeCalibration == PRESION_CAMARA)
            {
                flagPcamPrintCalibration = true;
                servMenuStateNew = SERV_FACT_PRIN;
                servMenuStateCurrent = servMenuStateNew;
            }
            if (modeCalibration == PRESION_BOLSA)
            {
                flagPbagPrintCalibration = true;
                servMenuStateNew = SERV_FACT_PRIN;
                servMenuStateCurrent = servMenuStateNew;
            }
            if (modeCalibration == PRESION_PACIENTE)
            {
                flagPpacPrintCalibration = true;
                servMenuStateNew = SERV_FACT_PRIN;
                servMenuStateCurrent = servMenuStateNew;
            }
            if (modeCalibration == VOLUMEN_PACIENTE)
            {
                coeftype = AMPL_1;
                currentValue = readFloat(eeprom_values::VOL_SCALE_ADDR);
                dataPrint = "Curva 1, Escala actual: " + String(currentValue, 5) + "\nIngrese nuevo valor: ";
                Serial.print(dataPrint);
                servMenuStateNew = SERV_ACQI_DATA;
                servMenuStateCurrent = servMenuStateNew;
            }
            /* code */
            break;
        case 2: // seleccion del menu 2
            /* code */
            if (modeCalibration == FLUJO_INSPIRATORIO)
            {
                coeftype = AMPL_1;
                currentValue = readFloat(eeprom_values::AMP_FI_1_ADDR);
                dataPrint = "Curva 1, Amplitud actual: " + String(currentValue, 5) + "\nIngrese nuevo valor: ";
                Serial.print(dataPrint);
                servMenuStateNew = SERV_ACQI_DATA;
                servMenuStateCurrent = servMenuStateNew;
            }
            if (modeCalibration == FLUJO_ESPIRATORIO)
            {
                coeftype = AMPL_1;
                currentValue = readFloat(eeprom_values::AMP_FE_1_ADDR);
                dataPrint = "Curva 1, Amplitud actual: " + String(currentValue, 5) + "\nIngrese nuevo valor: ";
                Serial.print(dataPrint);
                servMenuStateNew = SERV_ACQI_DATA;
                servMenuStateCurrent = servMenuStateNew;
            }
            if (modeCalibration == PRESION_CAMARA)
            {
                coeftype = AMPL_1;
                currentValue = readFloat(eeprom_values::AMP_CAM_1_ADDR);
                dataPrint = "Curva 1, Amplitud actual: " + String(currentValue, 5) + "\nIngrese nuevo valor: ";
                Serial.print(dataPrint);
                servMenuStateNew = SERV_ACQI_DATA;
                servMenuStateCurrent = servMenuStateNew;
            }
            if (modeCalibration == PRESION_BOLSA)
            {
                coeftype = AMPL_1;
                currentValue = readFloat(eeprom_values::AMP_BAG_2_ADDR);
                dataPrint = "Curva 1, Amplitud actual: " + String(currentValue, 5) + "\nIngrese nuevo valor: ";
                Serial.print(dataPrint);
                servMenuStateNew = SERV_ACQI_DATA;
                servMenuStateCurrent = servMenuStateNew;
            }
            if (modeCalibration == PRESION_PACIENTE)
            {
                coeftype = AMPL_1;
                currentValue = readFloat(eeprom_values::AMP_PAC_3_ADDR);
                dataPrint = "Curva 1, Amplitud actual: " + String(currentValue, 5) + "\nIngrese nuevo valor: ";
                Serial.print(dataPrint);
                servMenuStateNew = SERV_ACQI_DATA;
                servMenuStateCurrent = servMenuStateNew;
            }
            if (modeCalibration == VOLUMEN_PACIENTE)
            {
                amp1 = readFloat(eeprom_values::VOL_SCALE_ADDR);

                dataPrint = "Coeficientes escala volumen \n\n"
                            "VOL_SCALE = " +
                            String(amp1, 5) + "\n\n";
                Serial.print(dataPrint);

                servMenuStateNew = SERV_FAIN_CALI;
                servMenuStateCurrent = servMenuStateNew;
                printInternalFactoryMenu(modeCalibration);
            }

            break;
        case 3: // seleccion del menu 3, calibracion de fabrica
            /* code */
            if (modeCalibration == FLUJO_INSPIRATORIO)
            {
                coeftype = OFFS_1;
                currentValue = readFloat(eeprom_values::OFFS_FI_1_ADDR);
                dataPrint = "Curva 1, Offset actual: " + String(currentValue, 5) + "\nIngrese nuevo valor: ";
                Serial.print(dataPrint);
                servMenuStateNew = SERV_ACQI_DATA;
                servMenuStateCurrent = servMenuStateNew;
            }
            if (modeCalibration == FLUJO_ESPIRATORIO)
            {
                coeftype = OFFS_1;
                currentValue = readFloat(eeprom_values::OFFS_FE_1_ADDR);
                dataPrint = "Curva 1, Offset actual: " + String(currentValue, 5) + "\nIngrese nuevo valor: ";
                Serial.print(dataPrint);
                servMenuStateNew = SERV_ACQI_DATA;
                servMenuStateCurrent = servMenuStateNew;
            }
            if (modeCalibration == PRESION_CAMARA)
            {
                coeftype = OFFS_1;
                currentValue = readFloat(eeprom_values::OFFS_CAM_1_ADDR);
                dataPrint = "Curva 1, Offset actual: " + String(currentValue, 5) + "\nIngrese nuevo valor: ";
                Serial.print(dataPrint);
                servMenuStateNew = SERV_ACQI_DATA;
                servMenuStateCurrent = servMenuStateNew;
            }
            if (modeCalibration == PRESION_BOLSA)
            {
                coeftype = OFFS_1;
                currentValue = readFloat(eeprom_values::OFFS_BAG_2_ADDR);
                dataPrint = "Curva 1, Offset actual: " + String(currentValue, 5) + "\nIngrese nuevo valor: ";
                Serial.print(dataPrint);
                servMenuStateNew = SERV_ACQI_DATA;
                servMenuStateCurrent = servMenuStateNew;
            }
            if (modeCalibration == PRESION_PACIENTE)
            {
                coeftype = OFFS_1;
                currentValue = readFloat(eeprom_values::OFFS_PAC_3_ADDR);
                dataPrint = "Curva 1, Offset actual: " + String(currentValue, 5) + "\nIngrese nuevo valor: ";
                Serial.print(dataPrint);
                servMenuStateNew = SERV_ACQI_DATA;
                servMenuStateCurrent = servMenuStateNew;
            }
            if (modeCalibration == VOLUMEN_PACIENTE)
            {
                Serial.print("Salida Calibracion \n");
                if (ServiceMode == FACTORY)
                {
                    servMenuStateNew = SERV_MAFA_MENU;
                }
                else if (ServiceMode == SITE)
                {
                    servMenuStateNew = SERV_MAIN_MENU;
                }
                servMenuStateCurrent = servMenuStateNew;
            }

            break;
        case 4: // seleccion del menu 4
            /* code */

            if (modeCalibration == FLUJO_INSPIRATORIO)
            {
                coeftype = LIMS_1;
                currentValue = readFloat(eeprom_values::LIM_FI_1_ADDR);
                dataPrint = "Curva 1, Limite actual: " + String(currentValue, 5) + "\nIngrese nuevo valor: ";
                Serial.print(dataPrint);
                servMenuStateNew = SERV_ACQI_DATA;
                servMenuStateCurrent = servMenuStateNew;
            }
            if (modeCalibration == FLUJO_ESPIRATORIO)
            {
                coeftype = LIMS_1;
                currentValue = readFloat(eeprom_values::LIM_FE_1_ADDR);
                dataPrint = "Curva 1, Limite actual: " + String(currentValue, 5) + "\nIngrese nuevo valor: ";
                Serial.print(dataPrint);
                servMenuStateNew = SERV_ACQI_DATA;
                servMenuStateCurrent = servMenuStateNew;
            }
            if (modeCalibration == PRESION_CAMARA)
            {

                amp1 = readFloat(eeprom_values::AMP_CAM_1_ADDR);
                off1 = readFloat(eeprom_values::OFFS_CAM_1_ADDR);

                dataPrint = "Coeficientes presion camara \n\n"
                            "AMP_CAM = " +
                            String(amp1, 5) +
                            "\nOFF_CAM = " + String(off1, 5) + "\n\n";

                Serial.print(dataPrint);

                servMenuStateNew = SERV_FAIN_CALI;
                servMenuStateCurrent = servMenuStateNew;
                printInternalFactoryMenu(modeCalibration);
            }
            if (modeCalibration == PRESION_BOLSA)
            {
                amp1 = readFloat(eeprom_values::AMP_BAG_2_ADDR);
                off1 = readFloat(eeprom_values::OFFS_BAG_2_ADDR);

                dataPrint = "Coeficientes presion bolsa \n\n"
                            "AMP_BAG = " +
                            String(amp1, 5) +
                            "\nOFF_BAG = " + String(off1, 5) + "\n\n";
                Serial.print(dataPrint);

                servMenuStateNew = SERV_FAIN_CALI;
                servMenuStateCurrent = servMenuStateNew;
                printInternalFactoryMenu(modeCalibration);
            }
            if (modeCalibration == PRESION_PACIENTE)
            {
                amp1 = readFloat(eeprom_values::AMP_PAC_3_ADDR);
                off1 = readFloat(eeprom_values::OFFS_PAC_3_ADDR);

                dataPrint = "Coeficientes presion paciente \n\n"
                            "AMP_PAC = " +
                            String(amp1, 5) +
                            "\nOFF_PAC = " + String(off1, 5) + "\n\n";
                Serial.print(dataPrint);

                servMenuStateNew = SERV_FAIN_CALI;
                servMenuStateCurrent = servMenuStateNew;
                printInternalFactoryMenu(modeCalibration);
            }
            if (modeCalibration == VOLUMEN_PACIENTE)
            {
                Serial.print("Salida segura \n");
                servMenuStateNew = SERV_NULL_MENU;
                servMenuStateCurrent = servMenuStateNew;
                flagService = false;
            }

            break;
        case 5: // seleccion del menu 5, Salir

            if (modeCalibration == FLUJO_INSPIRATORIO)
            {
                coeftype = AMPL_2;
                currentValue = readFloat(eeprom_values::AMP_FI_2_ADDR);
                dataPrint = "Curva 2, Amplitud actual: " + String(currentValue, 5) + "\nIngrese nuevo valor: ";
                Serial.print(dataPrint);
                servMenuStateNew = SERV_ACQI_DATA;
                servMenuStateCurrent = servMenuStateNew;
            }
            if (modeCalibration == FLUJO_ESPIRATORIO)
            {
                coeftype = AMPL_2;
                currentValue = readFloat(eeprom_values::AMP_FE_2_ADDR);
                dataPrint = "Curva 2, Amplitud actual: " + String(currentValue, 5) + "\nIngrese nuevo valor: ";
                Serial.print(dataPrint);
                servMenuStateNew = SERV_ACQI_DATA;
                servMenuStateCurrent = servMenuStateNew;
            }

            if ((modeCalibration == PRESION_CAMARA) || (modeCalibration == PRESION_BOLSA) || (modeCalibration == PRESION_PACIENTE))
            {
                Serial.print("Salida Calibracion \n");
                if (ServiceMode == FACTORY)
                {
                    servMenuStateNew = SERV_MAFA_MENU;
                }
                else if (ServiceMode == SITE)
                {
                    servMenuStateNew = SERV_MAIN_MENU;
                }
                servMenuStateCurrent = servMenuStateNew;
            }

            break;
        case 6: // seleccion del menu 5, Salir

            if (modeCalibration == FLUJO_INSPIRATORIO)
            {
                coeftype = OFFS_2;
                currentValue = readFloat(eeprom_values::OFFS_FI_2_ADDR);
                dataPrint = "Curva 2, Offset actual: " + String(currentValue, 5) + "\nIngrese nuevo valor: ";
                Serial.print(dataPrint);
                servMenuStateNew = SERV_ACQI_DATA;
                servMenuStateCurrent = servMenuStateNew;
            }
            if (modeCalibration == FLUJO_ESPIRATORIO)
            {
                coeftype = OFFS_2;
                currentValue = readFloat(eeprom_values::OFFS_FE_2_ADDR);
                dataPrint = "Curva 2, Offset actual: " + String(currentValue, 5) + "\nIngrese nuevo valor: ";
                Serial.print(dataPrint);
                servMenuStateNew = SERV_ACQI_DATA;
                servMenuStateCurrent = servMenuStateNew;
            }

            if ((modeCalibration == PRESION_CAMARA) || (modeCalibration == PRESION_BOLSA) || (modeCalibration == PRESION_PACIENTE))
            {
                Serial.print("Salida segura \n");
                servMenuStateNew = SERV_NULL_MENU;
                servMenuStateCurrent = servMenuStateNew;
                flagService = false;
            }

            break;

        case 7:
            if (modeCalibration == FLUJO_INSPIRATORIO)
            {
                coeftype = LIMS_2;
                currentValue = readFloat(eeprom_values::LIM_FI_2_ADDR);
                dataPrint = "Curva 2, Limite actual: " + String(currentValue, 5) + "\nIngrese nuevo valor: ";
                Serial.print(dataPrint);
                servMenuStateNew = SERV_ACQI_DATA;
                servMenuStateCurrent = servMenuStateNew;
            }
            if (modeCalibration == FLUJO_ESPIRATORIO)
            {
                coeftype = LIMS_2;
                currentValue = readFloat(eeprom_values::LIM_FE_2_ADDR);
                dataPrint = "Curva 2, Limite actual: " + String(currentValue, 5) + "\nIngrese nuevo valor: ";
                Serial.print(dataPrint);
                servMenuStateNew = SERV_ACQI_DATA;
                servMenuStateCurrent = servMenuStateNew;
            }
            break;

        case 8:

            if (modeCalibration == FLUJO_INSPIRATORIO)
            {
                coeftype = AMPL_3;
                currentValue = readFloat(eeprom_values::AMP_FI_3_ADDR);
                dataPrint = "Curva 3, Amplitud actual: " + String(currentValue, 5) + "\nIngrese nuevo valor: ";
                Serial.print(dataPrint);
                servMenuStateNew = SERV_ACQI_DATA;
                servMenuStateCurrent = servMenuStateNew;
            }
            if (modeCalibration == FLUJO_ESPIRATORIO)
            {
                coeftype = AMPL_3;
                currentValue = readFloat(eeprom_values::AMP_FE_3_ADDR);
                dataPrint = "Curva 3, Amplitud actual: " + String(currentValue, 5) + "\nIngrese nuevo valor: ";
                Serial.print(dataPrint);
                servMenuStateNew = SERV_ACQI_DATA;
                servMenuStateCurrent = servMenuStateNew;
            }

            break;

        case 9:
            if (modeCalibration == FLUJO_INSPIRATORIO)
            {
                coeftype = OFFS_3;
                currentValue = readFloat(eeprom_values::OFFS_FI_3_ADDR);
                dataPrint = "Curva 3, Offset actual: " + String(currentValue, 5) + "\nIngrese nuevo valor: ";
                Serial.print(dataPrint);
                servMenuStateNew = SERV_ACQI_DATA;
                servMenuStateCurrent = servMenuStateNew;
            }
            if (modeCalibration == FLUJO_ESPIRATORIO)
            {
                coeftype = OFFS_3;
                currentValue = readFloat(eeprom_values::OFFS_FE_3_ADDR);
                dataPrint = "Curva 3, Offset actual: " + String(currentValue, 5) + "\nIngrese nuevo valor: ";
                Serial.print(dataPrint);
                servMenuStateNew = SERV_ACQI_DATA;
                servMenuStateCurrent = servMenuStateNew;
            }
            break;

        case 10:
            if (modeCalibration == FLUJO_INSPIRATORIO)
            {
                amp1 = readFloat(eeprom_values::AMP_FI_1_ADDR);
                off1 = readFloat(eeprom_values::OFFS_FI_1_ADDR);
                lim1 = readFloat(eeprom_values::LIM_FI_1_ADDR);
                amp2 = readFloat(eeprom_values::AMP_FI_2_ADDR);
                off2 = readFloat(eeprom_values::OFFS_FI_2_ADDR);
                lim2 = readFloat(eeprom_values::LIM_FI_2_ADDR);
                amp3 = readFloat(eeprom_values::AMP_FI_3_ADDR);
                off3 = readFloat(eeprom_values::OFFS_FI_3_ADDR);

                dataPrint = "Coeficientes flujo inspiratorio \n\n"
                            "AMP_FI_1 = " +
                            String(amp1, 5) +
                            "\nOFF_FI_1 = " + String(off1, 5) +
                            "\nLIM_FI_1 = " + String(lim1, 5) +
                            "\nAMP_FI_2 = " + String(amp2, 5) +
                            "\nOFF_FI_2 = " + String(off2, 5) +
                            "\nLIM_FI_2 = " + String(lim2, 5) +
                            "\nAMP_FI_3 = " + String(amp3, 5) +
                            "\nOFF_FI_3 = " + String(off3, 5) + "\n\n";
                Serial.print(dataPrint);

                servMenuStateNew = SERV_FAIN_CALI;
                servMenuStateCurrent = servMenuStateNew;
                printInternalFactoryMenu(modeCalibration);
            }
            if (modeCalibration == FLUJO_ESPIRATORIO)
            {
                amp1 = readFloat(eeprom_values::AMP_FE_1_ADDR);
                off1 = readFloat(eeprom_values::OFFS_FE_1_ADDR);
                lim1 = readFloat(eeprom_values::LIM_FE_1_ADDR);
                amp2 = readFloat(eeprom_values::AMP_FE_2_ADDR);
                off2 = readFloat(eeprom_values::OFFS_FE_2_ADDR);
                lim2 = readFloat(eeprom_values::LIM_FE_2_ADDR);
                amp3 = readFloat(eeprom_values::AMP_FE_3_ADDR);
                off3 = readFloat(eeprom_values::OFFS_FE_3_ADDR);

                dataPrint = "Coeficientes flujo inspiratorio \n\n"
                            "AMP_FI_1 = " +
                            String(amp1, 5) +
                            "\nOFF_FI_1 = " + String(off1, 5) +
                            "\nLIM_FI_1 = " + String(lim1, 5) +
                            "\nAMP_FI_2 = " + String(amp2, 5) +
                            "\nOFF_FI_2 = " + String(off2, 5) +
                            "\nLIM_FI_2 = " + String(lim2, 5) +
                            "\nAMP_FI_3 = " + String(amp3, 5) +
                            "\nOFF_FI_3 = " + String(off3, 5) + "\n\n";
                Serial.print(dataPrint);

                servMenuStateNew = SERV_FAIN_CALI;
                servMenuStateCurrent = servMenuStateNew;
                printInternalFactoryMenu(modeCalibration);
            }

            break;

        case 11: // seleccion del menu 5, Salir
            if ((modeCalibration == FLUJO_INSPIRATORIO) || (modeCalibration == FLUJO_ESPIRATORIO))
            {
                Serial.print("Salida Calibracion \n");
                if (ServiceMode == FACTORY)
                {
                    servMenuStateNew = SERV_MAFA_MENU;
                }
                else if (ServiceMode == SITE)
                {
                    servMenuStateNew = SERV_MAIN_MENU;
                }
                servMenuStateCurrent = servMenuStateNew;
            }
            break;
        case 12: // seleccion del menu 7, Salir
            if ((modeCalibration == FLUJO_INSPIRATORIO) || (modeCalibration == FLUJO_ESPIRATORIO))
            {
                Serial.print("Salida segura \n");
                servMenuStateNew = SERV_NULL_MENU;
                servMenuStateCurrent = servMenuStateNew;
                flagService = false;
            }
            break;
        default:
            break;
        }

        // agregar las opciones de menu
        Serial.println("Menu fabrica seleccionado");
    }
}

/** ****************************************************************************
 ** ************ END OF THE CODE ***********************************************
 ** ****************************************************************************/
/*
 * File:   lcdPartial.ino
 * Author: GIBIC UdeA
 *
 * Created on July 4, 2020, 13:41 PM
 */

/** ****************************************************************************
 ** ************ INCLUDES ******************************************************
 ** ****************************************************************************/
#include "lcdPartial.h"

/** ****************************************************************************
 ** ************ DEFINES *******************************************************
 ** ****************************************************************************/

/** ****************************************************************************
 ** ************ EXTERN VARIABLES **********************************************
 ** ****************************************************************************/
// **********************************************************
// definicion de lcd class
extern LiquidCrystal_I2C lcd;

// Variables para menu anterior
extern volatile unsigned int menuAnterior;  // valor de menu anterior
extern volatile unsigned int lineaAnterior; // valor de menu anterior
extern byte IAnte;
extern byte EAnte;
extern float PpicoAnte;
extern float PeepAnte;
extern float PconAnte;
extern unsigned int VTAnte;
extern byte maxPresionAnte;
extern byte frecRespiratoriaAnte;
extern byte minFR_Ante;
extern byte minVE_Ante;

// variables de menu
extern volatile unsigned int menuImprimir;
extern volatile unsigned int lineaAlerta;
extern volatile uint8_t flagAlreadyPrint;

// contadores de configuraciones en el menu de usuario
extern String relacion_IE;
extern byte newFrecRespiratoria;
extern int newRelacionIE;
extern byte currentFrecRespiratoria;
extern int currentRelacionIE;
extern byte trigger;
extern byte PeepMax;
extern byte maxPresion;
extern byte maxFR;
extern byte maxVE;

// Variables calculadas
extern byte frecRespiratoriaCalculada;
extern int calculatedE;
extern byte I;
extern byte E;

// variables de introduccion a los menus de configuracion
extern byte currentVentilationMode;

// Variables para menu actualizado
extern float Peep;
extern float Ppico;
extern float Pcon;
extern byte currentVE;
extern unsigned int VT;
extern byte newVE;
extern int presPac;
extern int flowPac;
extern int presPacAnte;
extern int flowPacAnte;

// banderas de cambio de valores
extern volatile uint8_t flagConfirm;

// variables de introduccion a los menus de configuracion
extern byte optionConfigMenu;
extern byte optionVentMenu;

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
extern volatile uint8_t flagLeakage;

/** ****************************************************************************
 ** ************ VARIABLES *****************************************************
 ** ****************************************************************************/

/** ****************************************************************************
 ** ************ FUNCTIONS *****************************************************
 ** ****************************************************************************/
// Ejecucion de rutina de refrescamiento parcial de LCD
void lcd_show_part(void)
{
    menuAnterior = menuImprimir;
    String newRelacion_IE;
    String calculatedRelacion_IE;
    if (newRelacionIE > 0)
    {
        newRelacion_IE = "1:" + String((float)newRelacionIE / 10, 1);
        calculatedRelacion_IE = "1:" + String((float)calculatedE / 10.0, 1);
    }
    else
    {
        newRelacion_IE = String(-(float)newRelacionIE / 10, 1) + ":1";
    }

    // Print the first line
    if (lineaAnterior != lineaAlerta)
    {
        lineaAnterior = lineaAlerta;
        lcdPrintFirstLine();
    }

    switch (menuImprimir)
    {
    case MAIN_MENU:
        //if (currentFrecRespiratoria != frecRespiratoriaAnte) {
        if (frecRespiratoriaCalculada != frecRespiratoriaAnte)
        {
            lcd.setCursor(4, 1);
            lcd.print(frecRespiratoriaCalculada);
            if (frecRespiratoriaCalculada < 10)
            {
                lcd.print(" ");
            }
            frecRespiratoriaAnte = frecRespiratoriaCalculada;
            // Serial.println("Changed freq");
        }

        if (Ppico != PpicoAnte)
        {
            lcd.setCursor(15, 1);
            lcd.print(String(int(Ppico)));
            if (Ppico < 10)
            {
                lcd.print(" ");
            }
            PpicoAnte = Ppico;
            // Serial.println("Changed Ppico");
        }

        if ((I != IAnte) || (calculatedE != EAnte))
        {
            lcd.setCursor(4, 2);
            lcd.print(calculatedRelacion_IE + " ");
            IAnte = I;
            EAnte = calculatedE;
            // Serial.println("Changed IE");
        }

        if (currentVentilationMode == 0)
        {
            if (Pcon != PconAnte)
            {
                lcd.setCursor(15, 2);
                lcd.print(String(int(Pcon)));
                if (Pcon < 10)
                {
                    lcd.print(" ");
                }
                PconAnte = Pcon;
            }
        }
        else
        {
            if (newVE != currentVE)
            {
                lcd.setCursor(15, 2);
                lcd.print(String(newVE / 10.0, 1));
                if (newVE < 100)
                {
                    lcd.print(" ");
                }
                currentVE = newVE;
                // Serial.println("Changed Ppico");
            }
        }
        if (Peep != PeepAnte)
        {
            lcd.setCursor(15, 3);
            lcd.print(String(int(Peep)));
            if (Peep < 10)
            {
                lcd.print(" ");
            }
            PeepAnte = Peep;
            // Serial.println("Changed Peep");
        }
        if (VT != VTAnte)
        {
            lcd.setCursor(4, 3);
            lcd.print(String(VT));
            if (VT < 10)
            {
                lcd.print("   ");
            }
            else if (VT < 100)
            {
                lcd.print("  ");
            }
            else if (VT < 1000)
            {
                lcd.print(' ');
            }
            VTAnte = VT;
            // Serial.println("Changed VT");
        }
        break;
    case CONFIG_MENU:
        lcd.setCursor(0, 0);
        if (optionConfigMenu == 0 && insideMenuFlag == true)
        {
            if (lineaAlerta == CONFIG_MENU)
            {
                lcd.write(126);
            }
        }
        else
        {
            if (lineaAlerta == CONFIG_MENU)
            {
                lcd.print(' ');
            }
        }
        if (currentVentilationMode == 1)
        { // A/C Mode
            lcd.setCursor(0, 2);
            if (optionConfigMenu == 1)
            {
                lcd.write(126);
                lcd.setCursor(5, 3);
                if (flagTrigger == true)
                {
                    lcd.write(60);
                    lcd.setCursor(2, 3);
                    lcd.print(trigger);
                    if (trigger < 10)
                    {
                        lcd.print(' ');
                    }
                }
                else
                {
                    lcd.print(' ');
                }
            }
            else
            {
                lcd.print(' ');
            }
            lcd.setCursor(7, 2);
            if (optionConfigMenu == 2)
            {
                lcd.write(126);
                lcd.setCursor(12, 3);
                if (flagFrecuencia == true)
                {
                    lcd.write(60);
                    //if (newFrecRespiratoria != currentFrecRespiratoria) {
                    lcd.setCursor(9, 3);
                    lcd.print(newFrecRespiratoria);
                    if (newFrecRespiratoria < 10)
                    {
                        lcd.print(" ");
                    }
                    frecRespiratoriaAnte = newFrecRespiratoria;
                    //}
                }
                else
                {
                    lcd.print(' ');
                }
            }
            else
            {
                lcd.print(' ');
            }
            lcd.setCursor(14, 2);
            if (optionConfigMenu == 3)
            {
                lcd.write(126);
                lcd.setCursor(19, 3);
                if (flagIE == true)
                {
                    lcd.write(60);
                    //if (newRelacionIE != currentRelacionIE) {
                    lcd.setCursor(14, 3);
                    lcd.print(newRelacion_IE);
                    IAnte = I;
                    EAnte = E;
                    //}
                }
                else
                {
                    lcd.print(' ');
                }
            }
            else
            {
                lcd.print(' ');
            }
        }
        else if (currentVentilationMode == 2)
        { // CPAP Mode
            lcd.setCursor(13, 2);
            lcd.print(int(Peep));
            lcd.setCursor(1, 3);
            if (optionConfigMenu == 1)
            {
                lcd.write(126);
            }
            else
            {
                lcd.print(' ');
            }
            lcd.setCursor(13, 3);
            lcd.print(PeepMax);
            if (PeepMax < 10)
            {
                lcd.print(' ');
            }
            lcd.setCursor(16, 3);
            if (flagPeepMax == true)
            {
                lcd.write(60);
            }
            else
            {
                lcd.print(' ');
            }
        }
        else
        { // P-CMV
            lcd.setCursor(3, 2);
            if (optionConfigMenu == 1)
            {
                lcd.write(126);
                lcd.setCursor(8, 3);
                if (flagFrecuencia == true)
                {
                    lcd.write(60);
                    //if (newFrecRespiratoria != currentFrecRespiratoria) {
                    lcd.setCursor(5, 3);
                    lcd.print(newFrecRespiratoria);
                    if (newFrecRespiratoria < 10)
                    {
                        lcd.print(" ");
                    }
                    frecRespiratoriaAnte = newFrecRespiratoria;
                    //}
                }
                else
                {
                    lcd.print(' ');
                }
            }
            else
            {
                lcd.print(' ');
            }
            lcd.setCursor(12, 2);
            if (optionConfigMenu == 2)
            {
                lcd.write(126);
                lcd.setCursor(18, 3);
                if (flagIE == true)
                {
                    lcd.write(60);
                    //if (newRelacionIE != currentRelacionIE) {
                    lcd.setCursor(12, 3);
                    lcd.print(newRelacion_IE);
                    IAnte = I;
                    EAnte = E;
                    //}
                }
                else
                {
                    lcd.print(' ');
                }
            }
            else
            {
                lcd.print(' ');
            }
        }
        break;
    case VENT_MENU:
        lcd.setCursor(0, 0);
        if (optionVentMenu == 0 && insideMenuFlag == true)
        {
            if (lineaAlerta == VENT_MENU)
            {
                lcd.write(126);
            }
        }
        else
        {
            if (lineaAlerta == VENT_MENU)
            {
                lcd.print(' ');
            }
        }
        lcd.setCursor(0, 2);
        if (optionVentMenu == 1)
        {
            lcd.write(126);
        }
        else
        {
            lcd.print(' ');
        }
        lcd.setCursor(8, 2);
        if (optionVentMenu == 2)
        {
            lcd.write(126);
        }
        else
        {
            lcd.print(' ');
        }
        lcd.setCursor(14, 2);
        if (optionVentMenu == 3)
        {
            lcd.write(126);
        }
        else
        {
            lcd.print(' ');
        }
        break;
    case CONFIG_ALARM:
        lcd.setCursor(0, 0);
        if (optionConfigMenu == 0 && insideMenuFlag == true)
        {
            if (lineaAlerta == CONFIG_ALARM)
            {
                lcd.write(126);
            }
        }
        else
        {
            if (lineaAlerta == CONFIG_ALARM)
            {
                lcd.print(' ');
            }
        }
        lcd.setCursor(0, 2);
        if (optionConfigMenu == 1)
        {
            lcd.write(126);
            lcd.setCursor(5, 3);
            if (flagPresion == true)
            {
                lcd.write(60);
                if (maxPresion != maxPresionAnte)
                {
                    lcd.setCursor(2, 3);
                    lcd.print(maxPresion);
                    if (maxPresion < 10)
                    {
                        lcd.print(" ");
                    }
                    maxPresionAnte = maxPresion;
                }
            }
            else
            {
                lcd.print(' ');
            }
        }
        else
        {
            lcd.print(' ');
        }
        lcd.setCursor(7, 2);
        if (optionConfigMenu == 2)
        {
            lcd.write(126);
            lcd.setCursor(12, 3);
            if (flagMinFR == true)
            {
                lcd.write(60);
                if (maxFR != minFR_Ante)
                {
                    lcd.setCursor(9, 3);
                    lcd.print(maxFR);
                    if (maxFR < 10)
                    {
                        lcd.print(" ");
                    }
                    minFR_Ante = maxFR;
                }
            }
            else
            {
                lcd.print(' ');
            }
        }
        else
        {
            lcd.print(' ');
        }
        lcd.setCursor(14, 2);
        if (optionConfigMenu == 3)
        {
            lcd.write(126);
            lcd.setCursor(19, 3);
            if (flagVE == true)
            {
                lcd.write(60);
                if (maxVE != minVE_Ante)
                {
                    lcd.setCursor(16, 3);
                    lcd.print(maxVE);
                    if (maxVE < 10)
                    {
                        lcd.print(" ");
                    }
                    minVE_Ante = maxVE;
                }
            }
            else
            {
                lcd.print(' ');
            }
        }
        else
        {
            lcd.print(' ');
        }

        break;
    case CONFIRM_MENU:
        if (flagConfirm == true)
        {
            lcd.setCursor(3, 2);
            lcd.write(126);
            lcd.setCursor(13, 2);
            lcd.print(' ');
        }
        else
        {
            lcd.setCursor(3, 2);
            lcd.print(' ');
            lcd.setCursor(13, 2);
            lcd.write(126);
        }
        break;
    case CPAP_MENU:
        lcd.setCursor(0, 1);
        lcd.print("                    ");
        lcd.setCursor(0, 2);
        lcd.print("   PEEP CPAP =      ");
        lcd.setCursor(14, 2);
        lcd.print(String(Peep, 0));
        break;

    case SERVICE_MENU:

        if (flowPac > -2 && flowPac < 2)
        {
            lcd.setCursor(0, 2);
            lcd.print("Prueba OK, sin fugas");
            //flagLeakage = true;
        }
        else
        {
            //if (flagLeakage == false) {
            lcd.setCursor(0, 2);
            lcd.print("Fugas en el circuito");
            //}
        }
        if (flowPacAnte != flowPac)
        {
            lcd.setCursor(6, 3);
            if (flowPac >= 0)
            {
                lcd.print(' ');
            }
            else
            {
                lcd.setCursor(7, 3);
            }
            lcd.print(flowPac);
            lcd.print(" ");

            flowPacAnte = flowPac;
        }
        if (presPacAnte != presPac)
        {
            lcd.setCursor(16, 3);
            if (presPac < 10)
            {
                lcd.print(' ');
            }
            lcd.print(presPac);

            presPacAnte = presPac;
        }

        break;
    }
    //Serial.println("I am in lcd_show()");
}

/** ****************************************************************************
 ** ************ END OF THE CODE ***********************************************
 ** ****************************************************************************/
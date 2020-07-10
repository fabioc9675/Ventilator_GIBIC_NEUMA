/*
 * File:   lcdComplete.ino
 * Author: GIBIC UdeA
 *
 * Created on July 4, 2020, 13:41 PM
 */

/** ****************************************************************************
 ** ************ INCLUDES ******************************************************
 ** ****************************************************************************/
#include "lcdComplete.h"

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
extern int currentFrecRespiratoria;
extern int currentRelacionIE;

// contadores de configuraciones en el menu de usuario
extern String relacion_IE;
extern int newFrecRespiratoria;
extern int newRelacionIE;
extern int currentFrecRespiratoria;
extern int currentRelacionIE;
extern int trigger;
extern int PeepMax;
extern int maxPresion;
extern int maxFR;
extern int maxVE;

// Variables calculadas
extern int frecRespiratoriaCalculada;
extern int calculatedE;
extern int I;
extern int E;

// variables de introduccion a los menus de configuracion
extern int currentVentilationMode;

// Variables para menu actualizado
extern float Peep;
extern float Ppico;
extern float Pcon;
extern byte currentVE;
extern unsigned int VT;

// banderas de cambio de valores
extern volatile uint8_t flagConfirm;

/** ****************************************************************************
 ** ************ VARIABLES *****************************************************
 ** ****************************************************************************/

/** ****************************************************************************
 ** ************ FUNCTIONS *****************************************************
 ** ****************************************************************************/
// Ejecucion de rutina de refrescamiento completo de LCD
void lcd_show_comp(void)
{
    menuAnterior = menuImprimir;
    lineaAnterior = lineaAlerta;
    flagAlreadyPrint = true;
    String calculatedRelacion_IE;
    if (currentRelacionIE > 0)
    {
        //relacion_IE = "1:" + String((float)currentRelacionIE / 10, 1);
        relacion_IE = "1:" + String((float)currentRelacionIE / 10, 1);
        calculatedRelacion_IE = "1:" + String((float)calculatedE / 10.0, 1);
        I = 1;
        E = (char)currentRelacionIE;
    }
    else
    {
        relacion_IE = String(-(float)currentRelacionIE / 10, 1) + ":1";
        I = (char)(-currentRelacionIE);
        E = 1;
    }

    // Print the first line
    lcdPrintFirstLine();

    switch (menuImprimir)
    {
    case MAIN_MENU:
        // lcd.home();
        lcd.setCursor(0, 1);
        lcd.print("FR        PIP     ");
        lcd.setCursor(4, 1);
        lcd.print(frecRespiratoriaCalculada);
        //lcd.print(currentFrecRespiratoria);
        lcd.setCursor(15, 1);
        lcd.print(String(Ppico, 0));
        lcd.setCursor(0, 2);
        if (currentVentilationMode == 0)
        {
            lcd.print("I:E       PCON      ");
            lcd.setCursor(15, 2);
            lcd.print(String(int(Pcon)));
        }
        else
        {
            lcd.print("I:E       VE        ");
            lcd.setCursor(15, 2);
            lcd.print(String(currentVE / 10.0, 1));
        }
        lcd.setCursor(4, 2);
        lcd.print(calculatedRelacion_IE + " ");
        lcd.setCursor(0, 3);
        lcd.print("VT        PEEP      ");
        lcd.setCursor(4, 3);
        lcd.print(String(VT));
        lcd.setCursor(15, 3);
        lcd.print(String(int(Peep)));

        frecRespiratoriaAnte = frecRespiratoriaCalculada;
        PpicoAnte = Ppico;
        IAnte = I;
        EAnte = E;
        PeepAnte = Peep;
        VTAnte = VT;
        PconAnte = Pcon;
        break;
    case CONFIG_MENU:
        lcd.setCursor(0, 1);
        lcd.print("                    ");
        if (currentVentilationMode == 1)
        { // A/C Mode
            lcd.setCursor(0, 2);
            lcd.print(" Trig | Frec | I:E  ");
            lcd.setCursor(0, 3);
            lcd.print("      |      |      ");
            lcd.setCursor(2, 3);
            lcd.print(trigger);
            lcd.setCursor(9, 3);
            lcd.print(currentFrecRespiratoria);
            lcd.setCursor(14, 3);
            lcd.print(relacion_IE);
            frecRespiratoriaAnte = currentFrecRespiratoria;
            IAnte = I;
            EAnte = E;
        }
        else if (currentVentilationMode == 2)
        { // CPAP Mode
            lcd.setCursor(0, 2);
            lcd.print("  PEEP-CPAP:        ");
            lcd.setCursor(13, 2);
            lcd.print(int(Peep));
            lcd.setCursor(0, 3);
            lcd.print("  CPAP MIN:         ");
            lcd.setCursor(13, 3);
            lcd.print(PeepMax);
        }
        else
        { // P-CMV
            lcd.setCursor(0, 2);
            lcd.print("    Frec  |  I:E    ");
            lcd.setCursor(0, 3);
            lcd.print("          |         ");
            lcd.setCursor(5, 3);
            lcd.print(currentFrecRespiratoria);
            lcd.setCursor(12, 3);
            lcd.print(relacion_IE);
            frecRespiratoriaAnte = currentFrecRespiratoria;
            IAnte = I;
            EAnte = E;
        }
        break;
    case VENT_MENU:
        lcd.setCursor(0, 1);
        lcd.print("                    ");
        lcd.setCursor(0, 2);
        lcd.print(" P-CMV   A/C   CPAP ");
        if (currentVentilationMode == 0)
        {
            lcd.setCursor(6, 2);
            lcd.print('*');
        }
        else if (currentVentilationMode == 1)
        {
            lcd.setCursor(12, 2);
            lcd.print('*');
        }
        else
        {
            lcd.setCursor(19, 2);
            lcd.print('*');
        }
        lcd.setCursor(0, 3);
        lcd.print("                    ");
        break;
    case CONFIG_ALARM:
        lcd.setCursor(0, 1);
        lcd.print("                    ");
        lcd.setCursor(0, 2);
        lcd.print("  PIP |  FR  |  VE  ");
        lcd.setCursor(0, 3);
        lcd.print("      |      |      ");
        lcd.setCursor(2, 3);
        lcd.print(maxPresion);
        lcd.setCursor(9, 3);
        lcd.print(maxFR);
        lcd.setCursor(16, 3);
        lcd.print(maxVE);
        maxPresionAnte = maxPresion;
        minFR_Ante = maxFR;
        minVE_Ante = maxVE;
        break;
    case CHECK_MENU:
        lcd.setCursor(0, 1);
        lcd.print("                    ");
        lcd.setCursor(0, 2);
        lcd.print("Valvulas            ");
        lcd.setCursor(0, 3);
        lcd.print("Sensores            ");
        break;
    case CONFIRM_MENU:
        lcd.setCursor(0, 1);
        lcd.print("                    ");
        lcd.setCursor(0, 2);
        if (flagConfirm == true)
        {
            lcd.print("   ");
            lcd.write(126);
            lcd.print("Si        No    ");
        }
        else
        {
            lcd.print("    Si       ");
            lcd.write(126);
            lcd.print("No    ");
        }
        lcd.setCursor(0, 3);
        lcd.print("                    ");
        break;
    case CPAP_MENU:
        lcd.setCursor(0, 1);
        lcd.print("                    ");
        lcd.setCursor(0, 2);
        lcd.print("   PEEP CPAP =      ");
        lcd.setCursor(14, 2);
        lcd.print(int(Peep));
        break;
    case SERVICE_MENU:
        lcd.setCursor(0, 1);
        lcd.print("                    ");
        lcd.setCursor(0, 2);
        lcd.print("   PEEP CPAP =      ");
        lcd.setCursor(14, 2);
        break;
    default:
        lcd.setCursor(0, 0);
        lcd.print("                    ");
        lcd.setCursor(0, 1);
        lcd.print("                    ");
        lcd.setCursor(0, 2);
        lcd.print("Default: ");
        lcd.print(12); // Escribimos el numero de segundos trascurridos
        lcd.print("           ");
        lcd.setCursor(0, 3);
        lcd.print("                    ");
        break;
    }
    //Serial.println("I am in lcd_show()");
}

/** ****************************************************************************
 ** ************ END OF THE CODE ***********************************************
 ** ****************************************************************************/
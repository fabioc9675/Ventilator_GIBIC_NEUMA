/*
 * File:   lcdConfig.ino
 * Author: GIBIC UdeA
 *
 * Created on July 4, 2020, 13:41 PM
 */

/** ****************************************************************************
 ** ************ INCLUDES ******************************************************
 ** ****************************************************************************/
#include "lcdConfig.h"

/** ****************************************************************************
 ** ************ DEFINES *******************************************************
 ** ****************************************************************************/

/** ****************************************************************************
 ** ************ EXTERN VARIABLES **********************************************
 ** ****************************************************************************/
// **********************************************************
extern portMUX_TYPE mux;

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
extern volatile signed int menu;

// variables de menu
extern volatile unsigned int menuImprimir;
extern volatile unsigned int lineaAlerta;
extern volatile uint8_t flagAlreadyPrint;
extern int menuAlerta[ALARM_QUANTITY + 1];

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
extern byte newVE;

// banderas de cambio de valores
extern volatile uint8_t flagConfirm;

// variables de introduccion a los menus de configuracion
extern int optionConfigMenu;
extern int optionVentMenu;
extern int batteryAlert;

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

// variables de estado de ventilacion
extern int stateMachine;
extern volatile bool flagService;

// banderas de botones de usuario
extern volatile uint8_t flagStandbyInterrupt;
extern volatile uint8_t flagSilenceInterrupt;
extern volatile uint8_t flagStabilityInterrupt;
extern volatile uint8_t flagBatterySilence;
extern volatile uint8_t flagAlerta;
extern volatile uint8_t flagBatteryAlert;

// variables contadores de conido de silencio y alarmas
extern unsigned int contSilence;
extern unsigned int contSilenceBattery;
extern unsigned int contStandby;

// variables de alerta
extern int alerPresionPIP;
extern int alerDesconexion;
extern int alerObstruccion;
extern int alerGeneral;
extern int alerPresionPeep;
extern int alerFR_Alta;
extern int alerVE_Alto;

/** ****************************************************************************
 ** ************ VARIABLES *****************************************************
 ** ****************************************************************************/
// variables contador para verificacion de alertas
unsigned int contLowAtten = 0;
int newBatteryAlert = 1;
int currentBatteryAlert = 1; // When is working with energy supply
unsigned int contBattery = 0;
unsigned int contBattery5min = 0;

volatile uint8_t flagChange2AC = false;
unsigned int contAlertas = 0;

// banderas de impresion de menu
volatile uint8_t flagFirst = false;
volatile uint8_t flagEntre = false;
unsigned int temp = 0;


/** ****************************************************************************
 ** ************ FUNCTIONS *****************************************************
 ** ****************************************************************************/
// Configuracion del LCD
void lcd_setup(void)
{
    lcd.begin(21, 22);
    lcd.backlight();

    if (flagService == true)
    {
        lcd.setCursor(0, 0);
        lcd.print("   MODO SERVICIO    ");
        lcd.setCursor(0, 2);
        lcd.print("AL FINALIZAR INICIE ");
        lcd.setCursor(0, 3);
        lcd.print("EL EQUIPO DE NUEVO  ");
    }
}

// Ejecucion de la rutina de refrescamiento del LCD
void lcdPrintFirstLine(void)
{
    lcd.setCursor(0, 0);
    switch (lineaAlerta)
    {
    case MAIN_MENU:
        if (stateMachine == STANDBY_STATE)
        {
            lcd.print("    Modo Standby    ");
        }
        else
        {
            lcd.print("  GIBIC Neuma ");
            if (currentVentilationMode == 0)
            {
                lcd.print("P-CMV ");
            }
            else if (currentVentilationMode == 1)
            {
                lcd.print("A/C   ");
            }
            else
            {
                lcd.print("CPAP  ");
            }
        }
        break;
    case CONFIG_MENU:
        if (currentVentilationMode == 0)
        {
            lcd.print(" Configuracion P-CMV");
        }
        else if (currentVentilationMode == 1)
        {
            lcd.print("  Configuracion A/C ");
        }
        else
        {
            lcd.print(" Configuracion CPAP ");
        }
        break;
    case CONFIG_ALARM:
        lcd.print("      Alarmas       ");
        break;
    case VENT_MENU:
        lcd.print(" Modo Ventilatorio  ");
        if (insideMenuFlag == true && optionVentMenu == 0)
        {
            lcd.setCursor(0, 0);
            lcd.write(126);
        }
        break;
    case SERVICE_MENU:
        lcd.print("  Chequeo de fugas  ");
        break;
    case ALE_PRES_PIP:
        lcd.print("Presion PIP elevada ");
        break;
    case ALE_PRES_DES:
        lcd.print("Desconexion Paciente");
        break;
    case ALE_OBSTRUCCION:
        lcd.print("    Obstruccion     ");
        break;
    case ALE_GENERAL:
        lcd.print("   Fallo general   ");
        break;
    case ALE_PRES_PEEP:
        lcd.print("  Perdida de PEEP   ");
        break;
    case BATTERY:
        lcd.print(" Fallo red electrica");
        break;
    case ALE_BATTERY_10MIN:
        lcd.print("Bateria baja 10 Min");
        break;
    case ALE_BATTERY_5MIN:
        lcd.print(" Bateria baja 5 Min ");
        break;
    case ALE_FR_ALTA:
        lcd.print("      FR alta       ");
        break;
    case ALE_VE_ALTO:
        lcd.print("    Vol/min alto    ");
        break;
    case ALE_APNEA:
        lcd.print("       Apnea        ");
        break;
    case CHECK_MENU:
        lcd.print("Comprobacion Inicial");
        break;
    case CONFIRM_MENU:
        lcd.print(" Confirmar cambios  ");
        break;
    case CPAP_MENU:
        lcd.print(" Configuracion CPAP ");
        break;
    default:
        break;
    }
}

// Atencion a las alertas
void alarmMonitoring(void)
{
    contLowAtten++; // contador para generar atencion a bajas frecuencias
    // ejecuta tareas de baja prioridad en tiempo
    if (contLowAtten == LOW_ATT_INT)
    {
        contLowAtten = 0;

        newBatteryAlert = digitalRead(BATTALARM);
        switch (batteryAlert)
        {
        case BATTERY_NO_ALARM:
            if (newBatteryAlert == 0)
            { // Battery Alarm
                portENTER_CRITICAL_ISR(&mux);
                flagBatterySilence = false;
                portEXIT_CRITICAL_ISR(&mux);
                contSilenceBattery = 0;
                currentBatteryAlert = newBatteryAlert;
                batteryAlert = batteryAlarm;
                digitalWrite(LUMINB, HIGH);
                sendSerialData();
            }
            break;
        case batteryAlarm:
            if (newBatteryAlert == 1)
            {
                contBattery++;
            }
            else if (contBattery > 2 && contBattery < 50)
            {
                contBattery = 0;
                batteryAlert = batteryAlarm10min;
                sendSerialData();
            }
            if (contBattery > 100)
            {
                contBattery = 0;
                currentBatteryAlert = newBatteryAlert;
                batteryAlert = BATTERY_NO_ALARM;
                digitalWrite(LUMINB, LOW);
                sendSerialData();
            }
            break;
        case batteryAlarm10min:
            if (newBatteryAlert == 1)
            {
                contBattery++;
            }
            else
            {
                contBattery = 0;
            }
            if (contBattery > 100)
            {
                contBattery = 0;
                contBattery5min = 0;
                currentBatteryAlert = newBatteryAlert;
                batteryAlert = BATTERY_NO_ALARM;
                digitalWrite(LUMINB, LOW);
                sendSerialData();
            }
            contBattery5min++;
            if (contBattery5min > 6000)
            {
                contBattery = 0;
                contBattery5min = 0;
                batteryAlert = batteryAlarm5min;
            }
            break;
        case batteryAlarm5min:
            if (newBatteryAlert == 1)
            {
                contBattery++;
            }
            if (contBattery > 100)
            {
                contBattery = 0;
                currentBatteryAlert = newBatteryAlert;
                batteryAlert = BATTERY_NO_ALARM;
                digitalWrite(LUMINB, LOW);
                sendSerialData();
            }
            break;
        default:
            break;
        }

        // verifica la condicion de silenciar alarmas
        silenceInterruptAttention();
        // activacion alerta de bateria
        if (currentBatteryAlert == 0)
        {
            menuAlerta[8] = BATTERY;
            if (batteryAlert == 2)
            {
                menuAlerta[9] = ALE_BATTERY_10MIN;
            }
            else if (batteryAlert == 3)
            {
                menuAlerta[9] = ALE_BATTERY_5MIN;
                flagAlerta = true;
            }
            flagBatteryAlert = true;
        }
        else
        {
            menuAlerta[8] = 0;
            menuAlerta[9] = 0;
            flagBatteryAlert = false;
        }

        if ((alerVE_Alto == 1) && (stateMachine == AC_STATE))
        {
            menuAlerta[7] = ALE_VE_ALTO;
            flagAlerta = true;
        }
        else
        {
            menuAlerta[7] = 0;
        }

        if ((alerFR_Alta == 1) && ((stateMachine == AC_STATE) || (stateMachine == CPAP_STATE)))
        {
            menuAlerta[6] = ALE_FR_ALTA;
            flagAlerta = true;
        }
        else if ((alerFR_Alta == 2))
        {
            menuAlerta[6] = ALE_APNEA;
            if (stateMachine != STANDBY_STATE && flagChange2AC == false)
            {
                flagChange2AC = true;
                stateMachine = AC_STATE;
                insideMenuFlag = false;
                flagPeepMax = false;
                optionConfigMenu = 0;
                menu = VENT_MENU;
                menuImprimir = menu;
                lineaAlerta = menu;
                flagAlreadyPrint = false;
            }
            currentVentilationMode = 1;
            flagAlerta = true;
        }
        else
        {
            menuAlerta[6] = 0;
            flagChange2AC = false;
        }

        // activacion alerta general
        if (alerObstruccion == 1)
        {
            menuAlerta[5] = ALE_OBSTRUCCION;
            flagAlerta = true;
        }
        else
        {
            menuAlerta[5] = 0;
        }
        // activacion alerta presion alta
        if (alerPresionPIP == 1)
        {
            menuAlerta[4] = ALE_PRES_PIP;
            flagAlerta = true;
        }
        else
        {
            menuAlerta[4] = 0;
        }

        if (alerPresionPeep == 1)
        {
            digitalWrite(LUMING, LOW);
            menuAlerta[3] = ALE_PRES_PEEP;
            flagAlerta = true;
        }
        else
        {
            menuAlerta[3] = 0;
        }
        // activacion alerta desconexion
        if (alerDesconexion == 1)
        {
            menuAlerta[2] = ALE_PRES_DES;
            flagAlerta = true;
        }
        else
        {
            menuAlerta[2] = 0;
        }
        if (alerGeneral == 1)
        {
            flagAlerta = true;
            menuAlerta[1] = ALE_GENERAL;
            stateMachine = FAILURE_STATE;
            digitalWrite(STANDBY_LED, LOW);
        }
        else
        {
            menuAlerta[1] = 0;
        }
        // desactivacion alertas
        if ((alerPresionPIP == 0) && (alerDesconexion == 0) && (alerObstruccion == 0) &&
            (alerPresionPeep == 0) && (alerGeneral == 0) && (menuAlerta[9] != ALE_BATTERY_5MIN) &&
            (menuAlerta[7] == 0) && (menuAlerta[6] == 0))
        {
            flagAlerta = false;
            for (int i = 1; i < ALARM_QUANTITY - 1; i++)
            {
                menuAlerta[i] = 0;
            }
            portENTER_CRITICAL_ISR(&mux);
            flagSilenceInterrupt = false;
            portEXIT_CRITICAL_ISR(&mux);
            contSilence = 0;
        }
    }
}




/* ***************************************************************************
 * **** Ejecucion de la rutina de refrescado de Display ++********************
 * ***************************************************************************/
void task_Display(void* pvParameters) {
	String taskMessage = "LCD Task one running on core ";
	taskMessage = taskMessage + xPortGetCoreID();
	// Serial.println(taskMessage);

	lcd_setup(); // inicializacion de LCD
	lcd.setCursor(0, 0);

	while (true) {
		/* *************************************************
		 * **** Analisis de los estados de alarma **********
		 * ************************************************/
		if (flagAlerta == true || flagBatteryAlert == true) {
			if (flagBatteryAlert == true) {
				contSilenceBattery++;
			}
			menuAlerta[0] = menu;
			contAlertas++;
			
			if (contAlertas == 3) {
				
				/*Serial.println(String(menuAlerta[0]) + ',' + String(menuAlerta[1]) + ',' +
					String(menuAlerta[2]) + ',' + String(menuAlerta[3]) + ',' +
					String(menuAlerta[4]) + ',' + String(menuAlerta[5]));
				Serial.println(temp);*/
				flagFirst = false;
				flagEntre = false;
				for (int i = 0; i < ALARM_QUANTITY + 1; i++) {
					if ((menuAlerta[i] != 0) && (i > temp) && (flagFirst == false)) {
						flagFirst = true;
						flagEntre = true;
						temp = i;
					}
					if (flagEntre == false && i == ALARM_QUANTITY) {
						temp = 0;
					}
				}

				//if (flagSilenceInterrupt == false || flagBatterySilence == false) {
				digitalWrite(BUZZER_BTN, LOW);
				//}
				if (flagAlerta == true) {
					digitalWrite(LUMINR, HIGH);
				}
				//digitalWrite(LUMING, HIGH);
				//digitalWrite(LUMINB, HIGH);
				digitalWrite(SILENCE_LED, HIGH);
			}
			else if (contAlertas == 10) {
				lineaAlerta = menuAlerta[temp];
				flagAlreadyPrint = false;
				if ((!flagSilenceInterrupt && flagAlerta) || (!flagBatterySilence && flagBatteryAlert)) {
					digitalWrite(BUZZER_BTN, HIGH);
				}
				else {
					digitalWrite(BUZZER_BTN, LOW);
				}
				digitalWrite(LUMINR, LOW);
				digitalWrite(SILENCE_LED, LOW);
			}
			else if (contAlertas == 13) {
				digitalWrite(BUZZER_BTN, LOW);
				digitalWrite(SILENCE_LED, HIGH);
				if (flagAlerta == true) {
					digitalWrite(LUMINR, HIGH);
				}
			}
			else if (contAlertas >= 20) {
				lineaAlerta = menu;
				flagAlreadyPrint = false;
				if ((!flagSilenceInterrupt && flagAlerta) || (!flagBatterySilence && flagBatteryAlert)) {
					digitalWrite(BUZZER_BTN, HIGH);
				}
				else {
					digitalWrite(BUZZER_BTN, LOW);
				}
				digitalWrite(LUMINR, LOW);
				digitalWrite(SILENCE_LED, LOW);
				contAlertas = 0;
			}
		}
		else {
			contAlertas++;
			if (contAlertas == 10) {
				lineaAlerta = menu;
				flagAlreadyPrint = false;
				digitalWrite(LUMINR, LOW);
				//digitalWrite(LUMING, LOW);
				digitalWrite(SILENCE_LED, LOW);
				digitalWrite(BUZZER_BTN, LOW);
				contSilence = 0;
				flagSilenceInterrupt = false;
			}
			else if (contAlertas >= 20) {
				lineaAlerta = menu;
				flagAlreadyPrint = false;
				contAlertas = 0;
			}
		}

		/* ****************************************************************
		 * **** Actualizacion de valores en pantalla LCD ******************
		 * ***************************************************************/
		if(flagService == false){
			if ((menuAnterior != menuImprimir) && (flagAlreadyPrint == false)) {
				lcd.clear();
				lineaAnterior = MODE_CHANGE;
				lcd_show_comp();
			}
			else {
				lcd_show_part();
			}
		}
		// delay de 100 ms en la escritura del LCD
		vTaskDelay(100 / portTICK_PERIOD_MS);
	}
}










/** ****************************************************************************
 ** ************ END OF THE CODE ***********************************************
 ** ****************************************************************************/
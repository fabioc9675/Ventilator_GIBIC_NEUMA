/*
 * File:   timer.ino
 * Author: GIBIC UdeA
 *
 * Created on July 4, 2020, 13:41 PM
 */

/** ****************************************************************************
 ** ************ INCLUDES ******************************************************
 ** ****************************************************************************/
#include "timer.h"

/** ****************************************************************************
 ** ************ DEFINES *******************************************************
 ** ****************************************************************************/

/** ****************************************************************************
 ** ************ EXTERN VARIABLES **********************************************
 ** ****************************************************************************/
// **********************************************************
extern volatile uint8_t flagTimerInterrupt;
extern volatile uint8_t flagAdcInterrupt;

extern SemaphoreHandle_t xSemaphoreTimer;
extern SemaphoreHandle_t xSemaphoreRaspberry;
extern SemaphoreHandle_t xSemaphoreAdc;

extern unsigned int contCiclos;	 // variable que controla el numero de ciclos que ha realizado un ventilador
extern volatile int contCycling; // variable que cuenta los ms del ciclado para cambiar estado de electrovalvulas

// Variables maquina de estados de modos ventilatorios
extern int currentStateMachine;
extern int newStateMachine;

/** ****************************************************************************
 ** ************ VARIABLES *****************************************************
 ** ****************************************************************************/

// inicializacion del contador del timer
hw_timer_t *timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

// variables contadores
unsigned int contEscrituraEEPROM = 0;

// contadores de tiempo
int second = 0;
int milisecond = 0;

/** ****************************************************************************
 ** ************ FUNCTIONS *****************************************************
 ** ****************************************************************************/

// Inicializacion del timer
void init_Timer(void)
{
	// Configuracion del timer a 1 kHz
	timer = timerBegin(0, 80, true);			 // Frecuencia de reloj 80 MHz, prescaler de 80, frec 1 MHz
	timerAttachInterrupt(timer, &onTimer, true); // Attach onTimer function to our timer
	timerAlarmWrite(timer, 1000, true);			 // Interrupcion cada 1000 conteos del timer, es decir 100 Hz
	timerAlarmEnable(timer);					 // Habilita interrupcion por timer
}

// Interrupcion por timer
void IRAM_ATTR onTimer(void)
{
	portENTER_CRITICAL_ISR(&timerMux);
	flagTimerInterrupt = true; // asignacion de banderas para atencion de interrupcion
	flagAdcInterrupt = true;
	xSemaphoreGiveFromISR(xSemaphoreTimer, NULL); // asignacion y liberacion de semaforos
	xSemaphoreGiveFromISR(xSemaphoreAdc, NULL);
	portEXIT_CRITICAL_ISR(&timerMux);
}

/* **************************************************************************
 * **** TAREA PARA EL MANEJO DE LA INTERRUPCION DEL TIMER *******************
 * **************************************************************************/
void task_Timer(void *arg)
{
	while (1)
	{
		// Se atiende la interrpcion del timer
		if (xSemaphoreTake(xSemaphoreTimer, portMAX_DELAY) == pdTRUE)
		{
			if (flagTimerInterrupt == true)
			{
				portENTER_CRITICAL(&timerMux);
				flagTimerInterrupt = false;
				portEXIT_CRITICAL(&timerMux);

				/* *************************************************************
				 * **** SECUENCIA DE FUNCIONAMIENTO, ESTADOS DEL VENTILADOR ****
				 * *************************************************************/
				switch (currentStateMachine)
				{

				case CHECK_STATE: // Estado de checkeo
					break;
				case SERVICE_STATE: // Estado de servicio, interface de calibracion
					break;
				case STANDBY_STATE: // Modo StandBy
					standbyRoutine();
					// Serial.println("Standby state on control Unit");
					break;
				case PCMV_STATE: // Modo Controlado por presion
					cycling();

					// Write the EEPROM each 10 minutes
					contEscrituraEEPROM++;
					if (contEscrituraEEPROM > 3600000)
					{
						contEscrituraEEPROM = 0;
						eeprom_wr_int(contCiclos, 'w');
					}
					// Serial.println("I am on PCMV_STATE");
					break;
				case AC_STATE: // Modo asistido controlado por presion
					cycling();
					// Write the EEPROM each 10 minutes
					contEscrituraEEPROM++;
					if (contEscrituraEEPROM > 3600000)
					{
						contEscrituraEEPROM = 0;
						eeprom_wr_int(contCiclos, 'w');
					}
					//Serial.println("I am on AC_STATE");
					break;
				case CPAP_STATE: // Modo CPAP
					cpapRoutine();
					//Serial.println("I am on CPAP_STATE");
					break;
				case FAILURE_STATE:
					currentStateMachine = STANDBY_STATE;
					//Serial.println("I am on FAILURE_STATE");
					break;
				default:
					currentStateMachine = STANDBY_STATE;
					//Serial.println("I am on DEFAULT_STATE");
					break;
				}

				// Secuencia de prueba, contador en segundos, borrar
				milisecond++;
				if (milisecond == 1000)
				{
					milisecond = 0;
					second++;
					digitalWrite(2, !digitalRead(2));

					if (second == 60)
					{
						second = 0;
					}
				}
			}
		}
	}
	vTaskDelete(NULL);
}

/** ****************************************************************************
 ** ************ END OF THE CODE ***********************************************
 ** ****************************************************************************/

/*
  Name:		configurationTest.ino
  Created:	12/06/2020 11:40:24
  Author:	Fabian Castano
*/

#include <nvs_flash.h>
#include <stdio.h>
#include <Wire.h>
#include "LiquidCrystal_I2C.h"

//LiquidCrystal_I2C lcd(0x27, 20, 4);
LiquidCrystal_I2C lcd(0x3F, 20, 4);

//********DEFINICIONES CONDICIONES******
#define TRUE 1
#define FALSE 0

//********DEFINICION DE PINES***********
#define A 18 //variable A a pin digital 2 (DT en modulo)
#define B 19 //variable B a pin digital 4 (CLK en modulo)
#define SW 5 //sw a pin digital 3 (SW en modulo)

#define BUZZER_BTN 12
#define SILENCE_BTN 26 // Silenciar alarma cambiar
#define SILENCE_LED 27 // Led Boton silencio
#define STABILITY_BTN 34
#define STABILITY_LED 25
#define STANDBY 32	   // Stabdby button
#define STANDBY_LED 33 // Stabdby button

#define LUMING 13 // Alarma luminosa
#define LUMINR 14
#define LUMINB 15
#define BATTALARM 4

#define LED 2

#define DEBOUNCE_ENC 50		   // tiempo para realizar antirrebote
#define DEBOUNCE_ENC_2 400	   // tiempo para realizar antirrebote
#define DEBOUNCE_ENC_OUT 300   // tiempo para realizar antirrebote
#define DEBOUNCE_ENC_OUT_2 800 // tiempo para realizar antirrebote
#define DEBOUNCE_ENC_SW 400	   // tiempo para realizar antirrebote
#define LOW_ATT_INT 50		   // Interrupcion cada 10 ms

#define ENCOD_INCREASE 1 // movimiento a la derecha, aumento
#define ENCOD_DECREASE 2 // movimiento a la derecha, aumento
#define ENCOD_COUNT    3 // cantidad de interrupciones antes de reconocer el conteo

volatile bool flagDetachInterrupt_A = false;
volatile bool flagDetachInterrupt_B = false;
volatile bool flagDetachInterrupt_A_B = false;
volatile bool flagDetachInterrupt_B_A = false;
volatile bool flagDetachInterrupt_S = false;
volatile bool flagDetach = false;
unsigned int contDetachA = 0;
unsigned int contDetachB = 0;
unsigned int contDetachS = 0;
unsigned int contAlarm = 0;
unsigned int contRecogIntA = 0; // contador para reconocimiento del conteo del encoder
unsigned int contRecogIntB = 0; // contador para reconocimiento del conteo del encoder
unsigned int contLowAtten = 0;

unsigned int fl_StateEncoder = 0;

//creo el manejador para el sem�foro como variable global
SemaphoreHandle_t xSemaphoreEncoder = NULL;
SemaphoreHandle_t xSemaphoreTimer = NULL;
//xQueueHandle timer_queue = NULL;

volatile uint8_t flagAEncoder = false;
volatile uint8_t flagBEncoder = false;
volatile uint8_t flagSEncoder = false;
volatile uint8_t flagTimerInterrupt = false;
volatile uint8_t flagSilenceInterrupt = false;
volatile uint8_t flagStandbyInterrupt = false;
volatile uint8_t flagStabilityInterrupt = false;

uint8_t fl1 = 0;
uint8_t fl2 = 0;
uint8_t fl3 = 0;

volatile uint8_t insideMenuFlag = false;

// inicializacion del contador del timer
hw_timer_t *timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

// definicion de interrupciones
void IRAM_ATTR onTimer(void); // funcion de interrupcion
void IRAM_ATTR swInterrupt(void);
void IRAM_ATTR encoderInterrupt_A(void);
void IRAM_ATTR encoderInterrupt_B(void);
void IRAM_ATTR standbyButtonInterrupt(void);
void IRAM_ATTR silenceButtonInterrupt(void);
void IRAM_ATTR stabilityButtonInterrupt(void);

// definicion de los core para ejecucion
static uint8_t taskCoreZero = 0;
static uint8_t taskCoreOne = 1;

unsigned int counterEncoder = 0;

/************************************************************
 ********** FUNCIONES DE INICIALIZACION *********************
 ***********************************************************/
void init_GPIO()
{
	//Encoder setup
	pinMode(A, INPUT_PULLUP);			// A como entrada
	pinMode(B, INPUT_PULLUP);			// B como entrada
	pinMode(SW, INPUT_PULLUP);			// SW como entrada
	pinMode(SILENCE_BTN, INPUT_PULLUP); // switch para el manejo de silencio
	pinMode(STANDBY, INPUT_PULLUP);
	pinMode(STABILITY_BTN, INPUT_PULLUP);
	pinMode(BUZZER_BTN, OUTPUT);
	pinMode(LUMINR, OUTPUT);
	pinMode(LUMING, OUTPUT);
	pinMode(LUMINB, OUTPUT);
	pinMode(SILENCE_LED, OUTPUT);
	pinMode(STANDBY_LED, OUTPUT);
	pinMode(STABILITY_LED, OUTPUT);

	pinMode(BATTALARM, INPUT);

	attachInterrupt(digitalPinToInterrupt(A), encoderInterrupt_A, FALLING);
	attachInterrupt(digitalPinToInterrupt(B), encoderInterrupt_B, FALLING);
	attachInterrupt(digitalPinToInterrupt(SW), swInterrupt, RISING);
	attachInterrupt(digitalPinToInterrupt(STANDBY), standbyButtonInterrupt, FALLING);
	attachInterrupt(digitalPinToInterrupt(SILENCE_BTN), silenceButtonInterrupt, FALLING);
	attachInterrupt(digitalPinToInterrupt(STABILITY_BTN), stabilityButtonInterrupt, FALLING);

	digitalWrite(STANDBY_LED, HIGH);
	digitalWrite(STABILITY_LED, HIGH);
	digitalWrite(SILENCE_LED, HIGH);
	pinMode(LED, OUTPUT);

	digitalWrite(LUMINR, HIGH);
	digitalWrite(LUMING, HIGH);
	digitalWrite(LUMINB, HIGH);
}

void init_TIMER()
{
	// Configuracion del timer a 1 kHz
	timer = timerBegin(0, 80, true);			 // Frecuencia de reloj 80 MHz, prescaler de 80, frec 1 MHz
	timerAttachInterrupt(timer, &onTimer, true); // Attach onTimer function to our timer
	timerAlarmWrite(timer, 1000, true);			 // Interrupcion cada 1000 conteos del timer, es decir 100 Hz
	timerAlarmEnable(timer);					 // Habilita interrupcion por timer
}

/* *********************************************************************
 * **** FUNCIONES DE ATENCION A INTERRUPCION ***************************
 * *********************************************************************/
// Interrupcion por presion del switch
void IRAM_ATTR swInterrupt(void)
{
	// da el semaforo para que quede libre para la tarea pulsador
	portENTER_CRITICAL_ISR(&mux);
	flagSEncoder = true;
	xSemaphoreGiveFromISR(xSemaphoreEncoder, NULL);
	portEXIT_CRITICAL_ISR(&mux);
}

// Interrupcion por encoder A
void IRAM_ATTR encoderInterrupt_A(void)
{
	portENTER_CRITICAL_ISR(&mux);
	flagAEncoder = true;
	xSemaphoreGiveFromISR(xSemaphoreEncoder, NULL);
	portEXIT_CRITICAL_ISR(&mux);
}

// Interrupcion por encoder B
void IRAM_ATTR encoderInterrupt_B(void)
{
	portENTER_CRITICAL_ISR(&mux);
	flagBEncoder = true;
	xSemaphoreGiveFromISR(xSemaphoreEncoder, NULL);
	portEXIT_CRITICAL_ISR(&mux);
}

// Interrupcion por timer
void IRAM_ATTR onTimer(void)
{
	portENTER_CRITICAL_ISR(&timerMux);
	flagTimerInterrupt = true;
	xSemaphoreGiveFromISR(xSemaphoreTimer, NULL);
	portEXIT_CRITICAL_ISR(&timerMux);
}

void IRAM_ATTR standbyButtonInterrupt()
{
	portENTER_CRITICAL_ISR(&mux);
	flagStandbyInterrupt = true;
	detachInterrupt(digitalPinToInterrupt(STANDBY));
	portEXIT_CRITICAL_ISR(&mux);
}

// Interrupcion por button silence
void IRAM_ATTR silenceButtonInterrupt(void)
{
	portENTER_CRITICAL_ISR(&mux);
	flagSilenceInterrupt = true;
	detachInterrupt(digitalPinToInterrupt(SILENCE_BTN));
	portEXIT_CRITICAL_ISR(&mux);
}

void IRAM_ATTR stabilityButtonInterrupt(void)
{
	portENTER_CRITICAL_ISR(&mux);
	flagStabilityInterrupt = true;
	detachInterrupt(digitalPinToInterrupt(STABILITY_BTN));
	portEXIT_CRITICAL_ISR(&mux);
}

/************************************************************
 ***** FUNCIONES DE ATENCION A INTERRUPCION TAREA TIMER *****
 ************************************************************/
void task_timer(void *arg)
{
	int contms = 0;
	uint8_t debounceENC = 0;
	uint8_t debounceENC_2 = 0;

	while (1)
	{
		// Se atiende la interrupcion del timer
		if (xSemaphoreTake(xSemaphoreTimer, portMAX_DELAY) == pdTRUE)
		{
			if (flagTimerInterrupt == true)
			{
				portENTER_CRITICAL(&timerMux);
				flagTimerInterrupt = false;
				portEXIT_CRITICAL(&timerMux);

				contms++;
				if (contms % 25 == 0)
				{
					digitalWrite(LED, !digitalRead(LED));

					silenceInterruptAttention();
					standbyInterruptAttention();
					stabilityInterruptAttention();
				}
				if (contms == 1000)
				{
					contms = 0;
					digitalWrite(BUZZER_BTN, 0);
					sendSerialData();
					attachInterrupt(digitalPinToInterrupt(STANDBY), standbyButtonInterrupt, FALLING);
					attachInterrupt(digitalPinToInterrupt(SILENCE_BTN), silenceButtonInterrupt, FALLING);
					attachInterrupt(digitalPinToInterrupt(STABILITY_BTN), stabilityButtonInterrupt, FALLING);
					// digitalWrite(LED, !digitalRead(LED));
					// Serial.print("LED_SEGUNDO ");
					// Serial.print("Core ");
					// Serial.println(xPortGetCoreID());
				}
				/****************************************************************************
				****  En esta seccion del codigo se agregan de nuevo las interrupciones  ****
				****************************************************************************/
				if (insideMenuFlag)
				{ // si esta configurando un parametro
					debounceENC = DEBOUNCE_ENC;
					debounceENC_2 = DEBOUNCE_ENC_2;
				}
				else
				{
					debounceENC = DEBOUNCE_ENC_OUT;
					debounceENC_2 = DEBOUNCE_ENC_OUT_2;
				}

				// Agregar interrupcion A
				if ((flagDetachInterrupt_A == true) || (flagDetachInterrupt_B_A == true))
				{
					contDetachA++;
					if ((contDetachA >= debounceENC) && (flagDetachInterrupt_A == true))
					{
						contDetachA = 0;
						flagDetachInterrupt_A = false;
						attachInterrupt(digitalPinToInterrupt(A), encoderInterrupt_A, FALLING);
					}
					if ((contDetachA >= debounceENC_2) && (flagDetachInterrupt_B_A == true))
					{
						contDetachB = 0;
						flagDetachInterrupt_B_A = false;
						attachInterrupt(digitalPinToInterrupt(B), encoderInterrupt_B, FALLING);
					}
				}
				// Agregar interrupcion B
				if ((flagDetachInterrupt_B == true) || (flagDetachInterrupt_A_B == true))
				{
					contDetachB++;
					if ((contDetachB >= debounceENC) && (flagDetachInterrupt_B == true))
					{
						contDetachB = 0;
						flagDetachInterrupt_B = false;
						attachInterrupt(digitalPinToInterrupt(B), encoderInterrupt_B, FALLING);
					}
					if ((contDetachB >= debounceENC_2) && (flagDetachInterrupt_A_B == true))
					{
						contDetachB = 0;
						flagDetachInterrupt_A_B = false;
						attachInterrupt(digitalPinToInterrupt(A), encoderInterrupt_A, FALLING);
					}
				}
				// Agregar interrupcion S
				if (flagDetachInterrupt_S == true)
				{
					contDetachS++;
					if (contDetachS >= DEBOUNCE_ENC_SW)
					{
						contDetachS = 0;
						flagDetachInterrupt_S = false;
						attachInterrupt(digitalPinToInterrupt(SW), swInterrupt, FALLING);
					}
				} // Finaliza agregar interrupciones
			}
		}
	}
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
						counterEncoder++;
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
						counterEncoder--;
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
				digitalWrite(LUMINR, !digitalRead(LUMINR));
				digitalWrite(LUMING, !digitalRead(LUMING));
				digitalWrite(LUMINB, !digitalRead(LUMINB));
			}
		}
	}
}

/* ***************************************************************************
 * **** Atencion a interrupcion por buttons **********************************
 * ***************************************************************************/
void silenceInterruptAttention()
{
	if (flagSilenceInterrupt == true)
	{
		flagSilenceInterrupt = false;
		digitalWrite(BUZZER_BTN, 1);
		digitalWrite(LUMINR, !digitalRead(LUMINR));
		digitalWrite(LUMING, !digitalRead(LUMING));
		digitalWrite(LUMINB, !digitalRead(LUMINB));
		digitalWrite(SILENCE_LED, !digitalRead(SILENCE_LED));
		fl1 = 1;
		
	}
}

void standbyInterruptAttention()
{
	//Serial.println("Standby Interrupt");
	if (flagStandbyInterrupt)
	{
		flagStandbyInterrupt = false;
		digitalWrite(BUZZER_BTN, 1);
		digitalWrite(LUMINR, !digitalRead(LUMINR));
		digitalWrite(LUMING, !digitalRead(LUMING));
		digitalWrite(LUMINB, !digitalRead(LUMINB));
		digitalWrite(STANDBY_LED, !digitalRead(STANDBY_LED));
		fl2 = 1;

	}
}

void stabilityInterruptAttention()
{
	//Serial.println("Standby Interrupt");
	if (flagStabilityInterrupt)
	{
		flagStabilityInterrupt = false;
		digitalWrite(BUZZER_BTN, 1);
		digitalWrite(LUMINR, !digitalRead(LUMINR));
		digitalWrite(LUMING, !digitalRead(LUMING));
		digitalWrite(LUMINB, !digitalRead(LUMINB));
		digitalWrite(STABILITY_LED, !digitalRead(STABILITY_LED));
		fl3 = 1;
		
	}
}

/************************************************************
 ***** Ejecucion de la rutina de refrescamiento del LCD *****
 ***********************************************************/
void lcd_show_comp()
{

	lcd.setCursor(0, 1);
	lcd.print("PRUEBA GIBIC NEUMA");
	lcd.setCursor(0, 2);
	lcd.print("CONTEO = ");
	//lcd.print(currentFrecRespiratoria);
	lcd.setCursor(10, 2);
	lcd.print(counterEncoder);
	lcd.print("     ");

	if (fl1 == 1) {
		fl1 = 0;
		lcd.setCursor(0, 3);
		lcd.print("Silence Pressed  ");
	}
	if (fl2 == 1) {
		fl2 = 0;
		lcd.setCursor(0, 3);
		lcd.print("Stadby Pressed   ");
	}
    if (fl3 == 1) {
		fl3 = 0;
		lcd.setCursor(0, 3);
		lcd.print("Stability Pressed  ");
	}


}

/* ***************************************************************************
 * **** Ejecucion de la rutina de comunicacion por serial ********************
 * ***************************************************************************/
// Function to receive data from serial communication
void task_Receive(void *pvParameters)
{
	// Clean Serial buffers
	vTaskDelay(1000 / portTICK_PERIOD_MS);
	Serial.flush();
	Serial2.flush();

	while (1)
	{
		if (Serial2.available() > 5)
		{
			// if (Serial.available() > 5) {  // solo para pruebas
			// Serial.println("Inside receiveData");
			String dataIn = Serial2.readStringUntil(';');
			if (dataIn[0] == 'O' && dataIn[1] == 'K')
			{
				Serial.println("dato recibido");
			}
		}
		vTaskDelay(10 / portTICK_PERIOD_MS);
	}
	//   vTaskDelete(NULL);
}

// envio de datos por serial para configuracion
void sendSerialData()
{
	String dataToSend = "COMUNICACION;";
	Serial2.print(dataToSend);
	//Serial.println(stateMachine);
}

/* ***************************************************************************
 * **** Ejecucion de la rutina de refrescado de Display ++********************
 * ***************************************************************************/
void task_Display(void *pvParameters)
{
	String taskMessage = "Task runs on core ";
	taskMessage = taskMessage + xPortGetCoreID();
	Serial.println(taskMessage);

	lcd_setup(); // inicializacion de LCD
	lcd.setCursor(0, 0);
	lcd.print(taskMessage);

	while (true)
	{
		/* *************************************************
		 * **** Analisis de los estados de alarma **********
		 * ************************************************/

		// lcd.clear();
		lcd_show_comp();
		lcd.setCursor(0, 0);
		lcd.print(taskMessage);

		// delay de 100 ms en la escritura del LCD
		vTaskDelay(200 / portTICK_PERIOD_MS);
	}
}

// Configuracion del LCD
void lcd_setup()
{
	lcd.begin(21, 22);
	lcd.backlight();
}

/* ***************************************************************************
 * **** CONFIGURACION ********************************************************
 * ***************************************************************************/
void setup()
{
	Serial.begin(115200);
	Serial2.begin(115200);
	Serial2.setTimeout(10);
	init_GPIO();
	init_TIMER();

	// nvs_flash_init();

	// se crea el sem�foro binario
	xSemaphoreEncoder = xSemaphoreCreateBinary();
	xSemaphoreTimer = xSemaphoreCreateBinary();

	// creo la tarea task_pulsador
	xTaskCreatePinnedToCore(task_Encoder, "task_Encoder", 2048, NULL, 4, NULL, taskCoreOne);
	// xTaskCreatePinnedToCore(task_Encoder_B, "task_Encoder_B", 10000, NULL, 1, NULL, taskCoreZero);

	xTaskCreatePinnedToCore(task_timer, "task_timer", 2048, NULL, 2, NULL, taskCoreOne);
	xTaskCreatePinnedToCore(task_Display, "task_Display", 2048, NULL, 3, NULL, taskCoreOne); // se puede colocar en el core cero
	xTaskCreatePinnedToCore(task_Receive, "task_Receive", 2048, NULL, 1, NULL, taskCoreOne);

	// Clean Serial buffers
	vTaskDelay(1000 / portTICK_PERIOD_MS);
}

/* ***************************************************************************
 * **** LOOP MAIN_MENU *******************************************************
 * ***************************************************************************/
void loop()
{
}

/* ***************************************************************************
 * **** FIN DEL PROGRAMA *****************************************************
 * ***************************************************************************/

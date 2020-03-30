#include <LiquidCrystal_I2C.h>

//********DEFINICION DE PINES***********
#define A     4      //variable A a pin digital 2 (DT en modulo)
#define B     35     //variable B a pin digital 4 (CLK en modulo)
#define SW    34     //sw a pin digital 3 (SW en modulo)  

//**********VALORES MAXIMOS**********
#define MENU_QUANTITY 3
#define MAX_FREC 40
#define MAX_RIE 4
#define MAX_PRESION 40
#define MAX_FLUJO 40

volatile unsigned int menu = 0;

// Global al ser usada en loop e ISR (encoder)
unsigned long tiempo1 = 0;
float relacionIE = 4;
float maxPresion = 4;
float maxFlujo = 4;
unsigned int frecRespiratoria = 10;

// Banderas utilizadas en las interrupciones
boolean insideMenuFlag = false;
boolean flagPresion = false;
boolean flagFlujo = false;
boolean flagFrecuencia = false;
boolean flagIE = false;
boolean flagSW = false;
int flagEncoder = 0;

void IRAM_ATTR swInterrupt();
void IRAM_ATTR encoder_Interrupt();

LiquidCrystal_I2C lcd(0x27, 20, 4);   // Objeto LCD

void lcd_setup() {

	//Encoder setup
	pinMode(A, INPUT_PULLUP);    // A como entrada
	pinMode(B, INPUT_PULLUP);    // B como entrada
	pinMode(SW, INPUT_PULLUP);   // SW como entrada

	attachInterrupt(digitalPinToInterrupt(A), encoder_Interrupt, FALLING);
	attachInterrupt(digitalPinToInterrupt(SW), swInterrupt, RISING);
	
	// LCD setup
	lcd.init();
	lcd.backlight();
}

void lcd_show() {

	String relacion_IE = "1:4.1";
	if (relacionIE > 0) {
		relacion_IE = "1:" + String(relacionIE, 1);
	}
	else {
		relacion_IE = String(-relacionIE, 1) + ":1";
	}

	switch (menu) {
	case 0:
		lcd.home();
		lcd.setCursor(0, 0);
		lcd.print("  InnspiraMED UdeA  ");
		lcd.setCursor(0, 1);
		lcd.print("FR        Ppico     ");
		lcd.setCursor(4, 1);
		lcd.print(frecRespiratoria);
		lcd.setCursor(0, 2);
		lcd.print("I:E       PEEP      ");
		lcd.setCursor(4, 2);
		lcd.print(relacion_IE);
		lcd.setCursor(0, 3);
		lcd.print("VT                  ");
		break;
	case 1:
		lcd.setCursor(0, 0);
		lcd.print("   Configuracion    ");
		lcd.setCursor(0, 1);
		lcd.print("                    ");
		lcd.setCursor(0, 2);
		if (flagFrecuencia) {
			lcd.print(" ");
			lcd.write(126);
			lcd.print("Frec:            ");
		}
		else {
			lcd.print("  Frec:             ");
		}
		lcd.setCursor(10, 2);
		lcd.print(frecRespiratoria);
		lcd.setCursor(0, 3);
		if (flagIE) {
			lcd.print(" ");
			lcd.write(126);
			lcd.print("I:E:              ");
		}
		else {
			lcd.print("  I:E:              ");
		}
		lcd.setCursor(10, 3);
		lcd.print(relacion_IE);
		break;
	case 2:
		lcd.setCursor(0, 0);
		lcd.print("      Alarmas       ");
		lcd.setCursor(0, 1);
		lcd.print("                    ");
		lcd.setCursor(0, 2);
		if (flagPresion) {
			lcd.print(" ");
			lcd.write(126);
			lcd.print("Presion:            ");
		}
		else {
			lcd.print("  Presion:            ");
		}
		lcd.setCursor(12, 2);
		lcd.print(maxPresion);
		lcd.setCursor(0, 3);
		if (flagFlujo) {
			lcd.print(" ");
			lcd.write(126);
			lcd.print("Flujo:            ");
			Serial.println("Flujo:  ");
		}
		else {
			lcd.print("  Flujo:            ");
		}
		lcd.setCursor(12, 3);
		lcd.print(maxFlujo);
		break;
	default:
		lcd.setCursor(0, 0);
		lcd.print("                    ");
		lcd.setCursor(0, 1);
		lcd.print("                    ");
		lcd.setCursor(0, 2);
		lcd.print("Default: ");
		lcd.print(12);                      // Escribimos el numero de segundos trascurridos
		lcd.print("           ");
		lcd.setCursor(0, 3);
		lcd.print("                    ");
		break;
	}
	//Serial.println("I am in lcd_show()");
}

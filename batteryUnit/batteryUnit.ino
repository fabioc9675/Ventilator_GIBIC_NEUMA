//#include <avr/sleep.h>

float val1 = 0;
float val2 = 0;
float val3 = 0;
long valorVin;
long valorVbat;
long valorIchar;

//valores para Timer1
const uint16_t t1_load =0; 
const uint16_t t1_comp =31250;

int ADC1 = A0; // voltaje entrada
int ADC2 = A1; // voltaje bateria
int ADC3 = A2; // corriente carga bateria
int ADC4 = A3; // corriente suministrada

//Sw power
int Power = 2;

//Actuadores
int rele = 6;   // energiza o desenergiza el ventilador
int mosfet = 9;   // carga de la bateria
int estado = 7;   //1: fuente; 0 Baterias

//battery status
int stat = 5;  // salida hacia el ventilador 1: fuenta AC, 0: bateria, pulsos: bateria baja
//Status LEDs
int lg = 13;
int lr = 12;
int lb = 10;

//voltage comparator
int vcomparador = 7;

volatile uint8_t modo=0;  // 0:ventilador apagado;1:ventilador encendido
uint8_t contPower=0;  //teporizador de apagado


void setup() {
 // put your setup code here, to run once:

   analogReference(EXTERNAL);         // se usa vref=4.5V
   pinMode(rele, OUTPUT);
   pinMode(mosfet, OUTPUT);
   pinMode(lg, INPUT);
   pinMode(lr, INPUT);
   pinMode(lb, INPUT);
   pinMode(vcomparador, INPUT);
   pinMode(Power, INPUT);
   pinMode(estado, INPUT);
   pinMode(stat, OUTPUT);
   Serial.begin(115200);
   //Serial1.begin(19200);
   delay(5000);
   Serial.print("hola mundo");
 
   
 
}

void loop() {
 // put your main code here, to run repeatedly:

  //  chargeBatt();
   Serial.print("hola mundo");
   Serial.print("Vin:");
   valorVin=analogRead(ADC1);
   valorVin=(valorVin*30000)/668;
   Serial.print(valorVin);
   Serial.print(", Vbat:");
   valorVbat=analogRead(ADC2);
   valorVbat=(valorVbat*30000)/668;
   Serial.print(valorVbat);
   Serial.print(",Ibat:");
   valorIchar=analogRead(ADC3);
   valorIchar=valorIchar*12-6288;
   Serial.print(valorIchar);
   Serial.print(",Iout:");
   Serial.print(analogRead(ADC4));
   Serial.print(", comparador:");
   
   Serial.println(digitalRead(vcomparador));
  
   Serial.print(", POwer:");
   
   Serial.println(digitalRead(Power));
  
   Serial.print(",rele:");
   
   Serial.println(digitalRead(rele));

   
   if (digitalRead(Power)){
    digitalWrite(rele,LOW);   // control de encendido 
    }
   else{
    digitalWrite(rele,HIGH);   // control de encendido
    }

   if (digitalRead(estado)){
    digitalWrite(stat,HIGH);   // control de encendido 
    }
   else{
    digitalWrite(stat,LOW);   // control de encendido
    contPower++;
    }

    if ((~digitalRead(estado))&&(contPower==21)&&(valorVbat<24000)){
      contPower=0;
       digitalWrite(stat,HIGH);
       delay(500);
      }

    if (valorVbat<27200){
      digitalWrite(mosfet,HIGH);
      }
    else{
      digitalWrite(mosfet,LOW);
      }  

    
   
  
   //toSleep();
   
  // digitalWrite(lr, LOW);
  delay(500);

}
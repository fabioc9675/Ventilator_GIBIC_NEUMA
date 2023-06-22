//#include <avr/sleep.h>
#include <EEPROM.h>
#include "LowPower.h"
#define __AVR_ATmega32U4__
float val1 = 0;
float val2 = 0;
float val3 = 0;
long valorVin;
long valorVbat;
long valorIchar;
long diferencia;

//valores para Timer1
const uint16_t t1_load =0; 
const uint16_t t1_comp =31250;
const uint16_t VBatMin =23000;  //Voltaje bajo (mV)
const uint16_t VBatFallo=22300; //voltaje para protección de la batería

const uint8_t ADC1 = A0; // voltaje entrada
const uint8_t ADC2 = A1; // voltaje bateria
const uint8_t ADC3 = A2; // corriente carga bateria
const uint8_t ADC4 = A3; // corriente suministradae

//Sw power
const uint8_t Power = 2;

//Actuadores
const uint8_t rele = 6;   // energiza o desenergiza el ventilador
const uint8_t mosfet = 9;   // carga de la bateria
const uint8_t estado = 4;   //estado del mosfet

//battery status
const uint8_t stat = 5;  // salida hacia el ventilador 1: fuenta AC, 0: bateria, pulsos: bateria baja
const uint8_t IsenseEn = 8; //
//Status LEDs
const uint8_t lg = 13;
const uint8_t lr = 12;
const uint8_t lb = 10;

//voltage comparator
const uint8_t  vcomparador = 7;

volatile uint8_t modo=0;  // 0:ventilador apagado;1:ventilador encendido
volatile uint8_t contPower=0;  //teporizador de apagado
volatile uint8_t ContadorPulsos=0; //contador para pulsos de stat
volatile uint8_t bandera_t3a=0;
volatile uint8_t contCarga=0;  //contador para carga de batería
uint8_t dato=0;

void setup() {
 // put your setup code here, to run once:
   
   analogReference(EXTERNAL);         // se usa Vref=4.5V
   pinMode(rele, OUTPUT);
   pinMode(mosfet, OUTPUT);
   pinMode(lg, OUTPUT);
   pinMode(lr, OUTPUT);
   pinMode(lb, OUTPUT);
   pinMode(vcomparador, INPUT);
   pinMode(Power, INPUT);
   pinMode(estado, INPUT);
   pinMode(stat, OUTPUT);
   pinMode(IsenseEn, OUTPUT);

   //estados iniciales LEDS:
   digitalWrite(lg,HIGH);
   digitalWrite(lr,HIGH);
   digitalWrite(lb,HIGH);   

   
   digitalWrite(IsenseEn,LOW);   //low: activado  High:desactivado
   digitalWrite(mosfet,LOW);
   
   //Serial.begin(115200);
   EEPROM.write(0, 'J');
   EEPROM.write(1, 'D');
   EEPROM.write(2, 'L');
   EEPROM.write(3, 'D');
   //Serial.print("Leido: ");
   dato = EEPROM.read(0);
   //Serial.print(dato);
   dato = EEPROM.read(1);
   //Serial.print(dato);
   dato = EEPROM.read(2);
   //Serial.print(dato);
   dato = EEPROM.read(3);
   //Serial.print(dato);
   delay(2000);

   //Configuración de enterrupcion IRQ1 en falling edge (boton de power)
   EICRA |= (1 << ISC10);      //1
   EICRA &= ~(1 << ISC11);     //0
    //habilitar INT1
   EIMSK|= (1 << INT1);     //1

   //Configuracion para timer 3 init
   TCCR3A = 0;   //salidas OCn desconectadas 
   //poner prescaler de T1 a 256 (CS12 =1, CS11 =0, CS10 =0): 
   TCCR3B |=(1 << CS32);
   TCCR3B &=~(1 << CS31);
   TCCR3B &=~(1 << CS30);
   TCNT3=t1_load; // valor inicial del temporizador (0)
   OCR3A = t1_comp; // valor para el O.C. 
   TIMSK3 |= (1 << OCIE3A); // habilita int de timer1
     
   //habilitar interrupciones:
   sei();   //cli(); las deshabilita
}

void loop() {
 // put your main code here, to run repeatedly:


   if (bandera_t3a==1){
       //Serial.print("Vin:");
       valorVin=analogRead(ADC1);
       valorVin=(valorVin*30000)/668;
       //Serial.print(valorVin);
       //Serial.print(", Vbat:");
       valorVbat=analogRead(ADC2);
       valorVbat=(valorVbat*30000)/668;
       //Serial.print(valorVbat);
       //Serial.print(",Ibat:");
       valorIchar=analogRead(ADC3);
       //valorIchar=valorIchar*12-6288;
       //Serial.print(valorIchar);
       //Serial.print(", comparador:");
    
       //Serial.println(digitalRead(vcomparador));
      
       //Serial.print(", POwer:");
       
       //Serial.println(digitalRead(Power));
      
       //Serial.print(", estado:");
       
       //Serial.println(digitalRead(estado));
     
       bandera_t3a=0;
      }

      //entrar a modo de bajo consumo si la batería está muy baja
    if ((valorVbat<VBatFallo)&&(!digitalRead(vcomparador))){
      //apaga LEDS y Senesor de corriente
      digitalWrite(IsenseEn,HIGH);
      digitalWrite(lg,HIGH);
      digitalWrite(lr,HIGH);
      digitalWrite(rele,LOW);  //apaga la salida
      LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
      delay(100);
      }

  //carga de la baterÃ­a
    if (digitalRead(vcomparador))  {
        if(contCarga<30){
          digitalWrite(mosfet,LOW); //apaga cargador para hacer medida de la baterÃ­a
          }
        if(contCarga==30){
          diferencia=valorVin-valorVbat;  //calcular diferencia de Vin y Vbat
          }
        if((diferencia>200)&&(diferencia<2200)){
          if(contCarga>30){
            digitalWrite(mosfet,HIGH); //carga 75%
            }
          }
        if((diferencia>2201)&&(diferencia<3700)){
          if((contCarga>30)&&(contCarga<76)){
            digitalWrite(mosfet,HIGH); //carga 38%
            }
           else{
            digitalWrite(mosfet,LOW);
            } 
          }
          
        if((diferencia>3701)&&(diferencia<5200)){
          if((contCarga>30)&&(contCarga<61)){
            digitalWrite(mosfet,HIGH); //carga 25%
            }
           else{
            digitalWrite(mosfet,LOW);
            } 
          }    
      
        if((diferencia>5201)&&(diferencia<6700)){
          if((contCarga>30)&&(contCarga<54)){
            digitalWrite(mosfet,HIGH); //carga 20%
            }
           else{
            digitalWrite(mosfet,LOW);
            } 
          }
        if((diferencia>6701)&&(diferencia<8200)){
          if((contCarga>30)&&(contCarga<42)){
            digitalWrite(mosfet,HIGH); //carga 10%
            }
           else{
            digitalWrite(mosfet,LOW);
            } 
          }
        if((diferencia>8201)&&(diferencia<9700)){
          if((contCarga>30)&&(contCarga<38)){
            digitalWrite(mosfet,HIGH); //carga 5%
            }
           else{
            digitalWrite(mosfet,LOW);
            } 
          }        

        if((diferencia>9701)){
          if((contCarga>30)&&(contCarga<35)){
            digitalWrite(mosfet,HIGH); //carga minima
            }
           else{
            digitalWrite(mosfet,LOW);
            } 
          }
        if(contCarga==120){
          contCarga=0;
          }
          
      
      }
  
}  

 ISR(INT1_vect) {

  //Serial.print("Power:");
  //Serial.println(!digitalRead(Power));
  digitalWrite(rele,!digitalRead(Power)); // control de activación del ventilador
   EIMSK&=~(1 << INT1);     //1
   contPower=1;
   
  }

 ISR(TIMER3_COMPA_vect) {
  TCNT3=t1_load; // reinicia T1
  bandera_t3a=1;
  digitalWrite(rele,!digitalRead(Power)); // control de activación del ventilador (respaldo)
//  digitalWrite(lr,!digitalRead(rele));
   digitalWrite(lg,!(digitalRead(lg)));
  
  //antirrebote INT1
  if (contPower>0){
     contPower++;
     if (contPower==2){
       contPower==0;
      //rehabilitar INT1
       EIFR &=~(1 << INTF1);   //borra posibles interrupciones pendientes
       EIMSK|= (1 << INT1);     //1
     }
   }

   digitalWrite(stat,digitalRead(vcomparador));
   if(!digitalRead(stat)){
    ContadorPulsos++;
    if ((ContadorPulsos>20)&&(valorVbat<VBatMin)){
      digitalWrite(stat,HIGH);   //pulso de bateria baja
      ContadorPulsos=0;
      }
    }
   
   digitalWrite(lr,!digitalRead(stat)); 
   contCarga++; 
}

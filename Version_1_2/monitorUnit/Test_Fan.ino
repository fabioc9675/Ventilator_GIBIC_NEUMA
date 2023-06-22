#define FAN_POW             19

hw_timer_t * timer = NULL;

void IRAM_ATTR onTimer(){
  digitalWrite(FAN_POW,  !digitalRead(FAN_POW)); }


  void setup() {

  timer = timerBegin(0, 80, true);                 //timer0,main clock 80MHZ , counter true
  timerAttachInterrupt(timer, &onTimer, true);     
  timerAlarmWrite(timer, 0.25, true);
  timerAlarmEnable(timer);
}


void loop() {  }

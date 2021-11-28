#define pin_out 6
#define pin_in A0
#define frequency 50   //Frequency in Hertz


int pulse_duration = 500/frequency;

void setup() {
  pinMode(pin_out, OUTPUT);
  pinMode(pin_in, INPUT);
  

}

void loop() {

   if(analogRead(pin_in) < 300){
      digitalWrite(pin_out, LOW);
   }else if(analogRead(pin_in) > 800){
      digitalWrite(pin_out, HIGH);
   }else{
     if (digitalRead(pin_in)==HIGH){
       digitalWrite(pin_out, HIGH);
       delay(pulse_duration);
       digitalWrite(pin_out, LOW);
       delay(pulse_duration);
     }else{
        digitalWrite(pin_out, LOW);
     }
   }
}

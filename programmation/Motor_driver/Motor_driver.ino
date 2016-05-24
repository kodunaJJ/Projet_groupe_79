const char ena=3;/*simple enable ou variation de vitesse si PWM*/
const char in1=2;/*broche de contrle du sens de rotation moteur et commande marche arret */
const char in2=4;
const char ForwardButton = 7;
const char BackwardButton = 8;
/*const int potar = 0;*/ /* la broche pour régler la vitesse en tant que test*/


void setup() {
 
   pinMode(ena,OUTPUT);
   pinMode(in1,OUTPUT);
   pinMode(in2,OUTPUT);
   pinMode(ForwardButton,INPUT_PULLUP);
   pinMode(BackwardButton,INPUT_PULLUP);
  // on met le moteur a l'arret
   analogWrite(ena,0);
   digitalWrite(in1,LOW);
   digitalWrite(in2,LOW);
}

unsigned char debounce_pullup_button(const char pin_input, unsigned int debounce_delay){
  
    if(digitalRead(pin_input)==0 || digitalRead(pin_input)==1) delay(debounce_delay);
     return digitalRead(pin_input); 
}

void motor_brake_cmd( const char ena, const char in1, const char in2, int pwm_value){
  
 analogWrite(ena,pwm_value);
  //digitalWrite(ena,HIGH);
  digitalWrite(in1,HIGH);
  digitalWrite(in2,LOW);
  // pour la phase de test
  //delay(10000);
}

void motor_loose_cmd( const char ena, const char in1, const char in2, int pwm_value){
  
  analogWrite(ena,pwm_value);
  //digitalWrite(ena,HIGH);
  digitalWrite(in1,LOW);
  digitalWrite(in2,HIGH);
  // pour la phase de test
  //delay(10000);
}

void motor_stop_cmd(const char ena, const char in1, const char in2, int pwm_value){
  
  analogWrite(ena,pwm_value);
  digitalWrite(in1,LOW);
  digitalWrite(in2,HIGH);
  // pour phase de test. Peut etre a garder ??
  delay(1000);
}

void motor_accel(const char ena, const char in1, const char in2, float t, float v_ordered){
  // a faire
}

void motor_decel(const char ena, const char in1, const char in2, float t, float v_cur){
  // a faire
}

void motor_manu_cmd(const char ena, const char in1, const char in2,const char ForwardButton,const char BackwardButton, unsigned int debounce_delay /*int pwm_value*/){
  unsigned char Fwd=digitalRead(ForwardButton);
  Fwd=debounce_pullup_button(ForwardButton,debounce_delay);
  unsigned char Bwd=digitalRead(BackwardButton);
  Bwd=debounce_pullup_button(BackwardButton,debounce_delay);
  
  if((Bwd && Fwd)||(Bwd==0 && Fwd==0)){
    motor_stop_cmd(ena,in1,in2,25);
  }
  else if(Fwd==0){
          motor_brake_cmd(ena,in1,in2,255);
       }
       else{
         motor_loose_cmd(ena,in1,in2,255);
       }
}

void loop(){
  // sequence de test  ecrire
  /*motor_brake_cmd(ena,in1,in2);
  motor_stop_cmd(ena,in1,in2);
  motor_loose_cmd(ena,in1,in2);
  motor_stop_cmd(ena,in1,in2);*/
  motor_manu_cmd(ena,in1,in2,ForwardButton,BackwardButton,50);
}

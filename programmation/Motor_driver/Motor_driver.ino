const char ena=3;/*simple enable ou variation de vitesse si PWM*/
const char in1=2;/*broche de contrle du sens de rotation moteur et commande marche arret */
const char in2=4;
/*const int potar = 0;*/ /* la broche pour régler la vitesse en tant que test*/


void setup() {
 
   pinMode(ena,OUTPUT);
   pinMode(in1,OUTPUT);
   pinMode(in2,OUTPUT);
  // on met le moteur a l'arret
   analogWrite(ena,0);
   digitalWrite(in1,LOW);
   digitalWrite(in2,LOW);
}

void motor_brake_cmd( const char ena, const char in1, const char in2)/*, const char pot)*/ {
  
  //int vitesse = analogRead(potar)/4;
 // analogWrite(ena,vitesse);
  digitalWrite(ena,HIGH);
  digitalWrite(in1,HIGH);
  digitalWrite(in2,LOW);
  // pour la phase de test
  delay(10000);
}

void motor_loose_cmd( const char ena, const char in1, const char in2)/*, const char pot)*/ {
  
  //int vitesse = analogRead(potar)/4;
  //analogWrite(ena,vitesse);
  digitalWrite(ena,HIGH);
  digitalWrite(in1,LOW);
  digitalWrite(in2,HIGH);
  // pour la phase de test
  delay(10000);
}

void motor_stop_cmd(const char ena, const char in1, const char in2){
  
  analogWrite(ena,0);
  digitalWrite(in1,LOW);
  digitalWrite(in2,LOW);
  // pour phase de test. Peut etre a garder ??
  delay(2000);
}

void motor_accel(const char ena, const char in1, const char in2, float t, float v_ordered){
  // a faire
}

void motor_decel(const char ena, const char in1, const char in2, float t, float v_cur){
  // a faire
}


void loop(){
  // sequence de test  ecrire
  motor_brake_cmd(ena,in1,in2);
  motor_stop_cmd(ena,in1,in2);
  motor_loose_cmd(ena,in1,in2);
  
}

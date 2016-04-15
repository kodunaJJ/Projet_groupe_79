const int Digit_pin = 13;
void setup() {
  // put your setup code here, to run once:
  pinMode(Digit_pin,OUTPUT); 

}

void loop() {
  // put your main code here, to run repeatedly: 
  digitalWrite(Digit_pin,HIGH);
  delay(500);
  digitalWrite(Digit_pin,LOW);
  delay(500);
  
}

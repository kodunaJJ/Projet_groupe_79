//programme de recuperation de la vitesse de pedalage de l'utilisateur
#include <LiquidCrystal.h>
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

const int motor_pin = A0;
  // declaration des variables utilisees
int analogValue = 0; // valeur numerique de la tension en sortie du capteur
float k_motor = 0.00094; // constante fem du moteur
float v = 0; // vitesse de pedalage en tr/min
float v_lin = 0; // vitesse du cycliste
float gear_ratio = 1; // rapport reduction pedalier/capteur vitesse 

  
// fonctions

double drive_speed(int analogValue, float gear_ratio, float motor_param){
  float v_d= analogValue/gear_ratio*5.0/1024.0/motor_param;
  return v_d;
}
  
double linear_speed(float v, float wheel_diam, float bracket){
   float wheel_size = 2.54*wheel_diam/100.0; //conversion centimetre --> metre
   float v_rd = v*bracket*3.14/30.0;
   float s = v_rd*wheel_size*3.6; // vitesse du cycliste en km/h
   return s;
}

void setup(){ // initialisation
  
  Serial.begin(9600); // communication arduino --> PC
    // set up the LCD's number of columns and rows: 
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.clear();
}

// debut programme

void loop(){
  analogValue = analogRead(motor_pin);
  v = drive_speed(analogValue,1,k_motor);
  v_lin = linear_speed(v,24,1);
  lcd.setCursor(0, 0);
  Serial.print("analogValue = ");
  Serial.print(analogValue);
  Serial.print("\t");
  Serial.print("vitesse = ");
  Serial.print(v);
  Serial.print("vitesse lin = ");
  Serial.print(v_lin);
  Serial.print("\n");
  lcd.print("anaVal = ");
  lcd.print(analogValue);
  lcd.setCursor(0,1);
  lcd.print("vitesse = ");
  lcd.print(v_lin);
  delay(1000);
}
  

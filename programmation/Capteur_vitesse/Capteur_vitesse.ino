//programme de recuperation de la vitesse de pedalage de l'utilisateur
#include <LiquidCrystal.h>
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

const int motor_pin = A0;
  // declaration des variables utilisees
float analogValue = 0; // valeur numerique de la tension en sortie du capteur
float k_motor = 0.00094; // constante fem du moteur
float v = 0; // vitesse de pedalage en tr/min
float v_lin = 0; // vitesse du cycliste
float gear_ratio_sensor = 5; // rapport reduction pedalier/capteur vitesse 
float gear_ratio = 1;        // rapport reduction plateau/roue

// average variable

const unsigned char numReadings = 4;
unsigned int readings[numReadings];      // the readings from the analog input


  
// fonctions

double drive_speed(int analogValue, float gear_ratio_sensor, float motor_param){
  float v_d= analogValue/gear_ratio_sensor*5.0/1023.0/motor_param;
  return v_d;
}
  
double linear_speed(float v, float wheel_diam, float gear_ratio){ 
  float wheel_size = 2.54*wheel_diam/100.0; //conversion centimetre --> metre
   float v_rd = v*gear_ratio*3.14/30.0;
   float s = v_rd*wheel_size*3.6; // vitesse du cycliste en km/h
   return s;
}

void init_tab(unsigned int* tab, unsigned char Size){
  for (int thisReading = 0; thisReading < Size; thisReading++)
    *(readings+thisReading) = 0;
}
//average function

float running_average(const int analogInputPin, const unsigned char numReading, unsigned int* tab){
  static unsigned char i = 0;
  static unsigned int index;                  // the index of the current reading
  static float total;                  // the running total
  static float average;
 if(i==0){ 
   index = 0;                 
   total = 0;                  
   average = 0; 
   i++;
 }
 total= total - *(tab+index);         
  // read from the sensor:  
  *(tab+index) = analogRead(analogInputPin);
  // add the reading to the total:
  total= total + *(tab+index);       
  // advance to the next position in the array:  
  index = index + 1;                    

  // if we're at the end of the array...
  if (index >= numReadings)              
    // ...wrap around to the beginning: 
    index = 0;                           

  // calculate the average:
  average = total / numReadings; 
  return average;
}

void setup(){ // initialisation
  
  Serial.begin(9600); // communication arduino --> PC
    // set up the LCD's number of columns and rows: 
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.clear();
  init_tab(readings,numReadings);
}

// debut programme

void loop(){
  analogValue = running_average(motor_pin,numReadings,readings);
  v = drive_speed(analogValue,gear_ratio_sensor,k_motor);
  v_lin = linear_speed(v,24,1);
  lcd.setCursor(0, 0);
  Serial.print("analogValue = ");
  Serial.print(analogValue);
  Serial.print("\t");
  Serial.print("vitesse = ");
  Serial.print(v);
  Serial.print("vitesse lin = ");
  Serial.print(v_lin);*/
  Serial.print("\n");
 /* lcd.print("anaVal = ");
  lcd.print(analogValue);*/
  //lcd.setCursor(0,1);
  lcd.print("vitesse = ");
  lcd.setCursor(0,1);
  lcd.print(v);
  lcd.setCursor(10,1);
  lcd.print("km/h");
  delay(1000);
}
  
